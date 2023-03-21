#include "Archive.h"

std::vector<std::string> ListFiles(const InputArgs& args) {
    SingleFileInfo file_info {};
    HAFFileHeader file_header {};
    std::ifstream archive_header(args.archive_name, std::ifstream::binary);
    std::ifstream archive(args.archive_name, std::ifstream::binary);
    archive_header.read(reinterpret_cast<char*>(&file_header.number_of_columns), sizeof(uint64_t));
    archive_header.read(reinterpret_cast<char*>(&file_header.number_of_files), sizeof(uint64_t));
    archive_header.read(reinterpret_cast<char*>(&file_info.file_name), sizeof(uint64_t));
    std::string subdirectory = args.archive_name.substr(0, args.archive_name.rfind('.'));
    archive.ignore(static_cast<int64_t>(file_info.file_name));
    std::filesystem::create_directory(subdirectory);
    std::vector<std::string> names;
    std::string name;
    for (int i = 0; i < file_header.number_of_files; i++) {
        archive_header.read(reinterpret_cast<char*>(&file_info.file_start), sizeof(uint64_t));
        archive_header.read(reinterpret_cast<char*>(&file_info.file_real_size), sizeof(uint64_t));
        name = "";
        for (int j = 0; j < file_info.file_start; j++) {
            char let = 0;
            archive.read(&let, 1);
            name += let;
        }
        uint64_t blocks;
        names.push_back(name);
        archive_header.read(reinterpret_cast<char*>(&file_info.file_name), sizeof(uint64_t));
        if (8 * file_info.file_real_size % static_cast<uint64_t>(kRowsNumber * file_header.number_of_columns
            - log2(kRowsNumber * file_header.number_of_columns) - 1) != 0) {
            blocks = (file_info.file_real_size * 8 / static_cast<uint64_t>(kRowsNumber * file_header.number_of_columns
                - log2(kRowsNumber * file_header.number_of_columns) - 1)) + 1;
        } else {
            blocks = (file_info.file_real_size * 8 / static_cast<uint64_t>(kRowsNumber * file_header.number_of_columns
                - log2(kRowsNumber * file_header.number_of_columns) - 1));
        }
        archive.ignore(sizeof(uint16_t) * file_header.number_of_columns * blocks);
    }
    return names;
}


