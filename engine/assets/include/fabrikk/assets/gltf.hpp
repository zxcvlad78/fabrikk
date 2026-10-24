// #pragma once

// #include <glm/glm.h>
// #include <fabrikk/core/types.hpp>

// #include <array>
// #include <span>
// #include <string>
// #include <string_view>
// #include <vector>

// namespace fabrikk::assets {
// struct GltfImage {
//     std::string name;
//     std::string uri;
//     u32 width = 0;
//     u32 height = 0;
//     std::vector<u8> rgba;
// };

// struct GltfMaterial {
//     std::string name;
//     Vec4f base_color{1.0f, 1.0f, 1.0f, 1.0f};
//     Vec3f emissive{0.0f, 0.0f, 0.0f};
//     f32 metallic = 1.0f;
//     f32 roughness = 1.0f;
//     i32 base_color_image = -1;
//     i32 normal_image = -1;
// };

// struct GltfSkinning {
//     std::vector<std::array<u16, 4>> joints;
//     std::vector<Vec4f> weights;

//     [[nodiscard]] bool empty() const { return joints.empty(); }
// };

// struct GltfPrimitive {
//     render::MeshData mesh;
//     u32 material = 0;
//     GltfSkinning skin;
// };

// struct GltfInstance {
//     Mat4f transform;
//     u32 primitive = 0;
// };

// struct GltfSkeleton {
//     std::string name;
//     std::vector<std::string> joint_names;
//     std::vector<i32> parents;
//     std::vector<u32> joint_nodes;
//     std::vector<Vec3f> bind_positions;
//     std::vector<Vec3f> bind_rotations;
//     std::vector<Vec3f> bind_scales;
//     std::vector<Mat4f> inverse_bind;
// };

// struct GltfAnimChannel {
//     enum class Path { Translation, Rotation, Scale };

//     u32 node = 0;
//     Path path = Path::Translation;
//     std::vector<f32> times;
//     std::vector<Vec3f> values;
//     std::vector<Vec4f> rotations;
// };

// struct GltfAnimation {
//     std::string name;
//     std::vector<GltfAnimChannel> channels;

//     [[nodiscard]] f32 duration() const;
// };

// struct GltfScene {
//     std::vector<GltfPrimitive> primitives;
//     std::vector<GltfMaterial> materials;
//     std::vector<GltfImage> images;
//     std::vector<GltfInstance> instances;
//     std::vector<GltfSkeleton> skeletons;
//     std::vector<GltfAnimation> animations;

//     static GltfScene load(std::string_view path);
//     static GltfScene parse(std::span<const u8> bytes, std::string_view base_dir = {});
// };

// } // namespace fabrikk::assets
