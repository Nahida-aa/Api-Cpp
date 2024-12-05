#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include "json.hpp"
#include "passenger.hpp"

class Flight {
public:
    static std::string json_file_path;

    std::string flight_number; // 航班号, 唯一标识
    int max_passengers; // 最大乘客数
    std::string departure_location; // 出发地
    std::string departure_time; // 出发时间
    std::string arrival_location; // 到达地
    std::string arrival_time; // 到达时间
    double price; // 价格
    int available_seats; // 可用座位数
    bool is_cancelled = false; // 是否取消
    int adjusted_cancel_time = 3600*24; // 距离航班取消的默认时间, 秒

    Flight() = default;
    Flight(const std::string& flight_number, int max_passengers, const std::string& departure_location,
           const std::string& departure_time, const std::string& arrival_location, const std::string& arrival_time, double price)
        : flight_number(flight_number), max_passengers(max_passengers), departure_location(departure_location),
          departure_time(departure_time), arrival_location(arrival_location), arrival_time(arrival_time), price(price), available_seats(max_passengers) {}

    static Flight create(const std::string& flight_number, int max_passengers, const std::string& departure_location,
        const std::string& departure_time, const std::string& arrival_location, const std::string& arrival_time, double price) {
        std::vector<Flight> flights = get_all();
        for (const auto& flight : flights) {
            if (flight.flight_number == flight_number) {
                throw std::runtime_error("航班已存在, 航班号是唯一标识");
            }
        }
        Flight new_flight(flight_number, max_passengers, departure_location, departure_time, arrival_location, arrival_time, price);
        flights.push_back(new_flight);
        save_all(flights);
        return new_flight;
    }
    static Flight create_from_obj_str(const std::string& obj_str) {
        Json reqBody = Json::loads(obj_str);
        return create(
            reqBody["flight_number"].as_string(),
            reqBody["max_passengers"].as_number(),
            reqBody["departure_location"].as_string(),
            reqBody["departure_time"].as_string(),
            reqBody["arrival_location"].as_string(),
            reqBody["arrival_time"].as_string(),
            reqBody["price"].as_number()
        );
    }

    static std::vector<Flight> get_all() {
        std::ifstream file(json_file_path);
        if (!file.is_open()) {
            std::cerr << "[ERR]: Failed to open file: " << json_file_path << std::endl;
            return {};
        }
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        Json data = Json::loads(content);
        std::vector<Flight> flights;
        for (const auto& item : data.as_array()) {
            Flight flight;
            from_json(item, flight);
            flights.push_back(flight);
        }
        return flights;
    }

    static Flight get(const std::string& flight_number) {
        std::vector<Flight> flights = get_all();
        for (const auto& flight : flights) {
            if (flight.flight_number == flight_number) {
                return flight;
            }
        }
        throw std::runtime_error("Flight not found");
    }

    static void update_seats(const std::string& flight_number, int num_tickets = 1) {
        std::vector<Flight> flights = get_all();
        for (auto& flight : flights) {
            if (flight.flight_number == flight_number) {
                flight.available_seats -= num_tickets;
                save_all(flights);
                return;
            }
        }
        throw std::runtime_error("航班未找到");
    }
    static void cancel(const std::string& flight_number) {
        std::vector<Flight> flights = get_all();
        for (auto& flight : flights) {
            if (flight.flight_number == flight_number) {
                flight.is_cancelled = true;
                flight.available_seats = flight.max_passengers; // 重置剩余座位数
                save_all(flights);
                return;
            }
        }
        throw std::runtime_error("Flight not found");
    }
    static void restore(const std::string& flight_number) { // 恢复航班
        std::vector<Flight> flights = get_all();
        for (auto& flight : flights) {
            if (flight.flight_number == flight_number) {
                flight.is_cancelled = false;
                save_all(flights);
                return;
            }
        }
        throw std::runtime_error("Flight not found");
    }
    static void remove(const std::string& flight_number) {
        std::vector<Flight> flights = get_all();
        auto it = std::remove_if(flights.begin(), flights.end(), [&flight_number](const Flight& flight) {
            return flight.flight_number == flight_number;
        });
        if (it == flights.end()) {
            throw std::runtime_error("Flight not found");
        }
        flights.erase(it, flights.end());
        save_all(flights);
    }
    
    static void save_all(const std::vector<Flight>& flights) {
        Json data = Json::loads("[]");
        for (const auto& flight : flights) {
            data.append(flight.to_json());
        }
        std::ofstream file(json_file_path);
        file << data.dumps();
    }

    static void check_availability(const std::string& flight_number, int num_tickets) {
        std::vector<Flight> flights = get_all();
        for (const auto& flight : flights) {
            if (flight.flight_number == flight_number) {
                if (flight.is_cancelled) {
                    throw std::runtime_error("航班已取消");
                }
                if (flight.available_seats < num_tickets) {
                    throw std::runtime_error("没有足够的座位了");
                }
                return;
            }
        }
        throw std::runtime_error("航班未找到");
    }
    static void auto_cancel() {
        std::vector<Flight> flights = get_all();
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);

        for (auto& flight : flights) {
            int booked_seats = flight.max_passengers - flight.available_seats;
            std::tm departure_tm = {};
            std::istringstream ss(flight.departure_time);
            ss >> std::get_time(&departure_tm, "%Y-%m-%d %H:%M:%S");
            std::time_t departure_time = std::mktime(&departure_tm);

            int remaining_time = std::difftime(departure_time, now_time) - flight.adjusted_cancel_time;

            if (booked_seats < flight.max_passengers * 0.1 && remaining_time <= 0) {
                flight.is_cancelled = true;
                std::vector<Passenger> passengers = Passenger::get_by_flight(flight.flight_number);
                for (const auto& passenger : passengers) {
                    Passenger::remove(passenger.id_number, flight.flight_number);
                    Passenger::add_notification(passenger.id_number, "您的航班 " + flight.flight_number + " 已被取消");
                }
            }
        }
        save_all(flights);
    }
    static Json to_json_array(const std::vector<Flight>& flights) {
        Json data = Json::loads("[]");
        std::cout << "初始化一个 json []: "<< data.dumps() << std::endl;
        for (const auto& flight : flights) {
            data.append(flight.to_json());
        }
        std::cout << "成功将 flights to json: "<< data.dumps() << std::endl;
        return data;
    }
    Json to_json() const {
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
    static void from_json(const Json& json, Flight& flight) {
        flight.flight_number = json["flight_number"].as_string();
        flight.max_passengers = json["max_passengers"].as_number();
        flight.departure_location = json["departure_location"].as_string();
        flight.departure_time = json["departure_time"].as_string();
        flight.arrival_location = json["arrival_location"].as_string();
        flight.arrival_time = json["arrival_time"].as_string();
        flight.price = json["price"].as_number();
        flight.available_seats = json["available_seats"].as_number();
        flight.is_cancelled = json["is_cancelled"].as_bool();
        flight.adjusted_cancel_time = json["adjusted_cancel_time"].as_number();
    }
};

std::string Flight::json_file_path = "./data/flights.json";