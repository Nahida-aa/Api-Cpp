@echo off
@REM g++ -std=c++20 -Iinclude -O2 src/flight.cpp  src/user.cpp src/base_cli.cpp -o build/base_cli.exe
setlocal enabledelayedexpansion

REM 设置编译器和编译选项
set CXX=g++

:: 编译器选项
set CXX=g++
set CXXFLAGS=-std=c++20 -Iinclude -O2
@REM set LDFLAGS=-pthread

REM 设置源文件和目标文件
set SRCDIR=src
set BUILDDIR=build
set TARGET=%BUILDDIR%\cli_app.exe

set SOURCES=^
 %SRCDIR%\cli_app.cpp

set OBJECTS=
for %%f in (%SOURCES%) do (
    set OBJECTS=!OBJECTS! %BUILDDIR%\%%~nf.obj
)

REM 创建构建目录
if not exist %BUILDDIR% (
    mkdir %BUILDDIR%
)

@REM :: 编译源文件并链接生成可执行文件
@REM %CXX% %CXXFLAGS% %SRCS% -o %TARGET%
REM 编译源文件
for %%f in (%SOURCES%) do (
    if exist %%f (
        %CXX% %CXXFLAGS% -c %%f -o %BUILDDIR%\%%~nf.obj
    )
)

REM 链接目标文件
%CXX% %CXXFLAGS% -o %TARGET% %OBJECTS% %LDFLAGS%

:: 检查编译是否成功
if %errorlevel% equ 0 (
    echo 编译成功，生成可执行文件：%TARGET%
    echo 正在运行 %TARGET%...
    %TARGET%
) else (
    echo 编译失败
)

endlocal