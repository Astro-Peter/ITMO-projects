//
// Created by Vanst on 07.02.2023.
//

#include "Methods.h"

namespace common_methods {

size_t GetSynchSafeInteger(std::string& bytes){
    size_t byte_0 = static_cast<unsigned char>(bytes[0]);
    size_t byte_1 = static_cast<unsigned char>(bytes[1]);
    size_t byte_2 = static_cast<unsigned char>(bytes[2]);
    size_t byte_3 = static_cast<unsigned char>(bytes[3]);
    return (byte_0 << 21) + (byte_1 << 14) + (byte_2 << 7) + byte_3;
}

bool HeaderCheckFlagsCorrectness(std::bitset<8> flags){
    return !(flags[4] || flags[5] || flags[6] || flags[7]);
}

bool FrameCheckFlagsCorrectness(std::bitset<16> flags){
    return !(flags[0] || flags[4] || flags[5] || flags[6] || flags[7] || flags[8] || flags[10] || flags[11]);
}

void OutputFrameEncoding(char encoding){
    std::cout << "encoding: ";
    switch (encoding) {
        case 0:
            std::cout << "ISO-8859-1 [ISO-8859-1]. Terminated with $00.\n";
            break;
        case 1:
            std::cout << "UTF-16 [UTF-16] encoded Unicode [UNICODE] with BOM. All\n"
                      << "strings in the same frame SHALL have the same byteorder.\n"
                      << "Terminated with $00 00.\n";
            break;
        case 2:
            std::cout << "UTF-16BE [UTF-16] encoded Unicode [UNICODE] without BOM.\n"
                      << "Terminated with $00 00.\n";
            break;
        case 3:
            std::cout << "UTF-8 [UTF-8] encoded Unicode [UNICODE]. Terminated with $00.\n";
            break;
    }
}

void OutputETCOEvent(std::pair<uint8_t, uint32_t> event) {
    switch (event.first) {
        case 0x0:
            std::cout << "padding (has no meaning)\n";
            break;
        case 0x1:
            std::cout << "end of initial silence\n";
            break;
        case 0x2:
            std::cout << "intro start\n";
            break;
        case 0x3:
            std::cout << "main part start\n";
            break;
        case 0x4:
            std::cout << "outro start\n";
            break;
        case 0x5:
            std::cout << "outro end\n";
            break;
        case 0x6:
            std::cout << "verse start\n";
            break;
        case 0x7:
            std::cout << "refrain start\n";
            break;
        case 0x8:
            std::cout << "interlude start\n";
            break;
        case 0x9:
            std::cout << "theme start\n";
            break;
        case 0xA:
            std::cout << "variation start\n";
            break;
        case 0xB:
            std::cout << "key change\n";
            break;
        case 0xC:
            std::cout << "time change\n";
            break;
        case 0xD:
            std::cout << "momentary unwanted noise (Snap, Crackle & Pop)\n";
            break;
        case 0xE:
            std::cout << "sustained noise\n";
            break;
        case 0xF:
            std::cout << "sustained noise end\n";
            break;
        case 0x10:
            std::cout << "intro end\n";
            break;
        case 0x11:
            std::cout << "main part end\n";
            break;
        case 0x12:
            std::cout << "verse end\n";
            break;
        case 0x13:
            std::cout << "refrain end\n";
            break;
        case 0x14:
            std::cout << "theme end\n";
            break;
        case 0x15:
            std::cout << "profanity\n";
            break;
        case 0x16:
            std::cout << "profanity end\n";
            break;
        case 0xFD:
            std::cout << "audio end (start of silence)\n";
            break;
        case 0xFE:
            std::cout << "audio file ends\n";
            break;
        case 0xFF:
            std::cout << "one more byte of events follows (all the following bytes with\n"
                      << "the value $FF have the same function)\n";
            break;
        default:
            std::cout << "reserved for future use\n";
            break;
    }
    std::cout << "timestamp: " << event.second << std::endl;
}

void OutputTimeStampFormat(char format) {
    std::cout << "timestamp format: ";
    if (format == 1){
        std::cout << "Absolute time, 32 bit sized, using MPEG [MPEG] frames as unit\n";
    } else {
        std::cout << "Absolute time, 32 bit sized, using milliseconds as unit\n";
    }
}

size_t GetUTF16StringToUTF8(std::string& target, std::fstream& file, size_t size) {
    char16_t k = 0;
    while (size > 0){
        file.get(reinterpret_cast<char*>(&k), sizeof(char16_t));
        if (k == 0){
            size -= 2;
            return size;
        }
        std::string inp = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(k);
        target.append(inp);
        size -= 2;
    }
    return 0;
}

void OutputContentType(char type) {
    std::cout << "content type: ";
    switch (type) {
        case 0:
            std::cout << "is other\n";
            break;
        case 1:
            std::cout << "is lyrics\n";
            break;
        case 2:
            std::cout << "is text transcription\n";
            break;
        case 3:
            std::cout << "is movement/part name (e.g. \"Adagio\")\n";
            break;
        case 4:
            std::cout << "is events (e.g. \"Don Quijote enters the stage\")\n";
            break;
        case 5:
            std::cout << "is chord (e.g. \"Bb F Fsus\")\n";
            break;
        case 6:
            std::cout << "is trivia/'pop up' information\n";
            break;
        case 7:
            std::cout << "is URLs to webpages\n";
            break;
        case 8:
            std::cout << "is URLs to images\n";
            break;
    }
}

void OutputChannelType(char channel) {
    std::cout << "channel type: ";
    switch (channel){
        case 0:
            std::cout << "other\n";
            break;
        case 1:
            std::cout << "Master volume\n";
            break;
        case 2:
            std::cout << "Front right\n";
            break;
        case 3:
            std::cout << "Front left\n";
            break;
        case 4:
            std::cout << "Back right\n";
            break;
        case 5:
            std::cout << "Back left\n";
            break;
        case 6:
            std::cout << "Front centre\n";
            break;
        case 7:
            std::cout << "Back centre\n";
            break;
        case 8:
            std::cout << "Subwoofer\n";
            break;
    }
}

void Received(char received_as) {
    std::cout << "received as: ";
    switch (received_as) {
        case 0:
            std::cout << "Other\n";
            break;
        case 1:
            std::cout << "Standard CD album with other songs\n";
            break;
        case 2:
            std::cout << "Compressed audio on CD\n";
            break;
        case 3:
            std::cout << "File over the Internet\n";
            break;
        case 4:
            std::cout << "Stream over the Internet\n";
            break;
        case 5:
            std::cout << "As note sheets\n";
            break;
        case 6:
            std::cout << "As note sheets in a book with other sheets\n";
            break;
        case 7:
            std::cout << "Music on other media\n";
            break;
        case 8:
            std::cout << "Non-musical merchandise\n";
            break;
    }
}
} // common_methods