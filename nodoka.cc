#include "nodoka.h"

#include <algorithm>
#include <cassert>
#include <sstream>
#include <unordered_set>

// 同じ面子構成を計算しないようにするための
// インデックス情報を保持する構造体
struct block_info_t {
    int index_first;
    int index_second;
};

// 面子/対子/ターツの最大ブロック数
const int kThreshold = 5;

void analyze4mentsu(std::vector<data_t>& result, data_t data, std::unordered_map<int, int>& hand, std::unordered_map<int, int>& remain, int mentsu, int toitsu, int tatsu, block_info_t& info_mentsu, block_info_t& info_block, int janto = 0);

int calcShanten(int mentsu, int toitsu, int tatsu);

bool isValid(int tile) {
    if (tile >= M1 && tile <= M9) {
        return true;
    }
    if (tile >= P1 && tile <= P9) {
        return true;
    }
    if (tile >= S1 && tile <= S9) {
        return true;
    }
    if (tile == Z1 || tile == Z2 || tile == Z3 || tile == Z4 ||
            tile == Z5 || tile == Z6 || tile == Z7) {
        return true;
    }
    return false;
}

int decode(char c1, char c2) {
    int tile = 0;
    c1 -= '1';
    if (c2 == 'm') {
        tile = M1 + c1;
    }
    else if (c2 == 'p') {
        tile = P1 + c1;
    }
    else if (c2 == 's') {
        tile = S1 + c1;
    }
    else if (c2 == 'z') {
        tile = Z1 + c1 * 4;
    }
    assert(isValid(tile));
    return tile;
}

std::unordered_map<int, int> decode(std::string s) {
    std::unordered_map<int, int> map;
    std::istringstream iss(s);
    char c1, c2;
    while (true) {
        iss >> c1;
        iss >> c2;
        if (iss.eof()) {
            break;
        }
        map[decode(c1, c2)]++;
    }
    return map;
}

std::string encode(int tile) {
    std::ostringstream oss;
    if (!isValid(tile)) {
        oss << "ee";
    }
    else if (tile <= M9) {
        oss << (tile - M1 + 1);
        oss << "m";
    }
    else if (tile <= P9) {
        oss << (tile - P1 + 1);
        oss << "p";
    }
    else if (tile <= S9) {
        oss << (tile - S1 + 1);
        oss << "s";
    }
    else {
        oss << ((tile - Z1) / 4 + 1);
        oss << "z";
    }
    return oss.str();
}

