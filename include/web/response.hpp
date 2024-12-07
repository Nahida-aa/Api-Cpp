#pragma once
#include <string>
#include <unordered_map>
#include <sstream>
#include <regex>
#include "./html_loader.hpp"

class Response {
public:
    int status_code;
    std::unordered_map<std::string, std::string> headers;
    std::string body;

    Response(int status_code = 200, const std::string_view& body = "", const std::unordered_map<std::string, std::string>& headers = {})
        : status_code(status_code), body(body), headers(headers) {
        if (this->body.ends_with(".html") || this->body.ends_with(".css") || this->body.ends_with(".js")) {
            this->body = HtmlLoader::load(this->body);
        }
        set_content_type();
    }

    std::string to_string() const {
        std::ostringstream response_stream;
        response_stream << "HTTP/1.1 " << status_code << " " << get_status_message(status_code) << "\r\n"; for (const auto& header : headers) {
            response_stream << header.first << ": " << header.second << "\r\n";
        } response_stream << "Content-Length: " << body.size() << "\r\n";
        response_stream << "\r\n";
        response_stream << body;
        return response_stream.str();
    }

private:
    std::string get_status_message(int status_code) const {
        switch (status_code) {
            case 200: return "OK";
            case 404: return "Not Found";
            case 400: return "Bad Request";
            default: return "Unknown";
        }
    }

    void set_content_type() {
        if (headers.find("Content-Type") == headers.end()) {
            if (is_json(body)) {
                headers["Content-Type"] = "application/json";
            } else if (is_html(body)) {
                headers["Content-Type"] = "text/html";
            } else {
                headers["Content-Type"] = "text/plain";
            }
        }
    }

    bool is_json(const std::string& str) const {
        return (str.front() == '{' && str.back() == '}') || (str.front() == '[' && str.back() == ']');
    }

    bool is_html(const std::string& str) const {
        return std::regex_search(str, std::regex("<[^>]+>"));
    }
};