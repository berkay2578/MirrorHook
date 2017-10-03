#pragma once
#ifndef MIRRORHOOK_INCLUDED
#define MIRRORHOOK_INCLUDED
#include <WinDef.h>

namespace MirrorHook {
   typedef bool(WINAPI* fnIsReady)();
   inline bool WINAPI IsReady() {
      return reinterpret_cast<fnIsReady>(GetProcAddress(GetModuleHandle(TEXT("MirrorHook.asi")), "IsReady"))();
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

      inline HRESULT WINAPI AddD3D9Extender(D3D9Extender extenderType, LPVOID extenderAddress) {
         return reinterpret_cast<fnAddD3D9Extender>(GetProcAddress(GetModuleHandle(TEXT("MirrorHook.asi")), "D3D9Extenders::AddD3D9Extender"))
            (extenderType, extenderAddress);
      }
      inline HWND WINAPI GetWindowHandle() {
         return reinterpret_cast<fnGetWindowHandle>(GetProcAddress(GetModuleHandle(TEXT("MirrorHook.asi")), "D3D9Extenders::GetWindowHandle"))();
      }
      inline LPDIRECT3DDEVICE9 WINAPI GetD3D9Device() {
         return reinterpret_cast<fnGetD3D9Device>(GetProcAddress(GetModuleHandle(TEXT("MirrorHook.asi")), "D3D9Extenders::GetD3D9Device"))();
      }
      inline bool WINAPI IsReady() {
         return reinterpret_cast<fnIsReady>(GetProcAddress(GetModuleHandle(TEXT("MirrorHook.asi")), "D3D9Extenders::IsReady"))();
      }
   }
   namespace DirectInput8 {
      enum class DirectInput8Device {
         Keyboard,
         Mouse
      };
      enum class DirectInput8Extender {
         GetDeviceState
      };

   #ifndef __DINPUT_INCLUDED__
      typedef LPVOID LPDIRECTINPUT8A;
      typedef LPVOID LPDIRECTINPUTDEVICE8A;
   #endif
      typedef HRESULT(WINAPI* fnAddDirectInput8Extender)(DirectInput8Device deviceType, DirectInput8Extender extenderType, LPVOID extenderAddress);
      typedef LPDIRECTINPUT8A(WINAPI* fnGetDirectInput8AInstance)();
      typedef LPDIRECTINPUTDEVICE8A(WINAPI* fnGetDirectInputDevice8A)(DirectInput8Device deviceType);

      inline HRESULT WINAPI AddDirectInput8Extender(DirectInput8Device deviceType, DirectInput8Extender extenderType, LPVOID extenderAddress) {
         return reinterpret_cast<fnAddDirectInput8Extender>(GetProcAddress(GetModuleHandle(TEXT("MirrorHook.asi")), "DirectInput8Extenders::AddDirectInput8Extender"))
            (deviceType, extenderType, extenderAddress);
      }
      inline LPDIRECTINPUT8A WINAPI GetDirectInput8AInstance() {
         return reinterpret_cast<fnGetDirectInput8AInstance>(GetProcAddress(GetModuleHandle(TEXT("MirrorHook.asi")), "DirectInput8Extenders::GetDirectInput8AInstance"))();
      }
      inline LPDIRECTINPUTDEVICE8A WINAPI GetDirectInputDevice8A(DirectInput8Device deviceType) {
         return reinterpret_cast<fnGetDirectInputDevice8A>(GetProcAddress(GetModuleHandle(TEXT("MirrorHook.asi")), "DirectInput8Extenders::GetDirectInputDevice8A"))
            (deviceType);
      }
      inline bool WINAPI IsReady() {
         return reinterpret_cast<fnIsReady>(GetProcAddress(GetModuleHandle(TEXT("MirrorHook.asi")), "DirectInput8Extenders::IsReady"))();
      }
   }
}
#endif