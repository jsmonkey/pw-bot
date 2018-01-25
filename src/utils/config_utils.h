#ifndef SRC_CONFIG_UTILS_H_
#define SRC_CONFIG_UTILS_H_

#include <vector>
#include <windows.h>
#include "json.hpp"

class ConfigUtils {
    public:
        static std::vector<DWORD> BuildOffsetsChain(nlohmann::json config, std::string sequence);
        static DWORD GetOffset(nlohmann::json config, std::string key);
};

#endif