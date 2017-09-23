#pragma once
#include "stdafx.h"
#include "Memory.h"
#include <map>

class VTableHook {
   VTableHook(const VTableHook&) = delete;

private:
   DWORD* pOrigVTable;
   std::map<UINT, DWORD> hookedIndexes;

public:
   VTableHook(PDWORD* ppClass) {
      pOrigVTable = *ppClass;
   }

   template<class Type>
   Type Hook(UINT index, Type fnNew) {
      DWORD fnOrig = pOrigVTable[index];
      if (!hookedIndexes.count(index)) {
         Memory::openMemoryAccess(fnOrig, 4);
         pOrigVTable[index] = (DWORD)fnNew;
         Memory::restoreMemoryAccess();

         hookedIndexes.insert(std::make_pair(index, fnOrig));
      }
      return (Type)fnOrig;
   }

   void Unhook(UINT index) {
      if (hookedIndexes.count(index)) {
         Memory::openMemoryAccess(pOrigVTable[index], 4);
         DWORD fnOrig = hookedIndexes.at(index);
         pOrigVTable[index] = fnOrig;
         Memory::restoreMemoryAccess();
         hookedIndexes.erase(index);
      }
   }
   void UnhookAll() {
      for (auto const& hook : hookedIndexes) {
         UINT index = hook.first;
         Memory::openMemoryAccess(pOrigVTable[index], 4);
         pOrigVTable[index] = hook.second;
         Memory::restoreMemoryAccess();
      }
      hookedIndexes.clear();
   }
};