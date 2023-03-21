//
// Created by Vanst on 04.11.2022.
//
#include <cstdint>
#ifndef LABWORK_4_ASTRO_PETER_FILE_STRUCTURE_HAFFILEHEADER_H_
#define LABWORK_4_ASTRO_PETER_FILE_STRUCTURE_HAFFILEHEADER_H_


struct HAFFileHeader {
    uint64_t number_of_columns;
    uint64_t number_of_files;
};

#endif //LABWORK_4_ASTRO_PETER_FILE_STRUCTURE_HAFFILEHEADER_H_
