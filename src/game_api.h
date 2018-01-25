#ifndef SRC_GAME_API_H_
#define SRC_GAME_API_H_

#include <windows.h>
#include <numeric>
#include <vector>
#include "json.hpp"

class GameAPI {
    public:
        static DWORD GetHeroHP(HANDLE process_handler, nlohmann::json offsets_config);
        static DWORD GetHeroMP(HANDLE process_handler, nlohmann::json offsets_config);
        static DWORD GetMobsCount(HANDLE process_handler, nlohmann::json offsets_config);
        static DWORD GetMobStruct(HANDLE process_handler, nlohmann::json offsets_config, unsigned int index);
        static DWORD GetMobWId(HANDLE process_handler, nlohmann::json offsets_config, DWORD mob_offset);
        static DWORD GetMobType(HANDLE process_handler, nlohmann::json offsets_config, DWORD mob_offset);
        static DWORD GetMobDistance(HANDLE process_handler, nlohmann::json offsets_config, DWORD mob_offset);
        static DWORD GetClosestMobWId(HANDLE process_handler, nlohmann::json offsets_config);
        static BOOL SendPacket(HANDLE process_handler, nlohmann::json game_functions_config, nlohmann::json offsets_config, BYTE* params, DWORD params_length);
        static BOOL TargetMob(HANDLE process_handler, nlohmann::json game_functions_config, nlohmann::json offsets_config, nlohmann::json packets_config, DWORD mob_wid);
        static BOOL SimpleTargetAttack(HANDLE process_handler, nlohmann::json game_functions_config, nlohmann::json offsets_config, nlohmann::json packets_config);
        template <typename T>
        static T GetInfo(HANDLE process_handler, std::vector<DWORD> offsets_chain) {
           return (T)std::accumulate(offsets_chain.begin(), offsets_chain.end(), 0, [&process_handler](DWORD prev, DWORD curr) {
               return ProcessManager::ReadFromProcessMemory<DWORD>(process_handler, prev + curr);
           });
        };
};

#endif