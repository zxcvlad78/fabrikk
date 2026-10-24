// #pragma once

// #include <fabrikk/core/types.hpp>

// #include <filesystem>
// #include <string>
// #include <string_view>
// #include <vector>

// namespace fabrikk::assets {

// enum class WatchEventKind {
//     modified,
//     removed,
// };

// struct WatchEvent {
//     WatchEventKind kind = WatchEventKind::modified;
//     std::string path;
// };

// class FileWatcher {
// public:
//     void watch(std::string_view path);
//     void unwatch(std::string_view path);
//     void clear();

//     [[nodiscard]] std::vector<WatchEvent> poll();
//     [[nodiscard]] usize watched_count() const { return watched_.size(); }

// private:
//     struct Watched {
//         std::string path;
//         std::filesystem::file_time_type mtime{};
//         u64 size = 0;
//         bool exists = false;
//     };

//     std::vector<Watched> watched_;
// };

// }
