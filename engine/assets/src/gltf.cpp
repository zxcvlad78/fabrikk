#include <fabrikk/assets/gltf.hpp>

#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

#include <stb_image.h>

#include <array>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

namespace fabrikk::assets {

namespace {

std::vector<u8> read_file(std::string_view path) {
    std::ifstream file(std::string(path), std::ios::binary | std::ios::ate);
    FK_ASSERT(file.is_open(), "glTF: cannot open '{}'", path);
    const std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<u8> bytes(static_cast<usize>(size));
    FK_ASSERT(file.read(reinterpret_cast<char*>(bytes.data()), size), "glTF: read failed '{}'",
              path);
    return bytes;
}

std::vector<u8> base64_decode(std::string_view in) {
    static constexpr std::array<int, 256> kTable = [] {
        std::array<int, 256> t{};
        t.fill(-1);
        const char* alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        for (int i = 0; i < 64; ++i) {
            t[static_cast<u8>(alpha[i])] = i;
        }
        return t;
    }();

    std::vector<u8> out;
    out.reserve(in.size() * 3 / 4);
    u32 acc = 0;
    int bits = 0;
    for (const char c : in) {
        const int v = kTable[static_cast<u8>(c)];
        if (v < 0) {
            if (c == '=') {
                break;
            }
            continue;
        }
        acc = (acc << 6) | static_cast<u32>(v);
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            out.push_back(static_cast<u8>((acc >> bits) & 0xFF));
        }
    }
    return out;
}

std::string parent_dir(std::string_view path) {
    const usize slash = path.find_last_of("/\\");
    return slash == std::string_view::npos ? std::string() : std::string(path.substr(0, slash));
}

const cgltf_accessor* find_attribute(const cgltf_primitive* prim, cgltf_attribute_type type) {
    for (cgltf_size i = 0; i < prim->attributes_count; ++i) {
        if (prim->attributes[i].type == type) {
            return prim->attributes[i].data;
        }
    }
    return nullptr;
}

void decode_image(GltfImage& out, const cgltf_image* img, std::string_view base_dir) {
    const auto load_memory = [&](const u8* data, cgltf_size size) {
        int w = 0;
        int h = 0;
        int comp = 0;
        const stbi_uc* px = stbi_load_from_memory(data, static_cast<int>(size), &w, &h, &comp, 4);
        if (px == nullptr) {
            FK_LOG_WARN("glTF: image decode failed ({})", stbi_failure_reason());
            return;
        }
        out.width = static_cast<u32>(w);
        out.height = static_cast<u32>(h);
        out.rgba.assign(px, px + static_cast<usize>(w) * h * 4);
        stbi_image_free(const_cast<stbi_uc*>(px));
    };

    if (img->name != nullptr) {
        out.name = img->name;
    }
    if (img->uri != nullptr) {
        out.uri = img->uri;
    }

    if (img->buffer_view != nullptr) {
        const cgltf_buffer_view* view = img->buffer_view;
        const auto* base = static_cast<const u8*>(view->buffer->data);
        load_memory(base + view->offset, view->size);
        return;
    }
    if (img->uri == nullptr) {
        return;
    }

    const std::string_view uri = img->uri;
    if (uri.starts_with("data:")) {
        const usize comma = uri.find(',');
        if (comma == std::string_view::npos) {
            FK_LOG_WARN("glTF: malformed data URI");
            return;
        }
        const std::vector<u8> raw = base64_decode(uri.substr(comma + 1));
        load_memory(raw.data(), raw.size());
        return;
    }

    std::string path(uri);
    if (!base_dir.empty()) {
        path = std::string(base_dir) + "/" + path;
    }
    int w = 0;
    int h = 0;
    int comp = 0;
    const stbi_uc* px = stbi_load(path.c_str(), &w, &h, &comp, 4);
    if (px == nullptr) {
        FK_LOG_WARN("glTF: cannot load image '{}'", path);
        return;
    }
    out.width = static_cast<u32>(w);
    out.height = static_cast<u32>(h);
    out.rgba.assign(px, px + static_cast<usize>(w) * h * 4);
    stbi_image_free(const_cast<stbi_uc*>(px));
}

void read_primitive(GltfPrimitive& out, const cgltf_primitive* prim) {
    render::MeshData& mesh = out.mesh;
    const cgltf_accessor* pos = find_attribute(prim, cgltf_attribute_type_position);
    FK_ASSERT(pos != nullptr, "glTF primitive without POSITION");
    const cgltf_size count = pos->count;

    mesh.vertices.resize(count);
    for (cgltf_size i = 0; i < count; ++i) {
        cgltf_accessor_read_float(pos, i, &mesh.vertices[i].position.x, 3);
    }

    if (const cgltf_accessor* uv = find_attribute(prim, cgltf_attribute_type_texcoord)) {
        for (cgltf_size i = 0; i < count; ++i) {
            cgltf_accessor_read_float(uv, i, &mesh.vertices[i].uv.x, 2);
        }
    }

    const cgltf_accessor* idx = prim->indices;
    mesh.indices.resize(idx != nullptr ? idx->count : count);
    for (cgltf_size i = 0; i < mesh.indices.size(); ++i) {
        mesh.indices[i] =
            idx != nullptr ? static_cast<u32>(cgltf_accessor_read_index(idx, i)) : static_cast<u32>(i);
    }

    if (const cgltf_accessor* joints = find_attribute(prim, cgltf_attribute_type_joints)) {
        const cgltf_accessor* weights = find_attribute(prim, cgltf_attribute_type_weights);
        if (weights == nullptr || joints->count != count || weights->count != count) {
            FK_LOG_WARN("glTF: primitive has mismatched skin attributes, ignoring");
        } else {
            out.skin.joints.resize(count);
            out.skin.weights.resize(count);
            for (cgltf_size i = 0; i < count; ++i) {
                cgltf_uint v[4] = {};
                cgltf_accessor_read_uint(joints, i, v, 4);
                out.skin.joints[i] = {static_cast<u16>(v[0]), static_cast<u16>(v[1]),
                                      static_cast<u16>(v[2]), static_cast<u16>(v[3])};
                cgltf_accessor_read_float(weights, i, &out.skin.weights[i].x, 4);
            }
        }
    }

    if (const cgltf_accessor* nrm = find_attribute(prim, cgltf_attribute_type_normal)) {
        for (cgltf_size i = 0; i < count; ++i) {
            cgltf_accessor_read_float(nrm, i, &mesh.vertices[i].normal.x, 3);
        }
        return;
    }

    for (render::VertexPNU& v : mesh.vertices) {
        v.normal = {0.0f, 0.0f, 0.0f};
    }
    for (usize t = 0; t + 2 < mesh.indices.size(); t += 3) {
        render::VertexPNU& a = mesh.vertices[mesh.indices[t]];
        render::VertexPNU& b = mesh.vertices[mesh.indices[t + 1]];
        render::VertexPNU& c = mesh.vertices[mesh.indices[t + 2]];
        const Vec3f e1 = b.position - a.position;
        const Vec3f e2 = c.position - a.position;
        const Vec3f face = Vec3f::cross(e1, e2);
        a.normal = a.normal + face;
        b.normal = b.normal + face;
        c.normal = c.normal + face;
    }
    for (render::VertexPNU& v : mesh.vertices) {
        v.normal = Vec3f::normalize(v.normal);
    }
}

struct NodeTRS {
    Vec3f position {};
    Vec3f rotation {};
    Vec3f scale {1.0f, 1.0f, 1.0f};
};

NodeTRS node_trs(const cgltf_node* node) {
    NodeTRS trs;
    if (node->has_matrix) {
        Mat4f m{};
        for (int k = 0; k < 16; ++k) {
            m.m[k] = node->matrix[k];
        }
        trs.position = {m.m[12], m.m[13], m.m[14]};
        const Vec3f c0 {m.m[0], m.m[1], m.m[2]};
        const Vec3f c1 {m.m[4], m.m[5], m.m[6]};
        const Vec3f c2 {m.m[8], m.m[9], m.m[10]};
        const f32 sx = Vec3f::length(c0);
        const f32 sy = Vec3f::length(c1);
        const f32 sz = Vec3f::length(c2);
        trs.scale = {sx, sy, sz};
        if (sx > 1e-8f && sy > 1e-8f && sz > 1e-8f) {
            Mat4f rot = Mat4f::identity();
            rot.m[0] = c0.x / sx;
            rot.m[1] = c0.y / sx;
            rot.m[2] = c0.z / sx;
            rot.m[4] = c1.x / sy;
            rot.m[5] = c1.y / sy;
            rot.m[6] = c1.z / sy;
            rot.m[8] = c2.x / sz;
            rot.m[9] = c2.y / sz;
            rot.m[10] = c2.z / sz;
            trs.rotation = euler_deg_from_mat(rot);
        }
        return trs;
    }
    trs.position = {node->translation[0], node->translation[1], node->translation[2]};
    if (node->has_rotation) {
        trs.rotation = quat_to_euler_deg(
            {node->rotation[0], node->rotation[1], node->rotation[2], node->rotation[3]});
    }
    if (node->has_scale) {
        trs.scale = {node->scale[0], node->scale[1], node->scale[2]};
    }
    return trs;
}

void read_skins(const cgltf_data* data, GltfScene& scene) {
    for (cgltf_size s = 0; s < data->skins_count; ++s) {
        const cgltf_skin& skin = data->skins[s];
        GltfSkeleton& out = scene.skeletons.emplace_back();
        out.name = skin.name != nullptr
                       ? skin.name
                       : "skin" + std::to_string(scene.skeletons.size() - 1);

        struct Joint {
            u32 node;
            u32 depth;
        };
        std::vector<Joint> joints(skin.joints_count);
        for (cgltf_size j = 0; j < skin.joints_count; ++j) {
            const cgltf_node* node = skin.joints[j];
            u32 depth = 0;
            for (const cgltf_node* p = node->parent; p != nullptr; p = p->parent) {
                ++depth;
            }
            joints[j] = {static_cast<u32>(node - data->nodes), depth};
        }
        std::stable_sort(joints.begin(), joints.end(),
                         [](const Joint& a, const Joint& b) { return a.depth < b.depth; });
        std::unordered_map<u32, u32> order;
        for (u32 j = 0; j < joints.size(); ++j) {
            order[joints[j].node] = j;
        }

        const cgltf_size count = skin.joints_count;
        out.joint_names.resize(count);
        out.parents.resize(count, -1);
        out.joint_nodes.resize(count);
        out.bind_positions.resize(count);
        out.bind_rotations.resize(count);
        out.bind_scales.resize(count);
        out.inverse_bind.resize(count, Mat4f::identity());

        for (u32 j = 0; j < count; ++j) {
            const cgltf_node* node = &data->nodes[joints[j].node];
            out.joint_nodes[j] = joints[j].node;
            out.joint_names[j] =
                node->name != nullptr ? node->name : "joint" + std::to_string(j);
            for (const cgltf_node* p = node->parent; p != nullptr; p = p->parent) {
                const auto it = order.find(static_cast<u32>(p - data->nodes));
                if (it != order.end()) {
                    out.parents[j] = static_cast<i32>(it->second);
                    break;
                }
            }
            const NodeTRS trs = node_trs(node);
            out.bind_positions[j] = trs.position;
            out.bind_rotations[j] = trs.rotation;
            out.bind_scales[j] = trs.scale;
        }

        if (skin.inverse_bind_matrices != nullptr) {
            const cgltf_accessor* acc = skin.inverse_bind_matrices;
            const cgltf_size n = std::min<cgltf_size>(acc->count, count);
            for (cgltf_size j = 0; j < n; ++j) {
                cgltf_accessor_read_float(acc, j, out.inverse_bind[j].m, 16);
            }
        }
    }
}

void read_animations(const cgltf_data* data, GltfScene& scene) {
    for (cgltf_size a = 0; a < data->animations_count; ++a) {
        const cgltf_animation& anim = data->animations[a];
        GltfAnimation& out = scene.animations.emplace_back();
        out.name = anim.name != nullptr
                       ? anim.name
                       : "anim" + std::to_string(scene.animations.size() - 1);

        for (cgltf_size c = 0; c < anim.channels_count; ++c) {
            const cgltf_animation_channel& src = anim.channels[c];
            if (src.target_node == nullptr || src.sampler == nullptr ||
                src.sampler->input == nullptr || src.sampler->output == nullptr) {
                continue;
            }
            GltfAnimChannel::Path path;
            switch (src.target_path) {
                case cgltf_animation_path_type_translation:
                    path = GltfAnimChannel::Path::Translation;
                    break;
                case cgltf_animation_path_type_rotation:
                    path = GltfAnimChannel::Path::Rotation;
                    break;
                case cgltf_animation_path_type_scale:
                    path = GltfAnimChannel::Path::Scale;
                    break;
                default:
                    continue;
            }
            const cgltf_accessor* input = src.sampler->input;
            const cgltf_accessor* output = src.sampler->output;
            if (input->count == 0 || input->count != output->count) {
                continue;
            }

            GltfAnimChannel& channel = out.channels.emplace_back();
            channel.node = static_cast<u32>(src.target_node - data->nodes);
            channel.path = path;
            channel.times.resize(input->count);
            for (cgltf_size i = 0; i < input->count; ++i) {
                cgltf_accessor_read_float(input, i, &channel.times[i], 1);
            }
            if (path == GltfAnimChannel::Path::Rotation) {
                channel.rotations.resize(input->count);
                for (cgltf_size i = 0; i < input->count; ++i) {
                    cgltf_accessor_read_float(output, i, &channel.rotations[i].x, 4);
                }
            } else {
                channel.values.resize(input->count);
                for (cgltf_size i = 0; i < input->count; ++i) {
                    cgltf_accessor_read_float(output, i, &channel.values[i].x, 3);
                }
            }
        }
    }
}

}

