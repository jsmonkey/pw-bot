#include "common_utils.h"

using namespace std;

unsigned long CommonUtils::ConvertHexStrToNumber(string hex_string) {
  return strtoul(hex_string.c_str(), NULL, 16);
};