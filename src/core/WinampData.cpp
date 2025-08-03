// WinampSaData.cpp
#include "WinampData.h"
#include <windows.h>

WinampData::WinampData(HWND winampHwnd) : hwnd(winampHwnd) {}

bool WinampData::Initialize() {
    *(void**)&export_sa_setreq = (void*)SendMessage(hwnd, WM_WA_IPC, 1, IPC_GETSADATAFUNC);
    *(void**)&export_sa_get     = (void*)SendMessage(hwnd, WM_WA_IPC, 2, IPC_GETSADATAFUNC);
    return export_sa_get != nullptr;
}

bool WinampData::GetData(char out[75 * 2 + 8]) {
    if (!export_sa_get) return false;
    char* p = export_sa_get(out);
    return p != nullptr;
}

int WinampData::getOutputTime() {
    int res = SendMessage(hwnd,WM_WA_IPC,0,IPC_GETOUTPUTTIME);
    return res;
}

int WinampData::getLength() {
    int res = SendMessage(hwnd,WM_WA_IPC,2,IPC_GETOUTPUTTIME);
    return res;
}

int WinampData::getCurVolume() {
    int vol = IPC_GETVOLUME(hwnd);
    return vol;
}

int WinampData::isPlaying() {
    int res = SendMessage(hwnd, WM_WA_IPC, 0, IPC_ISPLAYING);
    return res;
}

int WinampData::getKbps() {
    int kbps = SendMessageW(hwnd,WM_WA_IPC,1,IPC_GETINFO);
    return kbps;
}

int WinampData::getKhz() {
    int khz = SendMessageW(hwnd,WM_WA_IPC,0,IPC_GETINFO);
    return khz;
}

int WinampData::getRealKhz() {
    int khz = SendMessageW(hwnd,WM_WA_IPC,5,IPC_GETINFO);
    return khz;
}

int WinampData::getChannels() {
    int channels = SendMessageW(hwnd,WM_WA_IPC,2,IPC_GETINFO);
    return channels;
}

int WinampData::getPlaylistPos() {
    int pos = SendMessage(hwnd, WM_WA_IPC, 0, IPC_GETLISTPOS);
    return pos;
}

std::wstring WinampData::getPlayingTitle() {
    wchar_t* title = (wchar_t*)SendMessage(hwnd, WM_WA_IPC, 0, IPC_GET_PLAYING_TITLE);
    if (title) {
        return std::wstring(title).c_str();
    }
    return L" ";
}