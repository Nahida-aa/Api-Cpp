#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <optional>
#include <sstream>
#include <iomanip>
#include <cstring>
#include "json.hpp"

const std::string secret_key = "your_secret_key"; // 用于签名 JWT 的密钥

std::string base64_encode(const std::string& input) {
    static const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string output;
    int val = 0, valb = -6;
    for (unsigned char c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            output.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) output.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    while (output.size() % 4) output.push_back('=');
    return output;
}

std::string base64_decode(const std::string& input) {
    static const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) T[base64_chars[i]] = i;

    std::string output;
    int val = 0, valb = -8;
    for (unsigned char c : input) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            output.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return output;
}

void sha256(const std::string& data, unsigned char hash[32]) {
    std::vector<unsigned char> data_bytes(data.begin(), data.end());
    std::fill(hash, hash + 32, 0);
    for (size_t i = 0; i < data_bytes.size(); ++i) {
        hash[i % 32] ^= data_bytes[i];
    }
}

std::string hmac_sha256(const std::string& data, const std::string& key) {
    unsigned char hash[32];
    std::vector<unsigned char> k_ipad(64, 0);
    std::vector<unsigned char> k_opad(64, 0);

    for (size_t i = 0; i < key.size(); i++) {
        k_ipad[i] = key[i] ^ 0x36;
        k_opad[i] = key[i] ^ 0x5c;
    }

    std::string inner_data(k_ipad.begin(), k_ipad.end());
    inner_data += data;

    unsigned char inner_hash[32];
    sha256(inner_data, inner_hash);

    std::string outer_data(k_opad.begin(), k_opad.end());
    outer_data += std::string(inner_hash, inner_hash + 32);

    sha256(outer_data, hash);

    std::stringstream ss;
    for (int i = 0; i < 32; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

std::string generate_token(const std::string& id_number) {
    Json header = Json::loads(R"({"alg": "HS256", "typ": "JWT"})");
    Json payload;
    payload["iss"] = "auth0";
    payload["id_number"] = id_number;
    payload["iat"] = std::time(nullptr);

    std::string header_base64 = base64_encode(header.dumps());
    std::string payload_base64 = base64_encode(payload.dumps());

    std::string signature_base = header_base64 + "." + payload_base64;
    std::string signature = hmac_sha256(signature_base, secret_key);
    // DEBUG
    std::cout << "generate_token: signature: "<<signature<<std::endl;

    return header_base64 + "." + payload_base64 + "." + signature;
}

std::optional<Json> verify_token(const std::string& token) {
    size_t pos1 = token.find('.');
    size_t pos2 = token.find('.', pos1 + 1);

    if (pos1 == std::string::npos || pos2 == std::string::npos) {
        // DEBUG
        std::cout << "Invalid token, pos1: "<<pos1<< " pos2: "<<pos2 << std::endl;
        return std::nullopt;
    }

    std::string header_base64 = token.substr(0, pos1);
    std::string payload_base64 = token.substr(pos1 + 1, pos2 - pos1 - 1);
    std::string signature = token.substr(pos2 + 1);

    std::string signature_base = header_base64 + "." + payload_base64;
    std::string expected_signature = hmac_sha256(signature_base, secret_key);

    // 去除签名中的不可见字符
    signature.erase(std::remove_if(signature.begin(), signature.end(), ::isspace), signature.end());
    expected_signature.erase(std::remove_if(expected_signature.begin(), expected_signature.end(), ::isspace), expected_signature.end());

    if (signature != expected_signature) {
        // DEBUG
        std::cout << "Invalid signature, signature: "<< std::endl;
        std::cout << signature << std::endl;
        std::cout << "expected_signature:" << std::endl;
        std::cout << expected_signature << std::endl;
        return std::nullopt;
    }

    std::string payload_json = base64_decode(payload_base64);
    Json payload = Json::loads(payload_json);

    return payload;
}