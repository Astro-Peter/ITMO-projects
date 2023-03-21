//
// Created by Vanst on 12.02.2023.
//

#ifndef LAB7_TAGS_HEADER_H_
#define LAB7_TAGS_HEADER_H_
#include <cstdint>
#include <bitset>
#include <fstream>
#include "Methods.h"

namespace header {

class Header {
public:
    bool correct_ = true;
    char identifier_[3];
    uint16_t version_;
    std::bitset<8> flags_;
    size_t size_;
    size_t extended_header_size_ = 0;

    explicit Header(std::fstream& file);
    void DisplayInfo();
};
} // header

#endif //LAB7_TAGS_HEADER_H_
