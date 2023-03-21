//
// Created by Vanst on 07.02.2023.
//

#ifndef LAB7_COMMON_METHODS_METHODS_H_
#define LAB7_COMMON_METHODS_METHODS_H_
#include <cstdint>
#include <iostream>
#include <bitset>
#include <fstream>
#include <codecvt>

namespace common_methods {

size_t GetSynchSafeInteger(std::string& bytes);

bool HeaderCheckFlagsCorrectness(std::bitset<8> flags);

bool FrameCheckFlagsCorrectness(std::bitset<16> flags);

void OutputFrameEncoding(char encoding);

void OutputTimeStampFormat(char format);

void OutputETCOEvent(std::pair<uint8_t, uint32_t> event);

void OutputChannelType(char channel);

void OutputContentType(char type);

void Received(char received_as);

size_t GetUTF16StringToUTF8(std::string& target, std::fstream& file, size_t size);
} // common_methods

#endif //LAB7_COMMON_METHODS_METHODS_H_
