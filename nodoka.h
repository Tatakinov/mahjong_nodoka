#ifndef NODOKA_H_
#define NODOKA_H_

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

const int kOpen = 0x80;
const int kKan = 0x100;

// M9 <-> P1, P9 <-> S1, S9 <-> Z1-Z7は間を3つ以上空けることで
// 順子が存在するかを(hand[T] && hand[T + 1] && hand[T + 2])で
// 判定できるようにする
enum Tile {
    M1 = 1, M2, M3, M4, M5, M6, M7, M8, M9,
    P1 = 12, P2, P3, P4, P5, P6, P7, P8, P9,
    S1 = 23, S2, S3, S4, S5, S6, S7, S8, S9,
    Z1 = 35, Z2 = 39, Z3 = 43, Z4 = 47, Z5 = 51, Z6 = 55, Z7 = 59
};

enum MentsuType {
    S_M1 = 1, S_M2, S_M3, S_M4, S_M5, S_M6, S_M7,
    S_P1 = 12, S_P2, S_P3, S_P4, S_P5, S_P6, S_P7,
    S_S1 = 23, S_S2, S_S3, S_S4, S_S5, S_S6, S_S7,
    K_M1 = 34, K_M2, K_M3, K_M4, K_M5, K_M6, K_M7, K_M8, K_M9,
    K_P1 = 45, K_P2, K_P3, K_P4, K_P5, K_P6, K_P7, K_P8, K_P9,
    K_S1 = 56, K_S2, K_S3, K_S4, K_S5, K_S6, K_S7, K_S8, K_S9,
    K_Z1 = 68, K_Z2 = 72, K_Z3 = 76, K_Z4 = 80, K_Z5 = 84, K_Z6 = 88, K_Z7 = 92,
};

enum ShapeType {
    Chitoitsu, Kokushi, Other
};

enum Yaku {
    Richi = 1,
    Menzen,
    Pinfu,
    Tanyao,
    Haite,
    Hote,
    Rinshan,
    Chankan,
    Ipeko,

    Toitoi,
    Honroto,
    Sananko,
    Doko,
    Dojun,
    Shosangen,
    Ikki,
    Sankantsu,
    Daburi,
    Chanta,
    Chi,

    Honitsu,
    Junchan,
    Ryanpeko,

    Chinitsu,

    Daisangen,
    Sushiho,
    Tsuiso,
    Ryuiso,
    Chinroto,
    Suanko,
    Kokushimusou,
    Churen,
    Sukantsu,

    Fanpai,

    Dora = Fanpai + 5,
};

struct score_t {
    int han { 0 };
    int fu { 0 }; // FIXME stub
    std::unordered_set<int> yaku {};
};

struct data_t {
    std::unordered_map<int, int> valid;
    std::unordered_map<int, int> sute;
    std::unordered_map<int, int> composition;
    int shanten;
    ShapeType shape_type;
    score_t score;
};

class InvalidFormatException : public std::runtime_error {
    public:
        InvalidFormatException(const char *message) : std::runtime_error(message) {}
};

int decode(char c1, char c2);
std::unordered_map<int, int> decode(std::string s);
void decode(std::unordered_map<int, int>& map, std::unordered_map<int, int>& mentsu, std::string s);
std::string encode(int tile);

int analyze(std::vector<data_t>& result, std::unordered_map<int, int>& hand, const std::unordered_map<int, int>& visible, std::unordered_map<int, int>& mentsu);

score_t yaku(ShapeType type, std::unordered_map<int, int>& hand, std::unordered_map<int, int> mentsu, int agari, int ba, int ji, std::unordered_map<int, int>& dora, std::unordered_map<int, int>& sute);

#endif // NODOKA_H_
