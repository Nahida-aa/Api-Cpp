#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <cmath>
#include <variant>
#include <concepts>
#include <string_view>
#include <optional>

class Json {
public:
    enum class Type { Null, Boolean, Number, String, Array, Object };

    using ArrayType = std::vector<Json>;
    using ObjectType = std::unordered_map<std::string, Json>;

private:
    Type type_;
    std::variant<std::nullptr_t, bool, double, std::string, ArrayType, ObjectType> value_;

public:
    Json() : type_(Type::Null), value_(nullptr) {}
    Json(std::nullptr_t) : type_(Type::Null), value_(nullptr) {}
    Json(bool value) : type_(Type::Boolean), value_(value) {}
    Json(int value) : type_(Type::Number), value_(static_cast<double>(value)) {}
    Json(long value) : type_(Type::Number), value_(static_cast<double>(value)) {}
    Json(long long value) : type_(Type::Number), value_(static_cast<double>(value)) {}
    Json(unsigned value) : type_(Type::Number), value_(static_cast<double>(value)) {}
    Json(unsigned long value) : type_(Type::Number), value_(static_cast<double>(value)) {}
    Json(unsigned long long value) : type_(Type::Number), value_(static_cast<double>(value)) {}
    Json(double value) : type_(Type::Number), value_(value) {}
    Json(const std::string& value) : type_(Type::String), value_(value) {}
    Json(std::string_view value) : type_(Type::String), value_(std::string(value)) {}
    Json(const char* value) : type_(Type::String), value_(std::string(value)) {}
    Json(Type type) : type_(type) {
        switch (type) {
            case Type::Null: value_ = nullptr; break;
            case Type::Boolean: value_ = false; break;
            case Type::Number: value_ = 0.0; break;
            case Type::String: value_ = std::string(); break;
            case Type::Array: value_ = ArrayType(); break;
            case Type::Object: value_ = ObjectType(); break;
        }
    }
    Json(std::initializer_list<std::pair<const std::string, Json>> init) : type_(Type::Object), value_(ObjectType(init)) {}

    Type type() const { return type_; }
    bool is_null() const { return type_ == Type::Null; }
    bool is_bool() const { return type_ == Type::Boolean; }
    bool is_number() const { return type_ == Type::Number; }
    bool is_string() const { return type_ == Type::String; }
    bool is_array() const { return type_ == Type::Array; }
    bool is_object() const { return type_ == Type::Object; }

    size_t size() const {
        if (is_array()) return std::get<ArrayType>(value_).size();
        else if (is_object()) return std::get<ObjectType>(value_).size();
        return 1;
    }

    template<typename T>
    T value() const {
        return std::get<T>(value_);
    }

    template<typename T>
    Json get(const std::string& key, const T& default_value) const {
        if (is_object() && contains(key)) {
            return std::get<ObjectType>(value_).at(key);
        }
        return default_value;
    }
    Json get(const std::string& key) const {
        if (is_object() && contains(key)) {
            return std::get<ObjectType>(value_).at(key);
        }
        return Json(nullptr);
    }

    bool contains(const std::string& key) const {
        return is_object() && std::get<ObjectType>(value_).count(key) > 0;
    }
    // bool contains(const std::string& key) const {
    //     if (is_object()) {
    //         const auto& obj = std::get<ObjectType>(value_);
    //         return obj.find(key) != obj.end();
    //     }
    //     return false;
    // }

