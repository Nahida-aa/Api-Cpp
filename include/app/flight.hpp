#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <chrono>
#include <variant>
#include "../types/json.hpp"
#include "./passenger.hpp"
using namespace std;

// 静态成员变量, 基本字段属性, 构造函数, read 函数, create 函数, update 函数, delete 函数, other 函数
class Flight {
public:
    static string json_file_path; // json 文件路径

    string flight_number; // 航班号, 唯一标识
    int max_passengers; // 最大乘客数
    string departure_location; // 出发地
    string departure_time; // 出发时间
    string arrival_location; // 到达地
    string arrival_time; // 到达时间
    double price; // 价格
    int available_seats; // 可用座位数
    bool is_cancelled = false; // 是否取消
    int adjusted_cancel_time = 3600*24; // 距离航班取消的默认时间, 秒

    Flight() = default;
    Flight(const string& flight_number, int max_passengers,
        const string& departure_location,const string& departure_time, 
        const string& arrival_location, const string& arrival_time, double price): flight_number(flight_number), max_passengers(max_passengers), departure_location(departure_location),
        departure_time(departure_time), arrival_location(arrival_location), arrival_time(arrival_time), price(price), available_seats(max_passengers) {}

    // read
    static vector<Flight> get_all() {
        ifstream file(json_file_path);
        if (!file.is_open()) {
            cerr << "[ERR]: Failed to open file: " << json_file_path << endl;
            return {};
        }
        string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        Json data = Json::loads(content);
        vector<Flight> flights;
        for (const auto& item : data) { // 这里如有编译报错或运行终止, 则改成 data.value<Json::ArrayType>()， 以明确 data 是数组类型
            Flight flight;
            from_json(item, flight);
            flights.push_back(flight);
        }
        return flights;
    }
    static Flight get(const string& flight_number){
        vector<Flight> flights = get_all();
        for (const auto& flight : flights) {
            if (flight.flight_number == flight_number) {
                return flight;
            }
        }
        throw runtime_error("航班未找到");
    }
    // create
    static Flight create(const string& flight_number, int max_passengers, const string& departure_location,
        const string& departure_time, const string& arrival_location, const string& arrival_time, double price){
        vector<Flight> flights = get_all();
        for (const auto& flight : flights) {
            if (flight.flight_number == flight_number) {
                throw runtime_error("航班已存在, 航班号是唯一标识");
            }
        }
        Flight new_flight(flight_number, max_passengers, departure_location, departure_time, arrival_location, arrival_time, price);
        flights.push_back(new_flight);
        save_all(flights);
        return new_flight;
    }
    static Flight create_from_obj_str(const string& obj_str) {
        Json reqBody = Json::loads(obj_str);
        return create(
            reqBody["flight_number"],
            reqBody["max_passengers"],
            reqBody["departure_location"],
            reqBody["departure_time"],
            reqBody["arrival_location"],
            reqBody["arrival_time"],
            reqBody["price"]
        );
    }
    // update
    static void update(const string& flight_number, const Flight& updated_flight){
        vector<Flight> flights = get_all();
        for (auto& flight : flights) { // 这里如有编译报错或运行终止, 则改成 flights.value<Json::ArrayType>()， 以明确 flights 是数组类型
            if (flight.flight_number == flight_number) {
                flight = updated_flight;
                save_all(flights);
                return;
            }
        }
        throw runtime_error("航班未找到");
    }
    static void update_seats(const string& flight_number, int num_tickets = 1){ // 更新航班的剩余座位数
        vector<Flight> flights = get_all();
        for (auto& flight : flights) { // 同上, 这取决于我的 json 库的版本, 因为我刚开始开发一个用户友好的 json 库
            if (flight.flight_number == flight_number) {
                flight.available_seats -= num_tickets;
                save_all(flights);
                return;
            }
        }
        throw runtime_error("航班未找到");
    }
    static void cancel(const string& flight_number){ // 取消航班
        vector<Flight> flights = get_all();
        for (auto& flight : flights) {
            if (flight.flight_number == flight_number) {
                flight.is_cancelled = true;
                flight.available_seats = flight.max_passengers; // 重置剩余座位数
                save_all(flights);
                return;
            }
        }
        throw runtime_error("航班未找到");
    }

