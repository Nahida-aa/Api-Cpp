# types

## str

- `char`: 一个字节，通常用于表示 ASCII 字符。
-  `char*` or `const char*`: C 风格的字符串, 字符串指针，指向以`\0` 符结尾的字符数组。
- `wchar_t`: 两个字节，通常用于表示 Unicode 字符。
- `char16_t`: 两个字节，用于表示 UTF-16 字符。
- `char32_t`: 四个字节，用于表示 UTF-32 字符。
- `std::string`: C++ 标准库提供的字符串类，封装了动态分配的字符数组，并提供了许多字符串操作的成员函数
- `std::wstring`: 一个字符串，由多个 `wchar_t` 组成。
- `std::u16string`: 一个字符串，由多个 `char16_t` 组成。
- `std::u32string`: 一个字符串，由多个 `char32_t` 组成。
- `std::string_view`: C++17 引入的轻量级字符串视图类，表示一个不拥有字符数据的字符串视图
- `std::wstring_view`: 一个字符串视图，不拥有字符串的所有权。
- `std::u16string_view`: 一个字符串视图，不拥有字符串的所有权。
- `std::u32string_view`: 一个字符串视图，不拥有字符串的所有权。

### C 语言中的类型

#### 基本类型
- `char`
- `int`
- `float`
- `double`

#### 修饰符
- `signed`
- `unsigned`
- `short`
- `long`

#### 枚举类型
- `enum`

#### 复合类型
- `struct`
- `union`
- `array`
- `pointer`

#### 空类型
- `void`

### C++ 语言中的类型

#### 基本类型
- `bool` (C++98)
- `char`
- `wchar_t` (C++98)
- `char16_t` (C++11)
- `char32_t` (C++11)
- `int`
- `float`
- `double`

#### 修饰符
- `signed`
- `unsigned`
- `short`
- `long`

#### 枚举类型
- `enum`
- `enum class` (C++11)

#### 复合类型
- `struct`
- `union`
- `array`
- `pointer`
- `reference`
- `std::array` (C++11)
- `std::vector` (C++98)
- `std::list` (C++98)
- `std::deque` (C++98)
- `std::forward_list` (C++11)
- `std::map` (C++98)
- `std::unordered_map` (C++11)
- `std::set` (C++98)
- `std::unordered_set` (C++11)
- `std::tuple` (C++11)
- `std::pair` (C++98)

#### 空类型
- `void`

#### 特殊类型
- `std::nullptr_t` (C++11)

#### 类型别名
- `typedef`
- `using` (C++11)

#### 类型推导
- `auto` (C++11)
- `decltype` (C++11)

#### 用户自定义类型
- 类（`class`）

### C++20 和 C++23 新增类型

#### C++20
- `std::span`
- `std::bitset`
- `std::optional`
- `std::variant`
- `std::any`
- `std::byte`

#### C++23
- `std::expected`
- `std::generator`
- `std::move_only_function`

## get_type

`typeid(decltype(a)).name()` 和 `typeid(a).name()` 都用于获取变量 `a` 的类型信息，但它们在某些情况下有细微的区别。

### `typeid(a).name()`

- 直接获取变量 `a` 的类型信息。
- 如果 `a` 是一个对象，返回对象的类型。
- 如果 `a` 是一个引用，返回引用所指向的对象的类型。
- 如果 `a` 是一个指针，返回指针类型。

### `typeid(decltype(a)).name()`

- 使用 `decltype` 获取变量 `a` 的类型，然后再获取该类型的信息。
- `decltype` 会精确地获取变量的类型，包括引用和常量限定符。
- 如果 `a` 是一个对象，返回对象的类型。
- 如果 `a` 是一个引用，返回引用类型。
- 如果 `a` 是一个指针，返回指针类型。

### 示例

```cpp
#include <iostream>
#include <typeinfo>

int main() {
    int x = 42;
    int& ref = x;
    const int cx = 42;
    const int& cref = cx;

    std::cout << "typeid(x).name(): " << typeid(x).name() << std::endl;
    std::cout << "typeid(decltype(x)).name(): " << typeid(decltype(x)).name() << std::endl;

    std::cout << "typeid(ref).name(): " << typeid(ref).name() << std::endl;
    std::cout << "typeid(decltype(ref)).name(): " << typeid(decltype(ref)).name() << std::endl;

    std::cout << "typeid(cx).name(): " << typeid(cx).name() << std::endl;
    std::cout << "typeid(decltype(cx)).name(): " << typeid(decltype(cx)).name() << std::endl;

    std::cout << "typeid(cref).name(): " << typeid(cref).name() << std::endl;
    std::cout << "typeid(decltype(cref)).name(): " << typeid(decltype(cref)).name() << std::endl;

    return 0;
}
```

### 输出示例

```plaintext
typeid(x).name(): i
typeid(decltype(x)).name(): i
typeid(ref).name(): i
typeid(decltype(ref)).name(): i&
typeid(cx).name(): i
typeid(decltype(cx)).name(): i const
typeid(cref).name(): i
typeid(decltype(cref)).name(): i const&
```

从输出中可以看出：
- `typeid(x).name()` 和 `typeid(decltype(x)).name()` 返回相同的类型信息，因为 `x` 是一个普通的 `int` 变量。
- `typeid(ref).name()` 返回 `int`，而 `typeid(decltype(ref)).name()` 返回 `int&`，因为 `decltype` 保留了引用类型。
- `typeid(cx).name()` 返回 `int`，而 `typeid(decltype(cx)).name()` 返回 `const int`，因为 `decltype` 保留了常量限定符。
- `typeid(cref).name()` 返回 `int`，而 `typeid(decltype(cref)).name()` 返回 `const int&`，因为 `decltype` 保留了引用和常量限定符。

总结：
- `typeid(a).name()` 获取变量 `a` 的类型信息，但不保留引用和常量限定符。
- `typeid(decltype(a)).name()` 使用 `decltype` 获取变量 `a` 的类型信息，保留引用和常量限定符。