#include <iostream>
#include <vector>
#include <string>
#include "app/flight.hpp"
#include "app/passenger.hpp"
#include "app/user.hpp"
#include "cli/table.hpp"

using namespace std;

User current_user;

void print_admin_menu() {
    cout << "航班信息管理系统" << endl;
    cout << "1. 显示所有航班" << endl;
    cout << "2. 显示所有乘客" << endl;
    cout << "3. 查询航班" << endl;
    cout << "4. 增加航班" << endl;
    cout << "5. 取消航班" << endl;
    cout << "6. 恢复航班" << endl;
    cout << "7. 修改航班" << endl;
    cout << "8. 删除航班" << endl;
    cout << "9. 进入用户订票界面" << endl;
    cout << "0. 退出" << endl;
    cout << "请选择操作: ";
}

void print_user_menu() {
    cout << "用户订票界面" << endl;
    cout << "1. 显示所有航班" << endl;
    cout << "2. 显示自己的订票" << endl;
    cout << "3. 查询航班" << endl;
    cout << "4. 订票" << endl;
    cout << "5. 退票" << endl;
    cout << "6. 查看通知" << endl;
    if (current_user.is_admin) {
        cout << "9. 返回航班信息管理系统" << endl;
    }
    cout << "0. 退出" << endl;
    cout << "请选择操作: ";
}

void print_flights(const vector<Flight>& flights) {
    Table table({"航班号", "最大载客数", "起飞地点", "起飞时间", "降落地点", "降落时间", "单价", "可用座位数", "取消状态"});
    for (const auto& flight : flights) {
        table.add_row({
            flight.flight_number,
            to_string(flight.max_passengers),
            flight.departure_location,
            flight.departure_time,
            flight.arrival_location,
            flight.arrival_time,
            to_string(flight.price),
            to_string(flight.available_seats),
            flight.is_cancelled ? "已取消" : "正常"
        });
    }
    table.print();
}

void print_passengers(const vector<Passenger>& passengers) {
    Table table({"航班号", "身份证号码", "姓名", "性别", "出生年月", "座位号"});
    for (const auto& passenger : passengers) {
        table.add_row({
            passenger.flight_number,
            passenger.id_number,
            passenger.name,
            passenger.gender,
            passenger.birth_date,
            to_string(passenger.seat_number)
        });
    }
    table.print();
}

