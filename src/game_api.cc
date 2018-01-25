#include "game_api.h"
#include <iostream>

#include "utils/process_manager.h"
#include "utils/common_utils.h"
#include "utils/config_utils.h"

using namespace std;
using json = nlohmann::json;

DWORD GameAPI::GetHeroHP(HANDLE process_handler, json offsets_config) {
    vector<DWORD> offsets_chain = ConfigUtils::BuildOffsetsChain(offsets_config, "base_adr+game_ofs+player_structure_ofs+player_hp_ofs");
    return GameAPI::GetInfo<DWORD>(process_handler, offsets_chain);
};

DWORD GameAPI::GetHeroMP(HANDLE process_handler, json offsets_config) {
    vector<DWORD> offsets_chain = ConfigUtils::BuildOffsetsChain(offsets_config, "base_adr+game_ofs+player_structure_ofs+player_mp_ofs");
    return GameAPI::GetInfo<DWORD>(process_handler, offsets_chain);
};

DWORD GameAPI::GetMobsCount(HANDLE process_handler, json offsets_config) {
    vector<DWORD> offsets_chain = ConfigUtils::BuildOffsetsChain(offsets_config, "base_adr+game_ofs+mobs_structure_ofs+mob_list_ofs+mob_list_count_ofs");
    return GameAPI::GetInfo<DWORD>(process_handler, offsets_chain);
};

DWORD GameAPI::GetMobStruct(HANDLE process_handler, json offsets_config, unsigned int index) {
    DWORD mob_offset_in_array = ConfigUtils::GetOffset(offsets_config, "mob_structure_size") * index;
    vector<DWORD> offsets_chain = ConfigUtils::BuildOffsetsChain(offsets_config, "base_adr+game_ofs+mobs_structure_ofs+mob_list_ofs+mob_list_array_ofs");
    offsets_chain.push_back(mob_offset_in_array);
    return GameAPI::GetInfo<DWORD>(process_handler, offsets_chain);
};

DWORD GameAPI::GetMobWId(HANDLE process_handler, json offsets_config, DWORD mob_offset) {
    vector<DWORD> offsets_chain = {
        mob_offset + ConfigUtils::GetOffset(offsets_config, "mob_wid_ofs")
    };
    return GameAPI::GetInfo<DWORD>(process_handler, offsets_chain);
};

DWORD GameAPI::GetMobType(HANDLE process_handler, json offsets_config, DWORD mob_offset) {
    vector<DWORD> offsets_chain = {
        mob_offset + ConfigUtils::GetOffset(offsets_config, "mob_type_ofs")
    };
    return GameAPI::GetInfo<DWORD>(process_handler, offsets_chain);
};

DWORD GameAPI::GetMobDistance(HANDLE process_handler, json offsets_config, DWORD mob_offset) {
    vector<DWORD> offsets_chain = {
        mob_offset + ConfigUtils::GetOffset(offsets_config, "mob_distance_ofs")
    };
    return GameAPI::GetInfo<DWORD>(process_handler, offsets_chain);
};

DWORD GameAPI::GetClosestMobWId(HANDLE process_handler, json offsets_config) {
    float dist = INT_MAX;
    DWORD closest_mob_wid = NULL;
    DWORD mobs_count = GameAPI::GetMobsCount(process_handler, offsets_config);
    for(int i = 0; i < mobs_count; i++) {
        DWORD mob_struct_ofs = GameAPI::GetMobStruct(process_handler, offsets_config, i);
        DWORD mob_wid = GameAPI::GetMobWId(process_handler, offsets_config, mob_struct_ofs);
        if(mob_wid != 0) {
            DWORD mob_type = GameAPI::GetMobType(process_handler, offsets_config, mob_struct_ofs);
            if(mob_type == 6) {
                DWORD mob_distance = GameAPI::GetMobDistance(process_handler, offsets_config, mob_struct_ofs);
                if(mob_distance < dist) {
                    dist = mob_distance;
                    closest_mob_wid = mob_wid;
                }
            }
        } 
    }
    
    return closest_mob_wid;
};

