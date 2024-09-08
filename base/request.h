#ifndef SSTP_REQUEST_H_
#define SSTP_REQUEST_H_

#include <regex>
#include <sstream>
#include <string>

#include "base/header.h"

namespace base {

    template<const char *protocol_name, const char *protocol_version, const char *value, const char *arg>
        class Request {
            public:
                Request(const char *command) : command_(command), protocol_(std::string(protocol_name) + "/" + protocol_version), header_() {}
                ~Request() {}
                static Request parse(std::string str) {
                    auto result = std::remove_if(str.begin(), str.end(),
                            [](int ch) {return ch == '\x0d';}
                    );
                    str.erase(result, str.end());
                    std::istringstream iss(str);
                    return parse(iss);
                }
                static Request parse(std::istringstream& iss) {
                    Request ret;
                    std::string line;
                    getline(iss, line, '\x0a');
                    decltype(line)::size_type pos    = line.rfind(' ');
                    if (pos == decltype(line)::npos) {
                        return ret;
                    }
                    // assert protocol name
                    // line.substr(pos + 1)
                    std::string protocol = line.substr(pos + 1);
                    std::ostringstream pattern;
                    pattern << protocol_name << R"(/\d+\.\d+)";
                    if (!regex_match(protocol, std::regex(pattern.str()))) {
                        return ret;
                    }
                    ret.command_    = line.substr(0, pos);
                    ret.protocol_   = protocol;
                    ret.header_     = Header::parse(iss);
                    return ret;
                }
                std::string getCommand() { return command_; }
                std::string getProtocol() { return protocol_; }
                optional& operator[](std::string key) {
                    return header_[key];
                }
                optional& operator()() {
                    return header_[value];
                }
                optional& operator()(size_t index) {
                    std::ostringstream oss;
                    oss << arg << index;
                    return header_[oss.str()];
                }
                operator std::string() const {
                    std::ostringstream oss;
                    oss << command_ << " " << protocol_ << "\x0d\x0a";
                    oss << static_cast<std::string>(header_);
                    oss << "\x0d\x0a";
                    return oss.str();
                }

            private:
                std::string command_;
                std::string protocol_;
                Header header_;

                Request() : command_(), protocol_(), header_() {}
        };

}

#endif // SSTP_REQUEST_H_
