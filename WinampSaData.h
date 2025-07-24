// WinampSaData.h
#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class WinampSaData {
public:
    explicit WinampSaData(HWND winampHwnd);
    bool Initialize();
    void SetRequest(int want); // want = 0 or 1
    bool GetData(char out[75 * 2 + 8]);

private:
    HWND hwnd;
    typedef void (__cdecl *SetReqFunc)(int);
    typedef char* (__cdecl *GetDataFunc)(char[75 * 2 + 8]);

    SetReqFunc export_sa_setreq = nullptr;
    GetDataFunc export_sa_get = nullptr;
};
