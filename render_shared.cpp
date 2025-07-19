#include <string>
#include "render_shared.h"

template std::string getAttr<UIElement>(const UIElement&, const std::string&, const std::string&);

SDL_Texture* loadSkinTexture(SDL_Renderer* renderer, const std::string& imageFile, SDL_Surface** outSurface = nullptr) {
    std::vector<std::string> searchPaths = {
        g_skinPath + imageFile,
        "freeform/xml/wasabi/" + imageFile,
        "freeform/" + imageFile
    };

    for (const std::string& path : searchPaths) {
        SDL_Log("Trying to load image: %s", path.c_str());

        if (!std::filesystem::exists(path)) {
            SDL_Log("Path does not exist: %s", path.c_str());
            continue;
        }

        SDL_Surface* surface = IMG_Load(path.c_str());
        if (!surface) {
            SDL_Log("Failed to load %s: %s", path.c_str(), IMG_GetError());
            continue;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            SDL_Log("Failed to create texture from %s: %s", path.c_str(), SDL_GetError());
            SDL_FreeSurface(surface);
            continue;
        }

        SDL_Log("Successfully loaded texture: %s", path.c_str());

        if (outSurface) {
            *outSurface = surface;
        } else {
            SDL_FreeSurface(surface);
        }

        return texture;
    }

    SDL_Log("All attempts to load image '%s' failed.", imageFile.c_str());
    return nullptr;
}

SDL_Texture* getOrLoadTexture(SDL_Renderer* renderer, Skin& skin, SkinBitmap& bmp) {
    if (bmp.texture) return bmp.texture;

    bmp.texture = loadSkinTexture(renderer, bmp.file);
    if (!bmp.texture) {
        SDL_Log("Failed to load texture for SkinBitmap id: %s, and file: %s", bmp.id.c_str(), bmp.file.c_str());
    }

    return bmp.texture;
}