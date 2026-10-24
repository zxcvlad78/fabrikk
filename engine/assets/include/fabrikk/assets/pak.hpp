#pragma once

#include <fabrikk/core/types.hpp>

#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace fabrikk::assets {
struct PakEntry {
    std::string path;
    u64 offset = 0;
    u64 size = 0;
    u64 hash = 0;
};

std::string normalize_pak_path(std::string_view path);

u64 fnv1a64(std::span<const u8> data);

class PakWriter {
public:
    void add(std::string_view path, std::span<const u8> data);
    [[nodiscard]] std::vector<u8> build() const;
    bool write_to(std::string_view path) const;
    [[nodiscard]] usize size() const { return items_.size(); }

private:
    struct Item {
        std::string path;
        std::vector<u8> data;
    };
    std::vector<Item> items_;
};

class PakReader {
public:
    PakReader() = default;

    bool open_file(std::string_view path);
    bool open_memory(std::span<const u8> bytes);
    void close();

    [[nodiscard]] bool is_open() const { return view_.size() >= kHeaderSize; }
    [[nodiscard]] const std::vector<PakEntry>& entries() const { return entries_; }
    [[nodiscard]] const PakEntry* find(std::string_view path) const;
    [[nodiscard]] bool contains(std::string_view path) const { return find(path) != nullptr; }

    [[nodiscard]] bool read(const PakEntry& entry, std::vector<u8>& out) const;
    [[nodiscard]] bool read(std::string_view path, std::vector<u8>& out) const;

private:
    static constexpr u32 kHeaderSize = 32;

    bool parse(std::span<const u8> bytes);

    std::vector<u8> owned_;
    std::span<const u8> view_;
    std::vector<PakEntry> entries_;
};

}
