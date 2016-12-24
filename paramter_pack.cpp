
#include <string>
#include <iostream>

template<class T, class ... Types>
const T& get1(const T& t, const Types& ... args) { return t; }

template<class T, class Z, class ... Types>
const Z& get2(const T& t, const Z& z, const Types& ... args) { return z; }

template <class ... Types>
struct Base {
  void operator()(const Types&... args) {
    decltype(get1(args...)) arg1 = get1(args...);
    decltype(get2(args...)) arg2 = get2(args...);
    std::cout << sizeof(arg1) << ',' << arg1 << std::endl;
    std::cout << sizeof(arg2) << ',' << arg2 << std::endl;
  }
};

int main() {
  Base<int,short> x;
  x(1,6);
}
