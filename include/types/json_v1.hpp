#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <cmath>
#include <optional>
using namespace std;

class Json {
public:
    enum Type { Null, Boolean, Number, String, Array, Object };

    Json() : type_(Null) {}
    Json(bool value) : type_(Boolean), bool_value_(value) {}
    Json(int value) : type_(Number), number_value_(static_cast<double>(value)) {}
    Json(double value) : type_(Number), number_value_(value) {}
    Json(const string& value) : type_(String), string_value_(value) {}
    Json(const char* value) : type_(String), string_value_(value) {}
    Json(time_t value) : type_(Number), number_value_(static_cast<double>(value)) {}
    Json(Type type) : type_(type) {
        if (type == Array) {
            array_value_ = vector<Json>();
        } else if (type == Object) {
            object_value_ = unordered_map<string, Json>();
        }
    }

    Type type() const { return type_; }

    bool is_null() const { return type_ == Null; }
    bool is_bool() const { return type_ == Boolean; }
    bool is_number() const { return type_ == Number; }
    bool is_string() const { return type_ == String; }
    bool is_array() const { return type_ == Array; }
    bool is_object() const { return type_ == Object; }

    bool as_bool() const {
        if (!is_bool()) throw runtime_error("Not a boolean");
        return bool_value_;
    }

    double as_number() const {
        if (!is_number()) throw runtime_error("Not a number");
        return number_value_;
    }

    const string& as_string() const {
        if (!is_string()) throw runtime_error("Not a string");
        return string_value_;
    }

    vector<Json>& as_array()  {
        if (!is_array()) throw runtime_error("Not an array");
        return array_value_;
    }

    const unordered_map<string, Json>& as_object() const {
        if (!is_object()) throw runtime_error("Not an object");
        return object_value_;
    }

    void append(const Json& value) {
        if (!is_array()) {
            type_ = Array;
            array_value_.clear();
        }
        array_value_.push_back(value);
    }

    Json& operator[](const string& key) {
        if (!is_object()) {
            type_ = Object;
            object_value_.clear();
        }
        return object_value_[key];
    }

    const Json& operator[](const string& key) const {
        if (!is_object()) throw runtime_error("Not an object");
        auto it = object_value_.find(key);
        if (it == object_value_.end()) throw runtime_error("Key: ["+key+"] not found");
        return it->second;
    }

    bool is_contains(const string& key) const {
        if (!is_object()) throw runtime_error("Not an object");
        return object_value_.find(key) != object_value_.end();
    }

    Json get(const string& key, const Json& default_value) const {
        if (!is_object()) throw runtime_error("Not an object");
        auto it = object_value_.find(key);
        return it != object_value_.end() ? it->second : default_value;
    }

    Json& set(const string& key, const string& value) {
        (*this)[key] = value;
        return *this;
    }

    string dumps() const {
        ostringstream oss;
        to_string_impl(oss);
        return oss.str();
    }

    static Json loads(const string& json_string) {
        size_t pos = 0;
        return parse_value(json_string, pos);
    }

private:
    Type type_;
    bool bool_value_;
    double number_value_;
    string string_value_;
    vector<Json> array_value_;
    unordered_map<string, Json> object_value_;

    void to_string_impl(ostringstream& oss) const {
        switch (type_) {
            case Null: oss << "null"; break;
            case Boolean: oss << (bool_value_ ? "true" : "false"); break;
            case Number:
                if (floor(number_value_) == number_value_) {
                    oss << static_cast<long long>(number_value_);
                } else {
                    oss << number_value_;
                }
                break;
            case String: oss << '"' << escape_string(string_value_) << '"'; break;
            case Array:
                oss << '[';
                for (size_t i = 0; i < array_value_.size(); ++i) {
                    if (i > 0) oss << ',';
                    array_value_[i].to_string_impl(oss);
                }
                oss << ']';
                break;
            case Object:
                oss << '{';
                bool first = true;
                for (const auto& pair : object_value_) {
                    if (!first) oss << ',';
                    oss << '"' << escape_string(pair.first) << "\":";
                    pair.second.to_string_impl(oss);
                    first = false;
                }
                oss << '}';
                break;
        }
    }

