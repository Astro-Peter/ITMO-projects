//
// Created by Vanst on 30.03.2023.
//

#ifndef LABWORK_9_ASTRO_PETER_ALLOC_LIB_BETTERPOOL_H_
#define LABWORK_9_ASTRO_PETER_ALLOC_LIB_BETTERPOOL_H_
#include "Pool.h"
#include "Node.h"

Pool stack = Pool(100000);
Pool stack_for_stack = Pool(100000);
Pool map_memory = Pool(100000);

template<class T>
class MapAlloc {
public:
    using size_type = size_t;
    using value_type = T;
    using pointer = T*;
private:


    void PushNode(Node obj) {
        if (static_cast<char*>(stack_for_stack.last) - static_cast<char*>(stack_for_stack.start) != 0) {
            auto dest = reinterpret_cast<Node**>(stack_for_stack.last) - 1;
            **dest = obj;
        } else {
            auto dest = static_cast<Node*>(stack.allocate(1, sizeof(Node)));
            *dest = obj;
        }
    }

    Node* SearchStack(size_type n) {
        Node* return_val = nullptr;
        for (auto start = reinterpret_cast<Node*>(stack.start); start < static_cast<Node*>(stack.last); start++){
            if (start->free && start->space >= n && (return_val == nullptr || return_val->space > start->space)) {
                return_val = start;
            }
        }
        if ((return_val != nullptr)){
            return_val->free = false;
            auto tmp = reinterpret_cast<Node**>(stack_for_stack.allocate(1, sizeof(void*)));
            *tmp = return_val;
        }
        return return_val;
    };
public:
    MapAlloc() = default;

    template<class U>
    explicit MapAlloc(MapAlloc<U> const & b) {
    }

    pointer allocate(size_type n) {
        pointer return_val;
        auto tmp = SearchStack(n);
        if (tmp == nullptr) {
            return_val = static_cast<pointer>(map_memory.allocate(n, sizeof(T)));
        } else {
            if (tmp->space > n * sizeof(T)) {
                Node new_one;
                new_one.memory = static_cast<char*>(tmp->memory) + n * sizeof(T);
                new_one.space = tmp->space - n * sizeof(T);
                PushNode(new_one);
            }
            return_val = static_cast<pointer>(tmp->memory);
        }
        return return_val;
    }

    void deallocate(pointer p, size_type n) {
        Node tmp{.memory = p, .space = n};
        PushNode(tmp);
    }

    ~MapAlloc() = default;

};

template<class U, class T>
bool operator==(MapAlloc<U> a, MapAlloc<T> b) {
    return true;
}

template<class U, class T>
bool operator!=(MapAlloc<U> a, MapAlloc<T> b) {
    return false;
}

#endif //LABWORK_9_ASTRO_PETER_ALLOC_LIB_BETTERPOOL_H_
