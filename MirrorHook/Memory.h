#pragma once
#include <cstdarg>

namespace Memory {
   extern DWORD baseAddress;

   void openMemoryAccess(const DWORD& address, const int& size);
   void restoreMemoryAccess();
   DWORD makeAbsolute(const DWORD& relativeAddress);
   void Init();
}