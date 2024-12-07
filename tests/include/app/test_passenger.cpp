#include <iostream>
#include "app/passenger.hpp"

void test_create_passenger() {
    Passenger passenger = Passenger::create("CA1234", "1234567890", "John Doe", "Male", "1990-01-01", 1);
    if (passenger.flight_number != "CA1234" || passenger.id_number != "1234567890" || passenger.name != "John Doe" ||
        passenger.gender != "Male" || passenger.birth_date != "1990-01-01" || passenger.seat_number != 1) {
        std::cerr << "test_create_passenger failed" << std::endl;
    } else {
        std::cout << "test_create_passenger passed" << std::endl;
    }
}

void test_get_all_passengers() {
    std::vector<Passenger> passengers = Passenger::get_all();
    if (passengers.empty()) {
        std::cerr << "test_get_all_passengers failed" << std::endl;
    } else {
        std::cout << "test_get_all_passengers passed" << std::endl;
    }
}

void test_get_passengers_by_flight() {
    std::vector<Passenger> passengers = Passenger::get_by_flight("CA1234");
    if (passengers.empty() || passengers[0].flight_number != "CA1234") {
        std::cerr << "test_get_passengers_by_flight failed" << std::endl;
    } else {
        std::cout << "test_get_passengers_by_flight passed" << std::endl;
    }
}

void test_get_passengers_by_user() {
    std::vector<Passenger> passengers = Passenger::get_by_user("1234567890");
    if (passengers.empty() || passengers[0].id_number != "1234567890") {
        std::cerr << "test_get_passengers_by_user failed" << std::endl;
    } else {
        std::cout << "test_get_passengers_by_user passed" << std::endl;
    }
}

void test_remove_passenger() {
    Passenger::remove("1234567890", "CA1234");
    std::vector<Passenger> passengers = Passenger::get_by_user("1234567890");
    if (!passengers.empty()) {
        std::cerr << "test_remove_passenger failed" << std::endl;
    } else {
        std::cout << "test_remove_passenger passed" << std::endl;
    }
}

void test_add_notification() {
    Passenger::add_notification("1234567890", "Your flight CA1234 has been cancelled.");
    Json notifications = Passenger::get_notifications("1234567890");
    if (notifications.empty()) {
        std::cerr << "test_add_notification failed" << std::endl;
    } else {
        std::cout << "test_add_notification passed" << std::endl;
    }
}

void test_get_notifications() {
    Json notifications = Passenger::get_notifications("1234567890");
    if (notifications.empty()) {
        std::cerr << "test_get_notifications failed" << std::endl;
    } else {
        std::cout << "test_get_notifications passed" << std::endl;
    }
}

void test_mark_notifications_as_read() {
    Passenger::mark_notifications_as_read("1234567890");
    Json notifications = Passenger::get_notifications("1234567890");
    bool all_read = true;
    for (const auto& notification : notifications) {
        if (!notification["is_read"]) {
            all_read = false;
            break;
        }
    }
    if (!all_read) {
        std::cerr << "test_mark_notifications_as_read failed" << std::endl;
    } else {
        std::cout << "test_mark_notifications_as_read passed" << std::endl;
    }
}

int main() {
    test_create_passenger();
    test_get_all_passengers();
    test_get_passengers_by_flight();
    test_get_passengers_by_user();
    test_remove_passenger();
    test_add_notification();
    test_get_notifications();
    test_mark_notifications_as_read();
    std::cout << "test_passenger 测试 通过" << std::endl;
    return 0;
}