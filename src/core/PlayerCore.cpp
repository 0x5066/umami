#include "PlayerCore.h"

PlayerCore::PlayerCore() {}

bool PlayerCore::Initialize(HWND hwnd) {
    if (dataGetter) delete dataGetter;
    dataGetter = new WinampData(hwnd);
    return dataGetter->Initialize();
}

bool PlayerCore::GetSaData(char out[75 * 2 + 8]) {
    if (!dataGetter) return false;
    return dataGetter->GetData(out);
}

int PlayerCore::getOutputTime() {
    if (!dataGetter) return 0;
    return dataGetter->getOutputTime();
}

int PlayerCore::getLength() {
    if (!dataGetter) return 0;
    return dataGetter->getLength();
}

int PlayerCore::getCurVolume() {
    if (!dataGetter) return 0;
    return dataGetter->getCurVolume();
}

int PlayerCore::isPlaying() {
    if (!dataGetter) return 0;
    return dataGetter->isPlaying();
}

int PlayerCore::getKbps() {
    if (!dataGetter) return 0;
    return dataGetter->getKbps();
}

int PlayerCore::getKhz() {
    if (!dataGetter) return 0;
    return dataGetter->getKhz();
}

int PlayerCore::getRealKhz() {
    if (!dataGetter) return 0;
    return dataGetter->getRealKhz();
}

int PlayerCore::getChannels() {
    if (!dataGetter) return 0;
    return dataGetter->getChannels();
}

int PlayerCore::getPlaylistPos() {
    if (!dataGetter) return 0;
    return dataGetter->getPlaylistPos();
}

std::wstring PlayerCore::getPlayingTitle() {
    if (!dataGetter) return L"";
    return dataGetter->getPlayingTitle();
}