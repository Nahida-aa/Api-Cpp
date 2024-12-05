#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "json.hpp"

class User {
public:
    std::string id_number;
    std::string name;
    std::string gender;
    std::string birth_date;

    User() = default;
    User(const std::string& id_number, const std::string& name, const std::string& gender, const std::string& birth_date)
        : id_number(id_number), name(name), gender(gender), birth_date(birth_date) {}

    static User create(const std::string& id_number, const std::string& name, const std::string& gender, const std::string& birth_date) {
        std::vector<User> users = get_all();
        // DEBUG
        std::cout<<"users.size():"<<users.size()<<std::endl;
        for (const auto& user : users) {
            if (user.id_number == id_number) {
                throw std::runtime_error("用户已存在");
            }
        }
        User new_user(id_number, name, gender, birth_date);
        users.push_back(new_user);
        save_all(users);
        return new_user;
    }
    static std::string login(const std::string& body) {
        Json reqBody = Json::loads(body);
        std::string id_number = reqBody["id_number"].as_string();
        std::string name = reqBody["name"].as_string();
        std::string gender = reqBody["gender"].as_string();
        std::string birth_date = reqBody["birth_date"].as_string();
        std::cout << "用户登录:" << id_number << name << gender << birth_date << std::endl;

        // 简单的登录逻辑，可以根据需要扩展
        if (id_number.empty() || name.empty() || gender.empty() || birth_date.empty()) {
            throw std::runtime_error("登录信息不完整");
        }

        try {
            User user = get(id_number);
            if (user.name != name) {
                throw std::runtime_error("用户名与身份证号不匹配");
            }
        } catch (const std::runtime_error& e) {
            // 用户不存在，创建新用户
            std::cout << "用户不存在，创建新用户" << std::endl;
            create(id_number, name, gender, birth_date);
        }

        std::cout << "用户登录成功, 准备生成 token" << std::endl;
        return generate_token(id_number);
    }
    static User get(const std::string& id_number) {
        std::vector<User> users = get_all();
        for (const auto& user : users) {
            if (user.id_number == id_number) {
                return user;
            }
        }
        throw std::runtime_error("用户未找到");
    }

    static std::vector<User> get_all() {
        std::ifstream file("./data/users.json");
        if (!file.is_open()) {
            std::cerr << "[ERR]: 无法打开文件: ./data/users.json" << std::endl;
            return {};
        }
        std::cout << "打开文件: ./data/users.json" << std::endl;
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        Json data = Json::loads(content);
        std::vector<User> users;
        for (const auto& item : data.as_array()) {
            User user;
            from_json(item, user);
            users.push_back(user);
        }
        return users;
    }

    static void save_all(const std::vector<User>& users) {
        Json data = Json::loads("[]");
        for (const auto& user : users) {
            data.append(user.to_json());
        }
        std::ofstream file("./data/users.json");
        file << data.dumps();
    }

    Json to_json() const {
        Json json;
        json["id_number"] = id_number;
        json["name"] = name;
        json["gender"] = gender;
        json["birth_date"] = birth_date;
        return json;
    }

private:
    static void from_json(const Json& json, User& user) {
        user.id_number = json["id_number"].as_string();
        user.name = json["name"].as_string();
        user.gender = json["gender"].as_string();
        user.birth_date = json["birth_date"].as_string();
    }
};