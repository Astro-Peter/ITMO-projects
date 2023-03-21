#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

int main(int argc, char* argv[]) {
    bool bytes_count = true, line_count = true, word_count = true, char_count = false;
    size_t bytes, lines, words, chars;
    char cn;
    std::vector<std::string> files;
    bool first = true;
    std::fstream file;
    for (int n = 1; n < argc; n++) {
        std::string arg = argv[n];
        if ((arg == "-c") || (arg == "--bytes")){
            if (first) line_count = false, word_count = false, first = false;
            bytes_count = true;
        } else if ((arg == "-m") || (arg == "--chars")){
            if (first) bytes_count = false, word_count = false, first = false, line_count = false;
            char_count = true;
        } else if ((arg == "-l") || (arg == "--lines")){
            if (first) bytes_count = false, word_count = false, first = false;
            line_count = true;
        } else if ((arg == "-w") || (arg == "--words")){
            if (first) bytes_count = false, line_count = false, first = false;
            word_count = true;
        } else if (arg[0] == '-' && arg.find('.') > 300){
            if (first) bytes_count = false, line_count = false, word_count = false, first = false;
            if (arg.find('l') < 100) line_count = true;
            if (arg.find('w') < 100) word_count = true;
            if (arg.find('c') < 100) bytes_count = true;
            if (arg.find('m') < 100) char_count = true;
        } else files.push_back(arg);
    }
    for (const auto& arg: files) {
        file.open(arg, std::fstream::in);
        if (!file.is_open()) {
            std::cout << "failed to open " << arg << '\n';
        } else {
            if (file.peek() != -1) {
                lines = 1;
                bytes = 0;
                words = 0;
                chars = 0;
                if (!std::isspace(file.peek()) && std::isprint(file.peek())) {
                    words++;
                }
                while (file.get(cn)) {
                    if (cn == ' ') {
                        if (!std::isspace(file.peek()) && std::isprint(file.peek())) words++;
                        bytes++;
                    } else if (cn == '\n') {
                        if (!std::isspace(file.peek()) && std::isprint(file.peek())) words++;
                        bytes++;
                        lines++;
                    } else if (cn == '\t') {
                        if (!std::isspace(file.peek()) && std::isprint(file.peek())) words++;
                        bytes++;
                    } else if (!std::isspace(cn) && std::isprint(cn)) {
                        bytes++, chars++;
                    } else bytes++;
                }
                std::string output;
                if (line_count)
                    output += std::to_string(lines) + " ";
                if (word_count)
                    output += std::to_string(words) + " ";
                if (bytes_count)
                    output += std::to_string(bytes) + " ";
                if (char_count)
                    output += std::to_string(chars) + " ";
                output += std::to_string(float(chars) / float(lines)) + " ";
                output += arg + "\n";
                std::cout << output;
            } else {
                std::cout << arg << " is empty" << '\n';
            }
        }
        file.close();
    }
    return 0;
}
