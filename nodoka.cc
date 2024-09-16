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
    if (!isValid(tile)) {
        throw InvalidFormatException("invalid hai");
    }
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
void decode(std::unordered_map<int, int>& map, std::unordered_map<int, int>& mentsu, std::string s) {
    std::istringstream iss(s);
    char c;
    char c1, c2;
    std::vector<int> v;
    while (true) {
        v.clear();
        iss >> c;
        if (iss.eof()) {
            break;
        }
        switch (c) {
            case '<':
                while (true) {
                    iss >> c1;
                    if (c1 == '>') {
                        break;
                    }
                    if (iss.eof()) {
                        throw InvalidFormatException("unexpected EOS");
                    }
                    iss >> c2;
                    v.push_back(decode(c1, c2));
                }
                std::sort(v.begin(), v.end(), [](int a, int b) { return a < b; });
                if (v[0] == v[v.size() - 1]) {
                    if (v.size() != 3 && v.size() != 4) {
                        throw InvalidFormatException("unexpected size of hai");
                    }
                    if (v.size() == 3) {
                        mentsu[(K_M1 + v[0] - M1) ^ kOpen]++;
                    }
                    else {
                        mentsu[(K_M1 + v[0] - M1) ^ kOpen ^ kKan]++;
                    }
                }
                else {
                    if (v.size() != 3) {
                        throw InvalidFormatException("unexpected size of hai");
                    }
                    if (v[1] != v[0] + 1 || v[2] != v[0] + 2) {
                        throw InvalidFormatException("invalid mentsu format");
                    }
                    mentsu[(S_M1 + (v[0] - M1)) ^ kOpen]++;
                }
                break;
            case '(':
                while (true) {
                    iss >> c1;
                    if (c1 == ')') {
                        break;
                    }
                    if (iss.eof()) {
                        throw InvalidFormatException("unexpected EOS");
                    }
                    iss >> c2;
                    v.push_back(decode(c1, c2));
                }
                std::sort(v.begin(), v.end(), [](int a, int b) { return a < b; });
                if (v.size() != 4) {
                    throw InvalidFormatException("unexpected size of hai");
                }
                if (v[0] != v[3]) {
                    throw InvalidFormatException("different hai");
                }
                mentsu[(K_M1 + (v[0] - M1)) ^ kKan]++;
                break;
            default:
                c1 = c;
                iss >> c2;
                if (iss.eof()) {
                    throw InvalidFormatException("unexpected EOS");
                }
                map[decode(c1, c2)]++;
        }
    }
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

int analyze(std::vector<data_t>& result, std::unordered_map<int, int>& hand, const std::unordered_map<int, int>& visible, std::unordered_map<int, int>& mentsu) {
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
    int mentsu_size = 0;
    for (auto& [_, v] : mentsu) {
        mentsu_size += v;
    }

    // 副露した牌がある場合は七対子と国士無双の向聴数を調べない
    if (!mentsu_size) {

        // 七対子
        data_t chitoitsu = { {}, {}, mentsu, 6 , Chitoitsu, {} };
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
            chitoitsu.shanten += 7 - hand.size();
        }
        result.push_back(chitoitsu);

        // 国士無双
        data_t kokushi = { {}, {}, mentsu, 13, Kokushi, {} };
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
    // ターツ(0-1-1-0)の最初の1がM1になるようにするため-1する
    block_info_t info_block = {M1, M1 - 1};

    // 見えている牌(=自摸れない牌)を減らす
    for (auto& [k, v] : visible) {
        remain[k] -= v;
    }

    // 雀頭あり
    data_t data = { {}, {}, mentsu, 8, Other, {} };
    for (int i = M1; i <= Z7; i++) {
        if (hand[i] >= 2) {
            hand[i] -= 2;
            analyze4mentsu(result, data, hand, remain, mentsu_size, 0, 0, info_mentsu, info_block, i);
            hand[i] += 2;
        }
    }
    // 雀頭なし
    analyze4mentsu(result, data, hand, remain, mentsu_size, 0, 0, info_mentsu, info_block);
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
            info_mentsu.index_first = i;
            data.composition[i - M1 + S_M1]++;
            analyze4mentsu(result, data, hand, remain, mentsu + 1, toitsu, tatsu, info_mentsu, info_block, janto);
            data.composition[i - M1 + S_M1]--;
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
            info_mentsu.index_second = i;
            data.composition[i - M1 + K_M1]++;
            analyze4mentsu(result, data, hand, remain, mentsu + 1, toitsu, tatsu, info_mentsu, info_block, janto);
            data.composition[i - M1 + K_M1]--;
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
                    info_block.index_first = i;
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
                info_block.index_second = i;
                analyze4mentsu(result, data, hand, remain, mentsu, toitsu, tatsu + 1, info_mentsu, info_block, janto);
                info_block.index_second = bak;
                hand[i + 1]++;
                hand[i + 2]++;
                if (under) {
                    remain[i]++;
                    data.valid[i]--;
                }
                if (over) {
                    remain[i + 3]++;
                    data.valid[i + 3]--;
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
    bool is_dup = false;
    for (auto& e : result) {
        if (d.shanten != e.shanten) {
            continue;
        }
        std::unordered_map<int, int> a, b;
        for (auto& [k, v] : d.composition) {
            if (!v) {
                continue;
            }
            a[k] = v;
        }
        for (auto& [k, v] : e.composition) {
            if (!v) {
                continue;
            }
            b[k] = v;
        }
        if (a == b) {
            is_dup = true;
            break;
        }
    }
    if (!is_dup) {
        result.push_back(d);
    }
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

score_t yaku(ShapeType type, std::unordered_map<int, int>& hand, std::unordered_map<int, int> mentsu, int agari, int ba, int ji, std::unordered_map<int, int>& dora) {
    score_t score = {0, 0};
    auto base = hand;
    base[agari]++;
    // 平和
    do {
        auto h = base;
        int count = 0;
        bool is_open = false;
        for (auto& [k, v] : mentsu) {
            if (k & kOpen) {
                is_open = true;
                break;
            }
        }
        for (auto& [k, v] : mentsu) {
            if (
                    (k >= S_M1 && k <= S_M7) ||
                    (k >= S_P1 && k <= S_P7) ||
                    (k >= S_S1 && k <= S_S7)
               ) {
                h[k] -= v;
                h[k + 1] -= v;
                h[k + 2] -= v;
                count += v;
            }
        }
        int janto = 0;
        for (auto& [k ,v] : h) {
            if (v == 2) {
                janto = k;
            }
        }
        if (is_open || count != 4 || !janto || janto > Z4 || janto == ba || janto == ji) {
            break;
        }
        std::vector<data_t> result;
        std::unordered_map<int, int> _;
        auto map = hand;
        analyze(result, map, map, _);
        int shanten = result[0].shanten;
        std::unordered_set<int> valid;
        for (auto& e : result) {
            if (shanten < e.shanten) {
                break;
            }
            for (auto& [k, v] : e.valid) {
                if (!v) {
                    continue;
                }
                valid.emplace(k);
            }
        }
        if (valid.size() >= 2) {
            score.han += 1;
            score.yaku.emplace(Pinfu);
        }
    } while (false);
    // タンヤオ
    {
        auto h = base;
        bool established = true;
        for (auto& [k, v] : h) {
            if (!v) {
                continue;
            }
            if (k == M1 || k == M9 || k == P1 || k == P9 || k == S1 ||
                    k == S9 || k >= Z1) {
                established = false;
            }
        }
        if (established) {
            score.han += 1;
            score.yaku.emplace(Tanyao);
        }
    }
    // 一盃口
    {
        int count = 0;
        for (auto& [key, v] : mentsu) {
            auto k = key & 0x7f; // 鳴いているかを無視
            if (k >= S_M1 && k <= S_S7 && v == 2) {
                count++;
            }
        }
        if (count == 1) {
            score.han += 1;
            score.yaku.emplace(Ipeko);
        }
    }
    // 対々和
    {
        int count = 0;
        for (auto& [key, v] : mentsu) {
            auto k = key & 0x7f; // 鳴いているか/槓子かを無視
            if (k >= K_M1 && k <= K_Z7) {
                count++;
            }
        }
        if (count == 4) {
            score.han += 2;
            score.yaku.emplace(Toitoi);
        }
    }
    // 混老頭
    {
        auto h = base;
        bool established = true;
        bool has_ji = false;
        for (auto& [k, v] : h) {
            if (!v) {
                continue;
            }
            if (
                    (k >= M2 && k <= M8) ||
                    (k >= P2 && k <= P8) ||
                    (k >= S2 && k <= S8)
               ) {
                established = false;
                break;
            }
            if (k >= Z1 && k <= Z7) {
                has_ji = true;
            }
        }
        if (type == Other && has_ji && established) {
            score.han += 2;
            score.yaku.emplace(Honroto);
        }
    }
    // 三暗刻
    {
        int count = 0;
        for (auto& [key, v] : mentsu) {
            auto k = key & 0xff; // 槓子かを無視
            if (!v) {
                continue;
            }
            if (k >= K_M1 && k <= K_Z7) {
                count++;
            }
        }
        if (count >= 3) {
            score.han += 2;
            score.yaku.emplace(Sananko);
        }
    }
    // 三色同刻
    {
        bool established = false;
        for (int i = K_M1; i <= K_M9; i++) {
            if (mentsu[i] && mentsu[i + 11] && mentsu[i + 22]) {
                established = true;
            }
        }
        if (established) {
            score.han += 2;
            score.yaku.emplace(Doko);
        }
    }
    // 三色同順
    {
        bool established = false;
        for (int i = S_M1; i <= S_M7; i++) {
            if (mentsu[i] && mentsu[i + 11] && mentsu[i + 22]) {
                established = true;
            }
        }
        if (established) {
            score.han += 2;
            score.yaku.emplace(Dojun);
        }
    }
    // 小三元
    {
        auto h = base;
        int count = 0;
        for (auto& [k, v] : h) {
            if (k < Z5) {
                continue;
            }
            if (v >= 2) {
                count++;
                if (v >= 3) {
                    count++;
                }
            }
        }
        if (count >= 5) {
            score.han += 2;
            score.yaku.emplace(Shosangen);
        }
    }
    // 一気通貫
    {
        std::unordered_map<int, int> m;
        bool is_open = false;
        for (auto& [key, v] : mentsu) {
            if (!v) {
                continue;
            }
            auto k = key & 0x7f;
            if (k != key) {
                is_open = true;
            }
            m[k] += v;
        }
        if (
                (m[S_M1] && m[S_M4] && m[S_M7]) ||
                (m[S_M1] && m[S_M4] && m[S_M7]) ||
                (m[S_M1] && m[S_M4] && m[S_M7])
           ) {
            score.han += 2 - is_open;
            score.yaku.emplace(Ikki);
        }
    }
    // 三槓子
    {
        int count = 0;
        for (auto& [key, v] : mentsu) {
            auto k = key & 0x17f;
            if (!v) {
                continue;
            }
            if (k & kKan) {
                count++;
            }
        }
        if (count >= 3) {
            score.han += 2;
            score.yaku.emplace(Sankantsu);
        }
    }
    // チャンタ
    {
        auto h = base;
        int count = 0;
        bool is_open = false;
        bool has_ji = false;
        bool has_19 = false;
        for (auto& [k, v] : h) {
            if (!v) {
                continue;
            }
            if (k >= Z1) {
                has_ji = true;
                break;
            }
            if (k == M1 || k == M9 || k == P1 || k == P9 ||
                    k == S1 || k == S9) {
                has_19 = true;
            }
        }
        for (auto& [key, v] : mentsu) {
            auto k = key & 0x7f;
            if (!v) {
                continue;
            }
            if (k != key) {
                is_open = true;
            }
            if (k == S_M1 || k == S_M7 || k == S_P1 || k == S_P7 ||
                    k == S_S1 || k == S_S7
               ) {
                h[k] -= v;
                h[k + 1] -= v;
                h[k + 2] -= v;
                count += v;
            }
            if (k == K_M1 || k == K_M9 ||
                    k == K_P1 || k == K_P9 || k == K_S1 || k == K_S9 ||
                    k >= K_Z1
               ) {
                h[k] -= 3;
                count++;
            }
        }
        int janto = 0;
        for (auto& [k ,v] : h) {
            if (v == 2) {
                janto = k;
            }
        }
        if (has_ji && has_19 && count == 4 && janto && (
                janto == M1 || janto == M9 ||
                janto == P1 || janto == P9 || janto == S1 ||
                janto == S9 || janto >= Z1
                ) && !score.yaku.count(Honroto)
                ) {
            score.han += 2 - is_open;
            score.yaku.emplace(Chanta);
        }
    }
    // 七対子
    {
        if (type == Chitoitsu) {
            score.han += 2;
            score.yaku.emplace(Chi);
        }
    }
    // 混一色
    {
        auto h = base;
        bool is_open = false;
        for (auto& [k, v] : mentsu) {
            if (k & kOpen) {
                is_open = true;
                break;
            }
        }
        std::unordered_set<int> kind;
        for (auto& [k, v] : h) {
            if (!v) {
                continue;
            }
            if (k >= M1 && k <= M9) {
                kind.emplace(0);
            }
            if (k >= P1 && k <= P9) {
                kind.emplace(1);
            }
            if (k >= S1 && k <= S9) {
                kind.emplace(2);
            }
            if (k >= Z1 && k <= Z7) {
                kind.emplace(3);
            }
        }
        if (kind.size() == 2 && kind.count(3)) {
            score.han += 3 - is_open;
            score.yaku.emplace(Honitsu);
        }
    }
    // 純チャン
    {
        auto h = base;
        int count = 0;
        bool is_open = false;
        for (auto& [key, v] : mentsu) {
            auto k = key & 0x7f;
            if (!v) {
                continue;
            }
            if (k != key) {
                is_open = true;
            }
            if (k == S_M1 || k == S_M7 || k == S_P1 || k == S_P7 ||
                    k == S_S1 || k == S_S7
               ) {
                h[k] -= v;
                h[k + 1] -= v;
                h[k + 2] -= v;
                count += v;
            }
            if (k == K_M1 || k == K_M9 ||
                    k == K_P1 || k == K_P9 || k == K_S1 || k == K_S9
               ) {
                h[k] -= 3;
                count++;
            }
        }
        int janto = 0;
        for (auto& [k ,v] : h) {
            if (v == 2) {
                janto = k;
            }
        }
        if (count == 4 && janto && (
                    janto == M1 || janto == M9 ||
                    janto == P1 || janto == P9 || janto == S1 ||
                    janto == S9 || janto >= Z1
                    )) {
            score.han += 3 - is_open;
            score.yaku.emplace(Junchan);
        }
    }
    // 二盃口
    {
        int count = 0;
        for (auto& [k, v] : mentsu) {
            if (k >= S_M1 && k <= S_S7 && v == 2) {
                count++;
            }
        }
        if (count == 2) {
            score.han += 3;
            score.yaku.emplace(Ryanpeko);
        }
    }
    // 清一色
    {
        auto h = base;
        bool is_open = false;
        for (auto& [k, v] : mentsu) {
            if (k & kOpen) {
                is_open = true;
                break;
            }
        }
        std::unordered_set<int> kind;
        for (auto& [k, v] : h) {
            if (!v) {
                continue;
            }
            if (k >= M1 && k <= M9) {
                kind.emplace(0);
            }
            if (k >= P1 && k <= P9) {
                kind.emplace(1);
            }
            if (k >= S1 && k <= S9) {
                kind.emplace(2);
            }
            if (k >= Z1 && k <= Z7) {
                kind.emplace(3);
            }
        }
        if (kind.size() == 1 && !kind.count(3)) {
            score.han += 6 - is_open;
            score.yaku.emplace(Chinitsu);
        }
    }
    // 大三元
    {
        int count = 0;
        for (auto& [key, v] : mentsu) {
            auto k = key & 0x7f; // 鳴いたか/槓子かを無視
            if (!v) {
                continue;
            }
            if (k >= K_Z5 && k <= K_Z7) {
                count++;
            }
        }
        if (count == 3) {
            score.han += 13;
            score.yaku.emplace(Daisangen);
        }
    }
    // 四喜和
    {
        auto h = base;
        int count = 0;
        for (auto& [k, v] : h) {
            if (k < Z1 || k > Z4) {
                continue;
            }
            if (v >= 2) {
                count++;
                if (v >= 3) {
                    count++;
                }
            }
        }
        if (count >= 7) {
            score.han += 13;
            score.yaku.emplace(Sushiho);
        }
    }
    // 字一色
    {
        auto h = base;
        int count = 0;
        bool established = false;
        for (int i = Z1; i <= Z7; i++) {
            if (h[i] == 2) {
                established = true;
            }
        }
        for (auto& [key, v] : mentsu) {
            auto k = key & 0x7f;
            if (k >= K_Z1 && k <= K_Z7) {
                count++;
            }
        }
        if (count == 4 && established) {
            score.han += 13;
            score.yaku.emplace(Tsuiso);
        }
    }
    // 緑一色
    {
        auto h = base;
        bool established = true;
        for (auto& [k, v] : h) {
            if (!v) {
                continue;
            }
            if (k != S2 && k != S3 && k != S4 && k != S6 &&
                    k != S8 && k != Z6) {
                established = false;
                break;
            }
        }
        if (established) {
            score.han += 13;
            score.yaku.emplace(Ryuiso);
        }
    }
    // 清老頭
    {
        auto h = base;
        bool established = true;
        for (auto& [k, v] : h) {
            if (!v) {
                continue;
            }
            if (k != M1 && k != M9 && k != P1 && k != P9 &&
                    k != S1 && k != S9) {
                established = false;
                break;
            }
        }
        if (established) {
            score.han += 13;
            score.yaku.emplace(Chinroto);
        }
    }
    // 四暗刻
    {
        int count = 0;
        for (auto& [key, v] : mentsu) {
            auto k = key & 0xff;
            if (!v) {
                continue;
            }
            if (k >= K_M1 && k <= K_Z7) {
                count++;
            }
        }
        if (count == 4) {
            score.han += 13;
            score.yaku.emplace(Suanko);
        }
    }
    // 国士無双
    {
        auto h = base;
        int count = 0;
        std::unordered_set<int> kind;
        for (auto& [k, v] : h) {
            if (!v) {
                continue;
            }
            if (k == M1 || k == M9 || k == P1 || k == P9 || k == S1 ||
                    k == S9 || (k >= Z1 && k <= Z7)) {
                kind.emplace(k);
                if (v == 2) {
                    count++;
                }
            }
        }
        if (count == 1 && kind.size() == 13) {
            score.han += 13;
            score.yaku.emplace(Kokushimusou);
        }
    }
    // 九蓮宝燈
    {
        bool is_open = false;
        for (auto& [k, v] : mentsu) {
            if (k & kOpen) {
                is_open = true;
                break;
            }
        }
        for (int i = 0; i < 3; i++) {
            auto h = base;
            h[M1 + i * 11] -= 3;
            for (int j = 1; j < 8; j++) {
                h[M1 + j + i * 11]--;
            }
            h[M1 + 8 + i * 11] -= 3;
            int count = 0;
            int hai = 0;
            for (auto& [k, v] : h) {
                if (v == 1) {
                    hai = k;
                }
                if (v) {
                    count++;
                }
            }
            if (!is_open && count == 1 && hai >= M1 + i * 11 && hai < M1 + (i + 1) * 11) {
                score.han += 13;
                score.yaku.emplace(Churen);
                break;
            }
        }
    }
    // 四槓子
    {
        int count = 0;
        for (auto& [key, v] : mentsu) {
            auto k = key & 0x17f;
            if (!v) {
                continue;
            }
            if (k & kKan) {
                count++;
            }
        }
        if (count == 4) {
            score.han += 13;
            score.yaku.emplace(Sukantsu);
        }
    }
    // 翻牌
    {
        int count = 0;
        ba = ba - M1 + K_M1;
        ji = ji - M1 + K_M1;
        for (auto& [key, v] : mentsu) {
            auto k = key & 0x7f;
            if (k >= K_Z5) {
                count++;
            }
            if (k == ba) {
                count++;
            }
            if (k == ji) {
                count++;
            }
        }
        if (count) {
            score.han += count;
            score.yaku.emplace(Fanpai + count - 1);
        }
    }
    // ドラ
    {
        auto h = base;
        int count = 0;
        for (auto& [k, v] : dora) {
            count += h[k] * v;
        }
        if (count) {
            score.han += count;
            score.yaku.emplace(Dora + count - 1);
        }
    }
    return score;
}
