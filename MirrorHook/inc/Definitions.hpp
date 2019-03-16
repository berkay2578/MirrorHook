#pragma once
#ifndef __MIRRORHOOK_INCLUDED__
#define __MIRRORHOOK_INCLUDED__
#include <WinDef.h>

namespace MirrorHook {
   enum class Game {
      MostWanted = 0,
      Carbon
   };

   typedef bool(WINAPI* fn_PrepareFor)(Game gameType);
   typedef bool(WINAPI* fn_NoParam_ReturnsBool)();

   inline bool WINAPI PrepareFor(Game gameType) {
      return reinterpret_cast<fn_PrepareFor>(GetProcAddress(GetModuleHandle(TEXT("MirrorHook.asi")), "MirrorHookInternals::PrepareFor"))(gameType);
   }
   inline bool WINAPI PrepareForForce(const DWORD& dinput8Address, const DWORD& d3dDeviceAddress) {
      return reinterpret_cast<fn_PrepareForForce>(GetProcAddress(GetModuleHandle(TEXT("MirrorHook.asi")), "MirrorHookInternals::PrepareForForce"))(dinput8Address, d3dDeviceAddress);
   }
   inline bool WINAPI IsReady() {
      return reinterpret_cast<fn_NoParam_ReturnsBool>(GetProcAddress(GetModuleHandle(TEXT("MirrorHook.asi")), "MirrorHookInternals::IsReady"))();
   }
   inline bool WINAPI IsShowingInfoOverlay() {
      return reinterpret_cast<fn_NoParam_ReturnsBool>(GetProcAddress(GetModuleHandle(TEXT("MirrorHook.asi")), "MirrorHookInternals::IsShowingInfoOverlay"))();
   }

   namespace D3D9 {
      enum class D3D9Extension {
         BeginScene,
         EndScene,
         BeforeReset,
         AfterReset
      };

   #ifndef _D3D9_H_
      typedef LPVOID LPDIRECT3DDEVICE9;
   #endif
      typedef HRESULT(WINAPI* fnAddExtension)(D3D9Extension extenderType, LPVOID extensionAddress);
      typedef HRESULT(WINAPI* fnSetTestCooperativeLevelExtension)(LPVOID extensionAddress);
      typedef HWND(WINAPI* fnGetWindowHandle)();
      typedef LPDIRECT3DDEVICE9(WINAPI* fnGetD3D9Device)();

      inline HRESULT WINAPI AddExtension(D3D9Extension extensionType, LPVOID extensionAddress) {
         return reinterpret_cast<fnAddExtension>(GetProcAddress(GetModuleHandle(TEXT("MirrorHook.asi")), "MirrorHookInternals::D3D9Extender::AddExtension"))
            (extensionType, extensionAddress);
      }
      inline HRESULT WINAPI SetTestCooperativeLevelExtension(LPVOID extensionAddress) {
         return reinterpret_cast<fnSetTestCooperativeLevelExtension>(GetProcAddress(GetModuleHandle(TEXT("MirrorHook.asi")), "MirrorHookInternals::D3D9Extender::SetTestCooperativeLevelExtension"))
            (extensionAddress);
      }
      inline HWND WINAPI GetWindowHandle() {
         return reinterpret_cast<fnGetWindowHandle>(GetProcAddress(GetModuleHandle(TEXT("MirrorHook.asi")), "MirrorHookInternals::D3D9Extender::GetWindowHandle"))();
      }
      inline LPDIRECT3DDEVICE9 WINAPI GetD3D9Device() {
         return reinterpret_cast<fnGetD3D9Device>(GetProcAddress(GetModuleHandle(TEXT("MirrorHook.asi")), "MirrorHookInternals::D3D9Extender::GetD3D9Device"))();
      }
      inline bool WINAPI IsReady() {
         return reinterpret_cast<fn_NoParam_ReturnsBool>(GetProcAddress(GetModuleHandle(TEXT("MirrorHook.asi")), "MirrorHookInternals::D3D9Extender::IsReady"))();
      }
   }
   namespace DI8 {
      enum class DI8Device {
         Keyboard,
         Mouse
      };
      enum class DI8Extension {
         GetDeviceState
      };

   #ifndef __DINPUT_INCLUDED__
      typedef LPVOID LPDIRECTINPUT8A;
      typedef LPVOID LPDIRECTINPUTDEVICE8A;
   #endif
      typedef HRESULT(WINAPI* fnAddExtension)(DI8Device deviceType, DI8Extension extensionType, LPVOID extensionAddress);
      typedef LPDIRECTINPUT8A(WINAPI* fnGetDirectInput8A)();
      typedef LPDIRECTINPUTDEVICE8A(WINAPI* fnGetDirectInputDevice8A)(DI8Device deviceType);

      inline HRESULT WINAPI AddExtension(DI8Device deviceType, DI8Extension extensionType, LPVOID extensionAddress) {
         return reinterpret_cast<fnAddExtension>(GetProcAddress(GetModuleHandle(TEXT("MirrorHook.asi")), "MirrorHookInternals::DI8Extender::AddExtension"))
            (deviceType, extensionType, extensionAddress);
      }
      inline LPDIRECTINPUT8A WINAPI GetDirectInput8A() {
         return reinterpret_cast<fnGetDirectInput8A>(GetProcAddress(GetModuleHandle(TEXT("MirrorHook.asi")), "MirrorHookInternals::DI8Extender::GetDirectInput8A"))();
      }
      inline LPDIRECTINPUTDEVICE8A WINAPI GetDirectInputDevice8A(DI8Device deviceType) {
         return reinterpret_cast<fnGetDirectInputDevice8A>(GetProcAddress(GetModuleHandle(TEXT("MirrorHook.asi")), "MirrorHookInternals::DI8Extender::GetDirectInputDevice8A"))
            (deviceType);
      }
      inline bool WINAPI IsReady() {
         return reinterpret_cast<fn_NoParam_ReturnsBool>(GetProcAddress(GetModuleHandle(TEXT("MirrorHook.asi")), "MirrorHookInternals::DI8Extender::IsReady"))();
      }
   }
}
#endif