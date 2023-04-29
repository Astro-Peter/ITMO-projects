//
// Created by Vanst on 30.03.2023.
//

#ifndef LABWORK_9_ASTRO_PETER_ALLOC_LIB_NODE_H_
#define LABWORK_9_ASTRO_PETER_ALLOC_LIB_NODE_H_


struct Node{
    using size_type = size_t;
    void* memory;
    size_type space;
    bool free = true;
};

#endif //LABWORK_9_ASTRO_PETER_ALLOC_LIB_NODE_H_
