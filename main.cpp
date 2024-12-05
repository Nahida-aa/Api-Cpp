#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <stdexcept>
#include <algorithm>
#include <optional>
#include "json.hpp" // 假设您的 JSON 库头文件名为 json.hpp
#include "web_server.hpp" // 假设您的 Web 服务器头文件名为 web_server.hpp
#include "flight.hpp" // 添加这个头文件以使用 Flight 类
#include "passenger.hpp" // 添加这个头文件以使用 Passenger 类
#include "depend.hpp"
#include "user.hpp"

int main() {
    Api app;
    app.get("/", [](const Request& req) {
        return "./public/index.html";
    });
    app.post("/api/login", [](const Request& req) {
        string token = User::login(req.body);
        return "{\"token\": \"" + token + "\"}";
    });
    app.get("/admin", [](const Request& req) {
        return "./public/admin.html";
    });
    // 增加航班记录
    app.post("/api/flights", [](const Request& req) {
        Flight newFlight = Flight::create_from_obj_str(req.body);
        return newFlight.to_json().dumps();
    });
    // 航班取消
    app.put("/api/flights/cancel/([a-zA-Z0-9]+)", {"flight_number"}, [](const Request& req) {
        string flight_number = req.path_params.at("flight_number");
        Flight::cancel(flight_number);
        // 获取所有订票信息
        vector<Passenger> passengers = Passenger::get_by_flight(flight_number);
        for (const auto& passenger : passengers) {
            // 删除订票信息
            Passenger::remove(passenger.id_number, flight_number);
            // 添加通知
            Passenger::add_notification(passenger.id_number, "您的航班 " + flight_number + " 已被取消");
        }
        return R"({"message": "成功航班取消"})";
    });
    // 恢复航班
    app.put("/api/flights/restore/([a-zA-Z0-9]+)", {"flight_number"}, [](const Request& req) {
        Flight::restore(req.path_params.at("flight_number"));
        return R"({"message": "成功恢复航班"})";
    });
    // 删除航班
    app.del("/api/flights/([a-zA-Z0-9]+)", {"flight_number"}, [](const Request& req) {
        Flight::remove(req.path_params.at("flight_number"));
        return R"({"message": "成功删除航班"})";
    });
    // 航班查询
    app.get("/api/flights/([a-zA-Z0-9]+)", {"flight_number"}, [](const Request& req) {
        Flight flight = Flight::get(req.path_params.at("flight_number"));
        return flight.to_json().dumps();
    });

    // 获取所有航班
    app.get("/api/flights", [](const Request& req) {
        Flight::auto_cancel();
        Json response = Flight::to_json_array(Flight::get_all());
        return response.dumps();;
    });

    // 订票
    app.post("/api/bookings", [](const Request& req) {
        string id_number = get_authorization(req);
        cout << "航班订票: 用户" << id_number << endl;
        User user = User::get(id_number);
        cout << "航班订票: 成功获取用户" << user.name << endl;
        cout << "航班订票: 请求体" << req.body << endl;
        Json reqBody = Json::loads(req.body);
        string flight_number =reqBody["flight_number"].as_string();
        int num_tickets = reqBody["num_tickets"].as_number();
        
        // 检查航班是否已取消, 是否有足够座位, 是否存在
        Flight::auto_cancel();
        Flight::check_availability(flight_number, num_tickets);

        for (int i = 0; i < reqBody["num_tickets"].as_number(); ++i) {
            Passenger::create( // 创建乘客信息, 并保证(flight_number, seat_number)唯一
                flight_number, id_number, user.name,
                user.gender, user.birth_date, reqBody["seat_number"].as_string()
            );
        }

        Flight::update_seats(flight_number, num_tickets); // 更新航班座位信息
        return R"({"message": "订票成功"})";
    });
    // 显示用户 自己的订票信息
    app.get("/api/bookings", [](const Request& req) {
        string id_number = get_authorization(req);
        vector<Passenger> bookings = Passenger::get_by_user(id_number);
        Json response = Json::loads("[]");
        for (const auto& booking : bookings) {
            Json booking_json = booking.to_json();
            Flight flight = Flight::get(booking.flight_number);
            booking_json["departure_location"] = flight.departure_location;
            booking_json["departure_time"] = flight.departure_time;
            booking_json["arrival_location"] = flight.arrival_location;
            booking_json["arrival_time"] = flight.arrival_time;
            response.append(booking_json);
        }
        string response_body_str = response.dumps();

        return response_body_str;
    });
    // 退票
    app.del("/api/bookings", [](const Request& req) {
        string id_number = get_authorization(req);
        Json reqBody = Json::loads(req.body);
        string flight_number = reqBody["flight_number"].as_string();
        string seat_number = reqBody["seat_number"].as_string();

        // 获取所有乘客信息
        vector<Passenger> passengers = Passenger::get_all();
        passengers.erase(remove_if(passengers.begin(), passengers.end(),
            [&](const Passenger& passenger) {
                return passenger.id_number == id_number && passenger.flight_number == flight_number && passenger.seat_number == seat_number;
            }), passengers.end());

        // 保存所有乘客信息
        Passenger::save_all(passengers);

        // 更新航班信息
        vector<Flight> flights = Flight::get_all();
        for (auto& flight : flights) {
            if (flight.flight_number == flight_number) {
                flight.available_seats += 1;
                break;
            }
        }
        Flight::save_all(flights);

        return R"({"message": "退票成功"})";
    });
    // 获取当前用户的通知信息
    app.get("/api/notifications", [](const Request& req) {
        string id_number = get_authorization(req);
        Json notifications = Passenger::get_notifications(id_number);
        return notifications.dumps();
    });
    app.put("/api/notifications/read", [](const Request& req) {
        string id_number = get_authorization(req);
        Passenger::mark_notifications_as_read(id_number);
        return R"({"message": "所有通知已标记为已读"})";
    });

    Config config;
    WebServer server(config, app);
    server.run();
    return 0;
}