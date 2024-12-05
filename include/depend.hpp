#pragma once
#include "request.hpp"
#include <string>
#include "jwt.hpp"

// 辅助函数，用于获取和验证 Authorization 头部
std::string get_authorization(const Request& request) {
    std::string token;
    auto it = request.headers.find("Authorization");
    if (it != request.headers.end()) {
        token = it->second;
    } else {
        token = "";
    }
    std::cout << "Authorization: " << token << std::endl;
    auto decoded = verify_token(token);
    if (!decoded) {
        throw std::runtime_error("无效的令牌");
    }
    return decoded->get("id_number", "null").as_string();
}