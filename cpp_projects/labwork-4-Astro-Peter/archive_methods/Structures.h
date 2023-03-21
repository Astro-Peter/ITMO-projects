//
// Created by Vanst on 08.11.2022.
//

#ifndef LABWORK_4_ASTRO_PETER_ARCHIVE_METHODS_STRUCTURES_H_
#define LABWORK_4_ASTRO_PETER_ARCHIVE_METHODS_STRUCTURES_H_

#include <bitset>
#include <fstream>
#include <cstdint>
#include <vector>

struct HammingConversion {
    std::ifstream input_file;
    std::bitset<8> current_byte;
    int8_t passed_bits = 7;
};

struct ReverseConversion {
    std::ofstream output_file;
    std::bitset<8> current_byte;
    int8_t passed_bits = -1;
    uint64_t file_byte_cnt;
    uint64_t file_size;
};

struct InputArgs {
    bool create {false};
    std::string archive_name;
    bool list {false};
    bool extract {false};
    bool extract_all {false};
    bool append {false};
    bool delete_file {false};
    std::vector<std::string> files;
    bool merge {false};
    std::string first_to_merge;
    std::string second_to_merge;
    uint64_t number_of_columns {16};
};

#endif //LABWORK_4_ASTRO_PETER_ARCHIVE_METHODS_STRUCTURES_H_
