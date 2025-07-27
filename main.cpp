// main.cpp
#include "skin.h"
std::string g_skinPath;

extern bool renderContainer(SDL_Renderer* renderer, Skin& skin, const std::string& containerId, const std::string& basePath);

void renderLoop(SDL_Renderer* renderer, Skin& skin, const std::string& containerName, const std::string& basePath, int renderIntervalMs = 16)
{
    bool running = true;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) running = false;
        }

        SDL_SetRenderDrawColor(renderer, 58, 110, 165, 255);
        SDL_RenderClear(renderer);

        renderContainer(renderer, skin, containerName, basePath);

        // Shift sample data
        shift_vector_to_right(sample);

        SDL_Delay(renderIntervalMs);

        SDL_RenderPresent(renderer);
    }
}

std::vector<double> sample(75, 0.0);
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

    SDL_Window *window;
    SDL_Renderer *renderer;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    // doesnt fix the weird artifacting happening in accelerated mode
    //SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

    SDL_CreateWindowAndRenderer("Umami (SDL3)", 800, 600, SDL_WINDOW_VULKAN, &window, &renderer);

    TTF_Init();
    
    sample[0] = 15.0; // Set the first element to 15.0

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
    SDL_Quit();
    TTF_Quit();
    return 0;
}