    template<typename T>
    Json& operator[](T&& key) {
        // std::cout << "Json& operator[](T&& key):key: " << key << std::endl;
        if constexpr (std::is_convertible_v<T, std::string_view>) {
            if (!is_object()) value_ = ObjectType();
            return std::get<ObjectType>(value_)[std::string(std::forward<T>(key))];
        } else if constexpr (std::is_integral_v<std::remove_cvref_t<T>>) {
            if (!is_array()) throw std::runtime_error("Not an array");
            return std::get<ArrayType>(value_)[std::forward<T>(key)];
        } else {
            static_assert(always_false<T>, "Unsupported key type");
        }
    }
    template<typename T>
    const Json& operator[](T&& key) const {
        // std::cout << "const Json& operator[](T&& key):key: " << key << std::endl;
        if constexpr (std::is_convertible_v<T, std::string_view>) {
            return std::get<ObjectType>(value_).at(std::string(std::forward<T>(key)));
        } else if constexpr (std::is_integral_v<std::remove_cvref_t<T>>) {
            return std::get<ArrayType>(value_).at(std::forward<T>(key));
        } else {
            static_assert(always_false<T>, "Unsupported key type");
        }
    }
    template<typename T> // operator[]
    operator T() const { 
        // std::cout << "operator T() const" << std::endl;
        return value<T>(); }
    Json& set(const std::string& key, const Json& value) {
        if (!is_object()) value_ = ObjectType();
        std::get<ObjectType>(value_)[key] = value;
        return *this;
    }
    void append(const Json& value) {
        if (!is_array()) {
            if (size() == 0) {
                value_ = ArrayType();
            }else{
                throw std::runtime_error("Not an array");
            }
        }
        std::get<ArrayType>(value_).push_back(value);
    }

    ArrayType::iterator begin() {
        if (!is_array()) {
            throw std::runtime_error("Not an array");
        }
        return std::get<ArrayType>(value_).begin();
    }

    ArrayType::iterator end() {
        if (!is_array()) {
            throw std::runtime_error("Not an array");
        }
        return std::get<ArrayType>(value_).end();
    }

    ArrayType::const_iterator begin() const {
        if (!is_array()) {
            throw std::runtime_error("Not an array");
        }
        return std::get<ArrayType>(value_).begin();
    }

    ArrayType::const_iterator end() const {
        if (!is_array()) {
            throw std::runtime_error("Not an array");
        }
        return std::get<ArrayType>(value_).end();
    }

    bool operator==(const Json& other) const { return value_ == other.value_; }
    bool operator!=(const Json& other) const { return !(*this == other); }

    std::string dumps() const {
        std::ostringstream oss;
        to_string_impl(oss);
        return oss.str();
    }

    static Json loads(std::string_view json_string) {
        size_t pos = 0;
        return parse_value(json_string, pos);
    }

    friend std::ostream& operator<<(std::ostream& os, const Json& json) {
        os << json.dumps();
        return os;
    }

    // Implement arithmetic operators
    Json operator+(const Json& other) const {
        if (is_number() && other.is_number()) {
            return Json(value<double>() + other.value<double>());
        }
        throw std::runtime_error("Cannot add non-numeric JSON values");
    }

    Json operator-(const Json& other) const {
        if (is_number() && other.is_number()) {
            return Json(value<double>() - other.value<double>());
        }
        throw std::runtime_error("Cannot subtract non-numeric JSON values");
    }

    Json operator*(const Json& other) const {
        if (is_number() && other.is_number()) {
            return Json(value<double>() * other.value<double>());
        }
        throw std::runtime_error("Cannot multiply non-numeric JSON values");
    }

    Json operator/(const Json& other) const {
        if (is_number() && other.is_number()) {
            if (other.value<double>() == 0) {
                throw std::runtime_error("Division by zero");
            }
            return Json(value<double>() / other.value<double>());
        }
        throw std::runtime_error("Cannot divide non-numeric JSON values");
    }
private:
    template<typename T>
    static constexpr bool always_false = false;

