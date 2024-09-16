#include "lib.h"

#include "nodoka.h"
#include "saori.h"
#include "util.h"

bool __load(std::string path) {
    return true;
}

bool __unload() {
    return true;
}

std::string __request(std::string request) {
    saori::Request req = saori::Request::parse(request);
    if (!req(0) || !req(1) || !req(2)) {
        saori::Response res{204, "No Content"};
        return res;
    }
    if (req(0).value() == "shanten") {
        try {
            saori::Response res = {200, "OK"};
            std::unordered_map<int, int> hand, composition;
            decode(hand, composition, req(1).value());
            auto visible = decode(req(2).value());
            std::vector<data_t> result;
            analyze(result, hand, visible, composition);
            if (result.size() == 0) {
                res() = "14,,";
                return res;
            }
            // 最小向聴数のデータの有効牌/不要牌をまとめたものを返す
            int shanten = result[0].shanten;
            std::unordered_set<int> valid, sute;
            for (auto& e : result) {
                if (shanten < e.shanten) {
                    break;
                }
                for (auto& [k, v] : e.valid) {
                    if (v && !valid.count(k)) {
                        valid.emplace(k);
                    }
                }
                for (auto& [k, v] : e.sute) {
                    if (v && !sute.count(k)) {
                        sute.emplace(k);
                    }
                }
            }
            std::ostringstream oss;
            oss << shanten << ",";
            for (auto e : valid) {
                oss << encode(e);
            }
            oss << ",";
            for (auto e : sute) {
                oss << encode(e);
            }
            res() = oss.str();
            return res;
        }
        catch (const InvalidFormatException& e) {
            saori::Response res = {200, "OK"};
            res() = "Error";
            res(0) = e.what();
            return res;
        }
    }
    if (req(0).value() == "yaku") {
        try {
            saori::Response res = {200, "OK"};
            auto hand_s = req(1).value() + req(3).value();
            auto visible_s = req(2).value();
            auto tsumo_s = req(3).value();
            auto all_s = req(1).value();
            auto dora_s = req(4).value();
            auto ba_s = req(5).value();
            auto ji_s = req(5).value();

            std::unordered_map<int, int> hand, composition;
            decode(hand, composition, hand_s);
            auto visible = decode(visible_s);
            auto dora = decode(dora_s);
            int tsumo = 0, ba = 0, ji = 0;
            for (auto [k, v] : decode(tsumo_s)) {
                tsumo = k;
            }
            for (auto [k, v] : decode(ba_s)) {
                ba = k;
            }
            for (auto [k, v] : decode(ji_s)) {
                ji = k;
            }
            auto it = std::remove_if(all_s.begin(), all_s.end(), [](char c) {
                return c == '<' || c == '>' || c == '(' || c == ')';
            });
            all_s.erase(it, all_s.end());
            auto all = decode(all_s);

            std::vector<data_t> result;
            analyze(result, hand, visible, composition);
            if (result.size() == 0) {
                res() = "14,,";
                return res;
            }
            // 最小向聴数のデータの有効牌/不要牌をまとめたものを返す
            int shanten = result[0].shanten;
            std::unordered_set<int> valid, sute;
            std::vector<data_t> yaku_result;
            for (auto& e : result) {
                if (shanten < e.shanten) {
                    break;
                }
                e.score = yaku(e.shape_type, all, e.composition, tsumo, ba, ji, dora);
                yaku_result.push_back(e);
            }
            sort(yaku_result.begin(), yaku_result.end(), [](const data_t& a, const data_t& b) {
                return a.score.han > b.score.han;
            });
            int han = yaku_result[0].score.han;
            for (auto& e : yaku_result) {
                if (han > e.score.han) {
                    break;
                }
                for (auto& [k, v] : e.sute) {
                    if (v && !sute.count(k)) {
                        sute.emplace(k);
                    }
                }
            }
            std::ostringstream oss;
            oss << han << ",";
            for (auto e : sute) {
                oss << encode(e);
            }
            res() = oss.str();
            return res;
        }
        catch (const InvalidFormatException& e) {
            saori::Response res = {200, "OK"};
            res() = "Error";
            res(0) = e.what();
            return res;
        }
    }
    saori::Response res = {204, "No Content"};
    return res;
}
