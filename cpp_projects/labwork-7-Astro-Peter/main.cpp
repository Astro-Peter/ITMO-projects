#include "frame_classes/Frame.h"
#include "tags/Header.h"
#include <fstream>
#include <iostream>

frames::Frame* GetFrame(std::string& name, std::fstream& file){
    frames::Frame* result = nullptr;
    file.seekg(int64_t (file.tellg()) - 4);
    if (name[0] == 'T' && std::strcmp(name.c_str(), "TXXX") != 0){
        result = new frames::Text_Frame(file);
    } else if (std::strcmp(name.c_str(), "UFID") == 0){
        result = new frames::UFID_Frame(file);
    } else if(name[0] == 'W' && std::strcmp(name.c_str(), "WXXX") != 0){
        result = new frames::URL_Frame(file);
    } else if (std::strcmp(name.c_str(), "WXXX") == 0 || std::strcmp(name.c_str(), "TXXX") == 0){
        result = new frames::TXXX_WXXX_Frame(file);
    } else if (std::strcmp(name.c_str(), "ETCO") == 0){
        result = new frames::ETCO_Frame(file);
    } else if (std::strcmp(name.c_str(), "USLT") == 0 || std::strcmp(name.c_str(), "COMM") == 0){
        result = new frames::USLT_COMM_Frame(file);
    } else if (std::strcmp(name.c_str(), "SYLT") == 0){
        result = new frames::SYLT_Frame(file);
    } else if (std::strcmp(name.c_str(), "RVA2") == 0){
        result = new frames::RVA2_Frame(file);
    } else if (std::strcmp(name.c_str(), "EQU2") == 0){
        result = new frames::EQU2_Frame(file);
    } else if (std::strcmp(name.c_str(), "PCNT") == 0){
        result = new frames::PCNT_Frame(file);
    } else if (std::strcmp(name.c_str(), "POPM") == 0){
        result = new frames::POPM_Frame(file);
    } else if(std::strcmp(name.c_str(), "RBUF") == 0){
        result = new frames::RBUF_Frame(file);
    } else if (std::strcmp(name.c_str(), "LINK") == 0){
        result = new frames::LINK_Frame(file);
    } else if (std::strcmp(name.c_str(), "POSS") == 0){
        result = new frames::POSS_Frame(file);
    } else if (std::strcmp(name.c_str(), "USER") == 0){
        result = new frames::USER_Frame(file);
    } else if (std::strcmp(name.c_str(), "OWNE") == 0){
        result = new frames::OWNE_Frame(file);
    } else if (std::strcmp(name.c_str(), "COMR") == 0){
        result = new frames::COMR_Frame(file);
    } else if (std::strcmp(name.c_str(), "GRID") == 0 || std::strcmp(name.c_str(), "ENCR") == 0) {
        result = new frames::GRID_ENCR_Frame(file);
    } else if (std::strcmp(name.c_str(), "PRIV") == 0){
        result = new frames::PRIV_Frame(file);
    } else if (std::strcmp(name.c_str(), "SEEK") == 0){
        result = new frames::SEEK_Frame(file);
    } else {
        result = new frames::Frame(file, false);
    }
    result->DisplayInfo();
    return result;
}

void AddFrames(std::vector<frames::Frame*>& frames_collection, header::Header& file_header, std::fstream& file){
    size_t size = file_header.size_;
    file_header.DisplayInfo();
    std::cout << "-----------------------\n";
    while (size != 0 && file.tellg() != -1){
        std::string name(4, ' ');
        auto start_pos = file.tellg();
        file.read(&name[0], 4);
        if (name[0] == 0){
            break;
        }
        frames::Frame* tmp = GetFrame(name, file);
        frames_collection.push_back(tmp);
        std::cout << "-----------------------\n";
        size -= (10 + tmp->size_);
        file.seekg(int64_t (start_pos) + 10 + tmp->size_, std::ios::beg);
    }
}

int main(){
    std::cout << "input file name\n";
    std::string file_name;
    std::getline(std::cin, file_name, '\n');
    std::fstream file(file_name, std::fstream::in | std::fstream::binary);

    header::Header file_header(file);
    file.seekg(10);
    std::vector<frames::Frame*> frame_collection;
    AddFrames(frame_collection, file_header, file);
    for (auto& i : frame_collection){
        delete i;
    }
    return 0;
}