int analyze(std::vector<data_t>& result, std::unordered_map<int, int>& hand, const std::unordered_map<int, int>& visible, int mentsu) {
    // 残りの牌枚数
    std::unordered_map<int, int> remain = {
        {M1, 4}, {M2, 4}, {M3, 4},
        {M4, 4}, {M5, 4}, {M6, 4},
        {M7, 4}, {M8, 4}, {M9, 4},
        {P1, 4}, {P2, 4}, {P3, 4},
        {P4, 4}, {P5, 4}, {P6, 4},
        {P7, 4}, {P8, 4}, {P9, 4},
        {S1, 4}, {S2, 4}, {S3, 4},
        {S4, 4}, {S5, 4}, {S6, 4},
        {S7, 4}, {S8, 4}, {S9, 4},
        {Z1, 4}, {Z2, 4}, {Z3, 4},
        {Z4, 4}, {Z5, 4}, {Z6, 4}, {Z7, 4}
    };

    // 副露した牌がある場合は七対子と国士無双の向聴数を調べない
    if (!mentsu) {

        // 七対子
        data_t chitoitsu = { {}, {}, 6 , Chitoitsu};
        for (auto& [k, v] : hand) {
            if (v >= 2) {
                chitoitsu.shanten--;
            }
            if (v == 1) {
                if (remain[k]) {
                    chitoitsu.valid[k] = 1;
                }
                chitoitsu.sute[k] = 1;
            }
        }
        if (hand.size() < 7) {
            chitoitsu.shanten -= 7 - hand.size();
        }
        result.push_back(chitoitsu);

        // 国士無双
        data_t kokushi = { {}, {}, 13, Kokushi };
        std::unordered_set<int> kokushi_set = {M1, M9, P1, P9, S1, S9, Z1, Z2, Z3, Z4, Z5, Z6, Z7};
        bool has_toitsu = false;
        // ダブっている一九字牌が複数ある時は
        // 捨て牌候補に入れる
        for (auto e : kokushi_set) {
            if (hand[e] >= 2) {
                kokushi.sute[e] = 1;
            }
        }
        if (kokushi.sute.size() == 1) {
            kokushi.sute.clear();
        }
        for (auto e : kokushi_set) {
            if (hand[e] >= 3) {
                kokushi.sute[e] = hand[e] - 2;
            }
            if (hand[e]) {
                kokushi.shanten--;
                if (hand[e] >= 2) {
                    has_toitsu = true;
                }
            }
            else {
                kokushi.valid[e]++;
            }
        }
        for (auto& [k, v] : hand) {
            if (!kokushi_set.count(k)) {
                kokushi.sute[k] = v;
            }
        }
        if (has_toitsu) {
            kokushi.shanten--;
        }
        // 一九字牌が枯れている場合は上がれないので
        // 結果に格納しない
        bool can = true;
        for (auto e : kokushi_set) {
            if (!remain[e]) {
                can = false;
                break;
            }
        }
        if (can) {
            result.push_back(kokushi);
        }
    }

    // 4面子1雀頭
    block_info_t info_mentsu = {M1, M1};
    block_info_t info_block = {M1, M1};

    // 見えている牌(=自摸れない牌)を減らす
    for (auto& [k, v] : visible) {
        remain[k] -= v;
    }

    // 雀頭あり
    for (int i = M1; i <= Z7; i++) {
        if (hand[i] >= 2) {
            hand[i] -= 2;
            analyze4mentsu(result, { {}, {}, 8, Other }, hand, remain, mentsu, 0, 0, info_mentsu, info_block, i);
            hand[i] += 2;
        }
    }
    // 雀頭なし
    analyze4mentsu(result, { {}, {}, 8, Other }, hand, remain, mentsu, 0, 0, info_mentsu, info_block);
    std::sort(result.begin(), result.end(), [](auto& a, auto& b) { return a.shanten < b.shanten; });
    return result[0].shanten;
}