BOOL GameAPI::SendPacket(HANDLE process_handler, json game_functions_config, json offsets_config, BYTE* params, DWORD params_length) {

    json send_packet_node = game_functions_config.at("send_packet");
    json base_address_node = send_packet_node.at("params").at("base_address");
    json send_packet_address_node = send_packet_node.at("params").at("send_packet_address");
    json packet_params_node = send_packet_node.at("params").at("packet_params");
    json packet_params_length_node = send_packet_node.at("params").at("packet_params_length");

    int base_address_offset = base_address_node.at("range").at("offset").get<int>();
    int base_address_size = base_address_node.at("range").at("size").get<int>();

    int send_packet_address_offset = send_packet_address_node.at("range").at("offset").get<int>();
    int send_packet_address_size = send_packet_address_node.at("range").at("size").get<int>(); 

    int packet_params_offset = packet_params_node.at("range").at("offset").get<int>();
    int packet_params_size = packet_params_node.at("range").at("size").get<int>(); 

    int packet_params_length_offset = packet_params_length_node.at("range").at("offset").get<int>();
    int packet_params_length_size = packet_params_length_node.at("range").at("size").get<int>(); 


    vector<string> opcodes = game_functions_config.at("send_packet").at("opcodes").get<vector<string>>();
    DWORD base_address = ConfigUtils::GetOffset(offsets_config, "base_adr");
    DWORD send_packet_address = ConfigUtils::GetOffset(offsets_config, "send_packet_adr");

    vector<BYTE> opcode_bytes (opcodes.size());

    transform(opcodes.begin(), opcodes.end(), opcode_bytes.begin(), [](string opcode) {
        return (BYTE)stoul(opcode, NULL, 16);
    });

    BYTE* allocated_params = (BYTE*)ProcessManager::WriteToProcessMemory(process_handler, params, params_length);

    BYTE* opcodes_seq = opcode_bytes.data();

    DWORD allocated_params_address = DWORD(allocated_params);
    memcpy(opcodes_seq + base_address_offset, &base_address, base_address_size);
    memcpy(opcodes_seq + send_packet_address_offset, &send_packet_address, send_packet_address_size);
    memcpy(opcodes_seq + packet_params_offset, &allocated_params_address, packet_params_size);
    memcpy(opcodes_seq + packet_params_length_offset, &params_length, packet_params_length_size);

    return ProcessManager::Inject(process_handler, opcodes_seq, opcode_bytes.size(), NULL, 0);
};

BOOL GameAPI::TargetMob(HANDLE process_handler, json game_functions_config, json offsets_config, json packets_config, DWORD mob_wid) {
    json mob_wid_range_node = packets_config.at("target").at("params").at("mob_wid").at("range");

    vector<string> bytes_representation_seq = packets_config.at("target").at("bytes").get<vector<string>>();
    
    int mob_wid_offset = mob_wid_range_node.at("offset").get<int>();
    int mob_wid_size = mob_wid_range_node.at("size").get<int>();

    vector<BYTE> bytes_seq (bytes_representation_seq.size());

    transform(bytes_representation_seq.begin(), bytes_representation_seq.end(), bytes_seq.begin(), [](string byte_representation) {
        return (BYTE)stoul(byte_representation, NULL, 16);
    });

    BYTE* bytes_seq_array = bytes_seq.data();

    memcpy(bytes_seq_array + mob_wid_offset, &mob_wid, mob_wid_size);

    return GameAPI::SendPacket(process_handler, game_functions_config, offsets_config, bytes_seq_array, bytes_seq.size());
}

BOOL GameAPI::SimpleTargetAttack(HANDLE process_handler, json game_functions_config, json offsets_config, json packets_config) {
    vector<string> bytes_representation_seq = packets_config.at("attack_target").at("bytes").get<vector<string>>();

    vector<BYTE> bytes_seq (bytes_representation_seq.size());

    transform(bytes_representation_seq.begin(), bytes_representation_seq.end(), bytes_seq.begin(), [](string byte_representation) {
        return (BYTE)stoul(byte_representation, NULL, 16);
    });

    BYTE* bytes_seq_array = bytes_seq.data();

    return GameAPI::SendPacket(process_handler, game_functions_config, offsets_config, bytes_seq_array, bytes_seq.size());
}