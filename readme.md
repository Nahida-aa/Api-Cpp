
```sh
g++ -g main.cpp -o main.exe
main.exe
```
```sh
g++ -std=c++20 -O2 -Iinclude -lboost_system -lssl -lcrypto -pthread server_crow.cpp -o server_crow
g++ -g server_crow.cpp -o server_crow

g++ -std=c++20 -Iinclude -O2 main.cpp -o main
 -lws2_32 -lpthread
```
这个命令用于编译和链接一个名为 

server_crow.cpp

 的 C++ 源文件，并生成一个名为 `server_crow` 的可执行文件。以下是命令中各个部分的解释：

- `g++`: 调用 GNU C++ 编译器。
- `-std=c++11`: 使用 C++11 标准进行编译。
- `-O2`: 启用优化选项，以提高生成代码的运行速度。1
- `-lboost_system`: 链接 Boost.System 库。
- `-lssl`: 链接 OpenSSL 库。
- `-lcrypto`: 链接 OpenSSL 的加密库。
- `-pthread`: 启用多线程支持。

server_crow.cpp

: 要编译的源文件。
- `-o server_crow`: 指定输出的可执行文件名为 `server_crow`。

这个命令会将 

server_crow.cpp

 编译成一个可执行文件 `server_crow`，并且在编译过程中使用了 C++11 标准、优化选项以及所需的库。

# 项目名称

## 项目简介
这是一个使用 Crow 框架构建的航班信息管理系统示例，包含服务端、用户客户端和管理客户端。

## 使用方法
1. 解压缩 `project.zip` 文件到一个文件夹中。
2. 双击 `build.bat` 文件编译程序。
3. 编译成功后，双击 `run_server.bat` 文件运行服务端。
4. 双击 `run_user_client.bat` 文件运行用户客户端。
5. 双击 `run_admin_client.bat` 文件运行管理客户端。

## 路由示例
- `/`：返回 "Hello, World!"
- `/add/<int>/<int>`：返回两个整数的和。

## 注意事项
确保系统中已安装 g++ 编译器和 Boost 库。
