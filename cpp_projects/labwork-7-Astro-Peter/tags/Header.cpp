//
// Created by Vanst on 12.02.2023.
//

#include "Header.h"

namespace header {
Header::Header(std::fstream& file) {
    file.read(identifier_, 3);
    file.read(reinterpret_cast<char*>(&version_), 2);
    char f;
    file.get(f);
    flags_ = f;
    std::string synch_safe_size(4, ' ');
    file.read(&synch_safe_size[0], 4);
    std::cout << file.tellg() << '\n';
    size_ = common_methods::GetSynchSafeInteger(synch_safe_size);
    if (flags_[1]){

    }
}

void Header::DisplayInfo() {
    std::cout << "identifier: " << identifier_ << std::endl;
    std::cout << "version: " << version_ << std::endl;
    std::cout << "flags: " << flags_ << std::endl;
    std::cout << "size: " << size_ << std::endl;

}
} // header