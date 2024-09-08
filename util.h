#ifndef UTIL_H_
#define UTIL_H_

#if defined(_WIN32)


#define WIN32_LEAN_AND_MEAN
#define STRICT

#include <windows.h>

enum Charset {
    Utf8 = CP_UTF8,
    Ansi = CP_ACP,
};

#else

#error "unsupported"

#endif // OS

#include <string>

std::string conv(std::string input, int to, int from);

#endif // UTIL_H_
