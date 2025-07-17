#include "skin.h"
#include "render_shared.h"

// Renders a group of children at the given offset
bool renderGroup(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH) {
    SDL_Rect rect = computeElementRect(elem, parentX, parentY, parentW, parentH);

    // Try to render background bitmap if group has one
    std::string groupId = elem.attributes.count("id") ? elem.attributes.at("id") : "";
    if (!groupId.empty()) {
        auto it = skin.groupDefs.find(groupId);
        if (it != skin.groupDefs.end()) {
            const GroupDef& def = it->second;
            if (skin.groupDefs[groupId].attributes.count("background")) {
                const std::string& bgId = skin.groupDefs[groupId].attributes.at("background");
                auto bmpIt = skin.bitmaps.find(bgId);
                if (bmpIt != skin.bitmaps.end()) {
                    SDL_Surface* surface = nullptr;
                    const SkinBitmap& bmp = bmpIt->second;
                    std::string fullPath = g_skinPath + bmp.file;
                    surface = IMG_Load(fullPath.c_str());
                    if (!surface){ // try redirecting to freeform
                        #ifdef DEBUG
                        SDL_Log("Could not find file %s - using fallback", fullPath.c_str());
                        #endif // DEBUG
                        std::string wasabiPath = "freeform/xml/wasabi/" + bmp.file;
                        #ifdef DEBUG
                        std::cout << "DEBUG: new fallback: " << wasabiPath << std::endl;
                        #endif // DEBUG
                        surface = IMG_Load(wasabiPath.c_str());
                        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                        SDL_FreeSurface(surface);
                        if (texture) {
                            SDL_Rect src = { bmp.x, bmp.y, bmp.w, bmp.h };
                            SDL_Rect dst = { rect.x, rect.y, rect.w, rect.h };
                            SDL_RenderCopy(renderer, texture, &src, &dst);
                            SDL_DestroyTexture(texture);
                        }
                        if (!surface) {
                            #ifdef DEBUG
                            SDL_Log("Could not find file in fallback %s", wasabiPath.c_str());
                            #endif // DEBUG
                            return false;
                        }
                        if (!surface) {
                            #ifdef DEBUG
                            SDL_Log("FUCK ERROR: Could not find bitmap file: %s", bmp.file.c_str());
                            #endif // DEBUG
                        }
                    }
                }
            }
        }
    }

    // Render "id" group (main group)
    if (!groupId.empty()) {
        auto it = skin.groupDefs.find(groupId);
        if (it != skin.groupDefs.end()) {
            const GroupDef& def = it->second;
            for (const auto& child : def.elements) {
                renderElement(renderer, skin, *child, rect.x, rect.y, rect.w, rect.h);
            }
        } else {
            #ifdef DEBUG
            SDL_Log("renderGroup: group id='%s' not found", groupId.c_str());
            #endif // DEBUG
        }
    }

    // Render optional "content" group if defined
    if (elem.attributes.count("content")) {
        std::string contentId = elem.attributes.at("content");
        auto it = skin.groupDefs.find(contentId);
        if (it != skin.groupDefs.end()) {
            const GroupDef& def = it->second;
            for (const auto& child : def.elements) {
                renderElement(renderer, skin, *child, rect.x, rect.y, rect.w, rect.h);
            }
        } else {
            #ifdef DEBUG
            SDL_Log("renderGroup: content group id='%s' not found", contentId.c_str());
            #endif // DEBUG
        }
    }

    return true;
}

// Frame rendering, splits parent's rect and passes correct offsets to children
bool renderFrame(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH) {
    SDL_Rect frameRect = computeElementRect(elem, parentX, parentY, parentW, parentH);

    std::string orientation = getAttr(elem, "orientation", "v");
    std::string from = getAttr(elem, "from", "l");

    int width     = std::stoi(getAttr(elem, "width", "100"));
    int minwidth  = std::stoi(getAttr(elem, "minwidth", "0"));
    int maxwidth  = std::stoi(getAttr(elem, "maxwidth", "9999"));

    std::string firstId, secondId;
    if (orientation == "v") {
        firstId  = getAttr(elem, "left", "");
        secondId = getAttr(elem, "right", "");
    } else {
        firstId  = getAttr(elem, "top", "");
        secondId = getAttr(elem, "bottom", "");
    }

    int split = std::min(std::max(width, std::min(minwidth, maxwidth)), std::max(minwidth, maxwidth));

    SDL_Rect firstRect, secondRect;
    if (orientation == "v") {
        firstRect  = { frameRect.x, frameRect.y, split, frameRect.h };
        secondRect = { frameRect.x + split, frameRect.y, frameRect.w - split, frameRect.h };
    } else {
        firstRect  = { frameRect.x, frameRect.y, frameRect.w, split };
        secondRect = { frameRect.x, frameRect.y + split, frameRect.w, frameRect.h - split };
    }

    auto renderGroupById = [&](const std::string& id, SDL_Rect area) {
        auto it = skin.groupDefs.find(id);
        if (it == skin.groupDefs.end()) return;
        for (const auto& child : it->second.elements) {
            renderElement(renderer, skin, *child, area.x, area.y, area.w, area.h);
        }
    };

    renderGroupById(firstId, firstRect);
    renderGroupById(secondId, secondRect);

    // TODO: vgrabber / vbitmap rendering here

    return true;
}
