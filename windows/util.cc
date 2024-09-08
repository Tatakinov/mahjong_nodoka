#include "util.h"

#define WIN32_LEAN_AND_MEAN
#define STRICT

#include <windows.h>

std::string conv(std::string input, int to, int from) {
  int w_len = 0, mb_len = 0;
  wchar_t *w_str;
  char  *mb_str;
  std::string str;
  w_len = MultiByteToWideChar(from, MB_ERR_INVALID_CHARS, input.c_str(), input.length(), NULL, 0);
  if ( ! w_len) {
    goto error;
  }
  w_str = new wchar_t[w_len];
  if ( ! w_str) {
    goto error;
  }
  MultiByteToWideChar(from, 0, input.c_str(), input.length(), w_str, w_len);
  mb_len = WideCharToMultiByte(to, 0, w_str, w_len, NULL, 0, NULL, NULL);
  if ( ! mb_len) {
    goto error_w;
  }
  mb_str  = new char[mb_len];
  if ( ! mb_str) {
    goto error_w;
  }
  WideCharToMultiByte(to, 0, w_str, w_len, mb_str, mb_len, NULL, NULL);
  str = std::string(mb_str, mb_len);
  delete mb_str;
error_w:
  delete w_str;
error:
  return str;
}

