#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <regex>
// #include "./web_server.hpp"
#include "./request.hpp"
#include "./response.hpp"
#include "./depend.hpp"
#include <variant> // c++17
#include "../types/to_str.hpp"

using namespace std;
// Route handler function type, 定义路由处理函数类型
using ApiFuncResult = variant<const char*, string, string_view>;
using ApiFunc = function<ApiFuncResult(const Request&)>;
using RouteHandler = function<Response(const Request&)>;

// Route structure
struct Route {
    string method;
    string path;
    vector<string> param_names;
    RouteHandler handler;
    // 使用 {} 初始化列表 时, 不能将 lambda 表达式作为参数传递给 vector::push_back, 需要显式地构造 Route 对象
    Route(const string& method, const string& path, const vector<string>& param_names, RouteHandler handler)
        : method(method), path(path), param_names(param_names), handler(handler) {}
};

// Api class (replacing WebApi)
class Api {
public:
    template<typename Func>
    void addRoute(const string& method, const string& path, const vector<string>& param_names, Func api_func) {
        // routes.push_back({method, path, param_names, handler});
        routes.emplace_back(method, path, param_names, 
            // handler
            [api_func, method](const Request& req) -> Response {
                auto api_func_result = api_func(req);
                string body = to_string(api_func_result);
                if (method == "POST") {
                    return Response(201, body);
                } else {
                    return Response(200, body);
                }
            }
        );
    }

    template<typename Func>
    void get(const string& path, Func api_func) {
        addRoute("GET", path, {}, api_func);
    }
    template<typename Func>
    void get(const string& path, const vector<string>& param_names, Func api_func) {
        addRoute("GET", path, param_names, api_func);
    }

    template<typename Func>
    void post(const string& path, Func api_func) {
        addRoute("POST", path, {}, api_func);
    }
    template<typename Func>
    void post(const string& path, const vector<string>& param_names, Func api_func) {
        addRoute("POST", path, param_names, api_func);
    }

    template<typename Func>
    void put(const string& path, Func api_func) {
        addRoute("PUT", path, {}, api_func);
    }
    template<typename Func>
    void put(const string& path, const vector<string>& param_names, Func api_func) {
        addRoute("PUT", path, param_names, api_func);
    }

    template<typename Func>
    void del(const string& path, Func api_func) {
        addRoute("DELETE", path, {}, api_func);
    }
    template<typename Func>
    void del(const string& path, const vector<string>& param_names, Func api_func) {
        addRoute("DELETE", path, param_names, api_func);
    }

    Response handle_request(const string& request_str) { // web_server 会调用这个方法来 接收 请求字符串，得到响应字符串
        Request request(request_str);
        cout << "Api::handle_request: Method: " << request.method << ", Path: " << request.path << endl;
        for (const auto& route : routes) {
            if (route.method == request.method) {
                regex route_regex(route.path);
                smatch matches;
                if (regex_match(request.path, matches, route_regex)) {
                    vector<string> param_values;
                    for (size_t i = 1; i < matches.size(); ++i) {
                        param_values.push_back(matches[i].str());
                    }
                    request.set_path_params(route.param_names, param_values);
                    cout << "Api::handle_request: 准备调用处理函数: 准备获得响应体" << endl; // 调用处理函数
                    try {
                        Response response = route.handler(request);
                        cout << "Api::handle_request: 调用处理函数: 成功获得响应体: " 
                        // << response.body 
                        << endl; // 调用处理函数
                        return response;
                    } catch (const runtime_error& e) {
                        return Response(409, Json().set("error", e.what()).dumps()); // 409 Conflict
                    } catch (const exception& e) {
                        return Response(500, Json().set("error", e.what()).dumps());
                    }
                }
            }
        }
        return Response(404, "<div>404</div>");
    }

    void print_routes() const {
        cout << "Registered[注册的] Routes:" << endl;
        for (const auto& route : routes) {
            cout << route.method << " " << route.path << endl;
        }
    }

private:
    vector<Route> routes;
};