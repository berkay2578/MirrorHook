# MirrorHook
A wrapper for extending the existing D3D9 and DI8 implementation in NFS: Most Wanted (2005)

## How does it work?
**This only hooks the D3D9 implementation *for now*.**

It currently utilizes the [ASI Loader by ThirteenAG](https://github.com/ThirteenAG/Ultimate-ASI-Loader) to be injected to the game memory.
After it is injected, it waits for NFS: Most Wanted (2005) to create its D3D9 device and then it hooks 4 of its functions:
- BeginScene
- EndScene
- Reset
- BeginStateBlock

After hooking the functions, it lets external applications add their own extender functions.
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
      
      MirrorHook::D3D9::AddD3D9Extender(MirrorHook::D3D9::D3D9Extender::EndScene, &MySuperEndSceneExtension);
}
```
Moreover, the actual DirectX libraries are **not needed* to write extensions (as long as you don't actually need them).
```
// Example:
// Instead of ->
void WINAPI MySuperEndSceneExtension(LPDIRECT3DDEVICE9 pDevice);
// You can type ->
void WINAPI MySuperEndSceneExtension(LPVOID pDevice);
```

## Development dependencies
- [June 2010 DirectX SDK](https://www.microsoft.com/en-us/download/details.aspx?id=6812)
- C++14 compliant compiler [e.g., VS2015 and above](https://www.visualstudio.com)

## How to set up
- Change the parameters of `copy` in Post-Build Event command or remove it.
- Compile.

## Notes
This is a strictly WIN32-API library. The code will *not* work in any other environment unless you can provide the necessary libraries. (e.g., WineHQ)
The code utilizes C++14 and is built against the `Windows 7 SDK` with the `Visual Studio 2017 - Windows XP (v141_xp)` toolset. The output will work on Windows XP SP1 and above.