f32 GltfAnimation::duration() const {
    f32 result = 0.0f;
    for (const GltfAnimChannel& channel : channels) {
        if (!channel.times.empty()) {
            result = std::max(result, channel.times.back());
        }
    }
    return result;
}

GltfScene GltfScene::load(std::string_view path) {
    const std::vector<u8> bytes = read_file(path);
    return parse(bytes, parent_dir(path));
}

GltfScene GltfScene::parse(std::span<const u8> bytes, std::string_view base_dir) {
    cgltf_options options{};
    cgltf_data* raw = nullptr;
    FK_ASSERT(cgltf_parse(&options, bytes.data(), bytes.size(), &raw) == cgltf_result_success,
              "glTF: JSON/GLB parse failed");
    std::unique_ptr<cgltf_data, void (*)(cgltf_data*)> data(raw, cgltf_free);

    const std::string path_hint =
        base_dir.empty() ? std::string("memory.gltf") : std::string(base_dir) + "/memory.gltf";
    FK_ASSERT(cgltf_load_buffers(&options, data.get(), path_hint.c_str()) ==
                  cgltf_result_success,
              "glTF: buffer load failed");
    FK_ASSERT(cgltf_validate(data.get()) == cgltf_result_success, "glTF: validation failed");

    GltfScene scene;

    scene.images.reserve(data->images_count);
    for (cgltf_size i = 0; i < data->images_count; ++i) {
        GltfImage& img = scene.images.emplace_back();
        decode_image(img, &data->images[i], base_dir);
    }

    const auto image_index = [&](const cgltf_image* img) -> i32 {
        if (img == nullptr) {
            return -1;
        }
        return static_cast<i32>(img - data->images);
    };

    scene.materials.reserve(data->materials_count > 0 ? data->materials_count : 1);
    for (cgltf_size i = 0; i < data->materials_count; ++i) {
        const cgltf_material& src = data->materials[i];
        GltfMaterial m;
        if (src.name != nullptr) {
            m.name = src.name;
        }
        if (src.has_pbr_metallic_roughness) {
            const cgltf_pbr_metallic_roughness& pbr = src.pbr_metallic_roughness;
            m.base_color = {pbr.base_color_factor[0], pbr.base_color_factor[1],
                            pbr.base_color_factor[2], pbr.base_color_factor[3]};
            m.metallic = pbr.metallic_factor;
            m.roughness = pbr.roughness_factor;
            if (pbr.base_color_texture.texture != nullptr) {
                m.base_color_image = image_index(pbr.base_color_texture.texture->image);
            }
        }
        m.emissive = {src.emissive_factor[0], src.emissive_factor[1], src.emissive_factor[2]};
        if (src.normal_texture.texture != nullptr) {
            m.normal_image = image_index(src.normal_texture.texture->image);
        }
        scene.materials.push_back(m);
    }
    if (scene.materials.empty()) {
        scene.materials.push_back(GltfMaterial{});
    }

    std::vector<u32> mesh_prim_base(data->meshes_count);
    std::vector<u32> prim_remap;
    prim_remap.reserve(data->meshes_count * 2);
    u32 virtual_index = 0;
    u32 packed_index = 0;
    for (cgltf_size i = 0; i < data->meshes_count; ++i) {
        mesh_prim_base[i] = virtual_index;
        for (cgltf_size j = 0; j < data->meshes[i].primitives_count; ++j) {
            const cgltf_primitive& prim = data->meshes[i].primitives[j];
            if (prim.type != cgltf_primitive_type_triangles) {
                FK_LOG_WARN("glTF: skipping non-triangle primitive");
                prim_remap.push_back(UINT32_MAX);
            } else {
                prim_remap.push_back(packed_index++);
            }
            ++virtual_index;
        }
    }

    u32 default_material = 0;
    if (!scene.materials.empty() && data->materials_count > 0) {
        for (cgltf_size i = 0; i < data->meshes_count; ++i) {
            for (cgltf_size j = 0; j < data->meshes[i].primitives_count; ++j) {
                if (data->meshes[i].primitives[j].material == nullptr) {
                    default_material = static_cast<u32>(scene.materials.size());
                    scene.materials.push_back(GltfMaterial{});
                    i = data->meshes_count;
                    break;
                }
            }
        }
    }

    for (cgltf_size i = 0; i < data->meshes_count; ++i) {
        const cgltf_mesh& mesh = data->meshes[i];
        for (cgltf_size j = 0; j < mesh.primitives_count; ++j) {
            const u32 mapped = prim_remap[mesh_prim_base[i] + j];
            if (mapped == UINT32_MAX) {
                continue;
            }
            const cgltf_primitive& prim = mesh.primitives[j];
            GltfPrimitive& out = scene.primitives.emplace_back();
            read_primitive(out, &prim);
            if (prim.material != nullptr) {
                out.material = static_cast<u32>(prim.material - data->materials);
            } else {
                out.material = default_material;
            }
        }
    }

    const auto node_local = [](const cgltf_node* node) {
        if (node->has_matrix) {
            Mat4f m{};
            for (int k = 0; k < 16; ++k) {
                m.m[k] = node->matrix[k];
            }
            return m;
        }
        Mat4f local = Mat4f::translate({node->translation[0], node->translation[1],
                                        node->translation[2]});
        if (node->has_rotation) {
            local = local * quat_to_mat({node->rotation[0], node->rotation[1], node->rotation[2],
                                         node->rotation[3]});
        }
        if (node->has_scale) {
            local = local * Mat4f::scale({node->scale[0], node->scale[1], node->scale[2]});
        }
        return local;
    };

    std::function<void(const cgltf_node*, const Mat4f&)> visit = [&](const cgltf_node* node,
                                                                     const Mat4f& parent_world) {
        const Mat4f world = parent_world * node_local(node);
        if (node->mesh != nullptr) {
            const u32 mesh_index = static_cast<u32>(node->mesh - data->meshes);
            const cgltf_size prims = node->mesh->primitives_count;
            for (cgltf_size j = 0; j < prims; ++j) {
                const u32 mapped = prim_remap[mesh_prim_base[mesh_index] + j];
                if (mapped != UINT32_MAX) {
                    scene.instances.push_back({world, mapped});
                }
            }
        }
        for (cgltf_size c = 0; c < node->children_count; ++c) {
            visit(node->children[c], world);
        }
    };

    const cgltf_scene* gltf_scene =
        data->scene != nullptr ? data->scene
                               : (data->scenes_count > 0 ? &data->scenes[0] : nullptr);
    if (gltf_scene != nullptr) {
        for (cgltf_size i = 0; i < gltf_scene->nodes_count; ++i) {
            visit(gltf_scene->nodes[i], Mat4f::identity());
        }
    } else {
        for (cgltf_size i = 0; i < data->nodes_count; ++i) {
            if (data->nodes[i].parent == nullptr) {
                visit(&data->nodes[i], Mat4f::identity());
            }
        }
    }

    read_skins(data.get(), scene);
    read_animations(data.get(), scene);

    FK_LOG_INFO("glTF loaded: {} primitives, {} materials, {} images, {} instances, "
                "{} skeletons, {} animations",
                scene.primitives.size(), scene.materials.size(), scene.images.size(),
                scene.instances.size(), scene.skeletons.size(), scene.animations.size());
    return scene;
}

}
