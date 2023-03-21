#include "HammingMethods.h"

int BitSumColumn(uint16_t column, int start) {
    std::bitset<kRowsNumber> bits;
    bits = column;
    int cnt = 0;
    for (int i = start; i < kRowsNumber; i++) {
        cnt += bits[15 - i];
    }
    return cnt;
}

int BitSumRow(const uint16_t* rows, int start, int row, uint64_t number_of_columns) {
    int cnt = 0;
    for (int i = start; i < number_of_columns; i++) {
        std::bitset<kRowsNumber> bits = rows[i];
        cnt += bits[15 - row];
    }
    return cnt;
}

void CheckOrWriteParityBits(uint16_t* result, uint64_t number_of_columns, bool check) {
    bool no_losses = true;
    std::vector<int> columns_with_losses;
    std::vector<int> rows_with_losses;
    for (int i = 0; i < log2(number_of_columns); i++) {
        int cnt = 0;
        int column = static_cast<int>(std::pow(2, i));
        cnt += BitSumColumn(result[column], 1);
        for (int k = 1; k < column; k++) {
            cnt += BitSumColumn(result[k + column], 0);
        }
        for (int j = 3 * column; j < number_of_columns; j += 2 * column) {
            for (int k = 0; k < column; k++) {
                cnt += BitSumColumn(result[k + j], 0);
            }
        }
        if (check) {
            if (cnt % 2 != static_cast<std::bitset<kRowsNumber>>(result[column])[15]) {
                no_losses = false;
                columns_with_losses.push_back(column);
            }
        } else {
            result[column] += (cnt % 2) * static_cast<uint16_t>(std::pow(2, 15));
        }
    }
    for (int i = 0; i < 4; i++) {
        int cnt = 0;
        int row = static_cast<int>(std::pow(2, i));
        cnt += BitSumRow(result, 1, row, number_of_columns);
        for (int k = 1; k < row; k++) {
            cnt += BitSumRow(result, 0, row + k, number_of_columns);
        }
        for (int j = 3 * row; j < kRowsNumber; j += 2 * row) {
            for (int k = 0; k < row; k++) {
                cnt += BitSumRow(result, 0, k + j, number_of_columns);
            }
        }
        if (check) {
            if (cnt % 2 != static_cast<std::bitset<kRowsNumber>>(result[0])[15 - row]) {
                no_losses = false;
                rows_with_losses.push_back(row);
            }
        } else {
            result[0] += (cnt % 2) * static_cast<uint16_t>(std::pow(2, 15 - row));
        }
    }
    int cnt_total = 0;
    cnt_total += BitSumColumn(result[0], 1);
    for (int i = 1; i < number_of_columns; i++) {
        cnt_total += BitSumColumn(result[i], 0);
    }
    if (check) {
        if (cnt_total % 2 == static_cast<std::bitset<kRowsNumber>>(result[0])[15]) {
            if (!no_losses) {
                std::cout << "\ntoo many losses\n";
                return;
            }
        } else {
            no_losses = false;
        }
    } else {
        auto tmp = static_cast<std::bitset<kRowsNumber>>(result[0]);
        tmp[15] = cnt_total % 2;
        result[0] = static_cast<uint16_t>(tmp.to_ulong());
    }
    if (check && !no_losses) {
        bool right_number;
        int column = 0;
        int row = 0;
        if (!columns_with_losses.empty()) {
            for (int i = 1; i < number_of_columns; i++) {
                right_number = true;
                for (int columns_with_loss : columns_with_losses) {
                    if ((i / columns_with_loss) % 2 == 1) {
                        continue;
                    } else {
                        right_number = false;
                    }
                }
                if (right_number) {
                    column = i;
                    break;
                }
            }
        }
        if (!rows_with_losses.empty()) {
            for (int i = 1; i < kRowsNumber; i++) {
                right_number = true;
                for (int rows_with_loss : rows_with_losses) {
                    if ((i / rows_with_loss) % 2 == 1) {
                        continue;
                    } else {
                        right_number = false;
                    }
                }
                if (right_number) {
                    row = i;
                    break;
                }
            }
        }
        if (column == 0 && row == 0) {
            std::cout << "\ntoo many losses\n";
        } else {
            std::cout << "losses in column " << column << " row " << row << '\n';
            std::bitset<kRowsNumber> column_error = result[column];
            column_error[15 - row] = !column_error[15 - row];
            result[column] = static_cast<uint16_t>(column_error.to_ulong());
            std::cout << "\nlosses fixed\n";
        }
    } else if (check) {
        std::cout << "\nno losses\n";
    }
}

void TransformToRegularFile(ReverseConversion& output, uint16_t* rows, uint64_t number_of_columns) {
    for (int i = 0; i < log2(number_of_columns); i++) {
        for (int j = static_cast<int>(pow(2, i)) + 1; j < pow(2, i + 1); j++) {
            if (output.passed_bits == -1) {
                output.passed_bits = 7;
                if (output.file_size > 0) {
                    char byte = static_cast<char>(output.current_byte.to_ulong());
                    output.output_file.write(&byte, 1);
                    output.file_size--;
                }
            }
            output.current_byte[output.passed_bits--] = (static_cast<std::bitset<kRowsNumber>>(rows[j])[15]);
        }
    }
    for (int i = 1; i < kRowsNumber; i++) {
        int start = 0;
        if (i == 1 || i == 2 || i == 4 || i == 8) {
            start = 1;
        }
        for (int j = start; j < number_of_columns; j++) {
            if (output.passed_bits == -1) {
                output.passed_bits = 7;
                if (output.file_size > 0) {
                    char byte = static_cast<char>(output.current_byte.to_ulong());
                    output.output_file.write(&byte, 1);
                    output.file_size--;
                }
            }
            output.current_byte[output.passed_bits--] = (static_cast<std::bitset<kRowsNumber>>(rows[j])[15 - i]);
        }
    }
}

uint16_t* MakeRows(HammingConversion* input, uint64_t number_of_columns) {
    uint16_t* result;
    result = new uint16_t[number_of_columns] {};
    for (int i = 0; i < log2(number_of_columns); i++) {
        for (int j = static_cast<int>(pow(2, i)) + 1; j < pow(2, i + 1); j++) {
            if (input->passed_bits == -1) {
                input->passed_bits = 7;
                char byte;
                if (input->input_file.get(byte)) {
                    input->current_byte = static_cast<std::bitset<8>>(byte);
                } else {
                    input->current_byte = 0;
                }
            }
            result[j] += static_cast<uint16_t>(input->current_byte[input->passed_bits--] * std::pow(2, 15));
        }
    }
    for (int i = 1; i < kRowsNumber; i++) {
        int start = 0;
        if (i == 1 || i == 2 || i == 4 || i == 8) {
            start = 1;
        }
        for (int j = start; j < number_of_columns; j++) {
            if (input->passed_bits == -1) {
                input->passed_bits = 7;
                char byte;
                if (input->input_file.get(byte)) {
                    input->current_byte = static_cast<std::bitset<8>>(byte);
                } else {
                    input->current_byte = 0;
                }
            }
            result[j] += static_cast<uint16_t>(input->current_byte[input->passed_bits--] * std::pow(2, 15 - i));
        }
    }
    CheckOrWriteParityBits(result, number_of_columns, false);
    return result;
}