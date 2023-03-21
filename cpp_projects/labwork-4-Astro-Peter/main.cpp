#include "archive_methods/SingleFileSection.h"
#include "archive_methods/HAFFileHeader.h"
#include "archive_methods/Structures.h"
#include "archive_methods/Archive.cpp"

#include <string>
#include <bitset>
#include <vector>

void GetArgs(int argc, char* argv[], InputArgs* args) {
    for (int i = 0; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--create") == 0) {
                args->create = true;
            } else if (strcmp(argv[i], "-f") == 0) {
                args->archive_name = argv[i + 1];
                int j = 2;
                while (i + j < argc && argv[i + j][0] != '-') {
                    args->files.emplace_back(argv[i + j]);
                    j++;
                }
            } else if (strncmp(argv[i], "--file", 6) == 0) {
                args->archive_name = static_cast<std::string>(argv[i]).substr(7);
                int j = 1;
                while (argv[i + j][0] != '-') {
                    args->files.emplace_back(argv[i + j]);
                    j++;
                }
            } else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--list") == 0) {
                args->list = true;
            } else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--append") == 0) {
                args->append = true;
            } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--delete") == 0) {
                args->delete_file = true;
            } else if (strcmp(argv[i], "-A") == 0 || strcmp(argv[i], "--concatenate") == 0) {
                args->merge = true;
                args->first_to_merge = argv[i + 1];
                args->second_to_merge = argv[i + 2];
            } else if (strcmp(argv[i], "-s") == 0
                || strcmp(argv[i], "--set-columns") == 0) { //set number of columns for encoding, must be a power of 2
                if (std::pow(2, log2(std::stoi(argv[i + 1]))) == std::pow(2, (int) log2(std::stoi(argv[i + 1])))) {
                    args->number_of_columns = std::stoi(argv[i + 1]);
                } else {
                    std::cout << "Incorrect value given for number of columns\n";
                }
            } else if (strcmp(argv[i], "-x") == 0 || strcmp(argv[i], "--extract") == 0) {
                args->extract = true;
                int j = 1;
                while (argv[i + j][0] != '-') {
                    args->files.emplace_back(argv[i + j]);
                    j++;
                }
                if (j == 1) {
                    args->extract_all = true;
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    InputArgs args;
    GetArgs(argc, argv, &args);
    if (args.create) {
        CreateArchive(args);
    }
    if (args.extract) {
        OpenArchive(args);
    }
    if (args.list) {
        std::vector<std::string> files;
        files = ListFiles(args);
        for (const auto& i : files) {
            std::cout << i << '\n';
        }
    }
    if (args.merge) {
        std::cout << 'd';
        AppendArchives(args);
    }
    if (args.append) {
        AddFile(args);
    }
    if (args.delete_file) {
        DeleteFile(args);
    }
    return 0;
}