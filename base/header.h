#ifndef SSTP_HEADER_H_
#define SSTP_HEADER_H_

#include <algorithm>
#include <sstream>
#include <string>
#include <unordered_map>

#include "base/optional.h"

namespace base {

class Header {
    public:
        Header();
        ~Header();
        static Header parse(std::string str);
        static Header parse(std::istringstream& iss);
        void remove(std::string key);
        inline optional& operator[](std::string key) { return map_[key]; }
        operator std::string() const {
            std::ostringstream oss;
            std::for_each(map_.begin(), map_.end(), [&oss](decltype(map_)::value_type pair) {
                    if (pair.second) {
                        oss << pair.first << ": " << pair.second.value() << "\x0d\x0a";
                    }
            });
            return oss.str();
        }
    private:
        std::unordered_map<std::string, optional> map_;
};

}

#endif // SSTP_HEADER_H_
