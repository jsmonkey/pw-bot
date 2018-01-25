#include "process_manager.h"

using namespace std;

DWORD ProcessManager::GetPIDByName(string process_name) {
  PROCESSENTRY32 processEntry;
  HANDLE pHandle;
  DWORD pid = NULL;
  pHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  processEntry.dwSize = sizeof(PROCESSENTRY32);
  if (!Process32First(pHandle, &processEntry)) return pid;
  do {
    if (process_name != processEntry.szExeFile) continue;
    pid = processEntry.th32ProcessID;
    CloseHandle(pHandle);
    break;
  } while (Process32Next(pHandle, &processEntry));
  return pid;
};

HANDLE ProcessManager::OpenProcessHandler(DWORD pid) {
  HANDLE process_handler = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
  return process_handler ? process_handler : NULL;
};

void ProcessManager::CloseProcessHandler(HANDLE process_handler) {
  CloseHandle(process_handler);
};

void* ProcessManager::WriteToProcessMemory(HANDLE process_handler, void* data, unsigned int data_length) {
  void* allocated = VirtualAllocEx(process_handler, NULL, data_length, MEM_COMMIT, PAGE_READWRITE);
  if(allocated == NULL) {
    return NULL;
  } 

  BOOL is_written = WriteProcessMemory(process_handler, allocated, data, data_length, NULL);
  if(!is_written) {
    VirtualFreeEx(process_handler, allocated, 0, MEM_RELEASE);
    return NULL;
  }

  return allocated;
};

BOOL ProcessManager::Inject(HANDLE process_handler, void* func, unsigned int func_length, void* params, unsigned int params_length) {
  void* allocated_func = ProcessManager::WriteToProcessMemory(process_handler, func, func_length);
  if(allocated_func == NULL) {
    return false;
  }
  
  void* allocated_params = NULL;
  if(params) {
    allocated_params = ProcessManager::WriteToProcessMemory(process_handler, params, params_length);
    if(allocated_params == NULL) {
      return false;
    }
  }

  HANDLE hProcThread = CreateRemoteThread(process_handler, NULL, 0, (LPTHREAD_START_ROUTINE)allocated_func, allocated_params, 0, NULL);
  if (hProcThread == INVALID_HANDLE_VALUE) {
    VirtualFreeEx(process_handler, allocated_func, 0, MEM_RELEASE);
    if(params) {
      VirtualFreeEx(process_handler, allocated_params, 0, MEM_RELEASE);
    }
    return false;
  } 

  WaitForSingleObject(hProcThread, INFINITE);

  CloseHandle(hProcThread);
  VirtualFreeEx(process_handler, allocated_func, 0, MEM_RELEASE);
  if(params) {
    VirtualFreeEx(process_handler, allocated_params, 0, MEM_RELEASE);
  }
  return true;
};
