
## 应输入“;”C/C++(65)
### 函数定义不能嵌套在另一个函数内部
> 错误是因为在 main 函数内部定义了一个函数 api_func，这在 C++ 中是不允许的。函数定义不能嵌套在另一个函数内部。你需要将 api_func 函数定义移到 main 函数外部
```cpp
#include <variant>
#include <string>
using namespace std;

using ApiFuncResult = variant<const char*, string, string_view, char, int, double, bool>;

auto api_lambda_func = []() -> ApiFuncResult { // lambda 表达式
    return "Hello, World!";
};
int main(){
    ApiFuncResult api_func() {
        return "Hello, World!";
    }
}
```
#### 修改

##### 方案1: 将 api_func 函数定义移到 main 函数外部
```cpp
#include <variant>
#include <string>
using namespace std;

using ApiFuncResult = variant<const char*, string, string_view, char, int, double, bool>;
ApiFuncResult api_func() {
    return "Hello, World!";
}
auto api_lambda_func = []() -> ApiFuncResult { // lambda 表达式
    return "Hello, World!";
};
int main(){
}
```
##### 方案2: 将 api_func 函数定义为 lambda 表达式
```cpp
#include <variant>
#include <string>
using namespace std;

using ApiFuncResult = variant<const char*, string, string_view, char, int, double, bool>;

int main(){
    auto api_func = []() -> ApiFuncResult { // lambda 表达式
        return "Hello, World!";
    };
}
```
