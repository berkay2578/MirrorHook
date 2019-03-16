#include "stdafx.h"
#include "Memory.h"

#include <d3d9.h>
#include "imgui.h"
#include "dx9\imgui_impl_dx9.h"
#include "D3D9Types.h"

#include <dinput.h>
#include "DI8Types.h"

#include "Definitions.hpp"
using MirrorHook::D3D9::D3D9Extension;
using MirrorHook::DI8::DI8Device;
using MirrorHook::DI8::DI8Extension;

#include "VTableHook.hpp"
#include <memory>
using std::unique_ptr;
using std::make_unique;
#include <map>
using std::map;
#include <vector>
using std::vector;

namespace MirrorHookInternals {
   namespace DI8Extender {
      DWORD                    dinput8Address                    = NULL;

      LPDIRECTINPUT8A          di8Instance                       = nullptr;
      LPDIRECTINPUTDEVICE8A    device_Keyboard                   = nullptr;
      LPDIRECTINPUTDEVICE8A    device_Mouse                      = nullptr;

      map<DI8Device, vector<GetDeviceState_t>> mGetDeviceStateExtensions;

      bool                     isExtenderReady                   = false;

   #pragma region function hooks
      GetDeviceState_t         origGetDeviceState_Keyboard       = nullptr;
      GetDeviceState_t         origGetDeviceState_Mouse          = nullptr;

      HRESULT WINAPI hkGetDeviceState_Keyboard(HINSTANCE hInstance, DWORD cbData, LPVOID lpvData) {
         HRESULT retOrigGetDeviceState = origGetDeviceState_Keyboard(hInstance, cbData, lpvData);

         if (!mGetDeviceStateExtensions[DI8Device::Keyboard].empty()) {
            for (GetDeviceState_t keyboardExtension : mGetDeviceStateExtensions[DI8Device::Keyboard]) {
               if (keyboardExtension)
                  keyboardExtension(hInstance, cbData, lpvData);
            }
         }
         return retOrigGetDeviceState;
      }
      HRESULT WINAPI hkGetDeviceState_Mouse(HINSTANCE hInstance, DWORD cbData, LPVOID lpvData) {
         HRESULT retOrigGetDeviceState = origGetDeviceState_Mouse(hInstance, cbData, lpvData);

         if (!mGetDeviceStateExtensions[DI8Device::Mouse].empty()) {
            for (GetDeviceState_t mouseExtension : mGetDeviceStateExtensions[DI8Device::Mouse]) {
               if (mouseExtension)
                  mouseExtension(hInstance, cbData, lpvData);
            }
         }
         return retOrigGetDeviceState;
      }
   #pragma endregion

   #pragma region exported helpers
      HRESULT WINAPI AddExtension(DI8Device deviceType, DI8Extension extensionType, LPVOID extensionAddress) {
      #pragma ExportedFunction
         if (!isExtenderReady)
            return FALSE;

         switch (extensionType) {
            case DI8Extension::GetDeviceState:
            {
               switch (deviceType) {
                  case DI8Device::Keyboard:
                  case DI8Device::Mouse:
                     mGetDeviceStateExtensions[deviceType].push_back(reinterpret_cast<GetDeviceState_t>(extensionAddress));
                     break;
                  default:
                     return FALSE;
               }
               break;
            }
            default:
               return FALSE;
         }
         return TRUE;
      }
      LPDIRECTINPUT8A WINAPI GetDirectInput8A() {
      #pragma ExportedFunction
         if (!isExtenderReady)
            return nullptr;

         return di8Instance;
      }
      LPDIRECTINPUTDEVICE8A WINAPI GetDirectInputDevice8A(DI8Device deviceType) {
      #pragma ExportedFunction
         if (!isExtenderReady)
            return nullptr;

         switch (deviceType) {
            case DI8Device::Keyboard:
               return device_Keyboard;
            case DI8Device::Mouse:
               return device_Mouse;
         }
         return nullptr;
      }
      bool WINAPI IsReady() {
      #pragma ExportedFunction
         return isExtenderReady;
      }
   #pragma endregion

