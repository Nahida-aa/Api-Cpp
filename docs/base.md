
```sh
# g++
## Linux
g++ -std=c++11 -O2 base.cpp -o base && ./base
## Windows 
g++ -std=c++20 -Iinclude -O2 base.cpp -o base -lws2_32 && base
g++ -std=c++20 -Iinclude -O2  v4p.cpp -o v4p -lws2_32
```
