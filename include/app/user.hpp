#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "types/json.hpp"
#include "web/jwt.hpp"

// 静态成员变量, 基本字段属性, 构造函数, read 函数, create 函数, update 函数, delete 函数, other 函数
class User {
public:
    static string json_file_path; // json 文件路径

    std::string id_number; // 身份证号码
    std::string name; // 姓名
    std::string gender; // 性别
    std::string birth_date; // 出生年月
    bool is_admin = false; // 是否管理员

    User() = default;
    User(const std::string& id_number, const std::string& name, const std::string& gender, const std::string& birth_date): id_number(id_number), name(name), gender(gender), birth_date(birth_date) {}

    static std::vector<User> get_all() {
        std::ifstream file(json_file_path);
        if (!file.is_open()) {
            std::cerr << "[ERR]: 无法打开文件: " <<json_file_path << std::endl;
            return {};
        }
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        Json data = Json::loads(content);
        std::vector<User> users;
        for (const auto& item : data) {
            User user;
            from_json(item, user);
            users.push_back(user);
        }
        return users;
    }
    static User get(const std::string& id_number){
        std::vector<User> users = get_all();
        for (const auto& user : users) {
            if (user.id_number == id_number) {
                return user;
            }
        }
        throw std::runtime_error("用户未找到");
    }

    static User create(const std::string& id_number, const std::string& name, const std::string& gender, const std::string& birth_date){
        std::vector<User> users = get_all();
        // INFO
        // std::cout<<"users.size():"<<users.size()<<std::endl;
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
    static std::string login(const std::string& body) {// 用于 web app 登录
        Json reqBody = Json::loads(body);
        std::string id_number = reqBody["id_number"];
        std::string name = reqBody["name"];
        std::string gender = reqBody["gender"];
        std::string birth_date = reqBody["birth_date"];
        // std::cout << "用户登录:" << id_number << name << gender << birth_date << std::endl;
        // 简单的登录逻辑
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
            // std::cout << "用户不存在，创建新用户" << std::endl;
            create(id_number, name, gender, birth_date);
        }

        // std::cout << "用户登录成功, 准备生成 token" << std::endl;
        return generate_token(id_number);
    }

    static void save_all(const std::vector<User>& users){
        Json data = Json::loads("[]");
        for (const auto& user : users) {
            data.append(user.to_json());
        }
        std::ofstream file(json_file_path);
        file << data.dumps();
    }

    Json to_json() const{
        Json json;
        json["id_number"] = id_number;
        json["name"] = name;
        json["gender"] = gender;
        json["birth_date"] = birth_date;
        json["is_admin"] = is_admin;
        return json;
    }

private:
    static void from_json(const Json& json, User& user) {
        user.id_number = json.get<string>("id_number");
        user.name = json.get<string>("name");
        user.gender = json.get<string>("gender");
        user.birth_date = json.get<string>("birth_date");
        user.is_admin = json["is_admin"];
    }
};

std::string User::json_file_path = "./data/users.json";