void DeleteFile(const InputArgs& args) {
    SingleFileInfo file_info {};
    HAFFileHeader archive_header {};
    std::cout << "\nDo you want to make a new archive?(y/n)\n";
    char answer;
    bool make_new = false;
    std::cin >> answer;
    if (answer == 'y') {
        make_new = true;
    }
    std::vector<std::string> files = ListFiles(args);
    std::ifstream archive1_header {args.archive_name, std::ifstream::binary};
    std::ifstream archive1 {args.archive_name, std::ifstream::binary};
    std::string new_archive_name = args.archive_name.substr(0, args.archive_name.rfind('.')) + "_new" + ".haf";
    std::ofstream new_archive {new_archive_name, std::ofstream::binary};
    archive1_header.read(reinterpret_cast<char*>(&archive_header), sizeof(HAFFileHeader));
    archive1_header.read(reinterpret_cast<char*>(&file_info), sizeof(SingleFileInfo));
    archive1.ignore(file_info.file_name);
    uint64_t original_number_of_files = archive_header.number_of_files;
    for (auto& file : files) {
        if (std::find(args.files.begin(), args.files.end(), file) != args.files.end()) {
            archive_header.number_of_files--;
        }
    }
    new_archive.write(reinterpret_cast<const char*>(&archive_header), sizeof(archive_header));
    std::string name;
    uint64_t need_to_skip = (archive_header.number_of_files * 3 + 2) * sizeof(uint64_t);
    for (int j = 0; j < file_info.file_start; j++) {
        char let = 0;
        archive1.read(&let, 1);
        name += let;
    }
    std::cout << name << '\n';
    bool first = false;
    if (std::find(args.files.begin(), args.files.end(), name) == args.files.end()) {
        new_archive.write(reinterpret_cast<const char*>(&need_to_skip), sizeof(uint64_t));
        new_archive.write(reinterpret_cast<const char*>(&file_info.file_start), sizeof(uint64_t));
        new_archive.write(reinterpret_cast<const char*>(&file_info.file_real_size), sizeof(uint64_t));
        first = true;
    }
    for (int i = 1; i < original_number_of_files; i++) {
        name = "";
        archive1_header.read(reinterpret_cast<char*>(&file_info), sizeof(SingleFileInfo));
        if (first) {
            need_to_skip = file_info.file_name;
            first = false;
        }
        archive1.ignore(file_info.file_name);
        for (int j = 0; j < file_info.file_start; j++) {
            char let = 0;
            archive1.read(&let, 1);
            name += let;
        }
        if (std::find(args.files.begin(), args.files.end(), name) == args.files.end()) {
            std::cout << name << ' ' << 'm' << '\n';
            new_archive.write(reinterpret_cast<const char*>(&need_to_skip), sizeof(uint64_t));
            new_archive.write(reinterpret_cast<const char*>(&file_info.file_start), sizeof(uint64_t));
            new_archive.write(reinterpret_cast<const char*>(&file_info.file_real_size), sizeof(uint64_t));
            first = true;
        }
    }
    auto hamming_info = kRowsNumber * args.number_of_columns - 1
        - static_cast<uint64_t>(log2(kRowsNumber) + log2(args.number_of_columns));
    archive1.close();
    archive1.clear();
    archive1_header.close();
    archive1_header.clear();
    archive1_header.open(args.archive_name, std::ifstream::binary);
    archive1.open(args.archive_name, std::ifstream::binary);
    archive1_header.ignore(sizeof(uint64_t) * 2);
    archive1.ignore(8 * ((archive_header.number_of_files + args.files.size()) * 3 + 2));
    for (int i = 0; i < archive_header.number_of_files + args.files.size(); i++) {
        uint64_t info_size;
        archive1_header.read(reinterpret_cast<char*>(&file_info), sizeof(SingleFileInfo));
        if ((8 * file_info.file_real_size) % hamming_info != 0) {
            info_size = static_cast<uint64_t>(kRowsNumber * args.number_of_columns)
                * ((file_info.file_real_size * 8) / hamming_info + 1) / 8;
        } else {
            info_size = static_cast<uint64_t>(kRowsNumber * args.number_of_columns)
                * ((file_info.file_real_size * 8) / hamming_info) / 8;
        }
        name = "";
        for (int j = 0; j < file_info.file_start; j++) {
            char let = 0;
            archive1.read(&let, 1);
            name += let;
        }
        if (std::find(args.files.begin(), args.files.end(), name) == args.files.end()) {
            char* rows;
            rows = new char[info_size];
            archive1.read(reinterpret_cast<char*>(rows), sizeof(char) * info_size);
            std::cout << name << ' ' << i << '\n';
            new_archive.write(name.c_str(), name.size());
            new_archive.write(reinterpret_cast<char*>(rows), sizeof(char) * info_size);
            delete[] rows;
        } else {
            archive1.ignore(info_size);
        }
    }
    new_archive.close();
    archive1_header.close();
    archive1.close();
    if (!make_new) {
        std::filesystem::remove(args.archive_name);
        std::filesystem::rename(new_archive_name, args.archive_name);
    }
}