      BOOL CALLBACK enumCallback(LPCDIDEVICEINSTANCEA lpddi, LPVOID) {
         BYTE deviceType = LOBYTE(lpddi->dwDevType);
         if (deviceType == DI8DEVTYPE_KEYBOARD || deviceType == DI8DEVTYPE_MOUSE) {
            DWORD* inputTable;

            if (deviceType == DI8DEVTYPE_KEYBOARD) {
               di8Instance->CreateDevice(lpddi->guidInstance, &device_Keyboard, NULL);
               inputTable = *(PDWORD*)device_Keyboard;
            } else {
               di8Instance->CreateDevice(lpddi->guidInstance, &device_Mouse, NULL);
               inputTable = *(PDWORD*)device_Mouse;
            }

            Memory::openMemoryAccess(inputTable[9], 4);

            if (deviceType == DI8DEVTYPE_KEYBOARD) {
               origGetDeviceState_Keyboard = (GetDeviceState_t)(DWORD)inputTable[9];
               inputTable[9]               = (DWORD)hkGetDeviceState_Keyboard;
            } else {
               origGetDeviceState_Mouse    = (GetDeviceState_t)(DWORD)inputTable[9];
               inputTable[9]               = (DWORD)hkGetDeviceState_Mouse;
            }

            Memory::restoreMemoryAccess();
         }

         if (device_Keyboard && device_Mouse) {
            isExtenderReady = true;
            return DIENUM_STOP;
         }
         return DIENUM_CONTINUE;
      }
      void Init() {
         mGetDeviceStateExtensions[DI8Device::Keyboard] = vector<GetDeviceState_t>();
         mGetDeviceStateExtensions[DI8Device::Mouse]    = vector<GetDeviceState_t>();

         DWORD pDI8 = NULL;
         while (!pDI8) {
            pDI8 = *(DWORD*)dinput8Address;
            Sleep(100);
         }
         di8Instance = (LPDIRECTINPUT8A)pDI8;
         di8Instance->EnumDevices(DI8DEVCLASS_ALL, &enumCallback, NULL, DIEDFL_ATTACHEDONLY);
      }
   }
   namespace D3D9Extender {
      DWORD                  d3dDeviceAddress      = NULL;

      LPDIRECT3DDEVICE9      d3dDevice             = nullptr;
      HWND                   d3dWindow             = nullptr;

      TestCooperativeLevel_t testCooperativeLevelExtension = nullptr;
      vector<BeginScene_t>   vBeginSceneExtensions         = vector<BeginScene_t>();
      vector<EndScene_t>     vEndSceneExtensions           = vector<EndScene_t>();
      vector<Reset_t>        vBeforeResetExtensions        = vector<Reset_t>();
      vector<Reset_t>        vAfterResetExtensions         = vector<Reset_t>();

      bool                   useImGui                  = true;
      bool                   isImGuiReady              = false;
      unsigned int           infoOverlayFrame          = 0;
      unsigned int           infoOverlayFrame_MaxFrame = 300;

      bool                   isExtenderReady = false;

   #pragma region function hooks
      unique_ptr<VTableHook> d3dDeviceHook            = nullptr;
      TestCooperativeLevel_t origTestCooperativeLevel = nullptr;
      BeginScene_t           origBeginScene           = nullptr;
      EndScene_t             origEndScene             = nullptr;
      Reset_t                origReset                = nullptr;
      BeginStateBlock_t      origBeginStateBlock      = nullptr;

