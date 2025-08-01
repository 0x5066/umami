#include "skin.h"
#include "render_shared.h"

// Renders a "layer" (image) at x, y, width, height relative to parent
// stub for button, togglebutton, NStatesbutton, AnimatedLayer
bool renderLayer(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH) {
    // Log input parameters
#ifdef DEBUG
    SDL_Log("[renderLayer] tag=%s id=%s parent=(%d,%d,%d,%d)", 
        elem.tag.c_str(),
        elem.attributes.count("id") ? elem.attributes.at("id").c_str() : "(none)",
        parentX, parentY, parentW, parentH);

    //Extract and log all relevant attributes
    auto get = [&](const char* key) -> const char* {
        auto it = elem.attributes.find(key);
        return it != elem.attributes.end() ? it->second.c_str() : "(none)";
    };
    SDL_Log("[renderLayer] attrs: x=%s y=%s w=%s h=%s relatx=%s relaty=%s relatw=%s relath=%s fitparent=%s image=%s",
        get("x"), get("y"), get("w"), get("h"),
        get("relatx"), get("relaty"), get("relatw"), get("relath"),
        get("fitparent"), get("image"));
#endif // DEBUG

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

    auto it_img = elem.attributes.find("image");
    if (it_img == elem.attributes.end()) return false;
    const std::string& imageId = it_img->second;

    auto bmpIt = skin.bitmaps.find(imageId);
    if (bmpIt == skin.bitmaps.end()) return false;
    SkinBitmap& bmp = bmpIt->second;
    SDL_Texture* texture = getOrLoadTexture(renderer, skin, bmp);

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

    SDL_FRect src = { static_cast<float>(bmp.x), static_cast<float>(bmp.y), static_cast<float>(bmp.w), static_cast<float>(bmp.h)};
    SDL_FRect dst = { static_cast<float>(parentX + x), static_cast<float>(parentY + y), static_cast<float>(w), static_cast<float>(h)}; // <<--- add parent offset!
    SDL_RenderTexture(renderer, texture, &src, &dst);
    //SDL_DestroyTexture(texture);

    return true;
}

// how does wasabi determine how many frames there are in the image?
// something like framewidth divided by the width of the first frame?
bool renderAnimatedLayer(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH) {
    // Existing code for computing x, y, w, h
    int x = 0, y = 0, w = parentW, h = parentH, animW, animH;
    if (elem.attributes.count("x")) x = std::stoi(elem.attributes.at("x"));
    if (elem.attributes.count("y")) y = std::stoi(elem.attributes.at("y"));
    if (elem.attributes.count("w")) w = std::stoi(elem.attributes.at("w"));
    if (elem.attributes.count("h")) h = std::stoi(elem.attributes.at("h"));
    if (elem.attributes.count("relatx") && elem.attributes.at("relatx") == "1") x = parentW + x;
    if (elem.attributes.count("relaty") && elem.attributes.at("relaty") == "1") y = parentH + y;
    if (elem.attributes.count("relatw") && elem.attributes.at("relatw") == "1") w = parentW + w;
    if (elem.attributes.count("relath") && elem.attributes.at("relath") == "1") h = parentH + h;
    if (elem.attributes.count("framewidth")) animW = std::stoi(elem.attributes.at("framewidth")) ? std::stoi(elem.attributes.at("framewidth")) : w; else animW = w;
    if (elem.attributes.count("frameheight")) animH = std::stoi(elem.attributes.at("frameheight")) ? std::stoi(elem.attributes.at("frameheight")) : h; else animH = h;

    auto it_img = elem.attributes.find("image");
    if (it_img == elem.attributes.end()) return false;
    const std::string& imageId = it_img->second;

    auto bmpIt = skin.bitmaps.find(imageId);
    if (bmpIt == skin.bitmaps.end()) return false;
    SkinBitmap& bmp = bmpIt->second;
    //std::string fullPath = g_skinPath + bmp.file;
    SDL_Texture* texture = getOrLoadTexture(renderer, skin, bmp);

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

    SDL_FRect src = { static_cast<float>(bmp.x), static_cast<float>(bmp.y), static_cast<float>(animW), static_cast<float>(animH)};
    SDL_FRect dst = { static_cast<float>(parentX + x), static_cast<float>(parentY + y), static_cast<float>(w), static_cast<float>(h)}; // <<--- add parent offset!
    SDL_RenderTexture(renderer, texture, &src, &dst);
    //SDL_DestroyTexture(texture);

    return true;
}

bool renderNStatesButton(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH) {
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
    const std::string& imageId = it_img->second + "0";

    auto bmpIt = skin.bitmaps.find(imageId);
    if (bmpIt == skin.bitmaps.end()) return false;
    SkinBitmap& bmp = bmpIt->second;
    //std::string fullPath = g_skinPath + bmp.file;
    SDL_Texture* texture = getOrLoadTexture(renderer, skin, bmp);

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

    SDL_FRect src = { static_cast<float>(bmp.x), static_cast<float>(bmp.y), static_cast<float>(bmp.w), static_cast<float>(bmp.h)};
    SDL_FRect dst = { static_cast<float>(parentX + x), static_cast<float>(parentY + y), static_cast<float>(w), static_cast<float>(h)}; // <<--- add parent offset!
    SDL_RenderTexture(renderer, texture, &src, &dst);
    //SDL_DestroyTexture(texture);

    return true;
}