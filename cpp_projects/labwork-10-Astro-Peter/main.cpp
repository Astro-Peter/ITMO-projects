#include "alloc_lib/TreeAllocator.h"
#include "iostream"
#include "vector"
#include <chrono>

class a{
    int f;
    float k;

    bool abs() {
        return true;
    }
};

template<class alloc>
void time() {
    std::vector<a, alloc> g;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1 << 21; i++) {
        a l{};
        g.push_back(l);
        g.push_back(l);
        g.push_back(l);
    }
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() << '\n';
}


int main() {
    time<std::allocator<a>>();
    time<TreeAllocator<a>>();
    return 0;
}