void query_flight() {
    vector<Flight> flights = Flight::get_all();
    vector<Flight> result = flights;
    int choice;
    string input;

    while (true) {
        cout << "请选择查询条件（输入0结束选择）：" << endl;
        cout << "1. 航班号" << endl;
        cout << "2. 出发地" << endl;
        cout << "3. 出发时间" << endl;
        cout << "4. 目的地" << endl;
        cout << "5. 到达时间" << endl;
        cout << "6. 是否正常（非取消, 选择6, 将默认筛选出非取消航班）" << endl;
        cout << "请选择操作: ";
        cin >> choice;

        if (choice == 0) {
            break;
        }

        switch (choice) {
            case 1:
                cout << "请输入航班号: ";
                cin >> input;
                result.erase(remove_if(result.begin(), result.end(), [&](const Flight& flight) {
                    return flight.flight_number != input;
                }), result.end());
                break;
            case 2:
                cout << "请输入出发地: ";
                cin >> input;
                result.erase(remove_if(result.begin(), result.end(), [&](const Flight& flight) {
                    return flight.departure_location != input;
                }), result.end());
                break;
            case 3:
                cout << "请输入出发时间 (格式: YYYY-MM-DDTHH:MM:SS): ";
                cin >> input;
                result.erase(remove_if(result.begin(), result.end(), [&](const Flight& flight) {
                    return flight.departure_time != input;
                }), result.end());
                break;
            case 4:
                cout << "请输入目的地: ";
                cin >> input;
                result.erase(remove_if(result.begin(), result.end(), [&](const Flight& flight) {
                    return flight.arrival_location != input;
                }), result.end());
                break;
            case 5:
                cout << "请输入到达时间 (格式: YYYY-MM-DDTHH:MM:SS): ";
                cin >> input;
                result.erase(remove_if(result.begin(), result.end(), [&](const Flight& flight) {
                    return flight.arrival_time != input;
                }), result.end());
                break;
            case 6:
                result.erase(remove_if(result.begin(), result.end(), [&](const Flight& flight) {
                    return flight.is_cancelled;
                }), result.end());
                break;
            default:
                cout << "无效的选择，请重新输入。" << endl;
                break;
        }
    }

    if (result.empty()) {
        cout << "[msg]: 未找到符合条件的航班。" << endl;
    } else {
        print_flights(result);
    }
}
void update_flight() {
    string flight_number;
    cout << "请输入要修改的航班号: ";
    cin >> flight_number;

    try {
        Flight flight = Flight::get(flight_number);

        int choice;
        string input;
        int int_input;
        double double_input;

        while (true) {
            cout << "请选择要修改的属性（输入0结束修改）：" << endl;
            cout << "1. 最大载客数" << endl;
            cout << "2. 起飞地点" << endl;
            cout << "3. 起飞时间" << endl;
            cout << "4. 降落地点" << endl;
            cout << "5. 降落时间" << endl;
            cout << "6. 单价" << endl;
            cout << "请选择操作: ";
            cin >> choice;

            if (choice == 0) {
                break;
            }

            switch (choice) {
                case 1:
                    cout << "请输入新的最大载客数 (当前值: " << flight.max_passengers << "): ";
                    cin >> int_input;
                    if (!cin.fail() && int_input > 0) {
                        flight.max_passengers = int_input;
                    }
                    break;
                case 2:
                    cout << "请输入新的起飞地点 (当前值: " << flight.departure_location << "): ";
                    cin >> input;
                    if (!input.empty()) {
                        flight.departure_location = input;
                    }
                    break;
                case 3:
                    cout << "请输入新的起飞时间 (当前值: " << flight.departure_time << ", 格式: YYYY-MM-DDTHH:MM:SS): ";
                    cin >> input;
                    if (!input.empty()) {
                        flight.departure_time = input;
                    }
                    break;
                case 4:
                    cout << "请输入新的降落地点 (当前值: " << flight.arrival_location << "): ";
                    cin >> input;
                    if (!input.empty()) {
                        flight.arrival_location = input;
                    }
                    break;
                case 5:
                    cout << "请输入新的降落时间 (当前值: " << flight.arrival_time << ", 格式: YYYY-MM-DDTHH:MM:SS): ";
                    cin >> input;
                    if (!input.empty()) {
                        flight.arrival_time = input;
                    }
                    break;
                case 6:
                    cout << "请输入新的单价 (当前值: " << flight.price << "): ";
                    cin >> double_input;
                    if (!cin.fail() && double_input > 0) {
                        flight.price = double_input;
                    }
                    break;
                default:
                    cout << "无效的选择，请重新输入。" << endl;
                    break;
            }
        }

        Flight::update(flight_number, flight);
        cout << "航班记录已成功更新。" << endl;
    } catch (const runtime_error& e) {
        cout << "错误: " << e.what() << endl;
    }
}
void add_flight() {
    string flight_number, departure_location, departure_time, arrival_location, arrival_time;
    int max_passengers;
    double price;

    cout << "请输入航班号: ";
    cin >> flight_number;

    while (true) {
        cout << "请输入最大载客数: ";
        cin >> max_passengers;
        if (cin.fail() || max_passengers <= 0) {
            cin.clear(); // 清除错误标志
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略错误输入
            cout << "无效的输入，请输入一个正整数。" << endl;
        } else {
            break;
        }
    }

    cout << "请输入起飞地点: ";
    cin >> departure_location;

    cout << "请输入起飞时间 (格式: YYYY-MM-DDTHH:MM:SS): ";
    cin >> departure_time;

    cout << "请输入降落地点: ";
    cin >> arrival_location;

    cout << "请输入降落时间 (格式: YYYY-MM-DDTHH:MM:SS): ";
    cin >> arrival_time;

    while (true) {
        cout << "请输入单价: ";
        cin >> price;
        if (cin.fail() || price <= 0) {
            cin.clear(); // 清除错误标志
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略错误输入
            cout << "无效的输入，请输入一个正数。" << endl;
        } else {
            break;
        }
    }

    try {
        Flight::create(flight_number, max_passengers, departure_location, departure_time, arrival_location, arrival_time, price);
        cout << "航班记录已成功添加。" << endl;
    } catch (const runtime_error& e) {
        cout << "错误: " << e.what() << endl;
    }
}

