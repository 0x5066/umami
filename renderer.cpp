// renderer.cpp
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "skin.h"

bool renderSkinBitmaps(SDL_Renderer* renderer, Skin& skin, const std::string& basePath) {
    int x = 10, y = 10, rowHeight = 0;

    for (const auto& [id, bmp] : skin.bitmaps) {
        std::string fullPath = basePath + "/" + bmp.file;

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

static int compute_dimension(const std::string& value, int relatMode, int parent) {
    int val = std::stoi(value);
    switch (relatMode) {
        case 1: return (val * parent) / 255; // relative
        case 2: return val * parent;         // proportional
        default: return val;                // absolute
    }
}

bool renderContainer(SDL_Renderer* renderer, Skin& skin, const std::string& containerId, const std::string& basePath) {
    auto it = skin.containers.find(containerId);
    if (it == skin.containers.end()) return false;
    const Container& container = it->second;

    for (const auto& layout : container.layouts) {
        int parentWidth = 275, parentHeight = 116; // fallback default

        if (layout->attributes.count("minimum_w")) {
            parentWidth = std::stoi(layout->attributes.at("minimum_w"));
        }
        if (layout->attributes.count("minimum_h")) {
            parentHeight = std::stoi(layout->attributes.at("minimum_h"));
        }

        // --- Render background ONCE per layout, before elements ---
        if (layout->attributes.count("background")) {
            const std::string& bgId = layout->attributes.at("background");
            //std::cout << "Trying to render background: " << bgId << std::endl;
            auto bmpIt = skin.bitmaps.find(bgId);
            if (bmpIt != skin.bitmaps.end()) {
                const SkinBitmap& bmp = bmpIt->second;
                std::string fullPath = basePath + "/" + bmp.file;
                //std::cout << "Background file: " << fullPath << std::endl;
                SDL_Surface* surface = IMG_Load(fullPath.c_str());
                if (!surface) {
                    SDL_Log("Failed to load: %s", fullPath.c_str());
                } else {
                    int srcW = bmp.w > 0 ? bmp.w : surface->w;
                    int srcH = bmp.h > 0 ? bmp.h : surface->h;
                    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                    SDL_FreeSurface(surface);
                    if (texture) {
                        SDL_Rect src = { bmp.x, bmp.y, srcW, srcH };
                        SDL_Rect dst = { 0, 0, parentWidth, parentHeight };
                        //std::cout << "Rendering background src: " << bmp.x << "," << bmp.y << "," << srcW << "," << srcH << std::endl;
                        //std::cout << "Rendering background dst: 0,0," << parentWidth << "," << parentHeight << std::endl;
                        SDL_RenderCopy(renderer, texture, &src, &dst);
                        SDL_DestroyTexture(texture);
                    } else {
                        std::cout << "Failed to create texture from surface" << std::endl;
                    }
                }
            } else {
                std::cout << "Background bitmap id not found: " << bgId << std::endl;
            }
        }
        // --- End background ---

        for (const auto& elem : layout->elements) {
            std::string tag = elem->tag;
            if (tag == "layer" || tag == "button") {
                auto it_img = elem->attributes.find("image");
                if (it_img == elem->attributes.end()) continue;
                const std::string& imageId = it_img->second;

                auto bmpIt = skin.bitmaps.find(imageId);
                if (bmpIt == skin.bitmaps.end()) continue;
                const SkinBitmap& bmp = bmpIt->second;

                std::string fullPath = basePath + "/" + bmp.file;
                SDL_Surface* surface = IMG_Load(fullPath.c_str());
                if (!surface) continue;

                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_FreeSurface(surface);
                if (!texture) continue;

                int x = compute_dimension(elem->attributes.count("x") ? elem->attributes.at("x") : "0",
                                          std::stoi(elem->attributes.count("relatx") ? elem->attributes.at("relatx") : "0"),
                                          parentWidth);

                int y = compute_dimension(elem->attributes.count("y") ? elem->attributes.at("y") : "0",
                                          std::stoi(elem->attributes.count("relaty") ? elem->attributes.at("relaty") : "0"),
                                          parentHeight);

                int w = compute_dimension(std::to_string(bmp.w),
                                          std::stoi(elem->attributes.count("relatw") ? elem->attributes.at("relatw") : "0"),
                                          parentWidth);

                int h = compute_dimension(std::to_string(bmp.h),
                                          std::stoi(elem->attributes.count("relath") ? elem->attributes.at("relath") : "0"),
                                          parentHeight);

                SDL_Rect src = { bmp.x, bmp.y, bmp.w, bmp.h };
                SDL_Rect dst = { x, y, w, h };
                SDL_RenderCopy(renderer, texture, &src, &dst);
                SDL_DestroyTexture(texture);
            }
        }
    }
    return true;
}
