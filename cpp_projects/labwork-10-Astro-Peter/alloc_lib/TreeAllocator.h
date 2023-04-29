//
// Created by Vanst on 23.03.2023.
//

#ifndef LABWORK_9_ASTRO_PETER_ALLOC_LIB_TREEALLOCATOR_H_
#define LABWORK_9_ASTRO_PETER_ALLOC_LIB_TREEALLOCATOR_H_
#include <map>
#include <list>
#include <iostream>
#include "Pool.h"
#include "MapAlloc.h"
#include "Node.h"

Pool memory = Pool(1'048'576'0000);
std::map<size_t, std::list<Node, MapAlloc<Node>>, std::less<>, MapAlloc<std::pair<const size_t, std::list<Node, MapAlloc<Node>>>>> memory_map;

template<class T>
class TreeAllocator {
public:
    using list = std::list<Node, MapAlloc<Node>>;
    using value_type = T;
    using size_type = size_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using difference_type = std::ptrdiff_t;

public:

    TreeAllocator() = default;;

    template<class U>
    explicit TreeAllocator(TreeAllocator<U> size) {
    };

    [[nodiscard]] pointer allocate(size_type n) {
        auto poss = memory_map.lower_bound(n * sizeof(T));
        if (poss == memory_map.end()) {
            auto ans = static_cast<pointer>(memory.allocate(n, sizeof(T)));
            return ans;
        } else {
            auto ans = poss->second.back();
            poss->second.pop_back();
            if (poss->second.empty()){
                memory_map.erase(poss);
            }
            return static_cast<pointer>(ans.memory);
        }
    }

    void deallocate(pointer ptr, size_type n) {
        Node tmp;
        tmp.memory = ptr;
        tmp.space = n * sizeof(T);
        bool prev = true;
        bool next = true;
        for (auto i = memory_map.begin(); i != memory_map.end() && (prev || next); i++) {
            auto previt = i->second.end();
            auto nextit = i->second.end();
            for (auto j = i->second.begin(); j != i->second.end(); j++){
                if (reinterpret_cast<pointer>(static_cast<char*>(j->memory) + j->space) == ptr) {
                    tmp.memory = j->memory;
                    tmp.space += j->space;
                    previt = j;
                } else if (static_cast<pointer>(j->memory) == ptr + n) {
                    tmp.space += j->space;
                    nextit = j;
                }
            }
            if (nextit != i->second.end()) {
                next = false;
                i->second.erase(nextit);
            }
            if (previt != i->second.end()) {
                prev = false;
                i->second.erase(previt);
            }
        }
        if (static_cast<char*>(tmp.memory) + tmp.space == static_cast<char*>(memory.last)) {
            memory.last = tmp.memory;
            memory.free_size += tmp.space;
            return;
        }
        auto dest = memory_map.find(tmp.space);
        if (dest == memory_map.end()) {
            list new_list = {tmp};
            dest = memory_map.emplace(tmp.space, new_list).first;
        }
        dest->second.push_back(tmp);
    }

    ~TreeAllocator() = default;

};




template<class T, class U>
bool operator==(const TreeAllocator<T>& a, const TreeAllocator<U>& b) { return true; }

template<class T, class U>
bool operator!=(const TreeAllocator<T>& a, const TreeAllocator<U>& b) { return false; }

#endif //LABWORK_9_ASTRO_PETER_ALLOC_LIB_TREEALLOCATOR_H_