void analyze4mentsu(std::vector<data_t>& result, data_t data, std::unordered_map<int, int>& hand, std::unordered_map<int, int>& remain, int mentsu, int toitsu, int tatsu, block_info_t& info_mentsu, block_info_t& info_block, int janto) {
    bool has_mentsu = false;
    // 順子
    for (int i = info_mentsu.index_first; i <= S9; i++) {
        if (hand[i] && hand[i + 1] && hand[i + 2]) {
            has_mentsu = true;
            hand[i]--;
            hand[i + 1]--;
            hand[i + 2]--;
            int bak = info_mentsu.index_first;
            info_mentsu.index_first = i + 1;
            analyze4mentsu(result, data, hand, remain, mentsu + 1, toitsu, tatsu, info_mentsu, info_block, janto);
            info_mentsu.index_first = bak;
            hand[i]++;
            hand[i + 1]++;
            hand[i + 2]++;
        }
    }
    // 刻子
    for (int i = info_mentsu.index_second; i <= Z7; i++) {
        if (hand[i] >= 3) {
            has_mentsu = true;
            hand[i] -= 3;
            int bak = info_mentsu.index_second;
            info_mentsu.index_second = i + 1;
            analyze4mentsu(result, data, hand, remain, mentsu + 1, toitsu, tatsu, info_mentsu, info_block, janto);
            info_mentsu.index_second = bak;
            hand[i] += 3;
        }
    }
    if (has_mentsu) {
        return;
    }
    bool has_block = false;
    //6block以上出来る場合は捨て牌扱いにする
    if (mentsu + toitsu + tatsu + !!janto < kThreshold) {
        // 雀頭なしで対子が作れる==雀頭が作れる==すでに探索済み
        // なのでその場合は無視
        if (janto) {
            // 対子
            for (int i = info_block.index_first; i <= Z7; i++) {
                if (hand[i] >= 2 && remain[i]) {
                    has_block = true;
                    remain[i]--;
                    hand[i] -= 2;
                    data.valid[i]++;
                    int bak = info_block.index_first;
                    info_block.index_first = i + 1;
                    analyze4mentsu(result, data, hand, remain, mentsu, (janto) ? (toitsu + 1) : (toitsu), tatsu, info_mentsu, info_block, (janto) ? (janto) : (i));
                    info_block.index_first = bak;
                    data.valid[i]--;
                    hand[i] += 2;
                    remain[i]++;
                }
            }
        }
        // ターツ(0-1-1-0)
        for (int i = info_block.index_second; i <= S9; i++) {
            bool under = remain[i], over = remain[i + 3];
            if (hand[i + 1] && hand[i + 2] && (under || over)) {
                has_block = true;
                if (under) {
                    data.valid[i]++;
                    remain[i]--;
                }
                if (over) {
                    data.valid[i + 3]++;
                    remain[i + 3]--;
                }
                hand[i + 1]--;
                hand[i + 2]--;
                int bak = info_block.index_second;
                info_block.index_second = i + 1;
                analyze4mentsu(result, data, hand, remain, mentsu, toitsu, tatsu + 1, info_mentsu, info_block, janto);
                info_block.index_second = bak;
                hand[i + 1]++;
                hand[i + 2]++;
                if (under) {
                    remain[i]--;
                    data.valid[i]++;
                }
                if (over) {
                    remain[i + 3]--;
                    data.valid[i + 3]++;
                }
            }
        }
        // ターツ(1-0-1)
        for (int i = info_block.index_second; i <= S9; i++) {
            if (hand[i] && remain[i + 1] && hand[i + 2]) {
                has_block = true;
                data.valid[i + 1]++;
                remain[i + 1]--;
                hand[i]--;
                hand[i + 2]--;
                int bak = info_block.index_second;
                info_block.index_second = i + 1;
                analyze4mentsu(result, data, hand, remain, mentsu, toitsu, tatsu + 1, info_mentsu, info_block, janto);
                info_block.index_second = bak;
                hand[i]++;
                hand[i + 2]++;
                remain[i + 1]++;
                data.valid[i + 1]--;
            }
        }
    }
    if (has_block) {
        return;
    }
    // 残っている牌はすべて捨て牌候補
    std::unordered_map<int, int> sute;
    for (auto& [k, v] : hand) {
        if (v) {
            sute[k] = v;
        }
    }
    data.sute = sute;
    auto d = data;
    if (janto) {
        // 雀頭は対子としてカウントする
        toitsu++;
        // 雀頭以外に対子があるなら雀頭も有効牌になる
        if (toitsu >= 2 && remain[janto]) {
            d.valid[janto]++;
        }
    }
    else {
        // 残りの牌は雀頭になる可能性がある
        for (auto& [k, v] : sute) {
            // 雀頭が無い && 対子が残っている場合は
            // 雀頭有りのanalyze4mentsuですでに生成済みのため
            // 結果を格納しない
            if (v >= 2) {
                return;
            }
            if (remain[k]) {
                d.valid[k]++;
            }
        }
    }
    // 面子が足りない & 対子/ターツが不足している場合は
    // 残っている牌の順子形/刻子形は有効牌
    if (mentsu + toitsu + tatsu < kThreshold && mentsu < kThreshold - 1) {
        if (janto) {
            d.valid[janto] = 1;
        }
        for (auto& [k, _] : sute) {
            if (remain[k] >= 2) {
                d.valid[k] = 1;
            }
            if (remain[k - 2] && remain[k - 1]) {
                d.valid[k - 2] = 1;
                d.valid[k - 1] = 1;
            }
            if (remain[k - 1] && remain[k + 1]) {
                d.valid[k - 1] = 1;
                d.valid[k + 1] = 1;
            }
            if (remain[k + 1] && remain[k + 2]) {
                d.valid[k + 1] = 1;
                d.valid[k + 2] = 1;
            }
        }
    }
    d.shanten = calcShanten(mentsu, toitsu, tatsu);
    result.push_back(d);
}

int calcShanten(int mentsu, int toitsu, int tatsu) {
    int shanten = 8;
    int score = mentsu + toitsu + tatsu;
    // 6ブロック以降は向聴数に関与しない
    if (score > kThreshold) {
        for (; tatsu > 0 && score > kThreshold; tatsu--, score--);
        for (; toitsu > 0 && score > kThreshold; toitsu--, score--);
    }
    shanten -= 2 * mentsu + toitsu + tatsu;
    // 5ブロック出来ているのに対子(=雀頭)が無い場合は向聴数を増やす
    if (score == kThreshold && !toitsu) {
        shanten++;
    }
    return shanten;
}
