//
// Created by Vanst on 07.02.2023.
//

#include "Frame.h"
#include <codecvt>

namespace frames {



Frame::Frame(std::fstream& file) {
    Frame_ID_.resize(4, ' ');
    auto last_pos = file.tellg();
    file.read(&Frame_ID_[0], 4);
    std::string synch_safe_size(4, ' ');
    file.seekg(int64_t (last_pos) + 4, std::ios::beg);
    file.read(&synch_safe_size[0], 4);
    std::cout << "pos: " << file.tellg() << std::endl;
    file.seekg(int64_t (last_pos) + 8, std::ios::beg);
    size_ = common_methods::GetSynchSafeInteger(synch_safe_size);
    uint16_t flags;
    file.read(reinterpret_cast<char*>(&flags), 2);
    flags_ = static_cast<std::bitset<16>>(flags);
    if (!common_methods::FrameCheckFlagsCorrectness(flags_)){
        correct_frame_ = false;
        return;
    }
}

void Frame::DisplayInfo() {
    std::cout << "id: " << Frame_ID_[0] << Frame_ID_[1] << Frame_ID_[2] << Frame_ID_[3] << std::endl;
    std::cout << "size: " << size_ << std::endl;
    std::cout << "flags: " << flags_ << std::endl;
}

Frame::Frame(std::fstream& file, bool flag) {
    Frame_ID_.resize(4, ' ');
    file.read(&Frame_ID_[0], 4);
    std::string synch_safe_size(4, ' ');
    file.read(&synch_safe_size[0], 4);
    size_ = common_methods::GetSynchSafeInteger(synch_safe_size);
    char flags[2];
    file.read(flags, 2);
    flags_ = static_cast<std::bitset<16>>(flags);
    if (!common_methods::FrameCheckFlagsCorrectness(flags_)){
        correct_frame_ = false;
        //return;
    }
    file.ignore(size_);
}

UFID_Frame::UFID_Frame(std::fstream& file) : Frame(file) {
    std::getline(file, owner_identifier_);
    for (int i = 0; i < size_ - (owner_identifier_.size() + 1); i++){
        identifier_.push_back(file.get());
    }
}

void UFID_Frame::DisplayInfo() {
    Frame::DisplayInfo();
    std::cout << "owner: " << owner_identifier_ << '\n';
    std::cout << "id: ";
    for (auto i : identifier_){
        std::cout << static_cast<std::bitset<8>>(i) << ' ';
    }
    std::cout << std::endl;
}

Text_Frame::Text_Frame(std::fstream& file) : Frame(file) {
    file.get(encoding_);
    if (encoding_ == 1 || encoding_ == 2){
        common_methods::GetUTF16StringToUTF8(text_, file, size_ - 1);
    } else {
        std::string tmp(size_ - sizeof(char), ' ');
        file.read(&tmp[0], size_ - sizeof(char));
        text_ = std::move(tmp);
    }
}

void Text_Frame::DisplayInfo() {
    Frame::DisplayInfo();
    common_methods::OutputFrameEncoding(encoding_);
    std::cout << "text: " << text_ << std::endl;
}

URL_Frame::URL_Frame(std::fstream& file) : Frame(file) {
    std::string tmp(size_, ' ');
    file.read(&tmp[0], size_);
    url_ = std::move(tmp);
}

void URL_Frame::DisplayInfo() {
    Frame::DisplayInfo();
    std::cout << "URL: " << url_ << std::endl;
}

TXXX_WXXX_Frame::TXXX_WXXX_Frame(std::fstream& file) : Frame(file) {
    encoding_ = file.get();
    if (encoding_ == 0 || encoding_ == 3){
        std::getline(file, description_, '\0');
        std::string tmp(size_ -  description_.size() - sizeof(encoding_) - 1, ' ');
        file.read(&tmp[0], size_ -  description_.size() - sizeof(encoding_) - 1);
        text_ = std::move(tmp);
    } else {
        size_t bytes_left = common_methods::GetUTF16StringToUTF8(description_, file, size_ - sizeof(encoding_) - 1);
        std::string tmp(bytes_left, ' ');
        common_methods::GetUTF16StringToUTF8(tmp, file, bytes_left);
        text_ = std::move(tmp);
    }
}

void TXXX_WXXX_Frame::DisplayInfo() {
    Frame::DisplayInfo();
    common_methods::OutputFrameEncoding(encoding_);
    if (std::strcmp(Frame_ID_.c_str(), "TXXX") == 0){
        std::cout << "description: " << description_ << std::endl;
        std::cout << "text: " << text_ << std::endl;
    } else {
        std::cout << "description: " << description_ << std::endl;
        std::cout << "URL: " << text_ << std::endl;
    }
}

ETCO_Frame::ETCO_Frame(std::fstream& file) : Frame(file) {
    file.read(reinterpret_cast<char*>(&format_), sizeof(format_));
    for (size_t i = size_ - 1; i > 0; i -= 5){
        uint8_t event_type;
        uint32_t event_time;
        file.read(reinterpret_cast<char*>(&event_type), sizeof(event_type));
        file.read(reinterpret_cast<char*>(&event_time), sizeof(event_time));
        std::pair<uint8_t, uint32_t> event(event_type, event_time);
        events_.emplace_back(event);
    }
}

void ETCO_Frame::DisplayInfo() {
    Frame::DisplayInfo();
    common_methods::OutputTimeStampFormat(format_);
    for (auto i : events_){
        common_methods::OutputETCOEvent(i);
    }
}

USLT_COMM_Frame::USLT_COMM_Frame(std::fstream& file) : Frame(file) {
    encoding_ = file.get();
    file.read(language_, 3);
    if (encoding_ == 0 || encoding_ == 3){
        std::getline(file, content_descriptor_, '\0');
        std::string tmp(size_ -  content_descriptor_.size() - sizeof(encoding_) - 4, ' ');
        file.read(&tmp[0], size_ -  content_descriptor_.size() - sizeof(encoding_) - 4);
        content_ = std::move(tmp);
    } else {
        size_t bytes_left = common_methods::GetUTF16StringToUTF8(content_descriptor_, file, size_ - sizeof(encoding_) - 3);
        std::string tmp(bytes_left, ' ');
        common_methods::GetUTF16StringToUTF8(tmp, file, bytes_left);
        content_ = std::move(tmp);
    }
}

void USLT_COMM_Frame::DisplayInfo() {
    Frame::DisplayInfo();
    common_methods::OutputFrameEncoding(encoding_);
    std::cout << "language: " << language_ << std::endl;
    std::cout << "content description: " << content_descriptor_ << std::endl;
    std::cout << "content: " << content_ << std::endl;
}

SYLT_Frame::SYLT_Frame(std::fstream& file) : Frame(file) {
    encoding_ = file.get();
    file.read(language_, 3);
    file.read(reinterpret_cast<char*>(&format_), sizeof(uint8_t));
    file.read(reinterpret_cast<char*>(&type_), sizeof(uint8_t));
    if (encoding_ == 0 || encoding_ == 3){
        std::getline(file, descriptor_, '\0');
        size_t bytes_left = size_ - 6 * sizeof(uint8_t) - descriptor_.size() - 1;
        std::cout << bytes_left;
        while (bytes_left > 0){
            std::pair<std::string, uint32_t> event;
            std::getline(file, event.first, '\0');
            file.read(reinterpret_cast<char*>(&event.second), sizeof(uint32_t));
            bytes_left -= (event.first.size() + sizeof(uint32_t) + 1);
            events_.emplace_back(event);
            std::cout << bytes_left << '\n';
        }
    } else {
        size_t bytes_left = common_methods::GetUTF16StringToUTF8(descriptor_, file, size_ - 6 *  sizeof(uint8_t));
        while (bytes_left > 0){
            std::pair<std::string, uint32_t> event;
            bytes_left = common_methods::GetUTF16StringToUTF8(event.first, file, bytes_left);
            file.read(reinterpret_cast<char*>(&event.second), sizeof(uint32_t));
            bytes_left -= (sizeof(char16_t) + sizeof(uint32_t));
            events_.emplace_back(event);
        }
    }
}

void SYLT_Frame::DisplayInfo() {
    Frame::DisplayInfo();
    common_methods::OutputFrameEncoding(encoding_);
    std::cout << "language: " << language_ << std::endl;
    common_methods::OutputTimeStampFormat(format_);
    common_methods::OutputContentType(type_);
    std::cout << "content descriptor: " << descriptor_ << std::endl;
    for (const auto& i : events_){
        std::cout << "event: " << i.first << std::endl;
        std::cout << "time: " << i.second << std::endl;
    }
}

RVA2Adjustments::RVA2Adjustments(std::fstream& file) {
    file.read(reinterpret_cast<char*>(&channel_type_), sizeof(uint8_t));
    file.read(reinterpret_cast<char*>(&volume_adjustment_), sizeof(uint16_t));
    file.read(reinterpret_cast<char*>(&bits_representing_peak_), sizeof(uint8_t));
    for (int i = ((bits_representing_peak_ / 8) + (bits_representing_peak_ % 8 == 0)); i > 0; i--){
        uint8_t byte;
        file.read(reinterpret_cast<char*>(&byte), sizeof(uint8_t));
        peak_volume_.emplace_back(byte);
    }
}

size_t RVA2Adjustments::GetSize() {
    return sizeof(channel_type_) + sizeof(bits_representing_peak_) + sizeof(volume_adjustment_) + peak_volume_.size();
}

void RVA2Adjustments::DisplayRVA2Adjustments() {
    common_methods::OutputChannelType(channel_type_);
    std::cout << "volume adjustment: " << volume_adjustment_ << std::endl;
    std::cout << "bits representing peak: " << bits_representing_peak_ << std::endl;
    if (peak_volume_.size() > 0){
        std::cout << "peak: ";
        for (auto i : peak_volume_){
            std::cout << static_cast<std::bitset<8>>(i);
        }
    }
    std::cout << '\n';
}

RVA2_Frame::RVA2_Frame(std::fstream& file) : Frame(file) {
    std::getline(file, identification_, '\0');
    size_t bytes_left = size_ - identification_.size() + 1;
    while (bytes_left > 0){
        RVA2Adjustments tmp(file);
        info_.emplace_back(file);
        bytes_left-= tmp.GetSize();
    }
}

void RVA2_Frame::DisplayInfo() {
    Frame::DisplayInfo();
    std::cout << "identification: " << identification_ << std::endl;
    for (auto& i : info_){
        i.DisplayRVA2Adjustments();
    }
}

PCNT_Frame::PCNT_Frame(std::fstream& file) : Frame(file) {
    file.read(reinterpret_cast<char*>(counter_), size_);
}

void PCNT_Frame::DisplayInfo() {
    Frame::DisplayInfo();
    std::cout << "times played: " << counter_ << std::endl;
}

POPM_Frame::POPM_Frame(std::fstream& file) : Frame(file) {
    std::getline(file, email_, '\0');
    file.read(reinterpret_cast<char*>(&rating_), sizeof(uint8_t));
    std::cout << "szie" << size_ - email_.size() - 1 - 1;
    for (size_t i = 0; i < size_ - email_.size() - 1 - 1; i++){
        uint8_t byte;
        file.read(reinterpret_cast<char*>(&byte), 1);
        size_ += uint64_t(byte) << (size_ - email_.size() - 1 - 1 - i) * 8;
    }
    std::cout << 's';
}

void POPM_Frame::DisplayInfo() {
    Frame::DisplayInfo();
    std::cout << "email: " << email_ << std::endl;
    std::cout << "rating: " << rating_ << std::endl;
    std::cout << "counter: " << counter_ << std::endl;
}

RBUF_Frame::RBUF_Frame(std::fstream& file) : Frame(file) {
    file.read(reinterpret_cast<char*>(buffer_size_), sizeof(uint8_t) * 3);
    file.read(reinterpret_cast<char*>(embedded_), sizeof(bool));
    file.read(reinterpret_cast<char*>(offset_), sizeof(uint32_t));
}

void RBUF_Frame::DisplayInfo() {
    Frame::DisplayInfo();
    std::cout << "buffer size: " << buffer_size_ << std::endl;
    std::cout << "embedded: " << embedded_ << std::endl;
    std::cout << "offset: " << offset_ << std::endl;
}

LINK_Frame::LINK_Frame(std::fstream& file) : Frame(file) {
    file.get(frame_identifier_, 4);
    std::getline(file, url_, '\0');
    size_t bytes_left = size_ - url_.size() - sizeof(char) - 4;
    while (bytes_left > 0){
        char byte;
        std::string tmp;
        file.get(byte);
        bytes_left--;
        while (byte != 0 && bytes_left > 0){
            tmp.push_back(byte);
            file.get(byte);
            bytes_left--;
        }
        id_and_data_.push_back(tmp);
    }
}

void LINK_Frame::DisplayInfo() {
    Frame::DisplayInfo();
    std::cout << "frame identifier: " << frame_identifier_ << std::endl;
    std::cout << "url" << url_ << std::endl;
    for (auto& i : id_and_data_) {
        std::cout << "data: " << i << std::endl;
    }
}

POSS_Frame::POSS_Frame(std::fstream& file) : Frame(file) {
    file.read(reinterpret_cast<char*>(time_stamp_format_), sizeof(uint8_t));
    file.read(reinterpret_cast<char*>(time_stamp_), sizeof(uint32_t));
}

void POSS_Frame::DisplayInfo() {
    Frame::DisplayInfo();
    common_methods::OutputTimeStampFormat(time_stamp_format_);
    std::cout << "time stamp: " << time_stamp_ << std::endl;
}

USER_Frame::USER_Frame(std::fstream& file) : Frame(file) {
    file.read(reinterpret_cast<char*>(encoding_), sizeof(uint8_t));
    file.read(language_, 3);
    if (encoding_ == 0 || encoding_ == 3){
        std::string tmp(size_ -  3 - sizeof(encoding_), ' ');
        file.read(&tmp[0], size_ -  3 - sizeof(encoding_));
        text_ = std::move(tmp);
    } else {
        size_t bytes_left = size_ - 3 - sizeof(encoding_);
        common_methods::GetUTF16StringToUTF8(text_, file, bytes_left);
    }
}

void USER_Frame::DisplayInfo() {
    Frame::DisplayInfo();
    common_methods::OutputFrameEncoding(encoding_);
    std::cout << "language: " << language_ << std::endl;
    std::cout << "text: " << text_ << std::endl;
}

OWNE_Frame::OWNE_Frame(std::fstream& file) : Frame(file) {
    file.read(reinterpret_cast<char*>(encoding_), sizeof(uint8_t));
    std::getline(file, price_, '\0');
    file.read(date_, sizeof(char) * 8);
    std::string tmp(size_ - price_.size() - sizeof(uint8_t) - 8 * sizeof(char), ' ');
    file.read(&tmp[0], size_ - price_.size() - sizeof(uint8_t) - 8 * sizeof(char));
    seller_ = std::move(tmp);
}

void OWNE_Frame::DisplayInfo() {
    Frame::DisplayInfo();
    common_methods::OutputFrameEncoding(encoding_);
    std::cout << "price: " << price_ << std::endl;
    std::cout << "date: " << date_ << std::endl;
    std::cout << "seller: " << seller_ << std::endl;
}

SEEK_Frame::SEEK_Frame(std::fstream& file) : Frame(file) {
    file.read(reinterpret_cast<char*>(offset_), sizeof(uint32_t));
}

void SEEK_Frame::DisplayInfo() {
    Frame::DisplayInfo();
    std::cout << "offset: " << offset_ << std::endl;
}

PRIV_Frame::PRIV_Frame(std::fstream& file) : Frame(file) {
    std::getline(file, owner_identifier_, '\0');
    std::string tmp(size_ - owner_identifier_.size(), ' ');
    file.read(&tmp[0], size_ - owner_identifier_.size());
    private_data_ = std::move(tmp);
}

void PRIV_Frame::DisplayInfo() {
    Frame::DisplayInfo();
    std::cout << "owner identifier: " << owner_identifier_ << std::endl;
    std::cout << "private data: " << private_data_ << std::endl;
}

GRID_ENCR_Frame::GRID_ENCR_Frame(std::fstream& file) : URL_Frame(file) {
    std::getline(file, owner_identifier_, '\0');
    file.read(reinterpret_cast<char*>(group_symbol_), sizeof(uint8_t));
    std::string tmp(size_ - owner_identifier_.size() - sizeof(uint8_t), ' ');
    file.read(&tmp[0], size_ - owner_identifier_.size() - sizeof(uint8_t));
    group_data_ = std::move(tmp);
}

void GRID_ENCR_Frame::DisplayInfo() {
    URL_Frame::DisplayInfo();
    std::cout << "owner identifier: " << owner_identifier_ << std::endl;
    std::cout << "group symbol: " << group_symbol_ << std::endl;
    std::cout << "group data: " << group_data_ << std::endl;
}

EQU2_Frame::EQU2_Frame(std::fstream& file) : Frame(file) {
    file.read(reinterpret_cast<char*>(&interpolation_method_), 1);
    std::getline(file, identification_, '\0');
    for(size_t i = 0; i < size_ + 2 + identification_.size(); i+= 4){
        uint16_t freq;
        int16_t adj;
        file.read(reinterpret_cast<char*>(&freq), 2);
        file.read(reinterpret_cast<char*>(&adj), 2);
        frequency_.push_back(freq / 2);
        volume_.push_back(float(adj) / 512);
    }
}

void EQU2_Frame::DisplayInfo() {
    Frame::DisplayInfo();
    std::cout << "interpolation method: ";
    if (!interpolation_method_){
        std::cout << "No interpolation is made. A jump from one adjustment level to\n"
                  << "another occurs in the middle between two adjustment points.\n";
    } else {
        std::cout << "Interpolation between adjustment points is linear.\n";
    }
    std::cout << "identification: " << identification_ << std::endl;
    for (int i = 0; i < volume_.size(); i++){
        std::cout << "frequency: " << frequency_[i] << std::endl;
        std::cout << "volume adjustment: " << volume_[i] << std::endl;
    }
}

COMR_Frame::COMR_Frame(std::fstream& file) : Frame(file) {
    file.get(encoding_);
    std::getline(file, price_, '\0');
    until_.resize(8, ' ');
    file.read(&until_[0], 8);
    std::getline(file, url_, '\0');
    file.get(received_as_);
    size_t bytes_left = size_ - sizeof(char) * 5 - until_.size() - price_.size() - url_.size();
    if (encoding_ == 0 || encoding_ == 3){
        std::getline(file, name_, '\0');
        std::getline(file, description_, '\0');
        bytes_left -= (name_.size() + description_.size() + sizeof(char) * 2);
    } else {
        bytes_left = common_methods::GetUTF16StringToUTF8(name_, file, bytes_left);
        bytes_left = common_methods::GetUTF16StringToUTF8(description_, file, bytes_left);
    }
    std::getline(file, picture_, '\0');
    logo_.resize(bytes_left - picture_.size() - sizeof(char));
    file.read(&logo_[0], bytes_left - picture_.size() - sizeof(char));
}

void COMR_Frame::DisplayInfo() {
    Frame::DisplayInfo();
    common_methods::OutputFrameEncoding(encoding_);
    std::cout << "price: " << price_ << std::endl;
    std::cout << "valid until: " << until_ << std::endl;
    std::cout << "url: " << url_ << std::endl;
    common_methods::Received(received_as_);
    std::cout << "seller name: " << name_ << std::endl;
    std::cout << "description: " << description_ << std::endl;
}
} // frames