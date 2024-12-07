#pragma once
#include <string>
#include <sstream>
#include <map>
#include <unordered_map>
#include <vector>
#include <any>
#include <variant>
// 通用的模板函数，将任意类型转换为字符串
template <typename T>
std::string to_string(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

// 特化版本，用于处理布尔类型
template <>
std::string to_string<bool>(const bool& value) {
    return value ? "true" : "false";
}

// 特化版本，用于处理字符类型
template <>
std::string to_string<char>(const char& value) {
    return std::string(1, value);
}

// 特化版本，用于处理 C 风格字符串
template <>
std::string to_string<const char*>(const char* const& value) {
    return std::string(value);
}

// 特化版本，用于处理 std::string
template <>
std::string to_string<std::string>(const std::string& value) {
    return value;
}

// 特化版本，用于处理 std::map
template <typename K, typename V>
std::string to_string(const std::map<K, V>& m) {
    std::ostringstream oss;
    oss << "{";
    for (auto it = m.begin(); it != m.end(); ++it) {
        if (it != m.begin()) {
            oss << ", ";
        }
        oss << to_string(it->first) << ": " << to_string(it->second);
    }
    oss << "}";
    return oss.str();
}

// 特化版本，用于处理 std::unordered_map
template <typename K, typename V>
std::string to_string(const std::unordered_map<K, V>& m) {
    std::ostringstream oss;
    oss << "{";
    for (auto it = m.begin(); it != m.end(); ++it) {
        if (it != m.begin()) {
            oss << ", ";
        }
        oss << to_string(it->first) << ": " << to_string(it->second);
    }
    oss << "}";
    return oss.str();
}

// 特化版本，用于处理 std::vector
template <typename T>
std::string to_string(const std::vector<T>& v) {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        if (i > 0) {
            oss << ", ";
        }
        oss << to_string(v[i]);
    }
    oss << "]";
    return oss.str();
}

// 特化版本，用于处理 std::variant
template <typename... Types>
std::string to_string(const std::variant<Types...>& value) {
    return std::visit([](const auto& v) { return to_string(v); }, value);
}

// 特化版本，用于处理 std::any
std::string to_string(const std::any& value) {
    if (value.type() == typeid(int)) {
        return to_string(std::any_cast<int>(value));
    } else if (value.type() == typeid(double)) {
        return to_string(std::any_cast<double>(value));
    } else if (value.type() == typeid(bool)) {
        return to_string(std::any_cast<bool>(value));
    } else if (value.type() == typeid(char)) {
        return to_string(std::any_cast<char>(value));
    } else if (value.type() == typeid(const char*)) {
        return to_string(std::any_cast<const char*>(value));
    } else if (value.type() == typeid(std::string)) {
        return to_string(std::any_cast<std::string>(value));
    } else if (value.type() == typeid(std::nullptr_t)) {
        return "nullptr";
    } else {
        return "Unsupported type";
    }
}