#include <fabrikk/assets/FileWatcher.hpp>

namespace fabrikk::assets {
namespace {
bool probe(const std::string& path, std::filesystem::file_time_type& mtime, u64& size) {
    std::error_code ec;
    const auto time = std::filesystem::last_write_time(path, ec);
    if (ec) {
        return false;
    }
    const auto bytes = std::filesystem::file_size(path, ec);
    if (ec) {
        return false;
    }
    mtime = time;
    size = static_cast<u64>(bytes);
    return true;
}
}

void FileWatcher::watch(std::string_view path) {
    const std::string owned(path);
    for (const Watched& entry : watched_) {
        if (entry.path == owned) {
            return;
        }
    }
    Watched entry;
    entry.path = owned;
    entry.exists = probe(entry.path, entry.mtime, entry.size);
    watched_.push_back(std::move(entry));
}

void FileWatcher::unwatch(std::string_view path) {
    const std::string owned(path);
    for (usize i = 0; i < watched_.size(); ++i) {
        if (watched_[i].path == owned) {
            watched_.erase(watched_.begin() + static_cast<std::ptrdiff_t>(i));
            return;
        }
    }
}

void FileWatcher::clear() {
    watched_.clear();
}

std::vector<WatchEvent> FileWatcher::poll() {
    std::vector<WatchEvent> events;
    for (Watched& entry : watched_) {
        std::filesystem::file_time_type mtime{};
        u64 size = 0;
        const bool exists = probe(entry.path, mtime, size);

        if (!exists && entry.exists) {
            events.push_back({WatchEventKind::removed, entry.path});
        } else if (exists && (!entry.exists || mtime != entry.mtime || size != entry.size)) {
            events.push_back({WatchEventKind::modified, entry.path});
        }
        entry.exists = exists;
        if (exists) {
            entry.mtime = mtime;
            entry.size = size;
        }
    }
    return events;
}

}
