#include "header.h"

namespace base {

Header::Header() {}

Header::~Header() {}

Header Header::parse(std::string str) {
    auto result = std::remove_if(str.begin(), str.end(),
            [](int ch) {return ch == '\x0d';}
    );
    str.erase(result, str.end());
    std::istringstream iss(str);
    return parse(iss);
}

Header Header::parse(std::istringstream& iss) {
    Header tmp;
    std::string header;
    while (std::getline(iss, header, '\x0a')) {
        decltype(header)::size_type pos    = header.find(':');
        if (pos == decltype(header)::npos) {
            continue;
        }
        std::string key = header.substr(0, pos);
        std::string value = header.substr(pos + 2);
        tmp[key] = value;
    }
    return tmp;
}

void Header::remove(std::string key) {
    map_.erase(key);
}

}
