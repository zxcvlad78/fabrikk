#pragma once
#include <string>

namespace fabrikk::FileSystem {
    std::string get_config_dir();
    std::string get_full_path(const std::string& path);
    
    bool read_file(const std::string& path, std::string& out);
    bool write_file(const std::string& path, const std::string& text);

}