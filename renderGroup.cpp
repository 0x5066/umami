#include "skin.h"
#include "render_shared.h"

// renders groups and groups of groups (doesnt clip correctly)
bool renderGroup(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH) {
    SDL_Rect rect = computeElementRect(elem, parentX, parentY, parentW, parentH);
    SDL_RenderSetClipRect(renderer, &rect);
#ifdef DEBUG
    SDL_Log("renderGroup: elem id='%s' parent=(%d,%d,%d,%d) rect=(%d,%d,%d,%d)",
        elem.attributes.count("id") ? elem.attributes.at("id").c_str() : "",
        parentX, parentY, parentW, parentH,
        rect.x, rect.y, rect.w, rect.h);
#endif

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
                    SkinBitmap& bmp = bmpIt->second;
                    //std::string fullPath = g_skinPath + bmp.file;
                    SDL_Texture* texture = getOrLoadTexture(renderer, skin, bmp);
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

    SDL_RenderSetClipRect(renderer, nullptr);
    return true;
}

// wasabi:frame rendering, a bit wonky and i'm not sure what's at fault right now
bool renderFrame(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH) {
    SDL_Rect frameRect = computeElementRect(elem, parentX, parentY, parentW, parentH);
    SDL_RenderSetClipRect(renderer, &frameRect);

    std::string orientation = getAttr(elem, "orientation", "v");
    std::string from = getAttr(elem, "from", "l");

    int split; // split position (width or height depending on orientation)
    int minsplit, maxsplit;
    if (orientation == "v") {
        int width     = std::stoi(getAttr(elem, "width", "200"));
        int minwidth  = std::stoi(getAttr(elem, "minwidth", "0"));
        int maxwidth  = std::stoi(getAttr(elem, "maxwidth", "9999"));
        int frameW = frameRect.w;
        if (width < 0) width = frameW + width;
        if (maxwidth < 0) maxwidth = frameW + maxwidth;
        if (minwidth < 0) minwidth = frameW + minwidth;
        split = std::min(std::max(width, minwidth), maxwidth);
        split = std::max(0, std::min(split, frameW));
    } else {
        int height     = std::stoi(getAttr(elem, "width", "500")); // 'width' attribute controls height for horizontal orientation
        int minheight  = std::stoi(getAttr(elem, "minwidth", "0"));
        int maxheight  = std::stoi(getAttr(elem, "maxwidth", "9999"));
        int frameH = frameRect.h;
        if (height < 0) height = frameH + height;
        if (maxheight < 0) maxheight = frameH + maxheight;
        if (minheight < 0) minheight = frameH + minheight;
        split = std::min(std::max(height, minheight), maxheight);
        split = std::max(0, std::min(split, frameH));
    }

    std::string vbitmapId   = getAttr(elem, "vbitmap", "");
    std::string vgrabberId  = getAttr(elem, "vgrabber", "");

    std::string firstId, secondId;
    if (orientation == "v") {
        firstId  = getAttr(elem, "left", "");
        secondId = getAttr(elem, "right", "");
    } else {
        firstId  = getAttr(elem, "top", "");
        secondId = getAttr(elem, "bottom", "");
    }

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

    // --- Draw divider bitmaps if defined ---
    SkinBitmap* vbitmap = nullptr;
    SkinBitmap* vgrabber = nullptr;

    if (!vbitmapId.empty()) {
        auto it = skin.bitmaps.find(vbitmapId);
        if (it != skin.bitmaps.end()) {
            vbitmap = &it->second;
        }
    }
    if (!vgrabberId.empty()) {
        auto it = skin.bitmaps.find(vgrabberId);
        if (it != skin.bitmaps.end()) {
            vgrabber = &it->second;
        }
    }

    if (vbitmap) {
        SDL_Texture* tex = getOrLoadTexture(renderer, skin, *vbitmap);
        if (tex) {
            if (orientation == "v") {
                SDL_Rect src = { vbitmap->x, vbitmap->y, vbitmap->w, vbitmap->h };
                SDL_Rect dst = { frameRect.x + split, frameRect.y, vbitmap->w, frameRect.h };
                SDL_RenderCopy(renderer, tex, &src, &dst);
            } else {
                SDL_Rect src = { vbitmap->x, vbitmap->y, vbitmap->w, vbitmap->h };
                SDL_Rect dst = { frameRect.x, frameRect.y + split, frameRect.w, vbitmap->h };
                SDL_RenderCopy(renderer, tex, &src, &dst);
            }
        }
    }

    if (vgrabber) {
        SDL_Texture* tex = getOrLoadTexture(renderer, skin, *vgrabber);
        if (tex) {
            if (orientation == "v") {
                int centerY = frameRect.y + (frameRect.h - vgrabber->h) / 2;
                SDL_Rect src = { vgrabber->x, vgrabber->y, vgrabber->w, vgrabber->h };
                SDL_Rect dst = { frameRect.x + split, centerY, vgrabber->w, vgrabber->h };
                SDL_RenderCopy(renderer, tex, &src, &dst);
            } else {
                int centerX = frameRect.x + (frameRect.w - vgrabber->w) / 2;
                SDL_Rect src = { vgrabber->x, vgrabber->y, vgrabber->w, vgrabber->h };
                SDL_Rect dst = { centerX, frameRect.y + split, vgrabber->w, vgrabber->h };
                SDL_RenderCopy(renderer, tex, &src, &dst);
            }
        }
    }
    SDL_RenderSetClipRect(renderer, nullptr);
    return true;
}
