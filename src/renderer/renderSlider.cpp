#include "../skin/skin.h"
#include "render_shared.h"

static float valueF = 0.0f;
float progress = 0.0f;

extern PlayerCore* PlayerCore;

// Render a slider element (horizontal or vertical)
// whenever there is more than one slider object in a skin (as is usually the case), the calculations inside "speed up"
// need to find a way to prevent it from happening
bool renderSlider(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH) {
    // Determine orientation
    std::string orientation = getAttr(elem, "orientation", "h");
    bool isVertical = (orientation == "v" || orientation == "vertical");

    // Get ID references for bar pieces and thumb images, and more!
    std::string barLeftID    = getAttr(elem, "barleft", "");
    std::string barMiddleID  = getAttr(elem, "barmiddle", "");
    std::string barRightID   = getAttr(elem, "barright", "");
    std::string thumbID      = getAttr(elem, "thumb", "");
    std::string hoverThumbID = getAttr(elem, "hoverthumb", "");
    std::string downThumbID  = getAttr(elem, "downthumb", "");
    std::string wasabi_action = getAttr(elem, "action", "");
    std::string wasabi_param = getAttr(elem, "param", "");
    std::transform(wasabi_action.begin(), wasabi_action.end(), wasabi_action.begin(), ::tolower);
    std::transform(wasabi_param.begin(), wasabi_param.end(), wasabi_param.begin(), ::tolower);

    // Get value range
    int low  = std::stoi(getAttr(elem, "low", "0"));
    int high = std::stoi(getAttr(elem, "high", "255"));

    // Compute element rect
    SDL_Rect rect = computeElementRectSDL(elem, parentX, parentY, parentW, parentH);

    switch (wasabi_action[0]) {
        case 's': // seek
        {
            progress = ((float(PlayerCore->getOutputTime()) / PlayerCore->getLength()) * 255.f - low) / float(high - low);
            break;
        }
        case 'v': // volume
        {
            progress = (PlayerCore->getCurVolume() - low) / float(high - low);
            break;
        }
        case 'p': // pan
        {
            static float balanceF = 128.f;
            progress = (balanceF - low) / float(high - low);
            break;
        }
        case 'e': // EQ_PREAMP, because winamp3 was quirky like that
        {
            static float eqPreampF = 128.f;
            progress = (eqPreampF - low) / float(high - low);
            break;
        }
    default:
        progress = 1.0f; // default to 128 if no action matches
        break;
    }

    switch (wasabi_param[0]) {
        case 'p': // preamp
        {
            static float preampF = 128.0f;
            progress = (preampF - low) / float(high - low);
            break;
        }
        case '0': // first eq band
        {
            static float eq0F = 128.0f;
            progress = (eq0F - low) / float(high - low);
            break;
        }
        case '1': // second eq band
        {
            static float eq1F = 128.0f;
            progress = (eq1F - low) / float(high - low);
            break;
        }
        case '2': // third eq band
        {
            static float eq2F = 128.0f;
            progress = (eq2F - low) / float(high - low);
            break;
        }
        case '3': // fourth eq band
        {
            static float eq3F = 128.0f;
            progress = (eq3F - low) / float(high - low);
            break;
        }
        case '4': // fifth eq band
        {
            static float eq4F = 128.0f;
            progress = (eq4F - low) / float(high - low);
            break;
        }
        case '5': // sixth eq band
        {
            static float eq5F = 128.0f;
            progress = (eq5F - low) / float(high - low);
            break;
        }
        case '6': // seventh eq band
        {
            static float eq6F = 128.0f;
            progress = (eq6F - low) / float(high - low);
            break;
        }
        case '7': // eighth eq band
        {
            static float eq7F = 128.0f;
            progress = (eq7F - low) / float(high - low);
            break;
        }
        case '8': // ninth eq band
        {
            static float eq8F = 128.0f;
            progress = (eq8F - low) / float(high - low);
            break;
        }
        case '9': // tenth eq band
        {
            static float eq9F = 128.0f;
            progress = (eq9F - low) / float(high - low);
            break;
        }
        case '10': // eleventh eq band
        {
            static float eq10F = 128.0f;
            progress = (eq10F - low) / float(high - low);
            break;
        }
    }

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

        if (wasabi_action == "seek" && (PlayerCore->isPlaying() == 1 || PlayerCore->isPlaying() == 3)) {
            SDL_RenderTexture(renderer, tex, &src, &dst);
        } else if (wasabi_action == "seek") {
            // SORRY NOTHING
        } else {
            SDL_RenderTexture(renderer, tex, &src, &dst);
        }
    }

    return true;
}
