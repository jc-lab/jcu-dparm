/**
 * @file	intl_utils.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/08/14
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include "intl_utils.h"

namespace jcu {
namespace dparm {
namespace intl {

std::string readString(const unsigned char *buffer, int length, bool trim_right) {
  std::string out;
  int out_len = 0;
  out.resize(length);

  for (int i = 0; i < length; i++) {
    out[i] = buffer[i];
    if (out[i])
      out_len++;
    else
      break;
  }

  if (trim_right) {
    while ((out_len > 0) && (out[out_len - 1] == 0x20)) {
      out_len--;
    }
  }

  out.resize(out_len);
  return out;
}

std::string readStringRange(const unsigned char *buffer, int begin, int end, bool trim_right) {
  int length = end - begin;
  return readString(buffer + begin, length, trim_right);
}

std::string fixAtaStringOrder(const unsigned char *buffer, int length, bool trim_right) {
  std::string out;
  int out_len = 0;
  out.resize(length);

  for (int i = 0; i < length; i += 2) {
    out[i] = buffer[i + 1];
    out[i + 1] = buffer[i];
    if (out[i])
      out_len++;
    else
      break;
    if (out[i + 1])
      out_len++;
    else
      break;
  }

  if (trim_right) {
    while ((out_len > 0) && (out[out_len - 1] == 0x20)) {
      out_len--;
    }
  }

  out.resize(out_len);
  return out;
}

std::string trimString(const std::string &input) {
  const char *begin = input.c_str();
  int length = input.length();

  while ((*begin == ' ') || (*begin == '\r') || (*begin == '\n') || (*begin == '\t')) {
    begin++;
    length--;
  }

  while ((length > 0) && ((begin[length - 1] == ' ') || (begin[length - 1] == '\r') || (begin[length - 1] == '\n')
      || (begin[length - 1] == '\t'))) {
    length--;
  }

  if (length > 0) {
    return std::string(begin, length);
  }
  return std::string();
}

} // namespace intl
} // namespace dparm
} // namespace jcu

