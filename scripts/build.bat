@echo off
echo Compiling the program...
g++ -std=c++11 server.cpp -o server.exe -I include -lws2_32 -lboost_system
g++ -std=c++11 user_client.cpp -o user_client.exe -I include -lws2_32 -lboost_system
g++ -std=c++11 admin_client.cpp -o admin_client.exe -I include -lws2_32 -lboost_system
if %errorlevel% neq 0 (
    echo Compilation failed!
    exit /b %errorlevel%
)
echo Compilation successful!
pause