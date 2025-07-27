#include "skin.h"
#include "render_shared.h"

bool renderGrid(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH) {
    SDL_Rect rect = computeElementRectSDL(elem, parentX, parentY, parentW, parentH);

    const std::string parts[9] = {
        "topleft", "top", "topright",
        "left", "middle", "right",
        "bottomleft", "bottom", "bottomright"
    };

    SDL_Texture* textures[9] = {nullptr};
    SDL_FRect src[9] = {};
    SDL_FRect dst[9] = {};
    std::string bitmapIds[9];  // for logging

    // Load and prepare each piece
    for (int i = 0; i < 9; ++i) {
        const auto& attr = parts[i];
        auto it = elem.attributes.find(attr);
        if (it == elem.attributes.end()) continue;

        const std::string& bmpId = it->second;
        bitmapIds[i] = bmpId;

        auto bmpIt = skin.bitmaps.find(bmpId);
        if (bmpIt == skin.bitmaps.end()) continue;

        SkinBitmap& bmp = bmpIt->second;

        textures[i] = getOrLoadTexture(renderer, skin, bmp);

        if (!textures[i]) continue;

        src[i] = { bmp.x, bmp.y, bmp.w, bmp.h };

        if (src[i].w == 0 || src[i].h == 0) {
            int texW = 0, texH = 0;
            //SDL_QueryTexture(textures[i], nullptr, nullptr, &texW, &texH);
            if (src[i].w == 0) src[i].w = texW;
            if (src[i].h == 0) src[i].h = texH;
        }
    }

    // Compute edge sizes from available textures
    int wL = textures[0] ? src[0].w : (textures[3] ? src[3].w : 0);
    int wR = textures[2] ? src[2].w : (textures[5] ? src[5].w : 0);
    int hT = textures[0] ? src[0].h : (textures[1] ? src[1].h : 0);
    int hB = textures[6] ? src[6].h : (textures[7] ? src[7].h : 0);

    int totalMinW = wL + wR;
    int totalMinH = hT + hB;

    if (rect.w < totalMinW || rect.h < totalMinH) {
        #ifdef DEBUG
        SDL_Log("[renderGrid] area too small: (%d x %d), need at least %d x %d",
                rect.w, rect.h, totalMinW, totalMinH);
        #endif // DEBUG
        return false;
    }

    int midW = std::max(0, rect.w - totalMinW);
    int midH = std::max(0, rect.h - totalMinH);

    // 3x3 destination grid
    dst[0] = {rect.x, rect.y, wL, hT};                         // topleft
    dst[1] = {rect.x + wL, rect.y, midW, hT};                  // top
    dst[2] = {rect.x + wL + midW, rect.y, wR, hT};             // topright

    dst[3] = {rect.x, rect.y + hT, wL, midH};                  // left
    dst[4] = {rect.x + wL, rect.y + hT, midW, midH};           // middle
    dst[5] = {rect.x + wL + midW, rect.y + hT, wR, midH};      // right

    dst[6] = {rect.x, rect.y + hT + midH, wL, hB};             // bottomleft
    dst[7] = {rect.x + wL, rect.y + hT + midH, midW, hB};      // bottom
    dst[8] = {rect.x + wL + midW, rect.y + hT + midH, wR, hB}; // bottomright

    const std::string& gridId = elem.attributes.count("id") ? elem.attributes.at("id") : "(no-id)";

    // Render and log
    for (int i = 0; i < 9; ++i) {
        if (textures[i]) {
            #ifdef DEBUG
            SDL_Log("grid part %s from bitmap '%s' with grid ID '%s': src=(%d,%d,%d,%d) dst=(%d,%d,%d,%d)",
                    parts[i].c_str(), bitmapIds[i].c_str(), gridId.c_str(),
                    src[i].x, src[i].y, src[i].w, src[i].h,
                    dst[i].x, dst[i].y, dst[i].w, dst[i].h);
            #endif // DEBUG
            SDL_RenderTexture(renderer, textures[i], &src[i], &dst[i]);
            //SDL_DestroyTexture(textures[i]);
        } else {
            #ifdef DEBUG
            SDL_Log("grid part %s: MISSING TEXTURE", parts[i].c_str());
            #endif // DEBUG
        }
    }

    return true;
}

bool renderProgressGrid(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH) {
    SDL_Rect rect = computeElementRectSDL(elem, parentX, parentY, parentW, parentH);
    const std::string& id = elem.attributes.count("id") ? elem.attributes.at("id") : "(no-id)";
    std::string orientation = getAttr(elem, "orientation", "right"); // right, left, up, down

    std::string leftId   = getAttr(elem, "left", "");
    std::string middleId = getAttr(elem, "middle", "");
    std::string rightId  = getAttr(elem, "right", "");

    const SkinBitmap* leftBmp   = skin.bitmaps.count(leftId)   ? &skin.bitmaps[leftId]   : nullptr;
    const SkinBitmap* middleBmp = skin.bitmaps.count(middleId) ? &skin.bitmaps[middleId] : nullptr;
    const SkinBitmap* rightBmp  = skin.bitmaps.count(rightId)  ? &skin.bitmaps[rightId]  : nullptr;

    float fillPercent = 0.5f; // TODO: dynamic in future

    int fillW = static_cast<int>(rect.w * fillPercent);

    // Render left part (static)
    if (leftBmp) {
        SDL_FRect src = { leftBmp->x, leftBmp->y, leftBmp->w, leftBmp->h };
        SDL_FRect dst = { rect.x, rect.y, leftBmp->w, rect.h };
        std::string path = g_skinPath + leftBmp->file;
        if (SDL_Surface* s = IMG_Load(path.c_str())) {
            SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
            SDL_DestroySurface(s);
            SDL_RenderTexture(renderer, t, &src, &dst);
            SDL_DestroyTexture(t);
        }
    }

    // Render middle (stretchable fill)
    if (middleBmp) {
        SDL_FRect src = { middleBmp->x, middleBmp->y, middleBmp->w, middleBmp->h };
        SDL_FRect dst = { rect.x + (leftBmp ? leftBmp->w : 0), rect.y,
                         fillW - (leftBmp ? leftBmp->w : 0) - (rightBmp ? rightBmp->w : 0), rect.h };
        if (dst.w > 0) {
            std::string path = g_skinPath + middleBmp->file;
            if (SDL_Surface* s = IMG_Load(path.c_str())) {
                SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
                SDL_DestroySurface(s);
                SDL_RenderTexture(renderer, t, &src, &dst);
                SDL_DestroyTexture(t);
            }
        }
    }

    // Render right cap
    if (rightBmp) {
        SDL_FRect src = { rightBmp->x, rightBmp->y, rightBmp->w, rightBmp->h };
        SDL_FRect dst = { rect.x + fillW - rightBmp->w, rect.y, rightBmp->w, rect.h };
        std::string path = g_skinPath + rightBmp->file;
        if (SDL_Surface* s = IMG_Load(path.c_str())) {
            SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
            SDL_DestroySurface(s);
            SDL_RenderTexture(renderer, t, &src, &dst);
            SDL_DestroyTexture(t);
        }
    }

    return true;
}
