// renderer.cpp
#include "render_shared.h"
#include "skin.h"

extern bool renderElement(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int x, int y, int w, int h);

bool renderSkinBitmaps(SDL_Renderer* renderer, Skin& skin, const std::string& basePath) {
    int x = 10, y = 10, rowHeight = 0;

    for (const auto& [id, bmp] : skin.bitmaps) {


        std::string fullPath =  std::filesystem::absolute(basePath + "/" + bmp.file).string();
        SDL_Surface* surface = IMG_Load(fullPath.c_str());
        if (!surface) {
            SDL_Log("Failed to load: %s", fullPath.c_str());
            continue;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        if (!texture) continue;

        SDL_Rect src = { bmp.x, bmp.y, bmp.w, bmp.h };
        SDL_Rect dst = { x, y, bmp.w, bmp.h };

        SDL_RenderCopy(renderer, texture, &src, &dst);
        SDL_DestroyTexture(texture);

        x += bmp.w + 10;
        if (bmp.h > rowHeight) rowHeight = bmp.h;
        if (x > 800) { x = 10; y += rowHeight + 10; rowHeight = 0; }
    }

    return true;
}

int parentWidth, parentHeight; // fallback default
bool renderContainer(SDL_Renderer* renderer, Skin& skin, const std::string& containerId, const std::string& basePath) {
    auto it = skin.containers.find(containerId);
    if (it == skin.containers.end()) return false;
    const Container& container = it->second;

    for (const auto& layout : container.layouts) {
        parentWidth = 800; parentHeight = 600;

        // Check and apply minimum width
        if (layout->attributes.count("minimum_w")) {
            int minW = std::stoi(layout->attributes.at("minimum_w"));
            if (parentWidth < minW) parentWidth = minW;
        }

        // Check and apply minimum height
        if (layout->attributes.count("minimum_h")) {
            int minH = std::stoi(layout->attributes.at("minimum_h"));
            if (parentHeight < minH) parentHeight = minH;
        }

        // Check and apply minimum width
        if (layout->attributes.count("maximum_w")) {
            int maxW = std::stoi(layout->attributes.at("maximum_w"));
            if (parentWidth > maxW) parentWidth = maxW;
        }

        // Check and apply minimum height
        if (layout->attributes.count("maximum_h")) {
            int maxH = std::stoi(layout->attributes.at("maximum_h"));
            if (parentHeight > maxH) parentHeight = maxH;
        }

        // --- Render background ONCE per layout, before elements ---
        if (layout->attributes.count("background")) {
            const std::string& bgId = layout->attributes.at("background");
            //std::cout << "Trying to render background: " << bgId << std::endl;
            auto bmpIt = skin.bitmaps.find(bgId);
                if (bmpIt != skin.bitmaps.end()) {
                    SkinBitmap& bmp = bmpIt->second;
                    SDL_Texture* texture = getOrLoadTexture(renderer, skin, bmp);
                if (texture){
                    if (!(layout->attributes.count("minimum_w"))){
                            parentWidth = bmp.w;
                        }
                    if (!(layout->attributes.count("minimum_h"))){
                            parentHeight = bmp.h;
                        }
                    SDL_Rect src = { bmp.x, bmp.y, bmp.w, bmp.h };
                    SDL_Rect dst = { 0, 0, parentWidth, parentHeight };
                    SDL_RenderCopy(renderer, texture, &src, &dst);
                }
            }
        }
        // --- End background ---

        for (const auto& elem : layout->elements) {
            renderElement(renderer, skin, *elem, 0, 0, parentWidth, parentHeight);
        }
        break; // Only render the first layout
    }
    return true;
}