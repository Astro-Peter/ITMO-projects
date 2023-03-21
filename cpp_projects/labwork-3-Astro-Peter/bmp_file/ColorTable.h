//
// Created by Vanst on 15.10.2022.
//

#ifndef LABWORK_3_ASTRO_PETER_BMP_FILE_COLORTABLE_H_
#define LABWORK_3_ASTRO_PETER_BMP_FILE_COLORTABLE_H_


#include "rgba32_color.h"

class ColorTable {
  Color white { .blue=0xFF, .green=0xFF, .red=0xFF};
  Color green {.blue=0x00, .green=0xFF, .red=0x00};
  Color purple {.blue=0xFF, .green=0x00, .red=0xFF};
  Color yellow {.blue=0x00, .green=0xFF, .red=0xFF};
  Color black {.blue=0x00, .green=0x00, .red=0x00};
};

#endif //LABWORK_3_ASTRO_PETER_BMP_FILE_COLORTABLE_H_
