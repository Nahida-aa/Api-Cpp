#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include <unordered_map>
#include <vector>
#include <thread>
#include <mutex>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <regex>
#include "web/api.hpp"
#include "web/response.hpp"

#pragma comment(lib, "Ws2_32.lib")

// Config class (unchanged)
class Config {
public:
    std::string host;
    int port;
    bool auto_increment_port;

    Config(const std::string& host = "127.0.0.1", int port = 9339, bool auto_increment_port = true)
        : host(host), port(port), auto_increment_port(auto_increment_port) {}
};

// WebServer class
class WebServer {
public:
    WebServer(const Config& config, Api& api)
        : config_(config), api_(api), ListenSocket(INVALID_SOCKET) {}

    void run() {
        std::cout << "Nahida-aa web server and api" << std::endl;
        if (!initialize()) {
            return;
        }

        if (!bind_and_listen()) {
            cleanup();
            return;
        }

        std::cout << "Server listening on http://" << config_.host << ":" << config_.port << std::endl;
        api_.print_routes(); // 打印所有注册的路由

        while (running) {
            handle_client();
        }

        cleanup();
    }

private:
    Config config_;
    Api& api_;
    SOCKET ListenSocket;
    static volatile bool running;
    static const int MAX_PORT_ATTEMPTS = 10;
    std::mutex client_mutex;

    bool initialize() {
        if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
            std::cerr << "Error: Could not set control handler" << std::endl;
            return false;
        }

        WSADATA wsaData;
        int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            std::cerr << "WSAStartup failed: " << iResult << std::endl;
            return false;
        }

        ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (ListenSocket == INVALID_SOCKET) {
            std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
            WSACleanup();
            return false;
        }

        return true;
    }

    bool bind_and_listen() {
        int attempts = 0;
        while (attempts < MAX_PORT_ATTEMPTS) {
            if (bind_to_port()) {
                break;
            }
            if (config_.auto_increment_port) {
                config_.port++;
                attempts++;
            } else {
                return false;
            }
        }

        if (attempts == MAX_PORT_ATTEMPTS) {
            std::cerr << "Failed to bind to any port after " << MAX_PORT_ATTEMPTS << " attempts." << std::endl;
            return false;
        }

        if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
            std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
            return false;
        }

        return true;
    }

    bool bind_to_port() {
        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = inet_addr(config_.host.c_str());
        address.sin_port = htons(config_.port);

        if (bind(ListenSocket, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
            int error = WSAGetLastError();
            if (error == WSAEADDRINUSE || error == WSAEACCES) {
                std::cerr << "Port " << config_.port << " is not available. ";
                if (config_.auto_increment_port) {
                    std::cerr << "Trying next port." << std::endl;
                } else {
                    std::cerr << "Auto-increment is disabled." << std::endl;
                }
            } else {
                std::cerr << "Bind failed with error: " << error << std::endl;
            }
            return false;
        }
        return true;
    }

    void handle_client() {
        SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            if (running) {
                std::cerr << "accept failed: " << WSAGetLastError() << std::endl;
            }
            return;
        }

        std::thread client_thread(&WebServer::process_client, this, ClientSocket);
        client_thread.detach();
    }

    void process_client(SOCKET ClientSocket) {
        std::vector<char> recvbuf(4096);
        int iResult = recv(ClientSocket, recvbuf.data(), recvbuf.size(), 0);
        if (iResult > 0) {
            std::string request_str(recvbuf.begin(), recvbuf.begin() + iResult);
            std::cout << "Received[收到] request:\n" << request_str << std::endl; // 打印接收到的请求
            // 处理静态文件请求
            if (handle_static_file_request(ClientSocket, request_str)) {
                closesocket(ClientSocket); // 处理完静态文件请求后关闭客户端套接字
                return;
            }

            Response response = api_.handle_request(request_str);
            std::string response_str = response.to_string();
            // std::cout << "Sending[发出] response:\n" << response_str << std::endl; // 打印发送的响应

            int iSendResult = send(ClientSocket, response_str.c_str(), response_str.length(), 0);
            if (iSendResult == SOCKET_ERROR) {
                std::cerr << "send failed: " << WSAGetLastError() << std::endl;
            }
        } else if (iResult == 0) {
            std::cout << "Connection closing..." << std::endl;
        } else {
            std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
        }

        closesocket(ClientSocket);
    }
    bool handle_static_file_request(SOCKET ClientSocket, const std::string& request_str) {
        std::istringstream request_stream(request_str);
        std::string method, path, version;
        request_stream >> method >> path >> version;

        if (method == "GET" && path.find("/public/") == 0) {
            std::string file_path = "." + path; // 假设静态文件存储在当前目录下的 public 文件夹中
            std::ifstream file(file_path, std::ios::binary);
            if (file) {
                std::ostringstream oss;
                oss << file.rdbuf();
                std::string file_content = oss.str();

                std::string response = "HTTP/1.1 200 OK\r\n";
                response += "Content-Length: " + std::to_string(file_content.size()) + "\r\n";
                response += "Content-Type: text/css\r\n"; // 假设所有静态文件都是 CSS 文件
                response += "\r\n";
                response += file_content;

                send(ClientSocket, response.c_str(), response.size(), 0);
                return true;
            } else {
                std::string response = "HTTP/1.1 404 Not Found\r\n";
                response += "Content-Length: 0\r\n";
                response += "\r\n";
                send(ClientSocket, response.c_str(), response.size(), 0);
                return true;
            }
        }
        return false;
    }

    void cleanup() {
        closesocket(ListenSocket);
        WSACleanup();
    }

    static BOOL WINAPI ConsoleHandler(DWORD signal) {
        if (signal == CTRL_C_EVENT) {
            std::cout << "`Ctrl + C` : Shutting down server..." << std::endl;
            running = false;
            // 退出程序
            exit(0);
            return TRUE;
        }
        return FALSE;
    }
};

volatile bool WebServer::running = true;