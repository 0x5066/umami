#include "skin.h"
#include "render_shared.h"

bool renderStatus(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH) {
    std::string bitmapKey;
    /*switch (g_fakePlayerState) {
        case PlayerState::Playing: bitmapKey = getAttr(elem, "playBitmap", ""); break;
        case PlayerState::Paused:  bitmapKey = getAttr(elem, "pauseBitmap", ""); break;
        case PlayerState::Stopped: bitmapKey = getAttr(elem, "stopBitmap", ""); break;
    }*/

    bitmapKey = getAttr(elem, "playBitmap", "");

    if (bitmapKey.empty()) return false;

    auto bmpIt = skin.bitmaps.find(bitmapKey);
    if (bmpIt == skin.bitmaps.end()) return false;
    const SkinBitmap& bmp = bmpIt->second;

    std::string path = g_skinPath + bmp.file;
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) return false;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) return false;

    SDL_Rect dst = computeElementRect(elem, parentX, parentY, parentW, parentH);
    SDL_Rect src = { bmp.x, bmp.y, bmp.w, bmp.h };

    SDL_RenderCopy(renderer, texture, &src, &dst);
    SDL_DestroyTexture(texture);
    return true;
}
