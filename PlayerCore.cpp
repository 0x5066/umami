#include "PlayerCore.h"

PlayerCore::PlayerCore() {}

bool PlayerCore::Initialize(HWND hwnd) {
    if (saData) delete saData;
    saData = new WinampSaData(hwnd);
    return saData->Initialize();
}

bool PlayerCore::GetSaData(char out[75 * 2 + 8]) {
    if (!saData) return false;
    return saData->GetData(out);
}
