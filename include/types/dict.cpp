#include <iostream>
#include <unordered_map>
#include <string>
#include <any>
#include <stdexcept>
#include <memory>
#include <sstream>
#include <typeinfo>
#include <variant>
#include <optional>
#include "dict.hpp"


void dict_test() {
    // auto dict = std::make_shared<PyDict>();
    auto dict = PyDict();
    dict.set("key1", 42);
    dict.set(2, "value2");
    dict.set("string", std::string("string"));
    dict.set("string_view", std::string_view("string_view"));
    std::variant<int, std::nullptr_t> k_int_v = dict.get("key1");
    std::optional<std::string> k_string_v = dict.get("string");

    PyDict nested_dict;
    nested_dict.set("nested_key1", 3.14);
    dict.set("nested_dict", nested_dict);
    
    std::cout << dict.get("string") << std::endl; // 输出: 42
    std::cout << dict.get("key1") << std::endl; // 输出: 42
    const char* key2_v = dict.get(2);
    std::cout << "typeid(decltype(key2_v)).name(): " << typeid(decltype(key2_v)).name() << std::endl;
    std::cout << key2_v << std::endl; // 输出: value2
    auto string_view_v = dict.get("string_view");
    std::cout << "typeid(decltype(string_view_v)).name(): " << typeid(decltype(string_view_v)).name() << std::endl;
    std::cout << R"(typeid(decltype.name(R_str): )" << typeid(decltype(R"(R)")).name() << std::endl;
    std::cout << "string_view_v.type().name(): " << string_view_v.type().name() << std::endl;
    std::cout << string_view_v << std::endl; // 输出: value2

    PyDict retrieved_nested_dict = dict.get("nested_dict");
    std::cout << retrieved_nested_dict << std::endl; // 输出: 3.14
    PyDict retrieved_nested_dict2 = nested_dict.get("nested_key1");
    std::cout << retrieved_nested_dict2 << std::endl; // 输出: 3.14
    // std::cout << retrieved_nested_dict.get("nested_key1") << std::endl; // 输出: 3.14
    // std::cout << 3.14 << std::endl; // 输出: 3.14

    std::cout << dict << std::endl;
}

int main(){
    dict_test();
}