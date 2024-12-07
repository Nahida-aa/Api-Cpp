# func cpp

在 C++20 中，函数、模板函数和 lambda 函数有以下区别：

1. **普通函数**：
   - 定义：普通函数是具有特定名称和参数列表的代码块。
   - 用途：用于执行特定任务，可以被多次调用。
   - 示例：
     ```cpp
     int add(int a, int b) {
         return a + b;
     }
     ```

2. **模板函数**：
   - 定义：模板函数是泛型函数，可以处理不同类型的数据。
   - 用途：用于编写与类型无关的代码，允许在编译时生成特定类型的函数。
   - 示例：
     ```cpp
     template <typename T>
     T add(T a, T b) {
         return a + b;
     }
     ```

3. **Lambda 函数**：
   - 定义：Lambda 函数是匿名函数，可以在函数内部定义并使用。
   - 用途：用于编写简短的函数，特别是在需要传递函数作为参数时。
   - 示例：
     ```cpp
     auto add = [](int a, int b) {
         return a + b;
     };
     ```

总结：
- 普通函数有固定的名称和参数类型。
- 模板函数是泛型的，可以处理不同类型的数据。
- Lambda 函数是匿名的，通常用于简短的、临时的函数定义。


在 C++ 中，除了普通函数、模板函数和 lambda 函数之外，还有其他几种类型的函数。以下是一些常见的函数类型：

1. **成员函数**：
   - 定义：属于类的函数，可以访问类的成员变量和其他成员函数。
   - 示例：
     ```cpp
     class MyClass {
     public:
         void memberFunction() {
             // 成员函数代码
         }
     };
     ```

2. **静态成员函数**：
   - 定义：属于类的函数，但不依赖于类的实例，可以通过类名直接调用。
   - 示例：
     ```cpp
     class MyClass {
     public:
         static void staticMemberFunction() {
             // 静态成员函数代码
         }
     };
     ```

3. **虚函数**：
   - 定义：在基类中声明的函数，可以在派生类中重写，实现多态。
   - 示例：
     ```cpp
     class Base {
     public:
         virtual void virtualFunction() {
             // 基类虚函数代码
         }
     };

     class Derived : public Base {
     public:
         void virtualFunction() override {
             // 派生类重写虚函数代码
         }
     };
     ```

4. **纯虚函数**：
   - 定义：在基类中声明但不实现的虚函数，派生类必须实现它。
   - 示例：
     ```cpp
     class Base {
     public:
         virtual void pureVirtualFunction() = 0;
     };

     class Derived : public Base {
     public:
         void pureVirtualFunction() override {
             // 派生类实现纯虚函数代码
         }
     };
     ```

5. **内联函数**：
   - 定义：使用 `inline` 关键字声明的函数，建议编译器将函数体展开到调用点。
   - 示例：
     ```cpp
     inline int add(int a, int b) {
         return a + b;
     }
     ```

6. **友元函数**：
   - 定义：可以访问类的私有和保护成员的非成员函数。
   - 示例：
     ```cpp
     class MyClass {
     private:
         int value;
     public:
         friend void friendFunction(MyClass& obj);
     };

     void friendFunction(MyClass& obj) {
         obj.value = 10; // 访问私有成员
     }
     ```
