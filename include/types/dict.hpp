#pragma once
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
// #include <cstddef>

// 定义 Mapping 接口
template <typename KeyType, typename ValueType>
class Mapping {
public:
    virtual ValueType get(const KeyType& key) const = 0;
    virtual bool contains(const KeyType& key) const = 0;
    virtual ~Mapping() = default;
};

// 定义 MutableMapping 接口，继承自 Mapping
template <typename KeyType, typename ValueType>
class MutableMapping : public Mapping<KeyType, ValueType> {
public:
    virtual void set(const KeyType& key, const ValueType& value) = 0;
    virtual void remove(const KeyType& key) = 0;
};

// 定义 PyDict 类，支持嵌套字典
class PyDict : public MutableMapping<std::variant<std::string, int>, std::any> {
public:
    using KeyType = std::variant<std::string, int>;
    using ValueType = std::any;
    
    bool is_string(const ValueType& value) const {
        return value.type() == typeid(std::string);
    }

    template<typename T>
    T get(const KeyType& key) const {
        auto it = store.find(key);
        if (it != store.end()) {
            ValueType value = it->second;
            if (value.type() == typeid(std::string)) return std::any_cast<std::string>(value);
            else if (value.type() == typeid(int)) return std::any_cast<int>(value); 
            else if (value.type() == typeid(double)) return std::any_cast<double>(value); 
            else if (value.type() == typeid(const char*)) return std::string(std::any_cast<const char*>(value));
            else if (value.type() == typeid(std::string_view)) return std::string(std::any_cast<std::string_view>(value));
            else if (value.type() == typeid(PyDict)) return std::any_cast<PyDict>(value); 
            else return value;
        }
        throw std::out_of_range("Key not found");
    }

    ValueType get(const KeyType& key) const override {
        return get<ValueType>(key);
    }
    bool contains(const KeyType& key) const override {
        return store.find(key) != store.end();
    }

    void set(const KeyType& key, const ValueType& value) override {
        store[key] = value;
    }

    void remove(const KeyType& key) override {
        store.erase(key);
    }

    std::string toString() const {
        std::ostringstream oss;
        oss << "{";
        for (auto it = store.begin(); it != store.end(); ++it) {
            if (it != store.begin()) {
                oss << ", ";
            }
            oss << keyToString(it->first) << ": " << valueToString(it->second);
        }
        oss << "}";
        return oss.str();
    }

    friend std::ostream& operator<<(std::ostream& os, const PyDict& dict) {
        return os << dict.toString();
    }

private:
    std::unordered_map<KeyType, ValueType> store;

    std::string keyToString(const KeyType& key) const {
        return std::visit([](auto&& arg) -> std::string {
            if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, std::string>) {
                return arg;
            } else {
                return std::to_string(arg);
            }
        }, key);
    }

    std::string valueToString(const ValueType& value) const {
        if (value.type() == typeid(int)) {
            return std::to_string(std::any_cast<int>(value));
        } else if (value.type() == typeid(double)) {
            return std::to_string(std::any_cast<double>(value));
        } else if (value.type() == typeid(std::string)) {
            return std::any_cast<std::string>(value);
        } else if (value.type() == typeid(PyDict)) {
            return std::any_cast<PyDict>(value).toString();
        } else {
            return "unknown";
        }
    }
};
