#ifndef SRC_PROCESS_MANAGER_H_
#define SRC_PROCESS_MANAGER_H_

#include <string>
#include <windows.h>
#include <tlhelp32.h>

class ProcessManager {
 public:
  static DWORD GetPIDByName(std::string process_name);
  static HANDLE OpenProcessHandler(DWORD pid);
  static void CloseProcessHandler(HANDLE process_handler);
  static void* WriteToProcessMemory(HANDLE process_handler, void* data, unsigned int data_length);
  static BOOL Inject(HANDLE process_handler, void* func, unsigned int func_length, void* params, unsigned int params_length);
  template <typename T>
  static T ReadFromProcessMemory(HANDLE process_handler, DWORD addr) {
    T value;
    ReadProcessMemory(process_handler, (void*)addr, &value, sizeof(T), NULL);
    return value;
  };
};

#endif