void AddFile(const InputArgs& args) {
    SingleFileInfo file_info {};
    HAFFileHeader archive_header {};
    std::cout << "\nDo you want to make a new archive?(y/n)\n";
    char answer;
    bool make_new = false;
    std::cin >> answer;
    if (answer == 'y') {
        make_new = true;
    }
    std::ifstream archive1 {args.archive_name, std::ifstream::binary};
    std::string new_archive_name = args.archive_name.substr(0, args.archive_name.rfind('.')) + "_new" + ".haf";
    std::ofstream new_archive {new_archive_name, std::ofstream::binary};
    archive1.read(reinterpret_cast<char*>(&archive_header), sizeof(HAFFileHeader));
    archive_header.number_of_files += args.files.size();
    archive1.read(reinterpret_cast<char*>(&file_info), sizeof(SingleFileInfo));
    file_info.file_name += args.files.size() * 3 * 8;
    new_archive.write(reinterpret_cast<const char*>(&archive_header), sizeof(archive_header));
    new_archive.write(reinterpret_cast<const char*>(&file_info), sizeof(file_info));
    for (int i = 0; i < archive_header.number_of_files - 1 - args.files.size(); i++) {
        archive1.read(reinterpret_cast<char*>(&file_info), sizeof(SingleFileInfo));
        new_archive.write(reinterpret_cast<const char*>(&file_info), sizeof(file_info));
    }
    auto hamming_info = kRowsNumber * args.number_of_columns - 1
        - static_cast<uint64_t>(log2(kRowsNumber) + log2(args.number_of_columns));
    for (int i = 0; i < args.files.size(); i++) {
        if ((8 * file_info.file_real_size) % hamming_info != 0) {
            std::cout << file_info.file_real_size << '\n';
            file_info.file_name = static_cast<uint64_t>(kRowsNumber * args.number_of_columns)
                * ((file_info.file_real_size * 8) / hamming_info + 1) / 8;
        } else {
            file_info.file_name = static_cast<uint64_t>(kRowsNumber * args.number_of_columns)
                * ((file_info.file_real_size * 8) / hamming_info) / 8;
        }
        file_info.file_start = args.files[i].substr(args.files[i].rfind('\\') + 1).size();
        file_info.file_real_size = std::filesystem::file_size(args.files[i]);
        new_archive.write(reinterpret_cast<const char*>(&file_info), sizeof(file_info));
    }
    new_archive << archive1.rdbuf();
    archive1.close();
    HammingConversion input {};
    for (int i = 0; i < args.files.size(); i++) {
        std::cout << args.files[i] << '\n';
        new_archive.write(args.files[i].substr(args.files[i].rfind('\\') + 1).c_str(),
                          static_cast<int64_t>(args.files[i].substr(args.files[i].rfind('\\') + 1).size()));
        input.input_file.open(args.files[i], std::ifstream::binary | std::ifstream::beg);
        input.passed_bits = -1;
        while (input.input_file.good()) {
            uint16_t* rows;
            rows = MakeRows(&input, args.number_of_columns);
            new_archive.write(reinterpret_cast<const char*>(rows), static_cast<uint32_t>(2 * (args.number_of_columns)));
            delete[] rows;
        }
        input.input_file.close();
        input.input_file.clear();
    }
    new_archive.close();
    if (!make_new) {
        std::filesystem::remove(args.archive_name);
        std::filesystem::rename(new_archive_name, args.archive_name);
    }
}


