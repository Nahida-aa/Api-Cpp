##

main.cpp:3:24: error: default argument missing for parameter 2 of ‘int add(int, int)’
    3 | int add(int a = 0, int b) {
      |                    ~~~~^
main.cpp:3:13: note: ...following parameter 1 which has a default argument
    3 | int add(int a = 0, int b) {
      |         ~~~~^~~~~

C++ 要求如果一个参数有默认值，那么它后面的所有参数也必须有默认值
