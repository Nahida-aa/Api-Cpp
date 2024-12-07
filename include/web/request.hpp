#pragma once
#include <iostream>
#include <vector>
#include <functional>
#include <regex>

using namespace std;

class Request {
public:
    string raw_request; // 原始请求字符串
    string method;
    string path;
    unordered_map<string, string> headers;
    string body;
    unordered_map<string, string> path_params;

    Request(const string& request_str) : raw_request(request_str) {
        istringstream stream(request_str);
        string line;
        getline(stream, line);
        istringstream line_stream(line);
        line_stream >> method >> path;

        while (getline(stream, line) && line != "\r") {
            auto colon_pos = line.find(':');
            if (colon_pos != string::npos) {
                string key = line.substr(0, colon_pos);
                string value = line.substr(colon_pos + 2); // Skip ": "
                headers[key] = value;
            }
        }

        body = string(istreambuf_iterator<char>(stream), {});
    }

    void set_path_params(const vector<string>& param_names, const vector<string>& param_values) {
        for (size_t i = 0; i < param_names.size(); ++i) {
            path_params[param_names[i]] = param_values[i];
        }
    }
};