    static string escape_string(const string& s) {
        ostringstream oss;
        for (char c : s) {
            switch (c) {
                case '"': oss << "\\\""; break;
                case '\\': oss << "\\\\"; break;
                case '\b': oss << "\\b"; break;
                case '\f': oss << "\\f"; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;
                default:
                    if (static_cast<unsigned char>(c) < 0x20) {
                        oss << "\\u" << hex << setw(4) << setfill('0') << static_cast<int>(c);
                    } else {
                        oss << c;
                    }
            }
        }
        return oss.str();
    }

    static Json parse_value(const string& json, size_t& pos) {
        skip_whitespace(json, pos);
        if (pos >= json.length()) throw runtime_error("Unexpected end of input");

        switch (json[pos]) {
            case 'n': return parse_null(json, pos);
            case 't': case 'f': return parse_bool(json, pos);
            case '"': return parse_string(json, pos);
            case '[': return parse_array(json, pos);
            case '{': return parse_object(json, pos);
            default:
                if (json[pos] == '-' || (json[pos] >= '0' && json[pos] <= '9'))
                    return parse_number(json, pos);
                throw runtime_error("Unexpected character");
        }
    }

    static void skip_whitespace(const string& json, size_t& pos) {
        while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n' || json[pos] == '\r'))
            ++pos;
    }

    static Json parse_null(const string& json, size_t& pos) {
        if (json.substr(pos, 4) == "null") {
            pos += 4;
            return Json();
        }
        throw runtime_error("Invalid null value");
    }

    static Json parse_bool(const string& json, size_t& pos) {
        if (json.substr(pos, 4) == "true") {
            pos += 4;
            return Json(true);
        }
        if (json.substr(pos, 5) == "false") {
            pos += 5;
            return Json(false);
        }
        throw runtime_error("Invalid boolean value");
    }

    static Json parse_number(const string& json, size_t& pos) {
        size_t start = pos;
        while (pos < json.length() && (json[pos] == '-' || json[pos] == '+' || json[pos] == '.' || (json[pos] >= '0' && json[pos] <= '9') || json[pos] == 'e' || json[pos] == 'E'))
            ++pos;
        string num_str = json.substr(start, pos - start);
        return Json(stod(num_str));
    }

    static Json parse_string(const string& json, size_t& pos) {
        string result;
        ++pos; // Skip opening quote
        while (pos < json.length() && json[pos] != '"') {
            if (json[pos] == '\\') {
                ++pos;
                if (pos >= json.length()) throw runtime_error("Unexpected end of input in string");
                switch (json[pos]) {
                    case '"': result += '"'; break;
                    case '\\': result += '\\'; break;
                    case '/': result += '/'; break;
                    case 'b': result += '\b'; break;
                    case 'f': result += '\f'; break;
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    case 'u': {
                        if (pos + 4 >= json.length()) throw runtime_error("Unexpected end of input in unicode escape");
                        string hex = json.substr(pos + 1, 4);
                        result += static_cast<char>(stoi(hex, nullptr, 16));
                        pos += 4;
                        break;
                    }
                    default: throw runtime_error("Invalid escape sequence");
                }
            } else {
                result += json[pos];
            }
            ++pos;
        }
        if (pos >= json.length()) throw runtime_error("Unterminated string");
        ++pos; // Skip closing quote
        return Json(result);
    }

    static Json parse_array(const string& json, size_t& pos) {
        Json arr(Array);
        ++pos; // Skip opening bracket
        skip_whitespace(json, pos);
        if (json[pos] == ']') {
            ++pos;
            return arr;
        }
        while (true) {
            arr.append(parse_value(json, pos));
            skip_whitespace(json, pos);
            if (json[pos] == ']') {
                ++pos;
                return arr;
            }
            if (json[pos] != ',') throw runtime_error("Expected ',' or ']' in array");
            ++pos;
            skip_whitespace(json, pos);
        }
    }

    static Json parse_object(const string& json, size_t& pos) {
        Json obj(Object);
        ++pos; // Skip opening brace
        skip_whitespace(json, pos);
        if (json[pos] == '}') {
            ++pos;
            return obj;
        }
        while (true) {
            Json key = parse_string(json, pos);
            skip_whitespace(json, pos);
            if (json[pos] != ':') throw runtime_error("Expected ':' in object");
            ++pos;
            skip_whitespace(json, pos);
            obj[key.as_string()] = parse_value(json, pos);
            skip_whitespace(json, pos);
            if (json[pos] == '}') {
                ++pos;
                return obj;
            }
            if (json[pos] != ',') throw runtime_error("Expected ',' or '}' in object");
            ++pos;
            skip_whitespace(json, pos);
        }
    }
};