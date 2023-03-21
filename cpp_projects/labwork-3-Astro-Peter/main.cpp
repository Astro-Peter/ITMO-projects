#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <ostream>
#include <chrono>

#include "bmp_file/BitmapInfoHeader.h"
#include "bmp_file/BitmapFileHeader.h"
#include "bmp_file/ColorTable.h"

BitmapFileHeader file_head{ };
BitmapInfoHeader file_info{ };
const ColorTable kColors;
uint32_t padding_size;
size_t max_number_of_iterations = 0;
size_t frequency = 0;
uint16_t length, width;
char * output = (char *) "ads";
char * file_name = (char *) "da";
size_t cnt = 0;
const int kBitsIn4Bytes = 32;
const int kBitsIn1Byte = 8;
const int kBytesInHeadersAndColorTable = 74;

void GetArgs(int argc, char * argv[]){
  for (int i = 1; i < argc; i += 2){
    if (argv[i][1] == 'l' || argv[i][2] == 'l'){
      length =  stoi((std::string) argv[i + 1]);
    } else if (argv[i][1] == 'w' || argv[i][2] == 'w'){
      width = std::stoi(argv[i + 1]);
    } else if (argv[i][1] == 'i' || argv[i][2] == 'i'){
      file_name = argv[i + 1];
    } else if (argv[i][1] == 'o' || argv[i][2] == 'o'){
      output = argv[i + 1];
    } else if (argv[i][1] == 'm' || argv[i][2] == 'm'){
        max_number_of_iterations = std::stoi(argv[i + 1]);
    } else if (argv[i][1] == 'f' || argv[i][2] == 'f'){
      frequency = std::stoi(argv[i + 1]);
    }
  }
}

void CheckStability(const uint64_t * kSandPile, std::vector<uint32_t>& need_to_fall){
  for (uint32_t i = 0; i < length * width; i++){
    if (kSandPile[i] > 3){
      need_to_fall.push_back(i);
    }
  }
}

void StandardIteration(uint64_t * sand_pile, std::vector<uint32_t>& need_to_fall){
  for (uint32_t ind : need_to_fall){
    if (ind > width){
      sand_pile[ind - width] += 1;
    } if (ind < width * (length - 1)){
      sand_pile[ind + width] += 1;
    } if (ind % width > 0){
      sand_pile[ind - 1] += 1;
    } if (ind % width < width - 1){
      sand_pile[ind + 1] += 1;
    }
    sand_pile[ind] -= 4;
  }
}

void FileWrite(const uint64_t* kSandPile, const std::string& kOutput){
  std::ofstream bmp(kOutput, std::ofstream::binary);
  bmp.write(&file_head.kLetterB, 1);
  bmp.write(&file_head.kLetterM, 1);
  bmp.write(reinterpret_cast<const char*>(&file_head.file_size), 4);
  bmp.write(reinterpret_cast<const char*>(&file_head.kReserved), 4);
  bmp.write(reinterpret_cast<const char*>(&file_head.kOffset), 4);
  bmp.write(reinterpret_cast<const char*>(&file_info), sizeof file_info);
  bmp.write(reinterpret_cast<const char*>(&kColors), sizeof kColors);

  // writing the image by horizontal lines
  for (uint16_t i = 0; i < length; i++){
    for (uint16_t j = 0; j < width; j+=2){
      char pixel1;
      uint64_t first = std::min((uint64_t)kSandPile[j + i * width], (uint64_t) 4);
      if (j == width - 1){
        pixel1 = static_cast<char>(first * 16);
      } else {
        uint64_t second = std::min((uint64_t)kSandPile[j + 1 + i * width], (uint64_t) 4);
        pixel1 = static_cast<char>(first * 16 + second);
      }
      bmp.write(&pixel1, sizeof pixel1);
    }
    if (padding_size > 0){
      for (int k = 0; k < padding_size; k++){
        char s = 0;
        bmp.write(&s, sizeof s);
      }
    }
  }
}

bool GetCoordinates(uint64_t * sand_pile){
  std::fstream file(file_name, std::fstream::in);
  std::string x, y, num;
  bool unstable = false;
  while (file >> x){
    file >> y;
    file >> num;
    if (std::stoi(num) > 3){
      unstable = true;
    }
    sand_pile[(std::stoi(x) - 1) + (std::stoi(y) - 1) * width] = std::stoi(num);
  }
  return unstable;
}

bool EffectiveIteration(uint64_t * sand_pile){
  bool fell = false;
  for (uint32_t i = 0; i < length * width; i++) {
    if (sand_pile[i] > 3) {
      fell = true;
      if (i > length) {
        sand_pile[i - length] += sand_pile[i] / 4;
      }
      if (i < length * (width - 1)) {
        sand_pile[i + length] += sand_pile[i] / 4;
      }
      if (i % length > 0) {
        sand_pile[i - 1] += sand_pile[i] / 4;
      }
      if (i % length < length - 1) {
        sand_pile[i + 1] += sand_pile[i] / 4;
      }
      sand_pile[i] = sand_pile[i] % 4;
    }
  }
  return fell;
}

void TimeToSave(uint64_t * sand_pile){
  cnt++;
  if (frequency > 0 && cnt % frequency == 0){
    std::string freq_out = (std::string) output + std::to_string(cnt) + ".bmp";
    FileWrite(sand_pile, freq_out);
  }
}

int main(int argc, char * argv[]) {
  GetArgs(argc, argv);
  auto start = std::chrono::high_resolution_clock::now();
    padding_size = (kBitsIn4Bytes - width * file_info.kBitsPerPixel % kBitsIn4Bytes) / kBitsIn1Byte % (kBitsIn4Bytes / kBitsIn1Byte);
  file_head.file_size = kBytesInHeadersAndColorTable + length * (((uint32_t) width * file_info.kBitsPerPixel) / kBitsIn1Byte + padding_size);
  file_info.width = width;
  file_info.height = length;
  file_info.image_size = file_head.file_size - kBytesInHeadersAndColorTable;
  file_info.res_hor = 100;
  file_info.res_vert = 100;
  uint64_t *sand_pile;
  sand_pile = new uint64_t [length * width]{  };

  // reading the input file
  bool unstable = GetCoordinates(sand_pile);

  // start of the simulation
  if (max_number_of_iterations > 0){
    // start of the simulation limited by number of iterations
    // std::vector<uint32_t> need_to_fall;
    for (size_t j = 0; j < max_number_of_iterations; j++){
      std::vector<uint32_t> need_to_fall { };
      CheckStability(sand_pile, need_to_fall);
      StandardIteration(sand_pile, need_to_fall);
      TimeToSave(sand_pile);
    }

  } else{
      // start of the not limited simulation
      while (unstable) {
      unstable = EffectiveIteration(sand_pile);
      TimeToSave(sand_pile);
    }
  }

  std::string final_output = (std::string) output + ".bmp";
  FileWrite(sand_pile, final_output);
  delete[] sand_pile;
  auto end = std::chrono::high_resolution_clock::now();
  std::cout << "finished with runtime: " << (float (std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count())) / 1000 << '\t' << "total iterations: " << cnt;
  return 0;
}