void cancel_flight() {
    string flight_number;
    cout << "请输入要取消的航班号: ";
    cin >> flight_number;

    try {
        Flight::cancel(flight_number);
        cout << "航班已成功取消。" << endl;
    } catch (const runtime_error& e) {
        cout << "错误: " << e.what() << endl;
    }
}
void restore_flight() {
    string flight_number;
    cout << "请输入要恢复的航班号: ";
    cin >> flight_number;

    try {
        Flight::restore(flight_number);
        cout << "航班已成功恢复。" << endl;
    } catch (const runtime_error& e) {
        cout << "错误: " << e.what() << endl;
    }
}
void delete_flight() {
    string flight_number;
    cout << "请输入要删除的航班号: ";
    cin >> flight_number;

    try {
        Flight::remove(flight_number);
        cout << "航班已成功删除。" << endl;
    } catch (const runtime_error& e) {
        cout << "错误: " << e.what() << endl;
    }
}

void book_flight() {
    if (current_user.id_number.empty()) {
        cout << "请先登录。" << endl;
        return;
    }

    string flight_number;
    int num_tickets;

    cout << "请输入航班号: ";
    cin >> flight_number;
    cout << "请输入订票数: ";
    cin >> num_tickets;

    vector<int> seat_numbers(num_tickets);
    for (int i = 0; i < num_tickets; ++i) {
        cout << "请输入座位号 " << i + 1 << ": ";
        cin >> seat_numbers[i];
    }

    try {
        Flight::check_availability(flight_number, num_tickets);
        for (int i = 0; i < num_tickets; ++i) {
            Passenger::create(flight_number, current_user.id_number, current_user.name, current_user.gender, current_user.birth_date, seat_numbers[i]);
        }
        Flight::update_seats(flight_number, num_tickets);
        cout << "订票成功。" << endl;
    } catch (const runtime_error& e) {
        cout << "错误: " << e.what() << endl;
    }
}

void cancel_booking() {
    if (current_user.id_number.empty()) {
        cout << "请先登录。" << endl;
        return;
    }

    string flight_number, seat_number;

    cout << "请输入航班号: ";
    cin >> flight_number;
    cout << "请输入座位号: ";
    cin >> seat_number;

    try {
        Passenger::remove(current_user.id_number, flight_number);
        Flight::update_seats(flight_number, -1);
        cout << "退票成功。" << endl;
    } catch (const runtime_error& e) {
        cout << "错误: " << e.what() << endl;
    }
}

void view_notifications() {
    if (current_user.id_number.empty()) {
        cout << "请先登录。" << endl;
        return;
    }

    try {
        Json notifications = Passenger::get_notifications(current_user.id_number);
        if (notifications.empty()) {
            cout << "没有新的通知。" << endl;
        } else {
            cout << "通知列表：" << endl;
            for (const auto& notification : notifications) {
                cout << notification << endl;
            }
            Passenger::mark_notifications_as_read(current_user.id_number);
        }
    } catch (const runtime_error& e) {
        cout << "错误: " << e.what() << endl;
    }
}

