//
// Created by Vanst on 25.03.2023.
//

#ifndef LABWORK_9_ASTRO_PETER_ALLOC_LIB_POOL_H_
#define LABWORK_9_ASTRO_PETER_ALLOC_LIB_POOL_H_
#include <cstdlib>
#include <memory>

struct Pool {
    using size_type = size_t;
    void* start;
    void* last;
    size_type free_size;

    explicit Pool(size_type size = 1024) {
        start = std::malloc(size + 1);
        last = start;
        free_size = size;
    }

    void* allocate(size_type num, size_type obj_size) {
        if (num * obj_size > free_size) {
            throw std::bad_alloc();
        }
        auto answer = last;
        last = reinterpret_cast<char*>(last) + num * obj_size;
        free_size -= num * obj_size;
        return answer;
    };

    void deallocate(void* p, size_type n) {

    }

    Pool& operator=(Pool const & b)  noexcept = default;;

    ~Pool() {
        std::free(start);
    }
};

bool operator==(Pool const & a, Pool const & b) {
    return (a.last == b.last && a.free_size == b.free_size && a.start == b.start);
}
#endif //LABWORK_9_ASTRO_PETER_ALLOC_LIB_POOL_H_
