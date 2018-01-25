#include <windows.h>
#include <tlhelp32.h>
#include <limits>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include "json.hpp"

#include "game_api.h"
#include "utils/process_manager.h"
#include "utils/common_utils.h"

using namespace std;
using json = nlohmann::json;

int main(int argc, char** argv) {

  ifstream offset_config("address.json");
  ifstream game_functions_config("functions.json");
  ifstream packets_config("packets.json");
  
  json offsets;
  json game_functions;
  json packets;

  offset_config >> offsets;
  game_functions_config >> game_functions;
  packets_config >> packets;

  DWORD process_id = ProcessManager::GetPIDByName("elementclient.exe");
  HANDLE process_handler = ProcessManager::OpenProcessHandler(process_id);

  DWORD hp = GameAPI::GetHeroHP(process_handler, offsets);
  DWORD mp = GameAPI::GetHeroMP(process_handler, offsets);
  DWORD mobs_count = GameAPI::GetMobsCount(process_handler, offsets);
  cout << "hp: " << hp << endl << "mp: " << mp << endl << "mobs count: " << mobs_count << endl;

  DWORD closest_mob_wid = GameAPI::GetClosestMobWId(process_handler, offsets);
  cout << "mob wid: " << closest_mob_wid << endl;
  if(closest_mob_wid != NULL) {
    GameAPI::TargetMob(process_handler, game_functions, offsets, packets, closest_mob_wid);
    GameAPI::SimpleTargetAttack(process_handler, game_functions, offsets, packets);
  }
  system("pause");
  return 0;
};
