#include "ArgParser.h"
#include <utility>
#include <iostream>

using namespace ArgumentParser;

template<>
bool Argument<int>::GetArgsInt(const std::vector<std::string>& args, int index) {
    while (index < args.size()
        && (args[index][0] != '-' || std::strcmp(args[index].substr(2, this->long_name.size()).c_str(),
                                                 this->long_name.c_str()) == 0 || this->short_name == args[index][1])) {
        if (args[index][0] != '-') {
            value.push_back(std::stoi(args[index]));
        } else if (this->short_name == args[index][1]) {
            value.push_back(std::stoi(args[index].substr(3)));
        } else {
            value.push_back(std::stoi(args[index].substr(2 + this->long_name.size() + 1)));
        }
        index++;
    }
    if (store) {
        if (!multivalue) {
            reference_single[0].ref = value[0];
        } else {
            reference_multiple[0].ref = value;
        }
    }
    if (value.size() >= this->number_of_values && multivalue) {
        return true;
    } else if (value.size() == 1 && !multivalue) {
        return true;
    } else {
        return false;
    }
}

template<>
bool Argument<std::string>::GetArgsString(const std::vector<std::string>& args, int index) {
    std::vector<std::string> tmp;
    while (index < args.size() && (std::strcmp(args[index].substr(2, this->long_name.size()).c_str(),
                                               this->long_name.c_str()) == 0 || this->short_name == args[index][1]
        || args[index][0] != '-')) {
        if (std::strcmp(args[index].substr(2, this->long_name.size()).c_str(),
                        this->long_name.c_str()) == 0) {
            tmp.push_back(args[index].substr(2 + this->long_name.size() + 1));
        } else if (this->short_name == args[index][1]) {
            tmp.push_back(args[index].substr(3));
        } else {
            tmp.push_back(args[index]);
        }
        index++;
    }
    this->value = tmp;
    if (store) {
        if (!multivalue) {
            reference_single[0].ref = value[0];
        }
    }
    if (tmp.size() >= this->number_of_values && multivalue) {
        return true;
    } else if (tmp.size() == 1 && !multivalue) {
        return true;
    } else {
        return false;
    }
}

template<typename T>
Argument<T>::Argument(char first, const std::string& second) {
    short_name = first;
    long_name = second;
}

template<>
Argument<std::string>& Argument<std::string>::StoreValue(std::string& point) {
    ReferenceSingle<std::string> tmp {.ref = point};
    this->reference_single.push_back(tmp);
    this->store = true;
    return *this;
}

template<>
Argument<std::string>& Argument<std::string>::Default(const char* val) {
    this->default_value = true;
    this->value.emplace_back(val);
    return *this;
}

template<>
Argument<int>& Argument<int>::MultiValue(size_t num) {
    multivalue = true;
    number_of_values = num;
    return *this;
}

template<>
Argument<std::string>& Argument<std::string>::MultiValue(size_t num) {
    multivalue = true;
    number_of_values = num;
    return *this;
}

template<>
Argument<int>& Argument<int>::MultiValue() {
    multivalue = true;
    number_of_values = 0;
    return *this;
}

template<>
Argument<std::string>& Argument<std::string>::MultiValue() {
    multivalue = true;
    number_of_values = 0;
    return *this;
}


template<>
Argument<int>& Argument<int>::StoreValues(std::vector<int>& point) {
    ReferenceVector<int> tmp {.ref = point};
    reference_multiple.push_back(tmp);
    store = true;
    return *this;
}

template<>
void Argument<bool>::SetTrue() {
    this->value.push_back(true);
}


template<>
Argument<bool>& Argument<bool>::Default(bool val) {
    value.push_back(val);
    default_value = true;
    return *this;
}

//template<typename T>
//Argument<T> &Argument<T>::Default(T val) {
//    default_value = true;
//    value.push_back(val);
//    return this;
//}

template<>
Argument<bool>& Argument<bool>::StoreValue(bool& point) {
    ReferenceSingle<bool> tmp {.ref = point};
    reference_single.push_back(tmp);
    this->store = true;
    return *this;
}

template<>
Argument<int>& Argument<int>::Positional() {
    this->positional = true;
    return *this;
}

