#include <iostream>
#include "app/flight.hpp"

void test_create_flight() {
    Flight flight = Flight::create("CA1234", 200, "Beijing", "2023-12-01 08:00:00", "Shanghai", "2023-12-01 10:00:00", 500.0);
    if (flight.flight_number != "CA1234" || flight.max_passengers != 200 || flight.departure_location != "Beijing" ||
        flight.departure_time != "2023-12-01 08:00:00" || flight.arrival_location != "Shanghai" ||
        flight.arrival_time != "2023-12-01 10:00:00" || flight.price != 500.0 || flight.available_seats != 200 ||
        flight.is_cancelled) {
        std::cerr << "test_create_flight failed" << std::endl;
    } else {
        std::cout << "test_create_flight passed" << std::endl;
    }
}

void test_get_flight() {
    Flight flight = Flight::get("CA1234");
    if (flight.flight_number != "CA1234") {
        std::cerr << "test_get_flight failed" << std::endl;
    } else {
        std::cout << "test_get_flight passed" << std::endl;
    }
}

void test_update_flight() {
    Flight flight = Flight::get("CA1234");
    flight.price = 600.0;
    Flight::update("CA1234", flight);
    Flight updated_flight = Flight::get("CA1234");
    if (updated_flight.price != 600.0) {
        std::cerr << "test_update_flight failed" << std::endl;
    } else {
        std::cout << "test_update_flight passed" << std::endl;
    }
}

void test_delete_flight() {
    Flight::remove("CA1234");
    try {
        Flight flight = Flight::get("CA1234");
        std::cerr << "test_delete_flight failed" << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << "test_delete_flight passed" << std::endl;
    }
}

void test_update_seats() {
    Flight flight = Flight::create("CA1235", 200, "Beijing", "2023-12-01 08:00:00", "Shanghai", "2023-12-01 10:00:00", 500.0);
    Flight::update_seats("CA1235", 10);
    Flight updated_flight = Flight::get("CA1235");
    if (updated_flight.available_seats != 190) {
        std::cerr << "test_update_seats failed" << std::endl;
    } else {
        std::cout << "test_update_seats passed" << std::endl;
    }
    Flight::remove("CA1235");
}

void test_cancel_flight() {
    Flight flight = Flight::create("CA1236", 200, "Beijing", "2023-12-01 08:00:00", "Shanghai", "2023-12-01 10:00:00", 500.0);
    Flight::cancel("CA1236");
    Flight cancelled_flight = Flight::get("CA1236");
    if (!cancelled_flight.is_cancelled) {
        std::cerr << "test_cancel_flight failed" << std::endl;
    } else {
        std::cout << "test_cancel_flight passed" << std::endl;
    }
    Flight::remove("CA1236");
}

void test_restore_flight() {
    Flight flight = Flight::create("CA1237", 200, "Beijing", "2023-12-01 08:00:00", "Shanghai", "2023-12-01 10:00:00", 500.0);
    Flight::cancel("CA1237");
    Flight::restore("CA1237");
    Flight restored_flight = Flight::get("CA1237");
    if (restored_flight.is_cancelled) {
        std::cerr << "test_restore_flight failed" << std::endl;
    } else {
        std::cout << "test_restore_flight passed" << std::endl;
    }
    Flight::remove("CA1237");
}

void test_check_availability() {
    Flight flight = Flight::create("CA1238", 200, "Beijing", "2023-12-01 08:00:00", "Shanghai", "2023-12-01 10:00:00", 500.0);
    try {
        Flight::check_availability("CA1238", 10);
        std::cout << "test_check_availability passed" << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "test_check_availability failed" << std::endl;
    }
    Flight::remove("CA1238");
}

int main() {
    test_create_flight();
    test_get_flight();
    test_update_flight();
    test_delete_flight();
    test_update_seats();
    test_cancel_flight();
    test_restore_flight();
    test_check_availability();
    std::cout << "test_flight 测试 通过" << std::endl;
    return 0;
}