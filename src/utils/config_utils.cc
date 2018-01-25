#define BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE 

#include "config_utils.h"

#include <algorithm>
#include <boost/algorithm/string.hpp>

#include "common_utils.h"

using namespace std;
using json = nlohmann::json;

vector<DWORD> ConfigUtils::BuildOffsetsChain(nlohmann::json config, string sequence) {
    vector<string> splitted;
    boost::split(splitted, sequence, [](char symbol) {
        return symbol == '+';
    });
	vector<DWORD> transformed (splitted.size());
    std::transform(splitted.begin(), splitted.end(), transformed.begin(), [config, splitted](const string key) { 
        return CommonUtils::ConvertHexStrToNumber(config.at(key).get<string>());   
    });
    return transformed;
};

DWORD ConfigUtils::GetOffset(nlohmann::json config, std::string key) {
    return CommonUtils::ConvertHexStrToNumber(config.at(key).get<string>());
};