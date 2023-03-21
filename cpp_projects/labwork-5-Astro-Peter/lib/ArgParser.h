#pragma once
#include <string>
#include <vector>

namespace ArgumentParser {
template<typename T>
struct ReferenceSingle {
    T& ref;
};

template<typename T>
struct ReferenceVector {
    std::vector<T>& ref;
};

template<typename T>
class Argument {
public:
    bool positional = false;
    bool multivalue = false;
    bool default_value = false;
    bool store = false;

    char short_name;
    std::string long_name;
    std::string description;

    std::vector<T> value;
    std::vector<ReferenceSingle<T>> reference_single;
    std::vector<ReferenceVector<T>> reference_multiple;

    size_t number_of_values = 1;
    Argument(char first, const std::string& second);

    bool GetArgsInt(const std::vector<std::string>& args, int index);
    bool GetArgsString(const std::vector<std::string>& args, int index);
    void SetTrue();

    Argument<T>& MultiValue(size_t num);
    Argument<T>& MultiValue();

    Argument<bool>& Default(bool val);

    Argument<std::string>& Default(const char* val);

    Argument<std::string>& StoreValue(std::string& point);
    Argument<bool>& StoreValue(bool& point);
    Argument<int>& StoreValues(std::vector<int>& point);

    Argument<int>& Positional();
};


class ArgParser {
    Argument<bool> help = Argument<bool>('\0', "\0");
    bool get_help = false;
    std::string description;
    std::vector<Argument<std::string>> string_args {};
    std::vector<Argument<int>> int_args {};
    std::vector<Argument<bool>> flags {};
public:
    explicit ArgParser(std::string info);
    bool CheckArgs();

    Argument<int>& AddIntArgument(const std::string& param);
    Argument<int>& AddIntArgument(char param1, const std::string& param2);
    Argument<int>& AddIntArgument(char param1, const std::string& param2, const std::string& desc);
    Argument<int>& AddIntArgument(const std::string& param2, const std::string& desc);
    Argument<std::string>& AddStringArgument(const std::string& param);
    Argument<std::string>& AddStringArgument(char param1, const std::string& param2);
    Argument<std::string>& AddStringArgument(char param1, const std::string& param2, const std::string& desc);
    Argument<bool>& AddFlag(char param1, const std::string& name);
    Argument<bool>& AddFlag(char param1, const std::string& name, const std::string& desc);
    Argument<bool>& AddFlag(const std::string& name, const std::string& desc);


    bool Parse(const std::vector<std::string>& args);
    bool Parse(int argc, char* args[]);

    void ParseInts(const std::vector<std::string>& args, int index);

    void ParseStrings(const std::vector<std::string>& args, int index);

    void SetFlag(const std::vector<std::string>& args, int index);

    void SetFlags(const std::vector<std::string>& args, int index);


    std::string GetStringValue(const std::string& param);
    int GetIntValue(const std::string& param);
    int GetIntValue(const std::string& param, size_t pos);
    bool GetFlag(const std::string& param);
    bool CheckHelp(const std::string& param);
    bool AddHelp(char param1, const std::string& param2, const std::string& desc);
    bool Help();
    std::string HelpDescription();
};

} // namespace ArgumentParser