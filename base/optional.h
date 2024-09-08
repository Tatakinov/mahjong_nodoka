#ifndef SSTP_OPTIONAL_H_
#define SSTP_OPTIONAL_H_

#include <optional>
#include <string>

namespace base {

class optional : public std::optional<std::string> {
    public:
        template<typename T>
            optional& operator=(const T &rhs) {
                if constexpr (std::is_same_v<bool, T>) {
                    *this = rhs ? "true" : "false";
                }
                else if constexpr (std::is_fundamental<T>::value) {
                    *this = std::to_string(rhs);
                }
                else {
                    std::optional<std::string>::operator=(static_cast<std::string>(rhs));
                }
                return *this;
            }
        template<typename T>
            optional& operator=(const T &&rhs) {
                if constexpr (std::is_same_v<bool, T>) {
                    *this = rhs ? "true" : "false";
                }
                else if constexpr (std::is_fundamental<T>::value) {
                    *this = std::to_string(rhs);
                }
                else {
                    std::optional<std::string>::operator=(static_cast<std::string>(rhs));
                }
                return *this;
            }
};

}

#endif // SSTP_OPTIONAL_H_
