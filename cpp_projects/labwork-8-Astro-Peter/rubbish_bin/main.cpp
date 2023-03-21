#include "lib/CCircularBuffer.h"
#include <iostream>

int main() {
    CircBuff::CCircularBuffer<int> a(40);
    a = {10, 42, 13, 50, 43, 23, 5};
    for (auto& i: a) {
        std::cout << i << '\n';
    }
    return 0;
}