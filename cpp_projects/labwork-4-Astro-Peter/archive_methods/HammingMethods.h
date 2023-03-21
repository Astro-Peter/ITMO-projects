//
// Created by Vanst on 08.11.2022.
//

#ifndef LABWORK_4_ASTRO_PETER_ARCHIVE_METHODS_HAMMINGMETHODS_H_
#define LABWORK_4_ASTRO_PETER_ARCHIVE_METHODS_HAMMINGMETHODS_H_

#include <cstdint>
#include <iostream>
#include "Structures.h"

static const uint64_t kRowsNumber = 16;

int BitSumColumn(uint16_t column, int start);

int BitSumRow(const uint16_t* rows, int start, int row, uint64_t number_of_columns);

void CheckOrWriteParityBits(uint16_t* result, uint64_t number_of_columns, bool check);

void TransformToRegularFile(ReverseConversion& output, uint16_t* rows, uint64_t number_of_columns);

uint16_t* MakeRows(HammingConversion* input, uint64_t number_of_columns);

#endif //LABWORK_4_ASTRO_PETER_ARCHIVE_METHODS_HAMMINGMETHODS_H_
