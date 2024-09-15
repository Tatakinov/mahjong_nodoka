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
            std::unordered_map<int, int> hand = decode(req(1).value());
            std::unordered_map<int, int> visible = decode(req(2).value());
            std::vector<data_t> result;
            analyze(result, hand, visible, mentsu);
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
                for (auto& [k, _] : e.valid) {
                    if (!valid.count(k)) {
                        valid.emplace(k);
                    }
                }
                for (auto& [k, _] : e.sute) {
                    if (!sute.count(k)) {
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
    saori::Response res = {204, "No Content"};
    return res;
}
