#include "wasabiparser.h"
#include "umamistructs.h"

#ifdef _WIN32

#ifndef UNICODE
#define UNICODE
#endif

// windows.h conflicts with tinyxml2, so we need WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <vector>
#include <windows.h>

std::vector<HWND> windows;

std::vector<umamiContainer*> containers = {&umamiMain, &PL, &EQ, &MLibrary};
#endif // _WIN32

#ifdef _WIN32

HWND mainHwnd = NULL; // Global variable to store the main window handle

HWND SpawnCoolFuckingWindow(HINSTANCE hInstance, umamiContainer& container);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        if (hwnd == mainHwnd) {
            PostQuitMessage(0);
        } else {
            DestroyWindow(hwnd);
        }
        return 0;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // All painting occurs here, between BeginPaint and EndPaint.

            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_APPWORKSPACE));

            EndPaint(hwnd, &ps);
        }
    case WM_GETMINMAXINFO:
        {
            if (lParam == 0) break;
            LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
            if (lpMMI) {
                try {
                    lpMMI->ptMinTrackSize.x = 275;  // Default size if stoi fails
                    lpMMI->ptMinTrackSize.y = 116;
                } catch (const std::exception& e) {
                    OutputDebugStringA(("Error parsing min track size: " + std::string(e.what()) + "\n").c_str());
                }
            }
        }
        return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

HWND SpawnCoolFuckingWindow(HINSTANCE hInstance, umamiContainer& container)
{
    const wchar_t CLASS_NAME[] = L"umami_window";

    static bool classRegistered = false;
    if (!classRegistered) {
        WNDCLASS wc = {};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = CLASS_NAME;
        RegisterClass(&wc);
        classRegistered = true;
    }

    std::cout << "containerID " <<
    container.containerID << '\n' <<
    "defaultx " <<
    std::stoi(container.containerDefaultX) << '\n' <<
    "defaulty " <<
    std::stoi(container.containerDefaultY) << '\n' <<
    "MinimumW " <<
    std::stoi(container.containerSizeW) << '\n' <<
    "MinimumH " <<
    std::stoi(container.containerSizeH) << '\n' <<
    "default_visible " <<
    container.containerDefaultVisible << '\n';

    int defx = std::stoi(container.containerDefaultX);
    int defy = std::stoi(container.containerDefaultY);
    int minw = std::stoi(container.containerSizeW);
    int minh = std::stoi(container.containerSizeH);
    int defv;

    if (container.containerDefaultVisible == "@HAVE_LIBRARY@") {
        std::cout << "HIT!" << '\n';
        defv = 1;
    } else {
        std::cout << "HIT2!" << '\n';
        defv = std::stoi(container.containerDefaultVisible);
    }

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        std::wstring(container.containerID.begin(), container.containerID.end()).c_str(),
        WS_OVERLAPPEDWINDOW,
        defx, defy,
        minw, minh,
        (container.containerID == "main") ? NULL : mainHwnd, // Set parent to mainHwnd if not "main"
        NULL, hInstance, NULL
    );

    SetWindowLong(hwnd, GWL_STYLE,
        GetWindowLong(hwnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);  

    if (container.containerID == "main") {
        mainHwnd = hwnd; // Store the main window handle
    }

    if (!hwnd) {
        std::cerr << "Failed to create window: " << container.containerID << std::endl;
        return NULL;
    }

    if (defv == 1) {
        ShowWindow(hwnd, SW_SHOW);
    }

    return hwnd;
}

#endif // _WIN32

int main()
{
    //loadFreeform("freeform");
    WALvalidator("skin/skin.xml");
    //std::cout << umamiMain.containerID << '\n' << umamiMain.containerMinimumW << '\n' << umamiMain.containerMinimumH << '\n';
#ifdef _WIN32
    HINSTANCE hInstance = GetModuleHandle(NULL);

    for (auto* container : containers) {
        if (!container->containerID.empty()) {  // Ensure the container has valid data
            HWND hwnd = SpawnCoolFuckingWindow(hInstance, *container);
            if (hwnd) windows.push_back(hwnd);
        }
    }

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
#endif // _WIN32

#ifdef _LINUX
    // verify the elements we got from our parsed stuff
    for (const auto& element : elements) {
        std::cout << "Element Type: " << element.type << ", ID: " << element.id << ", File: " << element.file
                  << ", HotX: " << element.hotx << ", HotY: " << element.hoty
                  << ", ImageW: " << element.imagew << ", ImageH: " << element.imageh << '\n';
    }
#endif // _LINUX
    return 0;
}
