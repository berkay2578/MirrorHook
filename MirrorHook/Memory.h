#pragma once
#include "stdafx.h"
#include <cstdarg>

namespace Memory {
   extern DWORD baseAddress;

   void openMemoryAccess(const DWORD& address, const int& size);
   void restoreMemoryAccess();
   DWORD calculateRelativeAddress(const DWORD& from, const DWORD& to, const bool& isFromAbsolute = true);
   DWORD makeAbsolute(const DWORD& relativeAddress);
   DWORD* readPointer(const DWORD& baseOffset, const int offsetCount = 0, ...);
   void writeCall(const DWORD& from, const DWORD& to, const bool& isFromAbsolute = true);
   void writeJMP(const DWORD& from, const DWORD& to, const bool& isFromAbsolute = true);
   void writeInterrupt3(const DWORD& to, const int& amount, const bool& isFromAbsolute = true);
   void writeRet(const DWORD& to, const bool& isToAbsolute = true);
   void writeNop(const DWORD& to, const int& amount, const bool& isToAbsolute = true);
   void writeRaw(const DWORD& to, const bool& isToAbsolute, const int byteCount, ...);
   void Init();
}