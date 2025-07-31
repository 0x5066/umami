#include "skin.h"
#include "render_shared.h"

bool renderStatus(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH) {
    std::string bitmapKey;
    /*switch (g_fakePlayerState) {
        case PlayerState::Playing: bitmapKey = getAttr(elem, "playBitmap", ""); break;
        case PlayerState::Paused:  bitmapKey = getAttr(elem, "pauseBitmap", ""); break;
        case PlayerState::Stopped: bitmapKey = getAttr(elem, "stopBitmap", ""); break;
    }*/

    bitmapKey = getAttr(elem, "playbitmap", "");

    if (bitmapKey.empty()) return false;

    auto bmpIt = skin.bitmaps.find(bitmapKey);
    if (bmpIt == skin.bitmaps.end()) return false;
    SkinBitmap& bmp = bmpIt->second;
    SDL_Texture* texture = getOrLoadTexture(renderer, skin, bmp);

    int x = 0, y = 0, w = parentW, h = parentH;
    if (elem.attributes.count("x")) x = std::stoi(elem.attributes.at("x"));
    if (elem.attributes.count("y")) y = std::stoi(elem.attributes.at("y"));
    if (elem.attributes.count("w")) w = std::stoi(elem.attributes.at("w"));
    if (elem.attributes.count("h")) h = std::stoi(elem.attributes.at("h"));
    if (elem.attributes.count("relatx") && elem.attributes.at("relatx") == "1") x = parentW + x;
    if (elem.attributes.count("relaty") && elem.attributes.at("relaty") == "1") y = parentH + y;
    if (elem.attributes.count("relatw") && elem.attributes.at("relatw") == "1") w = parentW + w;
    if (elem.attributes.count("relath") && elem.attributes.at("relath") == "1") h = parentH + h;

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
