//
// Created by Vanst on 15.10.2022.
//

#ifndef LABWORK_3_ASTRO_PETER_BMP_FILE_RGBA32_COLOR_H_
#define LABWORK_3_ASTRO_PETER_BMP_FILE_RGBA32_COLOR_H_

#include <cstdint>

struct Color{
  uint8_t blue{};
  uint8_t green{};
  uint8_t red{};
  uint8_t zero{0x00};
};

#endif //LABWORK_3_ASTRO_PETER_BMP_FILE_RGBA32_COLOR_H_
