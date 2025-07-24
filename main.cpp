// main.cpp
#include "skin.h"
std::string g_skinPath;

bool running = true;

#if defined(__MINGW32__) || defined(__MINGW64__)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "wacup/wa_ipc.h"
#include "wacup/gen.h"
#include "PlayerCore.h"

HWND hwnd_winamp = FindWindow("Winamp v1.x", NULL);
PlayerCore g_PlayerCore;
PlayerCore* PlayerCore = &g_PlayerCore; // global access


WCHAR skinname_buffer[MAX_PATH];

#define PLUGIN_TITLE L"Umami Skin Renderer"

LRESULT CALLBACK WinampSubclass(HWND, UINT, WPARAM, LPARAM);
int init();
void quit();
// Plug-in structure
winampGeneralPurposePlugin plugin = {
	GPPHDR_VER_U,
	(char*)PLUGIN_TITLE,
	init,
	NULL,
	quit,
    NULL,
    NULL
};

Skin skin;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    return main(__argc, __argv); // call your real main if needed
}
#endif // _MINGW

extern bool renderContainer(SDL_Renderer* renderer, Skin& skin, const std::string& containerId, const std::string& basePath);

void renderLoop(SDL_Renderer* renderer, Skin& skin, const std::string& containerName, const std::string& basePath, int renderIntervalMs = 16)
{
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
        }

        SDL_SetRenderDrawColor(renderer, 58, 110, 165, 255);
        SDL_RenderClear(renderer);

        renderContainer(renderer, skin, containerName, basePath);

#if defined(__LINUX__)
        // Shift sample data
        shift_vector_to_right(sample);
#endif // __LINUX__
        SDL_Delay(renderIntervalMs);

        SDL_RenderPresent(renderer);
    }
}

#if defined(__MINGW32__) || defined(__MINGW64__)
static WNDPROC lpOldWinampWndProc;
#endif // _MINGW

#if defined(__LINUX__)
std::vector<double> sample(75, 0.0);
#endif // __LINUX__
char sample[75 * 2] = {0}; 
int main(int argc, char* argv[]) {
    if (argc == 1){
        std::cout << "No skin to render.\n";
        std::cout << "Usage: " << argv[0] << " path/to/skin container (e.g. 'main', leave empty to display main container)\n";
        return 1;
    }
    g_skinPath = argv[1];


#if defined(_WIN32)
    g_skinPath += "\\";
#endif // have i expressed my hate for windows?

#if defined(__linux__)
    char* container_name = "main";
#else
    const char* container_name = "main"; // i dont think so
#endif

    if (argv[2] != NULL){
        container_name = argv[2];
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return 1;
    }

    // doesnt fix the weird artifacting happening in accelerated mode
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_Log("IMG_Init Error: %s", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Umami (SDL2)", 100, 100, 800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_Log("CreateWindow Error: %s", SDL_GetError());
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("CreateRenderer Error: %s", SDL_GetError());
        return 1;
    }

    TTF_Init();
#if defined(__LINUX__)
    sample[0] = 15.0; // Set the first element to 15.0
#endif // __LINUX__
    Skin skin;
    skin.loadFromXML("freeform/xml/winamp/cover/cover.xml");
    skin.loadFromXML("freeform/xml/winamp/thinger/thinger.xml");
    skin.loadFromXML("freeform/xml/wasabi/wasabi.xml");
    skin.loadFromXML("freeform/xml/pathpicker/pathpicker.xml");
    skin.loadFromXML("freeform/xml/statusbar/statusbar.xml");
    skin.loadFromXML("freeform/xml/tabsheet/tabsheet.xml");
    skin.loadFromXML("freeform/xml/checkbox/checkbox.xml");
    skin.loadFromXML("freeform/xml/titlebox/titlebox.xml");
    skin.loadFromXML("freeform/xml/dropdownlist/dropdownlist.xml");
    skin.loadFromXML("freeform/xml/combobox/combobox.xml");
    skin.loadFromXML("freeform/xml/historyeditbox/historyeditbox.xml");
    skin.loadFromXML("freeform/xml/tooltips/tooltips.xml");

    skin.loadFromXML(g_skinPath + "skin.xml");
    std::cout << "Loaded skin from: " << g_skinPath << "skin.xml\n";


    std::cout << "Skin has " << skin.containers.size() << " container(s)\n";

    for (const auto& container_pair : skin.containers) {
        const auto& container_name = container_pair.first;     // container name (string)
        const auto& container = container_pair.second;         // container object

        std::cout << "Container: " << container_name << "\n";
        std::cout << " - " << container.layouts.size() << " layout(s)\n";

        // Iterate over all layouts in the container
        for (const auto& layout : container.layouts) {
            std::cout << "   - Layout ID: " << layout->id
            << ", with " << layout->elements.size()
            << " root element(s)\n";
        }

        std::cout << "\n"; // extra line for readability between containers
    }

    std::cout << "Skin loaded successfully.\n";
    std::cout << "Containers: " << skin.containers.size() << '\n';
    std::cout << "Bitmaps: " << skin.bitmaps.size() << '\n';
    std::cout << "GroupDefs: " << skin.groupDefs.size() << '\n';

    bool running = true;
    SDL_Event e;
    renderLoop(renderer, skin, container_name, g_skinPath, 16);

    skin.unload();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    TTF_Quit();
    return 0;
}

