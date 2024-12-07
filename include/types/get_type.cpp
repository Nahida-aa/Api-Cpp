#include <iostream>
#include <typeinfo>
#include <functional>

class MyClass {};
struct MyStruct {};

std::function<int(int)> f;

// get_type_name(var)
const char * get_type_name(auto var) {
    return typeid(var).name();
}
int main() {
    int a = 10;
    double b = 3.14;
    std::string c = "Hello";
    MyClass d;
    MyStruct e;
    auto auto_char = "word";

    std::cout << "Type of a: " << typeid(decltype(a)).name() << std::endl;
    std::cout << R"(Type of code["word"]: )" << typeid(decltype("word")).name() << std::endl;
    std::cout << "Type of auto_char: " << typeid(decltype(auto_char)).name() << std::endl;
    std::cout << "Type of b: " << typeid(b).name() << std::endl;
    std::cout << "Type of c: " << typeid(decltype(c)).name() << std::endl;
    std::cout << "Type of d: " << typeid(decltype(d)).name() << std::endl;
    std::cout << "Type of e: " << typeid(e).name() << std::endl;

    return 0;
}