void AppendArchives(const InputArgs& args) {
    SingleFileInfo file_info {};
    std::cout << args.first_to_merge << ' ' << args.second_to_merge << ' ' << args.archive_name;
    std::ifstream archive1 {args.first_to_merge, std::ifstream::binary};
    std::ifstream archive2 {args.second_to_merge, std::ifstream::binary};
    std::ofstream archive3 {args.archive_name, std::ofstream::binary};
    uint64_t archive1_columns_num, archive1_file_num, archive2_columns_num, archive2_file_num;
    archive1.read(reinterpret_cast<char*>(&archive1_columns_num), sizeof(uint64_t));
    archive2.read(reinterpret_cast<char*>(&archive2_columns_num), sizeof(uint64_t));
    if (archive1_columns_num != archive2_columns_num) {
        std::cout << "Archives differ in number of columns(archive1 = " << archive1_columns_num << ", archive2 = "
                  << archive2_columns_num << "), please remake archives";
        return;
    }
    auto hamming_info = kRowsNumber * archive2_columns_num - 1
        - static_cast<uint64_t>(log2(kRowsNumber) + log2(archive2_columns_num));
    archive1.read(reinterpret_cast<char*>(&archive1_file_num), sizeof(uint64_t));
    archive2.read(reinterpret_cast<char*>(&archive2_file_num), sizeof(uint64_t));
    uint64_t archive3_file_num = archive1_file_num + archive2_file_num;
    archive3.write(reinterpret_cast<const char*>(&archive2_columns_num), sizeof(uint64_t));
    archive3.write(reinterpret_cast<const char*>(&archive3_file_num), sizeof(uint64_t));

    archive1.read(reinterpret_cast<char*>(&file_info), sizeof(file_info));
    file_info.file_name = sizeof(uint64_t) * (archive3_file_num * 3 + 2);
    archive3.write(reinterpret_cast<const char*>(&file_info), sizeof(file_info));
    for (int i = 0; i < archive1_file_num - 1; i++) {
        archive1.read(reinterpret_cast<char*>(&file_info), sizeof(file_info));
        archive3.write(reinterpret_cast<const char*>(&file_info), sizeof(file_info));
    }
    uint64_t info_start;
    if (8 * file_info.file_real_size % hamming_info != 0) {
        info_start = static_cast<uint64_t>(kRowsNumber * archive2_columns_num)
            * ((file_info.file_real_size * 8) / hamming_info + 1) / 8;
    } else {
        info_start = static_cast<uint64_t>(kRowsNumber * archive2_columns_num)
            * ((file_info.file_real_size * 8) / hamming_info) / 8;
    }
    archive2.read(reinterpret_cast<char*>(&file_info), sizeof(file_info));
    file_info.file_name = info_start;
    archive3.write(reinterpret_cast<const char*>(&file_info), sizeof(file_info));
    for (int i = 0; i < archive2_file_num - 1; i++) {
        archive2.read(reinterpret_cast<char*>(&file_info), sizeof(file_info));
        archive3.write(reinterpret_cast<const char*>(&file_info), sizeof(file_info));
    }
    char byte;
    while (archive1.get(byte)) {
        archive3.write(reinterpret_cast<const char*>(&byte), sizeof(byte));
    }
    while (archive2.get(byte)) {
        archive3.write(reinterpret_cast<const char*>(&byte), sizeof(byte));
    }
    archive3.close();
    archive1.close();
    archive2.close();
}


void OpenArchive(const InputArgs& args) {
    SingleFileInfo file_info {};
    HAFFileHeader file_header {};
    std::ifstream archive_header(args.archive_name, std::ifstream::binary);
    std::ifstream archive(args.archive_name, std::ifstream::binary);
    archive_header.read(reinterpret_cast<char*>(&file_header.number_of_columns), sizeof(uint64_t));
    archive_header.read(reinterpret_cast<char*>(&file_header.number_of_files), sizeof(uint64_t));
    archive_header.read(reinterpret_cast<char*>(&file_info.file_name), sizeof(uint64_t));
    std::string name;
    std::string subdirectory = args.archive_name.substr(0, args.archive_name.rfind('.'));
    archive.ignore(static_cast<int64_t>(file_info.file_name));
    std::filesystem::create_directory(subdirectory);
    for (int i = 0; i < file_header.number_of_files; i++) {
        archive_header.read(reinterpret_cast<char*>(&file_info.file_start), sizeof(uint64_t));
        archive_header.read(reinterpret_cast<char*>(&file_info.file_real_size), sizeof(uint64_t));
        name = "";
        for (int j = 0; j < file_info.file_start; j++) {
            char let = 'j';
            archive.read(&let, 1);
            name += let;
        }
        uint64_t blocks;
        std::cout << name;
        archive_header.read(reinterpret_cast<char*>(&file_info.file_name), sizeof(uint64_t));
        if (8 * file_info.file_real_size % (kRowsNumber * file_header.number_of_columns
            - static_cast<uint64_t>(log2(kRowsNumber * file_header.number_of_columns)) - 1) != 0) {
            blocks = (8 * file_info.file_real_size / (kRowsNumber * file_header.number_of_columns
                - static_cast<uint64_t>(log2(kRowsNumber * file_header.number_of_columns)) - 1)) + 1;
        } else {
            blocks = 8 * file_info.file_real_size / (kRowsNumber * file_header.number_of_columns
                - static_cast<uint64_t>(log2(kRowsNumber * file_header.number_of_columns)) - 1);
        }
        std::cout << blocks << '\n';
        auto* rows = new uint16_t[file_header.number_of_columns];
        std::string file_destination = subdirectory;
        file_destination += '\\';
        file_destination += name;
        ReverseConversion output {.output_file = std::ofstream(file_destination,
                                                               std::ofstream::binary), .passed_bits = 7, .file_size = file_info.file_real_size};
        std::cout << blocks << 'b' << '\n';
        for (int j = 0; j < blocks; j++) {
            std::cout << "blocks " << j << ' ' << blocks << ' ' << file_info.file_real_size << '\n';
            archive.read(reinterpret_cast<char*>(rows), sizeof(uint16_t) * file_header.number_of_columns);
            CheckOrWriteParityBits(rows, file_header.number_of_columns, true);
            TransformToRegularFile(output, rows, file_header.number_of_columns);
        }
    }
}


