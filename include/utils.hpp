#pragma once
#include <string>
#include <iostream>
#include <filesystem>

inline void ensure_directory_exists(const std::string& path) {
    std::filesystem::path dir(path);
    if (!std::filesystem::exists(dir)) {
        std::filesystem::create_directories(dir);
        std::cout << "Created directory: " << path << std::endl;
    }
}