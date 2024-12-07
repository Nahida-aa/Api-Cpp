#pragma once
#include <iostream>

int add(int a = 0, int b = 0) {
    std::cout << "Function: add: a: "<< a <<" + b: "<< b<<" = " << a+b << std::endl;
    return a + b;
}

// 定义: 模板函数是泛型函数，可以处理不同类型的数据, 用途：用于编写与类型无关的代码，允许在编译时生成特定类型的函数
template <typename T>
T add_t(T a , T b = 0) {
    return a + b;
}

// 定义：Lambda 函数是匿名函数，可以在函数内部定义并使用; 用途：用于编写简短的函数，特别是在需要传递函数作为参数时
auto add_l = [](int a, int b = 0, int c = 0) {
    return a + b;
};

void base_test(){
    int result = add(1);
    std::cout << "Result0: " << result << std::endl;
    std::cout << "Result1: " << add_t(1) << std::endl;
    std::cout << "Result2: " << add_l(1) << std::endl;
}