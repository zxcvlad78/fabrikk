#include <fabrikk/assets/pak.hpp>

#include <cstring>
#include <fstream>

namespace fabrikk::assets {
namespace {

constexpr u32 kMagic = 0x4B504B46u; // 'F','K','P','K'
constexpr u32 kVersion = 1;
constexpr u64 kAlignment = 16;
constexpr u32 kMaxPathSize = 4096;
constexpr usize kHeaderSize = 32;

u64 align_up(u64 value) {
    return (value + kAlignment - 1) & ~(kAlignment - 1);
}

template <typename T>
void write_pod(std::vector<u8>& out, const T& value) {
    const usize at = out.size();
    out.resize(at + sizeof(T));
    std::memcpy(out.data() + at, &value, sizeof(T));
}

template <typename T>
bool read_pod(std::span<const u8> in, usize& at, T& value) {
    if (at + sizeof(T) > in.size()) {
        return false;
    }
    std::memcpy(&value, in.data() + at, sizeof(T));
    at += sizeof(T);
    return true;
}

}

std::string normalize_pak_path(std::string_view path) {
    std::string out;
    out.reserve(path.size());
    for (char raw : path) {
        char c = raw;
        if (c == '\\') {
            c = '/';
        }
        if (c >= 'A' && c <= 'Z') {
            c = static_cast<char>(c - 'A' + 'a');
        }
        if (c == '/' && !out.empty() && out.back() == '/') {
            continue;
        }
        out.push_back(c);
    }
    while (out.starts_with("./")) {
        out.erase(0, 2);
    }
    while (!out.empty() && out.front() == '/') {
        out.erase(0, 1);
    }
    return out;
}

u64 fnv1a64(std::span<const u8> data) {
    u64 hash = 14695981039346656037ull;
    for (const u8 byte : data) {
        hash ^= byte;
        hash *= 1099511628211ull;
    }
    return hash;
}

void PakWriter::add(std::string_view path, std::span<const u8> data) {
    const std::string normalized = normalize_pak_path(path);
    for (const Item& item : items_) {
        if (item.path == normalized) {
            return;
        }
    }
    items_.push_back(Item{normalized, std::vector<u8>(data.begin(), data.end())});
}

std::vector<u8> PakWriter::build() const {
    std::vector<u8> out;
    out.resize(kHeaderSize, 0);

    struct Placed {
        u64 offset;
        u64 size;
        u64 hash;
    };
    std::vector<Placed> placed;
    placed.reserve(items_.size());

    for (const Item& item : items_) {
        const u64 offset = align_up(out.size());
        out.resize(offset, 0);
        out.insert(out.end(), item.data.begin(), item.data.end());
        placed.push_back(
            {offset, static_cast<u64>(item.data.size()), fnv1a64(item.data)});
    }

    const u64 index_offset = align_up(out.size());
    out.resize(index_offset, 0);
    const usize index_start = out.size();
    for (usize i = 0; i < items_.size(); ++i) {
        const Item& item = items_[i];
        write_pod<u32>(out, static_cast<u32>(item.path.size()));
        out.insert(out.end(), item.path.begin(), item.path.end());
        write_pod<u64>(out, placed[i].offset);
        write_pod<u64>(out, placed[i].size);
        write_pod<u64>(out, placed[i].hash);
    }
    const u64 index_size = static_cast<u64>(out.size() - index_start);

    std::vector<u8> header;
    header.reserve(kHeaderSize);
    write_pod<u32>(header, kMagic);
    write_pod<u32>(header, kVersion);
    write_pod<u32>(header, static_cast<u32>(items_.size()));
    write_pod<u32>(header, 0u);
    write_pod<u64>(header, index_offset);
    write_pod<u64>(header, index_size);
    std::memcpy(out.data(), header.data(), header.size());
    return out;
}

bool PakWriter::write_to(std::string_view path) const {
    const std::vector<u8> bytes = build();
    std::ofstream file(std::string(path), std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        return false;
    }
    file.write(reinterpret_cast<const char*>(bytes.data()),
               static_cast<std::streamsize>(bytes.size()));
    return file.good();
}

void PakReader::close() {
    owned_.clear();
    view_ = {};
    entries_.clear();
}

bool PakReader::open_file(std::string_view path) {
    close();
    std::ifstream file(std::string(path), std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return false;
    }
    const std::streamsize size = file.tellg();
    if (size < 0) {
        return false;
    }
    file.seekg(0);
    owned_.resize(static_cast<usize>(size));
    if (size > 0) {
        file.read(reinterpret_cast<char*>(owned_.data()), size);
        if (!file.good()) {
            close();
            return false;
        }
    }
    if (!parse(owned_)) {
        close();
        return false;
    }
    return true;
}

bool PakReader::open_memory(std::span<const u8> bytes) {
    close();
    if (!parse(bytes)) {
        close();
        return false;
    }
    return true;
}

bool PakReader::parse(std::span<const u8> bytes) {
    if (bytes.size() < kHeaderSize) {
        return false;
    }
    usize at = 0;
    u32 magic = 0;
    u32 version = 0;
    u32 entry_count = 0;
    u32 flags = 0;
    u64 index_offset = 0;
    u64 index_size = 0;
    const bool header_ok = read_pod(bytes, at, magic) && read_pod(bytes, at, version) &&
                           read_pod(bytes, at, entry_count) && read_pod(bytes, at, flags) &&
                           read_pod(bytes, at, index_offset) &&
                           read_pod(bytes, at, index_size);
    if (!header_ok || magic != kMagic || version != kVersion ||
        index_offset < kHeaderSize || index_offset + index_size > bytes.size() ||
        index_offset + index_size < index_offset) {
        return false;
    }

    const std::span<const u8> index =
        bytes.subspan(static_cast<usize>(index_offset), static_cast<usize>(index_size));
    usize idx = 0;
    std::vector<PakEntry> entries;
    entries.reserve(entry_count);
    for (u32 i = 0; i < entry_count; ++i) {
        u32 path_size = 0;
        u64 offset = 0;
        u64 size = 0;
        u64 hash = 0;
        if (!read_pod(index, idx, path_size) || path_size == 0 ||
            path_size > kMaxPathSize || idx + path_size + 24 > index.size()) {
            return false;
        }
        PakEntry entry;
        entry.path.assign(reinterpret_cast<const char*>(index.data() + idx), path_size);
        idx += path_size;
        if (!read_pod(index, idx, offset) || !read_pod(index, idx, size) ||
            !read_pod(index, idx, hash) || offset < kHeaderSize ||
            offset + size < offset || offset + size > bytes.size() ||
            offset + size > index_offset) {
            return false;
        }
        entry.offset = offset;
        entry.size = size;
        entry.hash = hash;
        entries.push_back(std::move(entry));
    }

    entries_ = std::move(entries);
    view_ = bytes;
    return true;
}

const PakEntry* PakReader::find(std::string_view path) const {
    const std::string normalized = normalize_pak_path(path);
    for (const PakEntry& entry : entries_) {
        if (entry.path == normalized) {
            return &entry;
        }
    }
    return nullptr;
}

bool PakReader::read(const PakEntry& entry, std::vector<u8>& out) const {
    if (view_.empty() || entry.offset > view_.size() ||
        entry.size > view_.size() - entry.offset) {
        return false;
    }
    out.resize(static_cast<usize>(entry.size));
    if (entry.size > 0) {
        std::memcpy(out.data(), view_.data() + entry.offset, static_cast<usize>(entry.size));
    }
    return fnv1a64(out) == entry.hash;
}

bool PakReader::read(std::string_view path, std::vector<u8>& out) const {
    const PakEntry* entry = find(path);
    return entry != nullptr && read(*entry, out);
}

}
