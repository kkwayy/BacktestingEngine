#include <iostream>
#include <concepts>

// A C++20 concept check
template<typename T>
concept Numeric = std::is_arithmetic_v<T>;

void print_number(Numeric auto val) {
    std::cout << "Value: " << val << "\n";
}

int main() {
    print_number(42);
    return 0;
}