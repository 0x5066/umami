#pragma once
#include "WinampSaData.h"

class PlayerCore {
public:
    PlayerCore();                       // default constructor
    bool Initialize(HWND winampHwnd);   // takes HWND during init
    bool GetSaData(char out[75 * 2 + 8]);

private:
    WinampSaData* saData = nullptr;     // use pointer so we can init later
};
