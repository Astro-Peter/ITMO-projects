#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <istream>
#include <map>
#include <fstream>

namespace omfl {

class OMFLField_ {
public:
    std::string name;
    std::string contents;
    std::string type;
    std::vector<OMFLField_> arr;
    std::map<std::string, OMFLField_> keys;
    std::map<std::string, OMFLField_> sections;
    //std::string comment;

    bool right_info = true;

    [[nodiscard]] bool valid() const;
    [[nodiscard]] OMFLField_ Get(const std::vector<std::string>& section_name, int depth) const;
    [[nodiscard]] OMFLField_ OMFLField_::Get(const std::string& section) const;
    //OMFLField_ GetSection(const std::vector<std::string>& section_name, int depth);

    OMFLField_() {
        name = "empty";
        type = "no_type";
        contents = "";
    };
    OMFLField_(const std::string& section_name, const std::string& section) {
        name = section_name;
        contents = "section";
    };
    OMFLField_(const std::string& key_name, const std::string& value, const std::string& key_type);
    explicit OMFLField_(const std::string& key_name);

    OMFLField_ operator[](int pos);
    //OMFLField_& GetSection(const std::vector<std::string>& section_name, int depth);

    [[nodiscard]] bool IsInt() const;
    [[nodiscard]] bool IsFloat() const;
    [[nodiscard]] bool IsString() const;
    [[nodiscard]] bool IsArray() const;
    [[nodiscard]] bool IsBool() const;

    [[nodiscard]] int AsInt() const;
    [[nodiscard]] float AsFloat() const;
    [[nodiscard]] std::string AsString() const;
    [[nodiscard]] bool AsBool() const;
    [[nodiscard]] std::string AsStringOrDefault(std::string def) const;
    [[nodiscard]] float AsFloatOrDefault(float def) const;
    [[nodiscard]] int AsIntOrDefault(int def) const;

    bool MakeOneArg(const std::string& info);
    bool MakeArray(const std::string& info);
    bool MakeField(const std::string& info);
};

/*class OMFLField_ : public OMFLField_{

public:
    OMFLField_* parent = nullptr;
    std::map<std::string, OMFLField_> sections;
    std::map<std::string, OMFLField_> keys;
    OMFLField_();
    OMFLField_ GetSection(const std::vector<std::string>& section_name, int depth);
    explicit OMFLField_(const std::string& section_name) { name = section_name;};
};*/

//struct OMFLField_{
//    std::map<std::string, OMFLField_> subsections;
//    std::string name;
//    explicit OMFLField_(const std::string& str){ name = str;};
//};


//bool CheckKeyValidity(const std::string& key);

//std::string GetType(std::string value);

//OMFLField_ parse(const std::filesystem::path& path);
OMFLField_ parse_file(const std::string& info);

OMFLField_ parse(const std::string& info);

std::vector<std::string> GetName(OMFLField_);

}// namespace