void admin_menu();
void user_menu();

void register_user() {
    string id_number, name, gender, birth_date;

    cout << "请输入身份证号码: ";
    cin >> id_number;
    cout << "请输入姓名: ";
    cin >> name;
    cout << "请输入性别: ";
    cin >> gender;
    cout << "请输入出生年月 (格式: YYYY-MM-DD): ";
    cin >> birth_date;

    try {
        current_user = User::create(id_number, name, gender, birth_date);
        cout << "注册成功。" << endl;
        user_menu();
    } catch (const runtime_error& e) {
        cout << "错误: " << e.what() << endl;
        register_user();
    }
}

void login_user() {
    string id_number, name;

    cout << "请输入身份证号码: ";
    cin >> id_number;
    cout << "请输入姓名: ";
    cin >> name;

    try {
        current_user = User::get(id_number);
        if (current_user.name != name) {
            throw runtime_error("用户名与身份证号不匹配");
            login_user();
        }
        cout << "登录成功。" << endl;
        if (current_user.is_admin) {
            admin_menu();
        } else {
            user_menu();
        }
    } catch (const runtime_error& e) {
        cout << "错误: " << e.what() << endl;
        login_user();
    }
}

void user_menu() {
    while (true) {
        print_user_menu();
        int choice;
        cin >> choice;

        switch (choice) {
            case 1: {
                vector<Flight> flights = Flight::get_all();
                print_flights(flights);
                break;
            }
            case 2: {
                vector<Passenger> bookings = Passenger::get_by_user(current_user.id_number);
                print_passengers(bookings);
                break;
            }
            case 3:
                query_flight();
                break;
            case 4:
                book_flight();
                break;
            case 5:
                cancel_booking();
                break;
            case 6:
                view_notifications();
                break;
            case 9:
                if (current_user.is_admin) {
                    admin_menu();
                }
                return;
            case 0:
                exit(0);
            default:
                cout << "无效的选择，请重新输入。" << endl;
        }
    }
}

void admin_menu() {
    while (true) {
        print_admin_menu();
        int admin_choice;
        cin >> admin_choice;

        switch (admin_choice) {
            case 1: {
                vector<Flight> flights = Flight::get_all();
                print_flights(flights);
                break;
            }
            case 2: {
                vector<Passenger> passengers = Passenger::get_all();
                print_passengers(passengers);
                break;
            }
            case 3:
                query_flight();
                break;
            case 4:
                add_flight();
                break;
            case 5:
                cancel_flight();
                break;
            case 6:
                restore_flight();
                break;
            case 7:
                update_flight();
                break;
            case 8:
                delete_flight();
                break;
            case 9:
                user_menu();
                break;
            case 0:
                exit(0);
            default:
                cout << "无效的选择，请重新输入。" << endl;
        }
    }
}

int main() {
    while (true) {
        if (!current_user.id_number.empty()) {
            if (current_user.is_admin) {
                admin_menu();
            } else {
                user_menu();
            }
        } else {
            cout << "欢迎使用航班信息管理系统" << endl;
            cout << "1. 管理员登录" << endl;
            cout << "2. 用户登录" << endl;
            cout << "0. 退出" << endl;
            cout << "请选择操作: ";
            int choice;
            cin >> choice;

            switch (choice) {
                case 1:
                    login_user();
                    break;
                case 2:
                    cout << "1. 注册" << endl;
                    cout << "2. 登录" << endl;
                    cout << "请选择操作: ";
                    int user_choice;
                    cin >> user_choice;

                    switch (user_choice) {
                        case 1:
                            register_user();
                            break;
                        case 2:
                            login_user();
                            break;
                        default:
                            cout << "无效的选择，请重新输入。" << endl;
                    }
                    break;
                case 0:
                    exit(0);
                default:
                    cout << "无效的选择，请重新输入。" << endl;
            }
        }
    }
    return 0;
}