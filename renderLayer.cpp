#include "skin.h"
#include "render_shared.h"

// Renders a "layer" (image) at x, y, width, height relative to parent
// stub for button, togglebutton, NStatesbutton, AnimatedLayer
bool renderLayer(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH) {
    // Log input parameters
    /* SDL_Log("[renderLayer] tag=%s id=%s parent=(%d,%d,%d,%d)", 
        elem.tag.c_str(),
        elem.attributes.count("id") ? elem.attributes.at("id").c_str() : "(none)",
        parentX, parentY, parentW, parentH); */

    // Extract and log all relevant attributes
    auto get = [&](const char* key) -> const char* {
        auto it = elem.attributes.find(key);
        return it != elem.attributes.end() ? it->second.c_str() : "(none)";
    };
    /* SDL_Log("[renderLayer] attrs: x=%s y=%s w=%s h=%s relatx=%s relaty=%s relatw=%s relath=%s fitparent=%s image=%s",
        get("x"), get("y"), get("w"), get("h"),
        get("relatx"), get("relaty"), get("relatw"), get("relath"),
        get("fitparent"), get("image")); */

    // Existing code for computing x, y, w, h
    int x = 0, y = 0, w = parentW, h = parentH;
    if (elem.attributes.count("x")) x = std::stoi(elem.attributes.at("x"));
    if (elem.attributes.count("y")) y = std::stoi(elem.attributes.at("y"));
    if (elem.attributes.count("w")) w = std::stoi(elem.attributes.at("w"));
    if (elem.attributes.count("h")) h = std::stoi(elem.attributes.at("h"));
    if (elem.attributes.count("relatx") && elem.attributes.at("relatx") == "1") x = parentW + x;
    if (elem.attributes.count("relaty") && elem.attributes.at("relaty") == "1") y = parentH + y;
    if (elem.attributes.count("relatw") && elem.attributes.at("relatw") == "1") w = parentW + w;
    if (elem.attributes.count("relath") && elem.attributes.at("relath") == "1") h = parentH + h;

    // SDL_Log("[renderLayer] computed: x=%d y=%d w=%d h=%d (final rect: %d,%d,%d,%d)", x, y, w, h, parentX + x, parentY + y, w, h);

    auto it_img = elem.attributes.find("image");
    if (it_img == elem.attributes.end()) return false;
    const std::string& imageId = it_img->second;

    auto bmpIt = skin.bitmaps.find(imageId);
    if (bmpIt == skin.bitmaps.end()) return false;
    const SkinBitmap& bmp = bmpIt->second;

    std::string fullPath = g_skinPath + bmp.file;
    SDL_Surface* surface = IMG_Load(fullPath.c_str());
    if (!surface){
        SDL_Log("Could not find file %s", fullPath.c_str());
        return false;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture){
        SDL_Log("Something failed here, but I'm not sure what.");
        return false;
    }

    int relatx = elem.attributes.count("relatx") ? std::stoi(elem.attributes.at("relatx")) : 0;
    int relaty = elem.attributes.count("relaty") ? std::stoi(elem.attributes.at("relaty")) : 0;
    int relatw = elem.attributes.count("relatw") ? std::stoi(elem.attributes.at("relatw")) : 0;
    int relath = elem.attributes.count("relath") ? std::stoi(elem.attributes.at("relath")) : 0;

    x = compute_dimension(elem.attributes.count("x") ? elem.attributes.at("x") : "0", relatx, parentW);
    y = compute_dimension(elem.attributes.count("y") ? elem.attributes.at("y") : "0", relaty, parentH);

    w = bmp.w;
    if (elem.attributes.count("w")) {
        w = compute_dimension(elem.attributes.at("w"), relatw, parentW);
    } else if (relatw == 2) {
        w = compute_dimension(std::to_string(bmp.w), relatw, parentW);
    }

    h = bmp.h;
    if (elem.attributes.count("h")) {
        h = compute_dimension(elem.attributes.at("h"), relath, parentH);
    } else if (relath == 2) {
        h = compute_dimension(std::to_string(bmp.h), relath, parentH);
    }

    SDL_Rect src = { bmp.x, bmp.y, bmp.w, bmp.h };
    SDL_Rect dst = { parentX + x, parentY + y, w, h }; // <<--- add parent offset!
    SDL_RenderCopy(renderer, texture, &src, &dst);
    SDL_DestroyTexture(texture);

    return true;
}