      HRESULT WINAPI hkTestCooperativeLevel(LPDIRECT3DDEVICE9 pDevice) {
         HRESULT hr = origTestCooperativeLevel(pDevice);
         if (testCooperativeLevelExtension) {
            if (HRESULT exRet = testCooperativeLevelExtension(pDevice)) {
               return exRet;
            }
         }
         return hr;
      }
      HRESULT WINAPI hkBeginScene(LPDIRECT3DDEVICE9 pDevice) {
         if (pDevice->TestCooperativeLevel() == D3D_OK) {
            if (!vBeginSceneExtensions.empty()) {
               for (BeginScene_t beginSceneExtension : vBeginSceneExtensions) {
                  if (beginSceneExtension)
                     beginSceneExtension(pDevice);
               }
            }

            if (ImGui::IsKeyPressed(VK_F9, false)) {
               infoOverlayFrame_MaxFrame = -1;
               useImGui = !useImGui;
            }
            ImGui::GetIO().KeysDown[VK_F9] = GetKeyState(VK_F9) & 0x8000;

            if (!isImGuiReady) {
               ImGui_ImplDX9_Init(d3dWindow, d3dDevice);
               ImGuiIO& io = ImGui::GetIO();
               io.IniFilename = NULL;

               isImGuiReady = true;
            }
            ImGui_ImplDX9_NewFrame();
         }
         return origBeginScene(pDevice);
      }
      HRESULT WINAPI hkEndScene(LPDIRECT3DDEVICE9 pDevice) {
         if (pDevice->TestCooperativeLevel() == D3D_OK) {
            if (!vEndSceneExtensions.empty()) {
               for (EndScene_t endSceneExtension : vEndSceneExtensions) {
                  if (endSceneExtension)
                     endSceneExtension(pDevice);
               }
            }
            if (useImGui && isImGuiReady) {
               if (infoOverlayFrame_MaxFrame == -1
                  || infoOverlayFrame < infoOverlayFrame_MaxFrame) {
                  ImGui::SetNextWindowPos(ImVec2(10.0f, 40.0f), ImGuiCond_Once);
                  if (ImGui::Begin("##MirrorHook", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize)) {

                     ImGui::Text("MirrorHook v1.1");
                     ImGui::Text("https://github.com/berkay2578/MirrorHook");
                     ImGui::Text("by berkay(2578)");
                     ImGui::Separator();

                     ImGui::Text("D3D9 Extender         : %s", D3D9Extender::isExtenderReady ? "Ready" : "Not ready"); // redundant much?
                     ImGui::Text("DirectInput8 Extender : %s", DI8Extender::isExtenderReady ? "Ready" : "Not ready");
                     ImGui::Separator();

                     ImGui::Text("D3D9 Extender Information");
                     ImGui::Indent(5.0f);
                     {
                        ImGui::Text("BeginScene  | EndScene extensions   : %d | %d", vBeginSceneExtensions.size(), vEndSceneExtensions.size());
                        ImGui::Text("BeforeReset | AfterReset extensions : %d | %d", vBeforeResetExtensions.size(), vAfterResetExtensions.size());
                     }
                     ImGui::Unindent(5.0f);
                     ImGui::Separator();

                     ImGui::Text("DirectInput8 Extender Information");
                     ImGui::Indent(5.0f);
                     {
                        ImGui::Text("Keyboard | Mouse");
                        ImGui::Indent(2.5f);
                        {
                           ImGui::Text("GetDeviceState extensions : %d | %d",
                              DI8Extender::mGetDeviceStateExtensions[DI8Device::Keyboard].size(),
                              DI8Extender::mGetDeviceStateExtensions[DI8Device::Mouse].size());
                        }
                        ImGui::Unindent(2.5f);
                     }
                     ImGui::Unindent(5.0f);
                     ImGui::Separator();
                     ImGui::Text("Press F9 to toggle me.");

                     if (infoOverlayFrame_MaxFrame != -1) {
                        ImGui::Text("I will disappear in... %04u.", infoOverlayFrame_MaxFrame - infoOverlayFrame);

                        infoOverlayFrame++;
                        if (infoOverlayFrame >= infoOverlayFrame_MaxFrame) {
                           ImGui::End();
                           ImGui::Render();
                           useImGui = false;
                           return origEndScene(pDevice);
                        }
                     }
                  }
                  ImGui::End();
                  ImGui::Render();
               }
            }
         }
         return origEndScene(pDevice);
      }
      HRESULT WINAPI hkReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
         if (!vBeforeResetExtensions.empty()) {
            for (Reset_t beforeResetExtension : vBeforeResetExtensions) {
               if (beforeResetExtension)
                  beforeResetExtension(pDevice, pPresentationParameters);
            }
         }

         if (isImGuiReady)
            ImGui_ImplDX9_InvalidateDeviceObjects();

         auto retOrigReset = origReset(pDevice, pPresentationParameters);

         if (!vAfterResetExtensions.empty()) {
            for (Reset_t afterResetExtension : vAfterResetExtensions) {
               if (afterResetExtension)
                  afterResetExtension(pDevice, pPresentationParameters);
            }
         }

         if (isImGuiReady)
            ImGui_ImplDX9_CreateDeviceObjects();

         return retOrigReset;
      }
      HRESULT WINAPI hkBeginStateBlock(LPDIRECT3DDEVICE9 pDevice) {
         d3dDeviceHook->UnhookAll();

         auto retBeginStateBlock = origBeginStateBlock(pDevice);

         origTestCooperativeLevel = d3dDeviceHook->Hook(3, hkTestCooperativeLevel);
         origReset                = d3dDeviceHook->Hook(16, hkReset);
         origBeginScene           = d3dDeviceHook->Hook(41, hkBeginScene);
         origEndScene             = d3dDeviceHook->Hook(42, hkEndScene);
         origBeginStateBlock      = d3dDeviceHook->Hook(60, hkBeginStateBlock);
         return retBeginStateBlock;
      }
   #pragma endregion

