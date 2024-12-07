#include <iostream>
#include <cassert>
#include <cmath>
#include "types/json.hpp"

// ide 的错误提示请忽略, json 库还在改进，在这里不影响使用

using namespace std;

const char * get_type_name(auto var) {
    return typeid(var).name();
}

void test_basic_types() {
    Json null_json;
    assert(null_json.is_null());
    Json payload;
    payload["iat"] = time(nullptr);
    string err_what = "Invalid null value";
    string s = Json().set("error", err_what).dumps();

    Json bool_json(true);
    assert(bool_json.is_bool());
    assert(bool_json == true);

    Json int_json(42);
    assert(int_json.is_number());
    assert(int_json == 42);

    Json double_json(3.14);
    assert(double_json.is_number());
    assert(abs(double_json.value<double>() - 3.14) < 1e-6);

    Json string_json("hello");
    assert(string_json.is_string());
    assert(string_json == "hello");
}

void test_array() {
    Json array_json(Json::Type::Array);
    cout<< "array_json: " << array_json<<"size: "<<array_json.size() << endl;
    array_json.append(Json());
    cout<< "array_json: " << array_json<<"size: "<<array_json.size() << endl;
    array_json.append(Json(true));
    array_json.append(Json(42));
    array_json.append(Json(3.14));
    array_json.append(Json("hello"));

    assert(array_json.is_array());
    assert(array_json.size() == 5);
    assert(array_json[0].is_null());
    assert(array_json[1] == true);
    assert(array_json[2] == 42);
    assert(abs(array_json[3].value<double>() - 3.14) < 1e-6);
    assert(array_json[4] == "hello");

    cout<< "array_json: " << array_json << endl;
    Json array_json_1 = Json::loads("[]");
    cout<< "array_json_1: " << array_json_1<< "is_arr: "<< array_json_1.is_array()<< "size: "<<  array_json_1.size() << endl;
    array_json_1.append(1);
    array_json_1.append(2);
    array_json_1[1] = 9;
    cout<< "array_json_1: v_add 前" << array_json_1 << endl;
    // cout<< "array_json_1.value<Json::ArrayType>(): " << array_json_1.value<Json::ArrayType>().dumps() << endl;
    for (const auto& array_json_1_item : array_json_1.value<Json::ArrayType>()) {
        Json kv{{"key", array_json_1_item}};
        array_json_1.append(kv);
    }
    cout<< "array_json_1: v_add 后" << array_json_1 << endl;
    auto array_json_1_v_to_arr = array_json_1.value<Json::ArrayType>();
    cout <<R"(get_type_name(array_json_1.value<Json::ArrayType>()): )"<< get_type_name(array_json_1_v_to_arr) <<endl;
    // array_json_1_v_to_arr.get("");
    // array_json_1_v_to_arr.append(Json{{"key", 1}});
}

Json object_json(Json::Type::Object);
void test_object() {
    auto fake_key_v = object_json["fake_key"];
    cout << "fake_key_v: " << fake_key_v << endl;
    object_json["null"] = Json();
    object_json["bool"] = Json(true);
    bool bol_from_json = Json(true);
    cout << "bol_from_json: " << bol_from_json << endl;
    object_json["int"] = Json(42);
    object_json["double"] = Json(3.14);
    object_json["string"] = Json("hello");
    bool bol = object_json["bool"];
    cout <<R"(object_json["bool"]): )"<< object_json["bool"] <<endl;
    cout <<R"(get_type_name(object_json["bool"]): )"<<get_type_name(object_json["bool"])<<endl;
    cout <<R"(bool bol = object_json["bool"]: )"<< get_type_name(bol) <<endl;
    assert(object_json.is_object());
    cout<< "object_json.size(): " << object_json.size() << endl;
    cout<< "object_json: " << object_json << endl;
    assert(object_json.size() == 6);
    assert(object_json["null"].is_null());
    assert(object_json["bool"] == true);

    assert(object_json["int"] == 42);
    cout << object_json["int"] << endl;
    assert(abs(object_json["double"].value<double>() - 3.14) < 1e-6);
    // assert(abs(object_json["double"] - 3.14) < 1e-6);
    cout << object_json["double"] << endl;
    assert(object_json["string"] == "hello");
    cout << "test_object" << object_json["string"] << endl;
    Json object_json_from_kv({{"key1", "value1"}, {"key2", true}});
    assert(object_json_from_kv.is_object());
    assert(object_json_from_kv.size() == 2);
    assert(object_json_from_kv["key1"] == "value1");
    assert(object_json_from_kv["key2"] == true);
    cout << object_json_from_kv["key1"] << endl;
    cout << object_json_from_kv.dumps() << endl;
}