void CreateArchive(InputArgs args) {
    uint64_t file_num = args.files.size();
    HammingConversion input;
    HAFFileHeader header {.number_of_columns = args.number_of_columns, .number_of_files = file_num};
    SingleFileInfo file_info {};
    auto hamming_info = kRowsNumber * args.number_of_columns - 1
        - static_cast<uint64_t>(log2(kRowsNumber) + log2(args.number_of_columns));
    std::vector<std::string> names;
    std::ofstream archive(args.archive_name, std::ofstream::binary);
    archive.write(reinterpret_cast<const char*>(&header), sizeof(HAFFileHeader));
    uint64_t info_start = sizeof(uint64_t) * (3 * file_num + 2);
    std::cout << file_num << '\n';
    for (const auto& i : args.files) {
        std::cout << i << '\n';
    }
    for (int i = 0; i < file_num; i++) {
        file_info.file_name = info_start;
        archive.write(reinterpret_cast<const char*>(&file_info.file_name), sizeof(uint64_t));
        std::string tmp = args.files[i].substr(args.files[i].rfind('\\') + 1);
        names.emplace_back(tmp);
        file_info.file_start = names[i].size();
        archive.write(reinterpret_cast<const char*>(&file_info.file_start), sizeof(uint64_t));
        if (8 * std::filesystem::file_size(args.files[i]) % hamming_info != 0) {
            info_start = static_cast<uint64_t>(kRowsNumber * args.number_of_columns)
                * (std::filesystem::file_size(args.files[i]) * 8 / hamming_info + 1) / 8;
        } else {
            info_start = static_cast<uint64_t>(kRowsNumber * args.number_of_columns)
                * (std::filesystem::file_size(args.files[i]) * 8 / hamming_info) / 8;
        }
        uintmax_t real_file_size = std::filesystem::file_size(args.files[i]);
        archive.write(reinterpret_cast<const char*>(&real_file_size), sizeof(uintmax_t));
    }
    for (int i = 0; i < names.size(); i++) {
        std::cout << names[i] << '\n';
        archive.write(names[i].c_str(), static_cast<int64_t>(names[i].size()));
        input.input_file.open(args.files[i], std::ifstream::binary | std::ifstream::beg);
        input.passed_bits = -1;
        while (input.input_file.good()) {
            uint16_t* rows;
            rows = MakeRows(&input, args.number_of_columns);
            archive.write(reinterpret_cast<const char*>(rows), static_cast<uint32_t>(2 * (args.number_of_columns)));
            delete[] rows;
        }
        input.input_file.close();
        input.input_file.clear();
    }
}