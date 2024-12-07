#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

class Table {
public:
    Table(const std::vector<std::string>& headers) : headers(headers) {
        column_widths.resize(headers.size(), 0);
        for (size_t i = 0; i < headers.size(); ++i) {
            column_widths[i] = headers[i].size();
        }
    }

    void add_row(const std::vector<std::string>& row) {
        rows.push_back(row);
        for (size_t i = 0; i < row.size(); ++i) {
            column_widths[i] = std::max(column_widths[i], row[i].size());
        }
    }

    void print() const {
        print_row(headers);
        print_separator();
        for (const auto& row : rows) {
            print_row(row);
        }
    }

private:
    std::vector<std::string> headers;
    std::vector<std::vector<std::string>> rows;
    std::vector<size_t> column_widths;

    void print_separator() const {
        for (size_t width : column_widths) {
            std::cout << "|-" << std::string(width, '-') << "-";
        }
        std::cout << "|" << std::endl;
    }

    void print_row(const std::vector<std::string>& row) const {
        for (size_t i = 0; i < row.size(); ++i) {
            std::cout << "| " << std::left << std::setw(column_widths[i]) << row[i] << " ";
        }
        std::cout << "|" << std::endl;
    }
};