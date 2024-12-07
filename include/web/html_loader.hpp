#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

class HtmlLoader {
public:
    static std::string load(const std::string& file_path) {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            throw std::runtime_error("无法打开文件: " + file_path);
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
};