Json nested_json(Json::Type::Object);
void test_nested_structures() {
    nested_json["array"] = Json(Json::Type::Array);
    nested_json["array"].append(Json(1));
    nested_json["array"].append(Json(2));
    nested_json["array"].append(Json(3));

    nested_json["object"] = Json(Json::Type::Object);
    nested_json["object"]["key1"] = Json("value1");
    nested_json["object"]["key2"] = Json(true);

    assert(nested_json.is_object());
    assert(nested_json["array"].is_array());
    assert(nested_json["array"].size() == 3);
    assert(nested_json["array"][0] == 1);
    assert(nested_json["array"][1] == 2);
    assert(nested_json["array"][2] == 3);

    assert(nested_json["object"].is_object());
    assert(nested_json["object"]["key1"] == "value1");
    assert(nested_json["object"]["key2"] == true);

    nested_json["object"]["key1"] = nested_json["object"];

    cout << nested_json.dumps() << endl;
}

void test_serialization_deserialization() {
    Json original(Json::Type::Object);
    original["null"] = Json();
    original["bool"] = Json(true);
    original["int"] = Json(42);
    original["double"] = Json(3.14);
    original["string"] = Json("hello");
    original["array"] = Json(Json::Type::Array);
    original["array"].append(Json(1));
    original["array"].append(Json(2));
    original["object"] = Json(Json::Type::Object);
    original["object"]["key"] = Json("value");

    string json_str = original.dumps();
    Json parsed = Json::loads(json_str);

    assert(parsed.is_object());
    assert(parsed["null"].is_null());
    assert(parsed["bool"] == true);
    assert(parsed["int"] == 42);
    assert(abs(parsed["double"].value<double>() - 3.14) < 1e-6);
    assert(parsed["string"] == "hello");
    assert(parsed["array"].is_array());
    assert(parsed["array"].size() == 2);
    assert(parsed["array"][0] == 1);
    assert(parsed["array"][1] == 2);
    assert(parsed["object"].is_object());
    assert(parsed["object"]["key"] == "value");
}

void test_get(){
    cout << "------------------------ test_get -------------------------" << endl;
    cout << "test_get:object_json: " << object_json << endl;
    optional<bool> bol_or_null = object_json.get("bool");
    cout << "test_get:bol_or_null: " << object_json.get("bool") << endl;
    // variant<bool, nullptr_t> bol_or_null_v = Json(true);
    // variant<bool, nullptr_t> bol_or_null_v = object_json.get("bool");
    // variant<bool, string> bol_or_str = object_json.get("bool", "not found bool");
    cout << "test_get:bol_or_str: " << object_json.get("bool", "not found bool") << endl;
    bool bol = object_json.get("bool", false);

    optional<string> str_or_null_1 = object_json.get("string");
    cout << "test_get:str_or_null_1" << object_json.get("string") << endl;
    // variant<string, nullptr_t> str_or_null = object_json.get("string");
    
    // variant<string, nullptr_t> str_or_null_2 = object_json.get("string", nullptr);
    // // cout << "test_object:str_or_null_2" << string(str_or_null_2) << endl;
    optional<string> str_or_null_3 = object_json.get("string", nullptr);
    // cout << "test_object:str_or_null_3" << str_or_null_3 << endl;
    string test_object_str = object_json.get("string", "");
    string test_object_str1 = object_json.get("string1", "not found string");
    cout << "test_get:test_object_str1" << test_object_str1 << endl;
    // variant<string, int> test_object_str_or_int = object_json.get("string1", 0);
    // variant<string, bool> test_object_str_or_bool = object_json.get("string1", false);
    cout << "test_get:test_object_str1" << test_object_str << endl;
}

int main() {
    test_basic_types();
    test_array();
    test_object();
    test_nested_structures();
    test_serialization_deserialization();
    test_get();

    cout << "All tests passed successfully!" << endl;
    return 0;
}