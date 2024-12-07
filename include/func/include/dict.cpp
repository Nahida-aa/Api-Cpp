// #pragma once
#include <iostream>
#include <unordered_map>
#include <string>
#include <variant>
#include <stdexcept>

class PyDict : public std::unordered_map<std::variant<std::string, int>, std::variant<int, double, std::string>> { // 公用继承
public:
    using KeyType = std::variant<std::string, int>;
    using ValueType = std::variant<int, double, std::string>;
    using Base = std::unordered_map<KeyType, ValueType>;

    // 重载 get 方法
    ValueType get(const KeyType& key) const {
        auto it = this->find(key);
        if (it != this->end()) {
            return it->second;
        }
        return ValueType{};
    }

    ValueType get(const KeyType& key, const ValueType& default_value) const {
        auto it = this->find(key);
        if (it != this->end()) {
            return it->second;
        }
        return default_value;
    }

    template <typename T>
    std::variant<ValueType, T> get(const KeyType& key, const T& default_value) const {
        auto it = this->find(key);
        if (it != this->end()) {
            return it->second;
        }
        return default_value;
    }

    // 使用 this 指针访问自身
    void printData() const {
        for (const auto& pair : *this) {
            std::visit([](auto&& arg) { std::cout << arg << std::endl; }, pair.second);
        }
    }
};

int main() {
    PyDict dict;
    dict["key1"] = 42;
    // dict[2] = "value2";

    auto key1_value = dict.get("key1");
    auto key1_value2 = dict["key1"];
    std::cout << std::get<int>(key1_value) << std::endl; // 输出: 42
    // auto key1_value_1 = dict.get("key1", 0);
    // 使用 decltype 获取 key1_value_1 的类型
    using Key1ValueType = decltype(key1_value);
    // 打印类型信息
    if constexpr (std::is_same_v<Key1ValueType, int>) { //用于在编译时检查两个类型是否相同。它不能用于检查 std::variant 中存储的实际类型
        std::cout << "key1_value_1 is of type int" << std::endl;
    } else if constexpr (std::is_same_v<Key1ValueType, double>) {
        std::cout << "key1_value_1 is of type double" << std::endl;
    } else if constexpr (std::is_same_v<Key1ValueType, std::string>) {
        std::cout << "key1_value_1 is of type std::string" << std::endl;
    } else if constexpr (std::is_same_v<Key1ValueType, PyDict::ValueType>){
        std::cout << "key1_value_1 is of type PyDict::ValueType" << std::endl;
    }else {
        std::cout << "key1_value_1 is of unknown type" << std::endl;
    }
    if (std::holds_alternative<int>(key1_value)) { // 用于在运行时检查 std::variant 中存储的实际类型
        std::cout << "key1_value_1 is of type int" << std::endl;
    } else if (std::holds_alternative<double>(key1_value)) {
        std::cout << "key1_value_1 is of type double" << std::endl;
    } else if (std::holds_alternative<std::string>(key1_value)) {
        std::cout << "key1_value_1 is of type std::string" << std::endl;
    } else {
        std::cout << "key1_value_1 is of unknown type" << std::endl;
    }
    // std::cout << std::get<int>(key1_value_1) << std::endl; // 输出: 42
    // int key1_value_0 = dict.at("key1");
    // int key1_value_0 = dict.get("key1");
    // int key1_value_0 = dict_auto.at("key1");
    // auto key2_value = dict.get("key2", 0);
    // auto key3_value = dict.get(2, "default");
    // auto key4_value = dict.get("key3", "default");
    // std::cout << std::get<int>(dict.get(std::string("key1"), 0)) << std::endl; // 输出: 42
    // std::cout << std::get<int>(dict.get("key2", 0)) << std::endl; // 输出: 0    
    // // std::cout << std::get<int>(dict.get("key2", 0)) << std::endl; // 输出: 42
    // std::cout << std::get<std::string>(dict.get(2, "default")) << std::endl; // 输出: value2
    // std::cout << std::get<std::string>(dict.get("key3", "default")) << std::endl; // 输出: default

    return 0;
}