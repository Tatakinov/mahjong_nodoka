#ifndef NODOKA_H_
#define NODOKA_H_

#include <string>
#include <unordered_map>
#include <vector>

// M9 <-> P1, P9 <-> S1, S9 <-> Z1-Z7は間を3つ以上空けることで
// 順子が存在するかを(hand[T] && hand[T + 1] && hand[T + 2])で
// 判定できるようにする
enum Tile {
    M1 = 1, M2, M3, M4, M5, M6, M7, M8, M9,
    P1 = 12, P2, P3, P4, P5, P6, P7, P8, P9,
    S1 = 23, S2, S3, S4, S5, S6, S7, S8, S9,
    Z1 = 35, Z2 = 39, Z3 = 43, Z4 = 47, Z5 = 51, Z6 = 55, Z7 = 59
};

enum Type {
    Chitoitsu, Kokushi, Other
};

struct data_t {
    std::unordered_map<int, int> valid;
    std::unordered_map<int, int> sute;
    int shanten;
    Type type;
};

int decode(char c1, char c2);
std::unordered_map<int, int> decode(std::string s);
std::string encode(int tile);

int analyze(std::vector<data_t>& result, std::unordered_map<int, int>& hand, const std::unordered_map<int, int>& visible, int mentsu = 0);

#endif // NODOKA_H_