   #pragma region exported helpers
      HRESULT WINAPI AddExtension(D3D9Extension extensionType, LPVOID extensionAddress) {
      #pragma ExportedFunction
         switch (extensionType) {
            case D3D9Extension::BeginScene:
               vBeginSceneExtensions.push_back(reinterpret_cast<BeginScene_t>(extensionAddress));
               break;
            case D3D9Extension::EndScene:
               vEndSceneExtensions.push_back(reinterpret_cast<EndScene_t>(extensionAddress));
               break;
            case D3D9Extension::BeforeReset:
               vBeforeResetExtensions.push_back(reinterpret_cast<Reset_t>(extensionAddress));
               break;
            case D3D9Extension::AfterReset:
               vAfterResetExtensions.push_back(reinterpret_cast<Reset_t>(extensionAddress));
               break;
            default:
               return FALSE;
         }
         return TRUE;
      }
      HRESULT WINAPI SetTestCooperativeLevelExtension(LPVOID extensionAddress) {
      #pragma ExportedFunction
         testCooperativeLevelExtension = reinterpret_cast<TestCooperativeLevel_t>(extensionAddress);
         return TRUE;
      }
      LPDIRECT3DDEVICE9 WINAPI GetD3D9Device() {
      #pragma ExportedFunction
         if (!isExtenderReady)
            return nullptr;

         return d3dDevice;
      }
      HWND WINAPI GetWindowHandle() {
      #pragma ExportedFunction
         if (!isExtenderReady)
            return nullptr;

         return d3dWindow;
      }
      bool WINAPI IsReady() {
      #pragma ExportedFunction
         return isExtenderReady;
      }
   #pragma endregion

      void Init() {
         DWORD pD3DDevice = NULL;
         while (!pD3DDevice) {
            pD3DDevice = *(DWORD*)d3dDeviceAddress;
            Sleep(100);
         }
         d3dDevice = (LPDIRECT3DDEVICE9)pD3DDevice;

         D3DDEVICE_CREATION_PARAMETERS cParams;
         d3dDevice->GetCreationParameters(&cParams);
         d3dWindow = cParams.hFocusWindow;

         d3dDeviceHook            = make_unique<VTableHook>((PDWORD*)d3dDevice);
         origTestCooperativeLevel = d3dDeviceHook->Hook(3, hkTestCooperativeLevel);
         origReset                = d3dDeviceHook->Hook(16, hkReset);
         origBeginScene           = d3dDeviceHook->Hook(41, hkBeginScene);
         origEndScene             = d3dDeviceHook->Hook(42, hkEndScene);
         origBeginStateBlock      = d3dDeviceHook->Hook(60, hkBeginStateBlock);

         isExtenderReady = true;
      }
   }

   bool isInit = false;
   DWORD WINAPI Init(LPVOID) {
      D3D9Extender::Init();
      DI8Extender::Init();

      isInit = true;
      return TRUE;
   }

#pragma region exported helpers
   bool WINAPI PrepareFor(MirrorHook::Game gameType) {
   #pragma ExportedFunction
      if (!isInit && !DI8Extender::isExtenderReady && !D3D9Extender::isExtenderReady) {
         Memory::Init();
         switch (gameType) {
            case MirrorHook::Game::MostWanted:
            {
               DI8Extender::dinput8Address    = Memory::makeAbsolute(0x582D14);
               D3D9Extender::d3dDeviceAddress = Memory::makeAbsolute(0x582BDC);
            }
            break;
            case MirrorHook::Game::Carbon:
            {
               DI8Extender::dinput8Address    = Memory::makeAbsolute(0x71F5CC);
               D3D9Extender::d3dDeviceAddress = Memory::makeAbsolute(0x6B0ABC);
            }
            break;
         }
         CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&Init, 0, 0, 0);
         return true;
      } else return false;
   }
   bool WINAPI PrepareForce(const DWORD& dinput8Address, const DWORD& d3dDeviceAddress) {
   #pragma ExportedFunction
      if (!isInit && !DI8Extender::isExtenderReady && !D3D9Extender::isExtenderReady) {
         Memory::Init();
         
         DI8Extender::dinput8Address    = Memory::makeAbsolute(dinput8Address);
         D3D9Extender::d3dDeviceAddress = Memory::makeAbsolute(d3dDeviceAddress);
         
         CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&Init, 0, 0, 0);
         return true;
      } else return false;
   }
   bool WINAPI IsReady() {
   #pragma ExportedFunction
      return isInit;
   }
   bool WINAPI IsShowingInfoOverlay() {
   #pragma ExportedFunction
      return D3D9Extender::infoOverlayFrame < D3D9Extender::infoOverlayFrame_MaxFrame;
   }
#pragma endregion

   BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID) {
      if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
         DisableThreadLibraryCalls(hModule);
      }
      return TRUE;
   }
}