#pragma once
#include <WinDef.h>

namespace MirrorHook {
   typedef bool(WINAPI* fnIsReady)();
   bool WINAPI IsReady() {
      return reinterpret_cast<fnIsReady>(GetProcAddress(GetModuleHandle(L"MirrorHook.asi"), "IsReady"))();
   }

   namespace D3D9 {
      enum class D3D9Extender {
         BeforeReset,
         AfterReset,
         BeginScene,
         EndScene
      };

   #ifndef _D3D9_H_
      typedef LPVOID LPDIRECT3DDEVICE9;
   #endif
      typedef HRESULT(WINAPI* fnAddD3D9Extender)(D3D9Extender extenderType, LPVOID extenderAddress);
      typedef HWND(WINAPI* fnGetWindowHandle)();
      typedef LPDIRECT3DDEVICE9(WINAPI* fnGetD3D9Device)();
      typedef bool(WINAPI* fnIsReady)();

      HRESULT WINAPI AddD3D9Extender(D3D9Extender extenderType, LPVOID extenderAddress) {
         return reinterpret_cast<fnAddD3D9Extender>(GetProcAddress(GetModuleHandle(L"MirrorHook.asi"), "D3D9Extenders::AddD3D9Extender"))
            (extenderType, extenderAddress);
      }
      HWND WINAPI GetWindowHandle() {
         return reinterpret_cast<fnGetWindowHandle>(GetProcAddress(GetModuleHandle(L"MirrorHook.asi"), "D3D9Extenders::GetWindowHandle"))();
      }
      LPDIRECT3DDEVICE9 WINAPI GetD3D9Device() {
         return reinterpret_cast<fnGetD3D9Device>(GetProcAddress(GetModuleHandle(L"MirrorHook.asi"), "D3D9Extenders::GetD3D9Device"))();
      }
      bool WINAPI IsReady() {
         return reinterpret_cast<fnIsReady>(GetProcAddress(GetModuleHandle(L"MirrorHook.asi"), "D3D9Extenders::IsReady"))();
      }
   }

#ifdef __DINPUT_INCLUDED__

#include <dinput.h>
#endif
}