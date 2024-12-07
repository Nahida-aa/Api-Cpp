@echo off
setlocal

REM 设置编译器和编译选项
set CXX=g++
set CXXFLAGS=-std=c++20 -Iinclude -O2
set LDFLAGS=-pthread

REM 设置源文件和目标文件
set SRCDIR=tests
set BUILDDIR=build\tests

REM 创建构建目录
if not exist %BUILDDIR% (
    mkdir %BUILDDIR%
)

REM 编译和运行 test_json.cpp
%CXX% %CXXFLAGS% %SRCDIR%\include\types\test_json.cpp -o %BUILDDIR%\include\types\test_json.exe %LDFLAGS%
if %errorlevel% equ 0 (
    echo 编译成功，生成可执行文件：%BUILDDIR%\include\types\test_json.exe
    echo 正在运行 %BUILDDIR%\include\types\test_json.exe...
    %BUILDDIR%\include\types\test_json.exe
) else (
    echo 编译失败
)

REM 编译和运行 test_flight.cpp
@REM g++ -std=c++20 -Iinclude -O2 tests\include\app\test_flight.cpp -o build\tests\include\app\test_flight.exe -pthread
%CXX% %CXXFLAGS% %SRCDIR%\include\app\test_flight.cpp -o %BUILDDIR%\include\app\test_flight.exe %LDFLAGS%
if %errorlevel% equ 0 (
    echo 编译成功，生成可执行文件：%BUILDDIR%\include\app\test_flight.exe
    echo 正在运行 %BUILDDIR%\include\app\test_flight.exe...
    %BUILDDIR%\include\app\test_flight.exe
) else (
    echo 编译失败
)

REM 编译和运行 test_passenger.cpp
%CXX% %CXXFLAGS% %SRCDIR%\include\app\test_passenger.cpp -o %BUILDDIR%\include\app\test_passenger.exe %LDFLAGS%
if %errorlevel% equ 0 (
    echo 编译成功，生成可执行文件：%BUILDDIR%\include\app\test_passenger.exe
    echo 正在运行 %BUILDDIR%\include\app\test_passenger.exe...
    %BUILDDIR%\include\app\test_passenger.exe
) else (
    echo 编译失败
)

REM 编译和运行 test_user.cpp
%CXX% %CXXFLAGS% %SRCDIR%\include\app\test_user.cpp -o %BUILDDIR%\include\app\test_user.exe %LDFLAGS%
if %errorlevel% equ 0 (
    echo 编译成功，生成可执行文件：%BUILDDIR%\include\app\test_user.exe
    echo 正在运行 %BUILDDIR%\include\app\test_user.exe...
    %BUILDDIR%\include\app\test_user.exe
) else (
    echo 编译失败
)

endlocal