#include "skin.h"
#include "render_shared.h"

// Renders a group of children at the given offset
bool renderGroup(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH) {
    SDL_Rect rect = computeElementRect(elem, parentX, parentY, parentW, parentH);
    if (!elem.attributes.count("id")) return false;
    const std::string& groupId = elem.attributes.at("id");

    auto it = skin.groupDefs.find(groupId);
    if (it == skin.groupDefs.end()) return false;

    const GroupDef& def = it->second;
    for (const auto& child : def.elements) {
        renderElement(renderer, skin, *child, rect.x, rect.y, rect.w, rect.h);
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
