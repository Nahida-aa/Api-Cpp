#include <iostream>
#include <cassert>
#include "../include/json.hpp"

void test_json() {
    // 测试布尔值
    Json json_bool(true);
    assert(json_bool.is_bool());
    assert(json_bool.as_bool() == true);

    // 测试整数
    Json json_int(42);
    assert(json_int.is_number());
    assert(json_int.as_number() == 42);

    // 测试浮点数
    Json json_double(3.14);
    assert(json_double.is_number());
    assert(json_double.as_number() == 3.14);

    // 测试字符串
    Json json_string("hello");
    assert(json_string.is_string());
    assert(json_string.as_string() == "hello");

    // 测试数组
    Json json_array(Json::Array);
    json_array.append(json_bool);
    json_array.append(json_int);
    json_array.append(json_double);
    json_array.append(json_string);
    assert(json_array.is_array());
    assert(json_array.as_array().size() == 4);
    assert(json_array.as_array()[0].as_bool() == true);
    assert(json_array.as_array()[1].as_number() == 42);
    assert(json_array.as_array()[2].as_number() == 3.14);
    assert(json_array.as_array()[3].as_string() == "hello");

    // 测试对象
    Json json_object(Json::Object);
    json_object["bool"] = json_bool;
    json_object["int"] = json_int;
    json_object["double"] = json_double;
    json_object["double"] = -3.14;
    json_object["string"] = json_string;
    assert(json_object.is_object());
    assert(json_object.as_object().size() == 4);
    assert(json_object["bool"].as_bool() == true);
    assert(json_object["int"].as_number() == 42);
    assert(json_object["double"].as_number() == -3.14);
    assert(json_object["string"].as_string() == "hello");

    // 测试嵌套
    json_object["array"] = json_array;
    json_object["object"] = json_object;
    std::cout << json_object.dumps() << std::endl;
    std::cout << json_object["object"].dumps() << std::endl;
    std::cout << json_object["object"]["bool"].dumps() << std::endl;

    // 测试序列化
    std::string json_str = json_object.dumps();
    std::cout << "Serialized JSON: " << json_str << std::endl;

    // 测试反序列化
    Json parsed_json = Json::loads(json_str);
    assert(parsed_json.is_object());
    assert(parsed_json["bool"].as_bool() == true);
    assert(parsed_json["int"].as_number() == 42);
    assert(parsed_json["double"].as_number() == -3.14);
    assert(parsed_json["string"].as_string() == "hello");

    std::cout << "All tests passed!" << std::endl;
}

int main() {
    test_json();
    return 0;
}