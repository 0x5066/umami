#include "skin.h"
#include "render_shared.h"

// Render a slider element (horizontal or vertical)
// whenever there is more than one slider object in a skin (as is usually the case), the calculations inside "speed up"
// need to find a way to prevent it from happening
bool renderSlider(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH) {
    // Determine orientation
    std::string orientation = getAttr(elem, "orientation", "h");
    bool isVertical = (orientation == "v" || orientation == "vertical");

    // Get ID references for bar pieces and thumb images
    std::string barLeftID    = getAttr(elem, "barleft", "");
    std::string barMiddleID  = getAttr(elem, "barmiddle", "");
    std::string barRightID   = getAttr(elem, "barright", "");
    std::string thumbID      = getAttr(elem, "thumb", "");
    std::string hoverThumbID = getAttr(elem, "hoverthumb", "");
    std::string downThumbID  = getAttr(elem, "downthumb", "");

    // Get value range
    int low  = std::stoi(getAttr(elem, "low", "0"));
    int high = std::stoi(getAttr(elem, "high", "255"));

    // Compute element rect
    SDL_Rect rect = computeElementRectSDL(elem, parentX, parentY, parentW, parentH);

    // Calculate progress value from state (smooth slow rise)
    static float valueF = 0.0f;
    float increment = 0.25f; // adjust for speed
    valueF += increment;
    if (valueF > high) valueF = low;
    float progress = (valueF - low) / float(high - low);

    // Render background parts
    auto drawPart = [&](const std::string& id, int x, int y, int w, int h) {
        auto it = skin.bitmaps.find(id);
        if (it == skin.bitmaps.end()) return;
        SkinBitmap& bmp = it->second;
        SDL_Texture* tex = getOrLoadTexture(renderer, skin, bmp);
        SDL_FRect src = { static_cast<float>(bmp.x), static_cast<float>(bmp.y), static_cast<float>(bmp.w), static_cast<float>(bmp.h)};
        SDL_FRect dst = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(w), static_cast<float>(h)};
        SDL_RenderTexture(renderer, tex, &src, &dst);
    };

    if (isVertical) {
        int sectionH = 4;
        drawPart(barLeftID, rect.x, rect.y, rect.w, sectionH); // top cap
        drawPart(barMiddleID, rect.x, rect.y + sectionH, rect.w, rect.h - 2 * sectionH); // middle stretch
        drawPart(barRightID, rect.x, rect.y + rect.h - sectionH, rect.w, sectionH); // bottom cap
    } else {
        int sectionW = 4;
        drawPart(barLeftID, rect.x, rect.y, sectionW, rect.h); // left cap
        drawPart(barMiddleID, rect.x + sectionW, rect.y, rect.w - 2 * sectionW, rect.h); // middle stretch
        drawPart(barRightID, rect.x + rect.w - sectionW, rect.y, sectionW, rect.h); // right cap
    }

    // Render thumb (centered along progress line)
    auto thumbIt = skin.bitmaps.find(thumbID);
    if (thumbIt != skin.bitmaps.end()) {
        SkinBitmap& bmp = thumbIt->second;
        SDL_Texture* tex = getOrLoadTexture(renderer, skin, bmp);

        SDL_FRect src = { static_cast<float>(bmp.x), static_cast<float>(bmp.y), static_cast<float>(bmp.w), static_cast<float>(bmp.h)};
        SDL_FRect dst;

        if (isVertical) {
            int sliderY = rect.y + int(progress * (rect.h - bmp.h));
            dst = { static_cast<float>(rect.x) + (rect.w - bmp.w) / 2, static_cast<float>(sliderY), static_cast<float>(bmp.w), static_cast<float>(bmp.h)};
        } else {
            int sliderX = rect.x + int(progress * (rect.w - bmp.w));
            dst = { static_cast<float>(sliderX), static_cast<float>(rect.y) + (rect.h - bmp.h) / 2, static_cast<float>(bmp.w), static_cast<float>(bmp.h)};
        }

        SDL_RenderTexture(renderer, tex, &src, &dst);
    }

    return true;
}
