// WinampSaData.h
#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "../../wacup/wa_ipc.h"
#include <string>

class WinampData {
public:
    explicit WinampData(HWND winampHwnd);
    bool Initialize();
    bool GetData(char out[75 * 2 + 8]);
    int getOutputTime();
    int getCurVolume();
    int getLength();
    int isPlaying();
    int getKbps();
    int getKhz();
    int getRealKhz();
    int getChannels();
    int getPlaylistPos();
    std::wstring getPlayingTitle();

private:
    HWND hwnd;
    typedef void (__cdecl *SetReqFunc)(int);
    typedef char* (__cdecl *GetDataFunc)(char[75 * 2 + 8]);

    SetReqFunc export_sa_setreq = nullptr;
    GetDataFunc export_sa_get = nullptr;
};
