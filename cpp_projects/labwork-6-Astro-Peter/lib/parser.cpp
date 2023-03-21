#include "parser.h"
using namespace omfl;

std::vector<std::string> SplitString(const std::string& str) {
    std::istringstream iss(str);

    return {std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>()};
}

std::string* StripString(const std::string& info) {
    auto tmp = SplitString(info);
    auto* result = new std::string{};
    if (!tmp.empty()) {
        for (int i = 0; i < tmp.size() - 1; i++) {
            result->append(tmp[i]);
            result->append(" ");
        }
        result->append(tmp.back());
    }
    return result;
}

bool CheckNameValidity(const std::string& name) {
    for (int i = 0; i < name.size(); i++) {
        if (!(isalpha(name[i]) || name[i] == '-' || name[i] == '_' || isdigit(name[i]))
            && !(name[i] == '=' && i == name.size() - 1 && name.size() != 1)) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> GetArgAndName(const std::string& line) {
    std::string tmp = line.substr(0, std::min(line.rfind('#'), line.size()));
    auto info = StripString(tmp);
    std::vector<std::string> result;
    if (!info->empty()) {
        std::string str = info->substr(0, std::min(info->rfind('#'), info->size()));
        auto first = info->substr(0, info->find(' '));
        if (!str.empty()) {
            if (first[0] == '[' && first.back() == ']') {
                result.push_back(first.substr(1, first.size() - 2));
            } else if (CheckNameValidity(first) && first.back() == '=') {
                result.push_back(first.substr(0, first.size() - 1));
                str = str.substr(std::min(str.find(' ') + 1, str.size()));
                result.push_back(str);
            } else if (CheckNameValidity(first)) {
                result.push_back(first);
                str = str.substr(std::min(str.find(' ') + 3, str.size()));
                result.push_back(str);
            } else {
                result.emplace_back("");
                result.emplace_back("");
                result.emplace_back("");
            }
        }
    }
    return result;
}

size_t FindClosingBracket(const std::string& arr) {
    size_t bracket_counter = 1;
    if (arr[1] == ']') {
        return arr.size();
    }
    for (size_t i = 1; i < arr.size(); i++) {
        if (arr[i] == '[') {
            bracket_counter++;
        } else if (arr[i] == ']') {
            bracket_counter--;
        }
        if (bracket_counter == 0) {
            return i;
        }
    }
    return arr.size();
}

std::vector<std::string> SeparateByLine(const std::string& lines) {
    size_t finish = std::min(lines.find('\n'), lines.size());
    std::vector<std::string> result = {lines.substr(0, finish)};
    size_t start = 0;
    if (finish != std::string::npos) {
        while (finish < lines.size()) {
            start = finish + 1;
            finish = std::min(lines.substr(start).find('\n'), lines.substr(start).size());
            result.push_back(lines.substr(start, finish));
            finish += start;
        }
    }
    return result;
}

std::vector<std::string> GetSectionNames(const std::string& name) {
    std::vector<std::string> names;
    auto start = name;
    size_t cnt = std::count(name.begin(), name.end(), '.') + 1;
    for (int i = 0; i < cnt; i++) {
        names.push_back(start.substr(0, std::min(start.find('.'), start.size())));
        start = start.substr(std::min(start.find('.'), start.size() - 1) + 1);
    }
    return names;
}

OMFLField_ OMFLField_::operator[](int pos) {
    if (arr.size() > pos) {
        return arr[pos];
    } else {
        return {};
    }
}

bool OMFLField_::IsInt() const {
    return type == "int";
}

OMFLField_::OMFLField_(const std::string& key_name) {
    name = key_name;
    type = "section";
}
OMFLField_::OMFLField_(const std::string& key_name,
                       const std::string& value,
                       const std::string& key_type) {
    contents = value;
    name = key_name;
    type = key_type;
}

bool OMFLField_::valid() const {
    return right_info;
}

OMFLField_ OMFLField_::Get(const std::vector<std::string>& section_name, int depth) const {
    if (depth < section_name.size()) {
        if (sections.find(section_name[depth]) != sections.end()) {
            return sections.at(section_name[depth]).Get(section_name, depth + 1);
        } else if (keys.find(section_name[depth]) != keys.end()) {
            return keys.at(section_name[depth]);
        }
    }
    return *this;
}

OMFLField_ OMFLField_::Get(const std::string& section) const {
    auto section_name = GetSectionNames(section);
    int depth = 0;
    if (sections.find(section_name[depth]) != sections.end()) {
        return sections.at(section_name[depth]).Get(section_name, depth + 1);
    } else if (keys.find(section_name[depth]) != keys.end()) {
        return keys.at(section);
    }
    std::cout << "adsadsdsada";
    return OMFLField_();
}

OMFLField_& GetSection(const std::vector<std::string>& section_name, int depth, OMFLField_& parent) {
    if (depth < section_name.size() && parent.sections.find(section_name[depth]) != parent.sections.end()) {
        return GetSection(section_name, depth + 1, parent.sections.at(section_name[depth]));
    } else if (depth < section_name.size()) {
        auto tmp = OMFLField_(section_name[depth], "section");
        parent.sections.emplace(section_name[depth], tmp);
        return GetSection(section_name, depth + 1, parent.sections.at(section_name[depth]));
    } else {
        return parent;
    }
}

bool OMFLField_::MakeOneArg(const std::string& info) {
    if (isdigit(info[0]) || info[0] == '-' || info[0] == '+') {
        if (info == "-" || info == "+") {
            return false;
        }
        bool dbl = false;
        type = "int";
        for (int i = 1; i < info.size(); i++) {
            if (!isdigit(info[i])) {
                if (!dbl && info[i] == '.' && i < info.size() - 1 && (i > 1 || (info[0] != '-' && info[0] != '+'))) {
                    dbl = true;
                    type = "double";
                    continue;
                }
                return false;
            }
        }
        contents = info;
        return true;
    } else if (info[0] == '\"') {
        int cnt = 1;
        for (int i = 1; i < info.size(); i++) {
            if (info[i] == '\"' && info[i - 1] != '\\') {
                cnt++;
            }
        }
        if (cnt != 2) {
            return false;
        }
        contents = info.substr(1, info.size() - 2);
        type = "string";
        return true;
    } else if (info == "true" || info == "false") {
        type = "bool";
        contents = info;
        return true;
    } else {
        return false;
    }
}

bool OMFLField_::MakeArray(const std::string& info) {
    type = "array";
    auto remains = info.substr(1, info.size() - 2);
    while (!remains.empty()) {
        OMFLField_ tmp{};
        if (remains[0] != '[') {
            auto obj = remains.substr(0, remains.find(','));
            obj = *StripString(obj);
            if (!tmp.MakeOneArg(obj)) {
                return false;
            }
            tmp.MakeOneArg(obj);
            if (remains.find(',') != std::string::npos) {
                remains = remains.substr(remains.find(',') + 1);
            } else {
                remains = "";
            }
        } else {
            size_t end = FindClosingBracket(remains);
            if (remains.size() > end) {
                auto obj = remains.substr(0, end + 1);
                if (!tmp.MakeArray(obj)) {
                    return false;
                }
                tmp.MakeArray(obj);
                remains = remains.substr(std::min(end + 2, remains.size()));
            } else {
                return false;
            }
        }
        remains = *StripString(remains);
        arr.push_back(tmp);
    }
    return true;
}

bool OMFLField_::MakeField(const std::string& info) {
    if (info[0] != '[') {
        return MakeOneArg(info);
    } else if (info.back() == ']') {
        return MakeArray(info);
    } else {
        return false;
    }
}
int OMFLField_::AsInt() const {
    return std::stoi(contents);
}
bool OMFLField_::IsFloat() const {
    return type == "double";
}
bool OMFLField_::IsString() const {
    return type == "string";
}
bool OMFLField_::IsArray() const {
    return type == "array";
}
bool OMFLField_::IsBool() const {
    return type == "bool";
}
float OMFLField_::AsFloat() const {
    return std::stof(contents);
}
std::string OMFLField_::AsString() const {
    return contents;
}
bool OMFLField_::AsBool() const {
    std::cout << "gg" << contents << "gg";
    return contents == "true";
}
std::string OMFLField_::AsStringOrDefault(std::string def) const {
    if (type != "string") {
        return def;
    }
    return contents;
}
float OMFLField_::AsFloatOrDefault(float def) const {
    if (type != "double") {
        return def;
    }
    return std::stof(contents);
}
int OMFLField_::AsIntOrDefault(int def) const {
    if (type != "int") {
        return def;
    }
    return std::stoi(contents);
}

OMFLField_ omfl::parse_file(const std::string& info) {
    OMFLField_ res{};
    OMFLField_* current_section = &res;
    std::ifstream file(info);
    while (file.good()) {
        std::string i;
        std::getline(file, i, '\n');
        auto elems = GetArgAndName(i);
        if (elems.empty()) {
            continue;
        } else if (current_section->keys.find(elems[0]) != current_section->keys.end()) {
            res.right_info = false;
            break;
        } else if (elems.size() == 1) {
            auto names = GetSectionNames(elems[0]);
            for (const auto& j : names) {
                if (j.empty()) {
                    res.right_info = false;
                    break;
                }
            }
            current_section = &GetSection(names, 0, res);
        } else if (elems.size() == 2) {
            if (elems[1][0] == '[' && elems[1].back() == ']') {
                auto tmp = OMFLField_(elems[0], elems[1], "array");
                if (tmp.MakeArray(elems[1])) {
                    tmp.MakeArray(elems[1]);
                    current_section->keys.emplace(elems[0], tmp);
                } else {
                    res.right_info = false;
                    break;
                }
            } else {
                auto tmp = OMFLField_(elems[0], elems[1]);
                if (tmp.MakeOneArg(elems[1])) {
                    tmp.MakeOneArg(elems[1]);
                    current_section->keys.emplace(elems[0], tmp);
                } else {
                    res.right_info = false;
                    break;
                }
            }
        } else {
            res.right_info = false;
            break;
        }
    }
    return res;
}

OMFLField_ omfl::parse(const std::string& info) {
    OMFLField_ res{};
    OMFLField_* current_section = &res;
    if (std::filesystem::exists(info)) {
        return parse_file(info);
    }
    auto strings = SeparateByLine(info);
    for (const std::string& i : strings) {
        auto elems = GetArgAndName(i);
        if (elems.empty()) {
            continue;
        } else if (current_section->keys.find(elems[0]) != current_section->keys.end()) {
            res.right_info = false;
            break;
        } else if (elems.size() == 1) {
            auto names = GetSectionNames(elems[0]);
            for (const auto& j : names) {
                if (j.empty()) {
                    res.right_info = false;
                    break;
                }
            }
            current_section = &GetSection(names, 0, res);
        } else if (elems.size() == 2) {
            if (elems[1][0] == '[' && elems[1].back() == ']') {
                auto tmp = OMFLField_(elems[0], elems[1], "array");
                if (tmp.MakeArray(elems[1])) {
                    tmp.MakeArray(elems[1]);
                    current_section->keys.emplace(elems[0], tmp);
                } else {
                    res.right_info = false;
                    break;
                }
            } else {
                auto tmp = OMFLField_(elems[0], elems[1]);
                if (tmp.MakeOneArg(elems[1])) {
                    tmp.MakeOneArg(elems[1]);
                    current_section->keys.emplace(elems[0], tmp);
                } else {
                    res.right_info = false;
                    break;
                }
            }
        } else {
            res.right_info = false;
            break;
        }
    }
    return res;
}
