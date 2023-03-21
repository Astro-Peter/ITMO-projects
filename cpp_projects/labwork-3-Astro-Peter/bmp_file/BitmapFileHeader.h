#ifndef LABWORK_3_ASTRO_PETER_BITMAP_FILE_HEADER_H
#define LABWORK_3_ASTRO_PETER_BITMAP_FILE_HEADER_H

#include <cstdint>

struct BitmapFileHeader {
    const char kLetterB{'B'};
    const char kLetterM{'M'};
    uint32_t file_size{};
    const uint32_t kReserved{0};
    const uint32_t kOffset{60};
};


#endif //LABWORK_3_ASTRO_PETER_BITMAP_FILE_HEADER_H
