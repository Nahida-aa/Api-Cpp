
```h
#pragma once //
#include "crow/query_string.h" // 处理 URL 查询字符串
#include "crow/http_parser_merged.h" // HTTP 解析器
...
```

`#pragma once` 是一个非标准但广泛支持的预处理指令，用于防止头文件被多次包含。它的作用类似于传统的头文件保护宏（include guard），但更简洁和高效。

### 头文件保护宏 vs. `#pragma once`
传统的头文件保护宏通常是这样的：

```cpp
#ifndef CROW_H
#define CROW_H

// 头文件内容

#endif // CROW_H
```

而使用 `#pragma once` 可以简化为：

```cpp
#pragma once

// 头文件内容
```

### 优点
1. **简洁**：`#pragma once` 只需要一行代码，而头文件保护宏通常需要三行代码。
2. **避免冲突**：头文件保护宏需要定义唯一的宏名，如果不小心使用了相同的宏名，可能会导致冲突。而 `#pragma once` 不需要宏名，避免了这种风险。
3. **高效**：编译器可以更高效地处理 `#pragma once`，因为它不需要解析宏定义。

### 支持
虽然 `#pragma once` 不是标准的 C++ 预处理指令，但几乎所有现代编译器都支持它，包括：
- GCC
- Clang
- MSVC
- Intel C++ Compiler
