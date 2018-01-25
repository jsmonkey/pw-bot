#ifndef SRC_COMMON_UTILS_H_
#define SRC_COMMON_UTILS_H_

#include <string>

class CommonUtils {
    public: 
        static unsigned long ConvertHexStrToNumber(std::string hex_string);
};

#endif