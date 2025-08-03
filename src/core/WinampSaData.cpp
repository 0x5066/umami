// WinampSaData.cpp
#include "WinampSaData.h"
#include <windows.h>

#define WM_WA_IPC (WM_USER)
#define IPC_GETSADATAFUNC 800

WinampSaData::WinampSaData(HWND winampHwnd) : hwnd(winampHwnd) {}

bool WinampSaData::Initialize() {
    *(void**)&export_sa_setreq = (void*)SendMessage(hwnd, WM_WA_IPC, 1, IPC_GETSADATAFUNC);
    *(void**)&export_sa_get     = (void*)SendMessage(hwnd, WM_WA_IPC, 2, IPC_GETSADATAFUNC);
    return export_sa_get != nullptr;
}

void WinampSaData::SetRequest(int want) {
    if (export_sa_setreq) export_sa_setreq(want);
}

bool WinampSaData::GetData(char out[75 * 2 + 8]) {
    if (!export_sa_get) return false;
    char* p = export_sa_get(out);
    return p != nullptr;
}