bool ArgParser::AddHelp(char param1, const std::string& param2, const std::string& desc) {
    help.short_name = param1;
    help.long_name = param2;
    help.description = desc;
    return true;
}

ArgParser::ArgParser(std::string info) {
    description = std::move(info);
}

Argument<int>& ArgParser::AddIntArgument(const std::string& param) {
    Argument<int> tmp = Argument<int>('\0', param);
    int_args.push_back(tmp);
    return int_args.back();
}

void ArgParser::ParseInts(const std::vector<std::string>& args, int index) {
    for (int i = 0; i < int_args.size(); i++) {
        if (std::strcmp(args[index].substr(2, int_args[i].long_name.size()).c_str(),
                        int_args[i].long_name.c_str()) == 0 || int_args[i].short_name == args[index][1]) {
            int_args[i].GetArgsInt(args, index);
        }
    }
}

void ArgParser::ParseStrings(const std::vector<std::string>& args, int index) {
    for (int i = 0; i < string_args.size(); i++) {
        if (std::strcmp(args[index].substr(2, string_args[i].long_name.size()).c_str(),
                        string_args[i].long_name.c_str()) == 0 || string_args[i].short_name == args[index][1]) {
            string_args[i].GetArgsString(args, index);
        }
    }
}

bool ArgParser::CheckArgs() {
    for (const auto& i : this->string_args) {
        if (i.value.size() < i.number_of_values || i.value.empty()) {
            return false;
        }
    }
    for (const auto& i : this->int_args) {
        if (i.value.size() < i.number_of_values || i.value.empty()) {
            return false;
        }
    }
    return true;
}

void ArgParser::SetFlags(const std::vector<std::string>& args, int index) {
    for (int i = 1; i < args[index].size(); i++) {
        for (auto& flag : flags) {
            if (flag.short_name == args[index][i]) {
                flag.SetTrue();
                if (flag.store) {
                    flag.reference_single[0].ref = true;
                }
            }
        }
    }
}

void ArgParser::SetFlag(const std::vector<std::string>& args, int index) {
    for (auto& flag : flags) {
        if (std::strcmp(flag.long_name.c_str(), args[index].substr(2).c_str()) == 0) {
            flag.SetTrue();
            if (flag.store) {
                flag.reference_single[0].ref = true;
            }
        }
    }
}

bool ArgParser::CheckHelp(const std::string& param) {
    if (std::strcmp(param.substr(2).c_str(), help.long_name.c_str()) == 0 || param[1] == 'h') {
        get_help = true;
        std::cout << help.description << std::endl;
        std::cout << "Int Arguments" << std::endl;
        for (const auto& i : int_args) {
            if (i.short_name == '\0') {
                std::cout << i.long_name << '\t' << i.description << '\n';
            } else {
                std::cout << i.short_name << '\t' << i.long_name << '\t' << i.description << '\n';
            }
        }
        std::cout << "String Args" << std::endl;
        for (const auto& i : string_args) {
            if (i.short_name == '\0') {
                std::cout << i.long_name << '\t' << i.description << '\n';
            } else {
                std::cout << i.short_name << '\t' << i.long_name << '\t' << i.description << '\n';
            }
        }
        std::cout << "Flags" << std::endl;
        for (const auto& i : flags) {
            if (i.short_name == '\0') {
                std::cout << i.long_name << '\t' << i.description << '\n';
            } else {
                std::cout << i.short_name << '\t' << i.long_name << '\t' << i.description << '\n';
            }
        }
        return true;
    }
    return false;
}


bool ArgParser::Parse(const std::vector<std::string>& args) {
    for (auto& i : int_args) {
        if (i.positional) {
            i.GetArgsInt(args, 1);
        }
    }
    for (int i = 0; i < args.size(); i++) {
        if (args[i][0] == '-') {
            this->CheckHelp(args[i]);
            this->ParseInts(args, i);
            this->ParseStrings(args, i);
            this->SetFlag(args, i);
            if (args[i][1] != '-') {
                this->SetFlags(args, i);
            }
        }
    }
    if (!(CheckArgs() || Help())) {
        return false;
    }
    return true;
}

