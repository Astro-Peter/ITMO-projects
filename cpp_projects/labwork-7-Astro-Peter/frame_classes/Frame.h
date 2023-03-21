//
// Created by Vanst on 07.02.2023.
//

#ifndef LAB7_FRAME_CLASSES_FRAME_H_
#define LAB7_FRAME_CLASSES_FRAME_H_
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include "Methods.h"

namespace frames {

struct RVA2Adjustments{
    uint8_t channel_type_;
    uint16_t volume_adjustment_;
    uint8_t bits_representing_peak_;
    std::vector<uint8_t> peak_volume_{};
    explicit RVA2Adjustments(std::fstream& file);
    size_t GetSize();
    void DisplayRVA2Adjustments();
};

class Frame {
public:
    std::string Frame_ID_;
    size_t size_;
    std::bitset<16> flags_;
    bool correct_frame_ = true;

    explicit Frame(std::fstream& file);
    Frame(std::fstream& file, bool flag);
    virtual void DisplayInfo();
};

class UFID_Frame : public Frame {
public:
    std::string owner_identifier_;
    std::vector<uint8_t> identifier_;

    explicit UFID_Frame(std::fstream& file);
    void DisplayInfo() override;
};

class Text_Frame : public Frame {
public:
    char encoding_;
    std::string text_;

    explicit Text_Frame(std::fstream& file);
    void DisplayInfo() override;
};

class TXXX_WXXX_Frame : public Frame {
public:
    uint8_t encoding_;
    std::string description_;
    std::string text_;

    explicit TXXX_WXXX_Frame(std::fstream& file);
    void DisplayInfo() override;
};

class URL_Frame : public Frame {
public:
    std::string url_;

    explicit URL_Frame(std::fstream& file);
    void DisplayInfo() override;
};

class ETCO_Frame : public Frame {
public:
    uint8_t format_;
    std::vector<std::pair<uint8_t, uint32_t>> events_;

    explicit ETCO_Frame(std::fstream& file);
    void DisplayInfo() override;
};

class USLT_COMM_Frame : public Frame {
public:
    uint8_t encoding_;
    char language_[3];
    std::string content_descriptor_;
    std::string content_;

    explicit USLT_COMM_Frame(std::fstream& file);
    void DisplayInfo() override;
};

class SYLT_Frame : public Frame {
public:
    uint8_t encoding_;
    char language_[3];
    uint8_t format_;
    uint8_t type_;
    std::string descriptor_;
    std::vector<std::pair<std::string, uint32_t>> events_;

    explicit SYLT_Frame(std::fstream& file);
    void DisplayInfo() override;
};

class RVA2_Frame : public Frame {
public:
    std::string identification_;
    std::vector<RVA2Adjustments> info_;

    explicit RVA2_Frame(std::fstream& file);
    void DisplayInfo() override;
};

class EQU2_Frame : public Frame {
public:
    bool interpolation_method_;
    std::string identification_;
    std::vector<uint16_t> frequency_;
    std::vector<float> volume_;

    explicit EQU2_Frame(std::fstream& file);
    void DisplayInfo() override;
};

class PCNT_Frame : public Frame {
public:
    size_t counter_;

    explicit PCNT_Frame(std::fstream& file);
    void DisplayInfo() override;
};

class POPM_Frame : public Frame {
public:
    std::string email_;
    uint8_t rating_;
    size_t counter_;

    explicit POPM_Frame(std::fstream& file);
    void DisplayInfo() override;
};

class RBUF_Frame : public Frame {
public:
    uint32_t buffer_size_;
    bool embedded_;
    uint32_t offset_;

    explicit RBUF_Frame(std::fstream& file);
    void DisplayInfo() override;
};

class LINK_Frame : public Frame {
public:
    char frame_identifier_[4];
    std::string url_;
    std::vector<std::string> id_and_data_;

    explicit LINK_Frame(std::fstream& file);
    void DisplayInfo() override;
};

class POSS_Frame : public Frame {
public:
    uint8_t time_stamp_format_;
    uint32_t time_stamp_;

    explicit POSS_Frame(std::fstream& file);
    void DisplayInfo() override;
};

class USER_Frame : public Frame {
public:
    uint8_t encoding_;
    char language_[3];
    std::string text_;

    explicit USER_Frame(std::fstream& file);
    void DisplayInfo() override;
};

class OWNE_Frame : public Frame {
public:
    uint8_t encoding_;
    std::string price_;
    char date_[8];
    std::string seller_;

    explicit OWNE_Frame(std::fstream& file);
    void DisplayInfo() override;
};

class COMR_Frame : public Frame {
public:
    char encoding_;
    std::string price_;
    std::string until_;
    std::string url_;
    char received_as_;
    std::string name_;
    std::string description_;
    std::string picture_;
    std::string logo_;

    explicit COMR_Frame(std::fstream& file);
    void DisplayInfo() override;
};

class GRID_ENCR_Frame : public URL_Frame {
public:
    std::string owner_identifier_;
    uint8_t group_symbol_;
    std::string group_data_;

    explicit GRID_ENCR_Frame(std::fstream& file);
    void DisplayInfo() override;
};

class PRIV_Frame : public Frame {
public:
    std::string owner_identifier_;
    std::string private_data_;

    explicit PRIV_Frame(std::fstream& file);
    void DisplayInfo() override;
};

class SEEK_Frame : public Frame {
public:
    uint32_t offset_;

    explicit SEEK_Frame(std::fstream& file);
    void DisplayInfo() override;
};
} // frames

#endif //LAB7_FRAME_CLASSES_FRAME_H_
