#pragma once
#include <iostream>
#include <vector>
#include <functional>
#include <regex>

class Request {
public:
    std::string raw_request; // 原始请求字符串
    std::string method;
    std::string path;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
    std::unordered_map<std::string, std::string> path_params;

    Request(const std::string& request_str) : raw_request(request_str) {
        std::istringstream stream(request_str);
        std::string line;
        std::getline(stream, line);
        std::istringstream line_stream(line);
        line_stream >> method >> path;

        while (std::getline(stream, line) && line != "\r") {
            auto colon_pos = line.find(':');
            if (colon_pos != std::string::npos) {
                std::string key = line.substr(0, colon_pos);
                std::string value = line.substr(colon_pos + 2); // Skip ": "
                headers[key] = value;
            }
        }

        body = std::string(std::istreambuf_iterator<char>(stream), {});
    }

    void set_path_params(const std::vector<std::string>& param_names, const std::vector<std::string>& param_values) {
        for (size_t i = 0; i < param_names.size(); ++i) {
            path_params[param_names[i]] = param_values[i];
        }
    }
};