Argument<std::string>& ArgParser::AddStringArgument(const std::string& param) {
    Argument<std::string> tmp = Argument<std::string>('\0', param);
    string_args.push_back(tmp);
    return string_args.back();
}

std::string ArgParser::GetStringValue(const std::string& param) {
    for (const auto& i : this->string_args) {
        if (i.long_name == param) {
            return i.value[0];
        }
    }
    return nullptr;
}

Argument<std::string>& ArgParser::AddStringArgument(char param1, const std::string& param2) {
    Argument<std::string> tmp = Argument<std::string>(param1, param2);
    string_args.push_back(tmp);
    return string_args.back();
}

int ArgParser::GetIntValue(const std::string& param) {
    for (const auto& i : this->int_args) {
        if (i.long_name == param) {
            return i.value[0];
        }
    }
    return NULL;
}

int ArgParser::GetIntValue(const std::string& param, size_t pos) {
    for (const auto& i : this->int_args) {
        if (i.long_name == param) {
            return i.value[pos];
        }
    }
    return NULL;
}
Argument<int>& ArgParser::AddIntArgument(char param1, const std::string& param2) {
    Argument<int> tmp = Argument<int>(param1, param2);
    int_args.push_back(tmp);
    return int_args.back();
}

Argument<bool>& ArgParser::AddFlag(char param1, const std::string& name) {
    Argument<bool> tmp = Argument<bool>(param1, name);
    flags.push_back(tmp);
    return flags.back();
}

bool ArgParser::GetFlag(const std::string& param) {
    for (const auto& i : this->flags) {
        if (i.long_name == param) {
            return i.value[0];
        }
    }
    return false;
}
bool ArgParser::Help() {
    return get_help;
}
Argument<int>& ArgParser::AddIntArgument(char param1, const std::string& param2, const std::string& desc) {
    Argument<int> tmp = Argument<int>(param1, param2);
    tmp.description = desc;
    int_args.push_back(tmp);
    return int_args.back();
}
Argument<std::string>& ArgParser::AddStringArgument(char param1, const std::string& param2, const std::string& desc) {
    Argument<std::string> tmp = Argument<std::string>(param1, param2);
    tmp.description = desc;
    string_args.push_back(tmp);
    return string_args.back();
}
Argument<bool>& ArgParser::AddFlag(char param1, const std::string& name, const std::string& desc) {
    Argument<bool> tmp = Argument<bool>(param1, name);
    tmp.description = desc;
    flags.push_back(tmp);
    return flags.back();
}
Argument<int>& ArgParser::AddIntArgument(const std::string& param2, const std::string& desc) {
    Argument<int> tmp = Argument<int>('\0', param2);
    tmp.description = desc;
    int_args.push_back(tmp);
    return int_args.back();
}


Argument<bool>& ArgParser::AddFlag(const std::string& name, const std::string& desc) {
    Argument<bool> tmp = Argument<bool>('\0', name);
    tmp.description = desc;
    flags.push_back(tmp);
    return flags.back();
}


bool ArgParser::Parse(int argc, char** argv) {
    std::vector<std::string> args;
    for (int i = 0; i < argc; i++) {
        args.emplace_back(argv[i]);
    }
    return Parse(args);
}
std::string ArgParser::HelpDescription() {
    std::string value;
    value += help.description + '\n';
    value += "Int Arguments\n";
    for (const auto& i : int_args) {
        if (i.short_name == '\0') {
            value += i.long_name + '\t' + i.description + '\n';
        } else {
            value += i.short_name + '\t';
            value += i.long_name + '\t' + i.description + '\n';
        }
    }
    value += "String Args\n";
    for (const auto& i : string_args) {
        if (i.short_name == '\0') {
            value += i.long_name + '\t' + i.description + '\n';
        } else {
            value += i.short_name + '\t';
            value += i.long_name + '\t' + i.description + '\n';
        }
    }
    value += "Flags\n";
    for (const auto& i : flags) {
        if (i.short_name == '\0') {
            value += i.long_name + '\t' + i.description + '\n';
        } else {
            value += i.short_name + '\t';
            value += i.long_name + '\t' + i.description + '\n';
        }
    }
    return value;
}


