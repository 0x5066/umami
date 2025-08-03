#pragma once
#include "WinampData.h"

class PlayerCore {
public:
    PlayerCore();                       // default constructor
    bool Initialize(HWND winampHwnd);   // takes HWND during init
    bool GetSaData(char out[75 * 2 + 8]);
    int getOutputTime();                // get output time from Winamp
    int getLength();                    // get length of current track from Winamp
    int getCurVolume();                 // get current volume from Winamp
    int isPlaying();                    // check if Winamp is playing 
    int getKbps();                      // get bitrate of current track
    int getKhz();                       // get sample rate of current track
    int getRealKhz();                   // get *real* sample rate of current track
    int getChannels();                  // get number of channels in current track
    int getPlaylistPos();               // get current playlist position
    std::wstring getPlayingTitle();     // get title of currently playing track
    
private:
    WinampData* dataGetter = nullptr;     // use pointer so we can init later
};
