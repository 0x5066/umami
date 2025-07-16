// main.cpp
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "skin.h"
std::string g_skinPath;

extern bool renderContainer(SDL_Renderer* renderer, Skin& skin, const std::string& containerId, const std::string& basePath);

void renderLoop(SDL_Renderer* renderer, Skin& skin, const std::string& containerName, const std::string& basePath, int renderIntervalMs = 200)
{
    bool running = true;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
        }

        SDL_SetRenderDrawColor(renderer, 58, 110, 165, 255);
        SDL_RenderClear(renderer);

        renderContainer(renderer, skin, containerName, basePath);

        SDL_Delay(renderIntervalMs);

        SDL_RenderPresent(renderer);
    }
}


int main(int argc, char* argv[]) {
    if (argc == 1){
        std::cout << "No skin passed for render.\n";
        std::cout << "Usage: " << argv[0] << " [path_to_skin_directory] [container (leave it blank to display 'main' as default fallback)]\n";
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
    renderLoop(renderer, skin, container_name, g_skinPath, 200);



    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    TTF_Quit();
    return 0;
}
