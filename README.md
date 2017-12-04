# MirrorHook
A wrapper for extending the existing D3D9 and DI8 implementation in NFS: Most Wanted (2005)

## How does it work?
It is recommended to use the [ASI Loader by ThirteenAG](https://github.com/ThirteenAG/Ultimate-ASI-Loader) to inject this into the game memory.

### D3D9
After it is injected, it waits for NFS: Most Wanted (2005) to create its D3D9 device and then it hooks the following function(s):
- BeginScene
- EndScene
- Reset
- TestCooperativeLevel
- BeginStateBlock

After hooking the functions, it lets external applications add their own extension functions.
```
// Example:
#include "\path\to\MirrorHook\Definitions.hpp"

void WINAPI MySuperEndSceneExtension(LPDIRECT3DDEVICE9 pDevice) {
      // code...
}

void MyAwesomeInitFunction() {
      HMODULE hMirrorHook = nullptr;
      while (!hMirrorHook) {
         hMirrorHook = GetModuleHandle("MirrorHook.asi");
         Sleep(100);
      }
      
      while (!MirrorHook::D3D9::IsReady()) {
         Sleep(100);
      }
      
      MirrorHook::D3D9::AddExtension(MirrorHook::D3D9::D3D9Extension::EndScene, &MySuperEndSceneExtension);
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
Same as the D3D9 implementation explained above. After it is injected, it waits for NFS: Most Wanted (2005) to create its instance of DirectInput8, game controllers, and then it hooks the following function(s):
- GetDeviceState

Currently, only mouse and keyboard events are supported. 

## Development dependencies
- [June 2010 DirectX SDK](https://www.microsoft.com/en-us/download/details.aspx?id=6812)
- C++14 compliant compiler [e.g., VS2015 and above](https://www.visualstudio.com)

## How to set up
- Change the parameters of `copy` in Post-Build Event command or remove it.
- Compile.

## Notes
This is a strictly WIN32-API library. The code will *not* work in any other environment unless you can provide the necessary libraries. (e.g., WineHQ)
The code utilizes C++14 and is built against the `Windows 7 SDK` with the `Visual Studio 2017 - Windows XP (v141_xp)` toolset. The output will work on Windows XP SP1 and above.
