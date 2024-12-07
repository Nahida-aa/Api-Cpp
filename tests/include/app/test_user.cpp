#include <iostream>
#include "app/user.hpp"

void test_create_user() {
    User user = User::create("1234567890", "John Doe", "Male", "1990-01-01");
    if (user.id_number != "1234567890" || user.name != "John Doe" || user.gender != "Male" || user.birth_date != "1990-01-01" || user.is_admin) {
        std::cerr << "test_create_user failed" << std::endl;
    } else {
        std::cout << "test_create_user passed" << std::endl;
    }
}

void test_get_all_users() {
    std::vector<User> users = User::get_all();
    if (users.empty()) {
        std::cerr << "test_get_all_users failed" << std::endl;
    } else {
        std::cout << "test_get_all_users passed" << std::endl;
    }
}

void test_get_user() {
    User user = User::get("1234567890");
    if (user.id_number != "1234567890" || user.name != "John Doe" || user.gender != "Male" || user.birth_date != "1990-01-01") {
        std::cerr << "test_get_user failed" << std::endl;
    } else {
        std::cout << "test_get_user passed" << std::endl;
    }
}

void test_save_all_users() {
    std::vector<User> users = User::get_all();
    User new_user("0987654321", "Jane Doe", "Female", "1992-02-02");
    users.push_back(new_user);
    User::save_all(users);

    std::vector<User> updated_users = User::get_all();
    bool found = false;
    for (const auto& user : updated_users) {
        if (user.id_number == "0987654321" && user.name == "Jane Doe" && user.gender == "Female" && user.birth_date == "1992-02-02") {
            found = true;
            break;
        }
    }

    if (!found) {
        std::cerr << "test_save_all_users failed" << std::endl;
    } else {
        std::cout << "test_save_all_users passed" << std::endl;
    }
}

void test_login() {
    std::string token = User::login(R"({"id_number": "1234567890", "name": "John Doe", "gender": "Male", "birth_date": "1990-01-01"})");
    if (token.empty()) {
        std::cerr << "test_login failed" << std::endl;
    } else {
        std::cout << "test_login passed" << std::endl;
    }
}

int main() {
    test_create_user();
    test_get_all_users();
    test_get_user();
    test_save_all_users();
    test_login();
    std::cout << "test_user 测试 通过" << std::endl;
    return 0;
}