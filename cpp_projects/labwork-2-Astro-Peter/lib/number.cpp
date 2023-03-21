#include "number.h"
#include <string>

const int array_size = 34;
const int elem_size = 18;
const uint64_t nine_pow = (uint64_t) std::pow(10, elem_size / 2);
const uint64_t eighteen_pow = (uint64_t) std::pow(10, elem_size);



uint2022_t max_uint(){
    struct uint2022_t max_uint{};
    max_uint = from_string("481560916771158684800786922703235625631274322714142263414417884163925873322306437689024231009526751394401758326916367106052034484602375642882110959089521812209947069992139877256008949136579813164413834190131240610432508865633901300457687591589632190325582710683886781973951695733384278544896131740867054246692573031629150247882082682647773168904426336814855367810693467547461780797071163567159452928068892906992787178135839959347223507647240845924670958716173279750751341651541295792537288393481542519773223140547524361834615428274169543954961376881442030303829940191406452725012875774576546969913778507874303");
    return max_uint;
}

uint2022_t from_uint(uint32_t i) {
    struct uint2022_t res{};
    res.numb[0] = i;
    return res;
}

uint2022_t from_string(const char* buff) {
    struct uint2022_t res{};
    uint64_t number, cnt = 0;
    for (int i = (int)strlen(buff) - 1; i >= 0; i--){
        number = (int) buff[i] - (int) '0';
        res.numb[cnt / elem_size] += number * (uint64_t)std::pow(10, cnt % elem_size);
        cnt++;
    }
    return res;
}

uint2022_t operator+(const uint2022_t& lhs, const uint2022_t& rhs) {
    struct uint2022_t sum{};
    uint64_t prev = 0;
    for (int i = 0; i < array_size; i++){
        sum.numb[i] = (lhs.numb[i] + rhs.numb[i] + prev) % uint64_t(std::pow(10, elem_size));
        prev = (lhs.numb[i] + rhs.numb[i] + prev) / uint64_t(std::pow(10, elem_size));
    }
    return sum;
}

uint2022_t operator-(const uint2022_t& lhs, const uint2022_t& rhs) {
    struct uint2022_t res{};
    int cnt;
    if (rhs <= lhs){
        for (int i = array_size - 1; i >= 0; i--){
            if (lhs.numb[i] >= rhs.numb[i]){
                res.numb[i] = lhs.numb[i] - rhs.numb[i];
            } else{
                cnt = i + 1;
                while (res.numb[cnt] == 0){
                    res.numb[cnt] = eighteen_pow - 1;
                    cnt++;
                }
                res.numb[cnt] -= 1;
                res.numb[i] = eighteen_pow + lhs.numb[i] - rhs.numb[i];
            }
        }
    }
    return res;
}

uint2022_t operator*(const uint2022_t& l, const uint2022_t& r) {
    struct uint2022_t res {.numb{}}, rhs{}, lhs{};
    uint64_t left, right, rest;
    if (l <= r){
        lhs = l;
        rhs = r;
    } else{
        lhs = r;
        rhs = l;
    }
    for (int i = 0; i < array_size; i++) {
        rest = 0;
        right = rhs.numb[i] % nine_pow;
        for (int j = 0; j < array_size; j++) {
            if (i + j > array_size - 1) {
                break;
            }
            left = lhs.numb[j] % nine_pow;
            res.numb[j + i] += left * right + rest;
            left = lhs.numb[j] / nine_pow;
            res.numb[j + i] += ((left * right) % nine_pow) * nine_pow;
            rest = (left * right) / nine_pow + res.numb[j + i] / eighteen_pow;
            res.numb[j + i] = res.numb[j + i] % eighteen_pow;
        }
        right = rhs.numb[i] / nine_pow;
        for (int j = 0; j < array_size; j++) {
            if (i + j > array_size - 1) {
                break;
            }
            left = lhs.numb[j] % nine_pow;
            res.numb[j + i] += rest;
            res.numb[j + i] += ((left * right) % nine_pow) * nine_pow;
            rest = (left * right) / nine_pow;
            left = lhs.numb[j] / nine_pow;
            rest += left * right + res.numb[j + i] / eighteen_pow;
            res.numb[j + i] = res.numb[j + i] % eighteen_pow;
        }
    }
    return res;
}

uint2022_t operator/(const uint2022_t& lhs, const uint2022_t& rhs) {
    return uint2022_t();
}

bool operator==(const uint2022_t& lhs, const uint2022_t& rhs) {
    for (int i = 0; i < array_size; i++){
        if (lhs.numb[i] != rhs.numb[i]){
            return false;
        }
    }
    return true;
}

bool operator!=(const uint2022_t& lhs, const uint2022_t& rhs) {
    return !(lhs == rhs);
}

bool operator<=(const uint2022_t& lhs, const uint2022_t& rhs){
    for (int i = array_size - 1; i >= 0; i--){
        if (lhs.numb[i] < rhs.numb[i]){
            return true;
        }
        else if (lhs.numb[i] > rhs.numb[i]){
            return false;
        }
    }
    return true;
}

std::ostream& operator<<(std::ostream& stream, const uint2022_t& value) {
    bool first = true;
    for (int i = array_size - 1; i >= 0; i--){
        //last number in the array is actually the beginning of our long number, so we start from the end of the array
        if (value.numb[i] == 0 && i != 0 && first){
            continue;
        }
        std::string zeroes;
        for (int j = elem_size - 1; j >= 0; j--){
            if (first){
                stream << std::to_string(value.numb[i]);
                break;
            }
            if ((value.numb[i] / (uint64_t) std::pow(10, j) != 0) || (j == 0)){
                stream << ' ' << zeroes << std::to_string(value.numb[i]);
                break;
            }
            zeroes += '0';
        }
        first = false;
    }
    return stream;
}
