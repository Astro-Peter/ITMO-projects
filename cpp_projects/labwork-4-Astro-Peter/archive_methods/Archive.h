#ifndef LABWORK_4_ASTRO_PETER_ARCHIVE_METHODS_ARCHIVE_H_
#define LABWORK_4_ASTRO_PETER_ARCHIVE_METHODS_ARCHIVE_H_

#include "Structures.h"
#include "HAFFileHeader.h"
#include "SingleFileSection.h"
#include "HammingMethods.h"

#include <filesystem>
#include <iostream>
#include <fstream>

void DeleteFile(const InputArgs& args);

void AddFile(const InputArgs& args);

void AppendArchives(const InputArgs& args);

std::vector<std::string> ListFiles(const InputArgs& args);

void OpenArchive(const InputArgs& args);

void CreateArchive(InputArgs args);

#endif //LABWORK_4_ASTRO_PETER_ARCHIVE_METHODS_ARCHIVE_H_