    void to_string_impl(std::ostringstream& oss) const {
        std::visit([&](const auto& v) {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                oss << "null";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (v ? "true" : "false");
            } else if constexpr (std::is_same_v<T, double>) {
                oss << v;
            } else if constexpr (std::is_same_v<T, std::string>) {
                oss << '"' << escape_string(v) << '"';
            } else if constexpr (std::is_same_v<T, ArrayType>) {
                oss << '[';
                for (size_t i = 0; i < v.size(); ++i) {
                    if (i > 0) oss << ',';
                    v[i].to_string_impl(oss);
                }
                oss << ']';
            } else if constexpr (std::is_same_v<T, ObjectType>) {
                oss << '{';
                bool first = true;
                for (const auto& [key, value] : v) {
                    if (!first) oss << ',';
                    oss << '"' << escape_string(key) << "\":";
                    value.to_string_impl(oss);
                    first = false;
                }
                oss << '}';
            }
        }, value_);
    }

    static std::string escape_string(const std::string& s) {
        std::ostringstream oss;
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
                        oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
                    } else {
                        oss << c;
                    }
            }
        }
        return oss.str();
    }

    static Json parse_value(std::string_view json, size_t& pos) {
        skip_whitespace(json, pos);
        if (pos >= json.length()) throw std::runtime_error("Unexpected end of input");

        switch (json[pos]) {
            case 'n': return parse_null(json, pos);
            case 't': case 'f': return parse_bool(json, pos);
            case '"': return parse_string(json, pos);
            case '[': return parse_array(json, pos);
            case '{': return parse_object(json, pos);
            default:
                if (json[pos] == '-' || (json[pos] >= '0' && json[pos] <= '9'))
                    return parse_number(json, pos);
                throw std::runtime_error("Unexpected character");
        }
    }

    static void skip_whitespace(std::string_view json, size_t& pos) {
        while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n' || json[pos] == '\r'))
            ++pos;
    }

    static Json parse_null(std::string_view json, size_t& pos) {
        if (json.substr(pos, 4) == "null") {
            pos += 4;
            return Json();
        }
        throw std::runtime_error("Invalid null value");
    }

    static Json parse_bool(std::string_view json, size_t& pos) {
        if (json.substr(pos, 4) == "true") {
            pos += 4;
            return Json(true);
        }
        if (json.substr(pos, 5) == "false") {
            pos += 5;
            return Json(false);
        }
        throw std::runtime_error("Invalid boolean value");
    }

    static Json parse_number(std::string_view json, size_t& pos) {
        size_t start = pos;
        while (pos < json.length() && (json[pos] == '-' || json[pos] == '+' || json[pos] == '.' || 
               (json[pos] >= '0' && json[pos] <= '9') || json[pos] == 'e' || json[pos] == 'E'))
            ++pos;
        std::string num_str(json.substr(start, pos - start));
        return Json(std::stod(num_str));
    }

    static Json parse_string(std::string_view json, size_t& pos) {
        std::string result;
        ++pos; // Skip opening quote
        while (pos < json.length() && json[pos] != '"') {
            if (json[pos] == '\\') {
                ++pos;
                if (pos >= json.length()) throw std::runtime_error("Unexpected end of input in string");
                switch (json[pos]) {
                    case '"': result += '"'; break;
                    case '\\': result += '\\'; break;
                    case '/':result += '/'; break;
                    case 'b': result += '\b'; break;
                    case 'f': result += '\f'; break;
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    case 'u': {
                        if (pos + 4 >= json.length()) throw std::runtime_error("Unexpected end of input in unicode escape");
                        std::string hex(json.substr(pos + 1, 4));
                        result += static_cast<char>(std::stoi(hex, nullptr, 16));
                        pos += 4;
                        break;
                    }
                    default: throw std::runtime_error("Invalid escape sequence");
                }
            } else {
                result += json[pos];
            }
            ++pos;
        }
        if (pos >= json.length()) throw std::runtime_error("Unterminated string");
        ++pos; // Skip closing quote
        return Json(result);
    }

    static Json parse_array(std::string_view json, size_t& pos) {
        Json arr(Type::Array);
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
            if (json[pos] != ',') throw std::runtime_error("Expected ',' or ']' in array");
            ++pos;
            skip_whitespace(json, pos);
        }
    }

    static Json parse_object(std::string_view json, size_t& pos) {
        Json obj(Type::Object);
        ++pos; // Skip opening brace
        skip_whitespace(json, pos);
        if (json[pos] == '}') {
            ++pos;
            return obj;
        }
        while (true) {
            Json key = parse_string(json, pos);
            skip_whitespace(json, pos);
            if (json[pos] != ':') throw std::runtime_error("Expected ':' in object");
            ++pos;
            skip_whitespace(json, pos);
            obj[key.value<std::string>()] = parse_value(json, pos);
            skip_whitespace(json, pos);
            if (json[pos] == '}') {
                ++pos;
                return obj;
            }
            if (json[pos] != ',') throw std::runtime_error("Expected ',' or '}' in object");
            ++pos;
            skip_whitespace(json, pos);
        }
    }
};
