#include "stdafx.h"
#include "Memory.h"

namespace Memory {
   DWORD oldMemoryAccess;
   DWORD memoryAccessAddress;
   int memoryAccessSize;
   DWORD baseAddress = 0;

   void openMemoryAccess(const DWORD& address, const int& size) {
      memoryAccessAddress = address;
      memoryAccessSize = size;
      VirtualProtect((LPVOID)address, size, PAGE_EXECUTE_READWRITE, &oldMemoryAccess);
   }
   void restoreMemoryAccess() {
      VirtualProtect((LPVOID)memoryAccessAddress, memoryAccessSize, oldMemoryAccess, &oldMemoryAccess);
      memoryAccessAddress = 0;
      memoryAccessSize = 0;
   }

   DWORD makeAbsolute(const DWORD& relativeAddress) {
      return baseAddress + relativeAddress;
   }

   void Init() {
      baseAddress = (DWORD)GetModuleHandle(0);
   }
}