#if defined(__MINGW32__) || defined(__MINGW64__)

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

int init() {
    // Initialize Winamp plugin
    SendMessage(hwnd_winamp, WM_WA_IPC, (WPARAM)skinname_buffer, IPC_GETSKINW);
    MessageBoxExW(hwnd_winamp, L"Umami plugin initialized", L"umami", MB_OK, 0);
    lpOldWinampWndProc = WNDPROC(SetWindowLongPtr(plugin.hwndParent, GWLP_WNDPROC, reinterpret_cast<intptr_t>(&WinampSubclass)));
    PlayerCore->Initialize(hwnd_winamp);

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return 1;
    }

    // doesnt fix the weird artifacting happening in accelerated mode
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_Log("IMG_Init Error: %s", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    window = SDL_CreateWindow("Umami (SDL2)", 100, 100, 800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_Log("CreateWindow Error: %s", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("CreateRenderer Error: %s", SDL_GetError());
        return 1;
    }

    TTF_Init();
    return 0; // Return success
}

void quit() {
    // Cleanup Winamp plugin
    if (plugin.hwndParent) {
        SetWindowLongPtr(plugin.hwndParent, GWLP_WNDPROC, reinterpret_cast<intptr_t>(lpOldWinampWndProc));
    }
    skin.unload();
    running = false;
    SDL_Quit();
    IMG_Quit();
    TTF_Quit();
    MessageBoxExW(hwnd_winamp, L"Umami plugin quit", L"umami", MB_OK, 0);
}

LRESULT CALLBACK WinampSubclass(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {

    case WM_WA_IPC: 
        if (lParam == IPC_SKIN_CHANGED) {
            skin.unload();
            SendMessage(hwnd_winamp, WM_WA_IPC, (WPARAM)skinname_buffer, IPC_GETSKINW);
            std::wstring msg = L"Skin changed to: " + std::wstring(skinname_buffer);
            //MessageBoxExW(hwnd, msg.c_str(), L"umami", MB_OK, 0);
            std::wstring skinname_wstr(skinname_buffer);
            int len = WideCharToMultiByte(CP_UTF8, 0, skinname_wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
            std::string skinname_utf8(len, 0);
            WideCharToMultiByte(CP_UTF8, 0, skinname_wstr.c_str(), -1, skinname_utf8.data(), len, nullptr, nullptr);
            skinname_utf8.pop_back(); // remove trailing null
            g_skinPath = skinname_utf8 + "\\";

            skin.loadFromXML("freeform/xml/winamp/cover/cover.xml");
            skin.loadFromXML("freeform/xml/winamp/thinger/thinger.xml");
            skin.loadFromXML("freeform/xml/wasabi/wasabi.xml");
            skin.loadFromXML("freeform/xml/pathpicker/pathpicker.xml");
            skin.loadFromXML("freeform/xml/statusbar/statusbar.xml");
            skin.loadFromXML("freeform/xml/tabsheet/tabsheet.xml");
            skin.loadFromXML("freeform/xml/checkbox/checkbox.xml");
            skin.loadFromXML("freeform/xml/titlebox/titlebox.xml");
            skin.loadFromXML("freeform/xml/dropdownlist/dropdownlist.xml");
            skin.loadFromXML("freeform/xml/combobox/combobox.xml");
            skin.loadFromXML("freeform/xml/historyeditbox/historyeditbox.xml");
            skin.loadFromXML("freeform/xml/tooltips/tooltips.xml");

            skin.loadFromXML(g_skinPath + "skin.xml");
            renderLoop(renderer, skin, "main", g_skinPath, 16);
        }
    break;

    }

    /* Call previous window procedure */
    return CallWindowProc((WNDPROC)lpOldWinampWndProc, hwnd, message, wParam, lParam);
}

// Plugin getter function
extern "C" __declspec(dllexport) winampGeneralPurposePlugin * winampGetGeneralPurposePlugin()
{
    return &plugin;
}
#endif // _MINGW