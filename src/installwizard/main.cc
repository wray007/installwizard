#include <windows.h>
#include "third_party/nsis/pluginapi.h"

HINSTANCE g_hInstance;

BOOL WINAPI DllMain(HMODULE hInst, 
                    ULONG ul_reason_for_call, 
                    LPVOID lpReserved) {
  g_hInstance = hInst;
  return TRUE;
}