    static void restore(const string& flight_number) { // 恢复航班
        vector<Flight> flights = get_all();
        for (auto& flight : flights) {
            if (flight.flight_number == flight_number) {
                flight.is_cancelled = false;
                save_all(flights);
                return;
            }
        }
        throw runtime_error("Flight not found");
    }

    static void auto_cancel(){ // 自动取消航班, 可以在 web api app 更好的体验效果
        vector<Flight> flights = get_all();
        auto now = chrono::system_clock::now();
        time_t now_time = chrono::system_clock::to_time_t(now);

        for (auto& flight : flights) {
            int booked_seats = flight.max_passengers - flight.available_seats;
            tm departure_tm = {};
            istringstream ss(flight.departure_time);
            ss >> get_time(&departure_tm, "%Y-%m-%d %H:%M:%S");
            time_t departure_time = mktime(&departure_tm);

            int remaining_time = difftime(departure_time, now_time) - flight.adjusted_cancel_time; // 剩余时间

            if (booked_seats < flight.max_passengers * 0.1 && remaining_time <= 0) {
                flight.is_cancelled = true;
                vector<Passenger> passengers = Passenger::get_by_flight(flight.flight_number);
                for (const auto& passenger : passengers) {
                    Passenger::remove(passenger.id_number, flight.flight_number); // r如果航班取消, 则删除乘客的订票记录
                    Passenger::add_notification(passenger.id_number, "您的航班 " + flight.flight_number + " 已被取消"); // 给乘客发送通知
                }
            }
        }
        save_all(flights);
        // cout << "自动取消航班检查完成" << endl;
    }

    // del
    static void remove(const string& flight_number){
        vector<Flight> flights = get_all();
        auto it = remove_if(flights.begin(), flights.end(), [&flight_number](const Flight& flight) {
            return flight.flight_number == flight_number;
        });
        if (it == flights.end()) {
            throw runtime_error("Flight not found");
        }
        flights.erase(it, flights.end());
        save_all(flights);
    }

    // other
    static void save_all(const vector<Flight>& flights){
        Json data = Json::loads("[]");
        for (const auto& flight : flights) {
            data.append(flight.to_json());
        }
        ofstream file(json_file_path);
        file << data.dumps();
    }

    static void check_availability(const string& flight_number, int num_tickets) { // 检查航班有效性, 用于用户订票
        vector<Flight> flights = get_all();
        for (const auto& flight : flights) {
            if (flight.flight_number == flight_number) {
                if (flight.is_cancelled) {
                    throw runtime_error("航班已取消");
                }
                if (flight.available_seats < num_tickets) {
                    throw runtime_error("没有足够的座位了");
                }
                return;
            }
        }
        throw runtime_error("航班未找到");
    }

    static Json to_json_array(const vector<Flight>& flights) { // 用于简化操作的函数
        Json data = Json::loads("[]");
        for (const auto& flight : flights) { // 同上
            data.append(flight.to_json());
        }
        return data;
    }
    Json to_json() const { // Flight -> json
        Json json;
        json["flight_number"] = flight_number;
        json["max_passengers"] = max_passengers;
        json["departure_location"] = departure_location;
        json["departure_time"] = departure_time;
        json["arrival_location"] = arrival_location;
        json["arrival_time"] = arrival_time;
        json["price"] = price;
        json["available_seats"] = available_seats;
        json["is_cancelled"] = is_cancelled;
        json["adjusted_cancel_time"] = adjusted_cancel_time;
        return json;
    }

private:
    static void from_json(const Json& json, Flight& flight){ // json -> Flight
        flight.flight_number = json.get<string>("flight_number");
        flight.max_passengers = json["max_passengers"];
        flight.departure_location = json.get<string>("departure_location");
        flight.departure_time = json.get<string>("departure_time");
        flight.arrival_location = json.get<string>("arrival_location");
        flight.arrival_time = json.get<string>("arrival_time");
        flight.price = json["price"];
        flight.available_seats = json["available_seats"];
        flight.is_cancelled = json["is_cancelled"];
        flight.adjusted_cancel_time = json["adjusted_cancel_time"];
    }
};

string Flight::json_file_path = "./data/flights.json";