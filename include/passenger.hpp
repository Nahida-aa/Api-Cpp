#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include "json.hpp"

class Passenger {
public:
    static string json_file_path;
    static string notifications_file_path;

    string flight_number;
    string id_number;
    string name;
    string gender;
    string birth_date;
    string seat_number;

    Passenger() = default;
    Passenger(const string& flight_number, const string& id_number, const string& name,
              const string& gender, const string& birth_date, const string& seat_number)
        : flight_number(flight_number), id_number(id_number), name(name), gender(gender), birth_date(birth_date), seat_number(seat_number) {}

    static Passenger create(
        const string& flight_number, 
        const string& id_number, 
        const string& name,
        const string& gender, 
        const string& birth_date, 
        const string& seat_number) {
        // DEBUG
        cout << "Passenger::create: 准备获取全部乘客" << endl;
        vector<Passenger> passengers = get_all();
        cout << "Passenger::create: 成功获取全部乘客, 准备 new 一个 乘客" << endl;
        for (const auto& passenger : passengers) {
            if (passenger.flight_number == flight_number && passenger.seat_number == seat_number) {
                throw runtime_error("座位已被预订");
            }
        }
        Passenger new_passenger(flight_number, id_number, name, gender, birth_date, seat_number);
        passengers.push_back(new_passenger);
        cout << "Passenger::create: 成功 new 一个 乘客到 乘客列表中, 准备保存全部乘客" << endl;
        save_all(passengers);
        return new_passenger;
    }

    static vector<Passenger> get_all() {
        ifstream file(json_file_path);
        // DEBUG
        cout << "Passenger::get_all: 尝试打开文件" << endl;
        if (!file.is_open()) {
            throw runtime_error("File not found: " + json_file_path);
            return {};
        }
        // DEBUG
        cout << "Passenger::get_all: 成功打开文件, 准备读取文件内容" << endl;
        string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        // 如果文件内容为空，返回空的乘客列表
        if (content.empty()) {
            cout << "Passenger::get_all: 文件内容为空，返回空的乘客列表" << endl;
            return {};
        }
        // DEBUG
        cout << "Passenger::get_all: 成功读取文件内容: "<< content <<" 准备解析 json 数据" << endl;
        Json data = Json::loads(content);
        // DEBUG
        cout << "Passenger::get_all: 成功解析 json 数据, 准备遍历数据" << endl;
        vector<Passenger> passengers;
        for (const auto& item : data.as_array()) {
            Passenger passenger;
            from_json(item, passenger);
            passengers.push_back(passenger);
        }
        return passengers;
    }

    static vector<Passenger> get_by_flight(const string& flight_number) {
        vector<Passenger> passengers = get_all();
        vector<Passenger> result;
        for (const auto& passenger : passengers) {
            if (passenger.flight_number == flight_number) {
                result.push_back(passenger);
            }
        }
        return result;
    }
    static vector<Passenger> get_by_user(const string& id_number) {
        vector<Passenger> passengers = get_all();
        vector<Passenger> result;
        for (const auto& passenger : passengers) {
            if (passenger.id_number == id_number) {
                result.push_back(passenger);
            }
        }
        return result;
    }
    static void remove(const string& id_number, const string& flight_number) {
        vector<Passenger> passengers = get_all();
        passengers.erase(remove_if(passengers.begin(), passengers.end(),
            [&](const Passenger& passenger) {
                return passenger.id_number == id_number && passenger.flight_number == flight_number;
            }), passengers.end());
        save_all(passengers);
    }

    static void add_notification(const string& id_number, const string& message) {
        Json notifications = get_notifications();
        Json notification;
        notification["id_number"] = id_number;
        notification["time"] = time(nullptr);
        notification["msg"] = message;
        notification["is_read"] = false;
        notifications.append(notification);
        save_notifications(notifications);
    }
    static Json get_notifications(const string& id_number) {
        Json notifications = get_notifications();
        Json user_notifications = Json::loads("[]");
        for (const auto& notification : notifications.as_array()) {
            if (notification["id_number"].as_string() == id_number) {
                user_notifications.append(notification);
            }
        }
        return user_notifications;
    }
    static void mark_notifications_as_read(const string& id_number) {
        Json notifications = get_notifications();
        for (auto& notification : notifications.as_array()) {
            if (notification["id_number"].as_string() == id_number) {
                notification["is_read"] = true;
            }
        }
        save_notifications(notifications);
    }
    static Json get_notifications() {
        ifstream file(notifications_file_path);
        if (!file.is_open()) {
            return Json::loads("[]");
        }
        string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        if (content.empty()) {
            return Json::loads("[]");
        }
        return Json::loads(content);
    }
    static void save_notifications(const Json& notifications) {
        ofstream file(notifications_file_path);
        file << notifications.dumps();
    }
    static Json to_json_array(const vector<Passenger>& passengers) {
        Json data = Json::loads("[]");
        for (const auto& passenger : passengers) {
            data.append(passenger.to_json());
        }
        return data;
    }

    Json to_json() const {
        Json json;
        json["flight_number"] = flight_number;
        json["id_number"] = id_number;
        json["name"] = name;
        json["gender"] = gender;
        json["birth_date"] = birth_date;
        json["seat_number"] = seat_number;
        return json;
    }
    static void save_all(const vector<Passenger>& passengers) {
        Json data = Json::loads("[]");
        for (const auto& passenger : passengers) {
            data.append(passenger.to_json());
        }
        ofstream file(json_file_path);
        file << data.dumps();
    }
private:


    static void from_json(const Json& json, Passenger& passenger) {
        passenger.flight_number = json["flight_number"].as_string();
        passenger.id_number = json["id_number"].as_string();
        passenger.name = json["name"].as_string();
        passenger.gender = json["gender"].as_string();
        passenger.birth_date = json["birth_date"].as_string();
        passenger.seat_number = json["seat_number"].as_string();
    }
};

string Passenger::json_file_path = "./data/passengers.json";
string Passenger::notifications_file_path = "./data/notifications.json";