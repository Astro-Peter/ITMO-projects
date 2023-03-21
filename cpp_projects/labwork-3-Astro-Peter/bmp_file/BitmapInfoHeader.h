#ifndef LABWORK_3_ASTRO_PETER_BMP_FILE_BITMAPINFOHEADER_H_
#define LABWORK_3_ASTRO_PETER_BMP_FILE_BITMAPINFOHEADER_H_

#include <cstdint>

struct BitmapInfoHeader {
    const int32_t kHeaderSize{40};
    int32_t width{};
    int32_t height{};
    const uint16_t kColorPlanes{1};
    const uint16_t kBitsPerPixel{4};
    const uint32_t kCompression{0};
    uint32_t image_size{};
    int32_t res_hor{};
    int32_t res_vert{};
    const uint32_t kColorsNumber{5};
    const uint32_t kImportantColors{0};
};

#endif //LABWORK_3_ASTRO_PETER_BMP_FILE_BITMAPINFOHEADER_H_
