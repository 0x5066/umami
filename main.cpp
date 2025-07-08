// main.cpp
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "skin.h"

extern bool renderContainer(SDL_Renderer* renderer, Skin& skin, const std::string& containerId, const std::string& basePath);

int main(int argc, char* argv[]) {
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

    Skin skin;
    if (!skin.loadFromXML("skin/skin.xml")) {
        SDL_Log("Failed to load skin\n");
        return 1;
    }

    std::cout << "Skin has " << skin.containers.size() << " container(s)\n";

    auto it = skin.containers.find("main");
    if (it == skin.containers.end()) {
        std::cout << "Container 'main' not found!\n";
    } else {
        std::cout << "'main' container has " << it->second.layouts.size() << " layout(s)\n";
        for (const auto& layout : it->second.layouts) {
            std::cout << " - layout id: " << layout->id << ", with " << layout->elements.size() << " root element(s)\n";
        }
    }


    std::cout << "Skin loaded successfully.\n";
    std::cout << "Containers: " << skin.containers.size() << '\n';
    std::cout << "Bitmaps: " << skin.bitmaps.size() << '\n';
    std::cout << "GroupDefs: " << skin.groupDefs.size() << '\n';

    bool running = true;
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
        }

        SDL_SetRenderDrawColor(renderer, 58, 110, 165, 255);
        SDL_RenderClear(renderer);

        renderContainer(renderer, skin, "main", "skin");

        SDL_Delay(100);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
