# MirrorHook
A wrapper for extending the existing D3D9 and DI8 implementation in some NFS games.

### D3D9
After it is injected, it waits for the game to create its D3D9 device and then it hooks the following function(s):
- BeginScene
- EndScene
- Reset
- TestCooperativeLevel
- BeginStateBlock (to ensure hook stays running)

After hooking the functions, it lets external applications add their own extension functions.
```
// Example:
#include "path\to\MirrorHook\inc\Definitions.hpp"

void WINAPI MySuperEndSceneExtension(LPDIRECT3DDEVICE9 pDevice) {
      // code...
}

void MyAwesomeInitFunction() {
      // Make sure MirrorHook was loaded into the memory.
      HMODULE hMirrorHook = nullptr;
      while (!hMirrorHook) {
         hMirrorHook = GetModuleHandle("MirrorHook.asi");
         Sleep(100);
      }
      
      // Add some super extender
      MirrorHook::D3D9::AddExtension(MirrorHook::D3D9::D3D9Extension::EndScene, &MySuperEndSceneExtension);
      
      // Wait for the hook to finish, to get the window handle for example.
      while (!MirrorHook::D3D9::IsReady()) {
         Sleep(100);
      }
}
```
The actual DirectX libraries are **not needed** to write extensions, `void*` can be used for the parameters instead.
```
// Example:
// Instead of ->
void WINAPI MySuperEndSceneExtension(LPDIRECT3DDEVICE9 pDevice);
// You can type ->
void WINAPI MySuperEndSceneExtension(LPVOID pDevice);
```
### DirectInput8
Same as the D3D9 implementation explained above. After it is injected, it waits for the game to create its instance of DirectInput8 and then it hooks the following function(s):
- GetDeviceState

Currently, only mouse and keyboard events are supported. 

## Development dependencies
- [June 2010 DirectX SDK](https://www.microsoft.com/en-us/download/details.aspx?id=6812)
- C++14 compliant compiler [e.g., VS2015 and above](https://www.visualstudio.com)
- Universal CRT SDK

## How to set up
- Change the parameters of the `copy` command in Post-Build Event or remove it.
- Compile.

## Notes
This is a strictly WIN32-API library. The code will *not* work in any other environment unless you can provide the necessary libraries. (e.g., WineHQ)
The code utilizes C++14 and is built against the `Windows 7 SDK` with the `Visual Studio 2017 - Windows XP (v141_xp)` toolset. The output will work on Windows XP SP1 and above.
