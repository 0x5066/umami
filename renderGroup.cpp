#include "skin.h"
#include "render_shared.h"

// renders groups and groups of groups (doesnt clip correctly)
bool renderGroup(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH) {
    // Calculate this element's rectangle relative to parent
    SDL_Rect rect = computeElementRectSDL(elem, parentX, parentY, parentW, parentH);
    SDL_SetRenderClipRect(renderer, &rect);
#ifdef DEBUG
    SDL_Log("renderGroup: elem id='%s' parent=(%d,%d,%d,%d) rect=(%d,%d,%d,%d)",
        elem.attributes.count("id") ? elem.attributes.at("id").c_str() : "",
        parentX, parentY, parentW, parentH,
        rect.x, rect.y, rect.w, rect.h);
#endif

    // Render background bitmap if group has one
    std::string groupId = elem.attributes.count("id") ? elem.attributes.at("id") : "";
    if (!groupId.empty()) {
        auto it = skin.groupDefs.find(groupId);
        if (it != skin.groupDefs.end()) {
            const GroupDef& def = it->second;
            if (def.attributes.count("background")) {
                const std::string& bgId = def.attributes.at("background");
                auto bmpIt = skin.bitmaps.find(bgId);
                if (bmpIt != skin.bitmaps.end()) {
                    const SkinBitmap& bmp = bmpIt->second;
                    std::string fullPath = g_skinPath + bmp.file;
                    SDL_Surface* surface = IMG_Load(fullPath.c_str());
                    if (!surface) {
                        #ifdef DEBUG
                        SDL_Log("Could not find file %s - using fallback", fullPath.c_str());
                        #endif // DEBUG
                        std::string wasabiPath = "freeform/xml/wasabi/" + bmp.file;
                        #ifdef DEBUG
                        std::cout << "DEBUG: new fallback: " << wasabiPath << std::endl;
                        #endif // DEBUG
                        surface = IMG_Load(wasabiPath.c_str());
                        if (!surface) {
                            #ifdef DEBUG
                            SDL_Log("Could not find file in fallback %s", wasabiPath.c_str());
                            #endif // DEBUG
                            SDL_SetRenderClipRect(renderer, nullptr);
                            return false;
                        }
                    }
                    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                    SDL_DestroySurface(surface);
                    if (texture) {
                        SDL_FRect src = { bmp.x, bmp.y, bmp.w, bmp.h };
                        SDL_FRect dst = { float(rect.x), float(rect.y), float(rect.w), float(rect.h) };
                        SDL_RenderTexture(renderer, texture, &src, &dst);
                        SDL_DestroyTexture(texture);
                    }
                }
            }
        }
    }

    /* old shit
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
    */
    // Render children of this UIElement (the cloned group elements)
    for (const auto& child : elem.children) {
        renderElement(renderer, skin, *child, rect.x, rect.y, rect.w, rect.h);
    }

    // Render "content" group elements if defined in attributes
    if (elem.attributes.count("content")) {
        std::string contentId = elem.attributes.at("content");
        // Try to find the groupDef for content and render its elements
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

    SDL_SetRenderClipRect(renderer, nullptr);
    return true;
}

// wasabi:frame rendering, improved to render cloned group children
bool renderFrame(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH) {
    SDL_Rect frameRect = computeElementRectSDL(elem, parentX, parentY, parentW, parentH);
    SDL_SetRenderClipRect(renderer, &frameRect);

    std::string orientation = getAttr(elem, "orientation", "v");

    int split = 0;
    if (orientation == "v") {
        int width     = std::stoi(getAttr(elem, "width", "200"));
        int minwidth  = std::stoi(getAttr(elem, "minwidth", "0"));
        int maxwidth  = std::stoi(getAttr(elem, "maxwidth", "500"));
        int frameW = frameRect.w;
        if (width < 0) width = frameW + width;
        if (minwidth < 0) minwidth = frameW + minwidth;
        if (maxwidth < 0) maxwidth = frameW + maxwidth;
        split = std::min(std::max(width, minwidth), maxwidth);
        split = std::max(0, std::min(split, frameW));
    } else {
        int height     = std::stoi(getAttr(elem, "height", "500"));
        int minheight  = std::stoi(getAttr(elem, "minheight", "0"));
        // what's the true behavior here?
        int maxheight  = !strcmp(getAttr(elem, "maxheight", "500").c_str(), "null") ? 500 : std::stoi(getAttr(elem, "maxheight", "500"));
        int frameH = frameRect.h;
        if (height < 0) height = frameH + height;
        if (minheight < 0) minheight = frameH + minheight;
        if (maxheight < 0) maxheight = frameH + maxheight;
        split = std::min(std::max(height, minheight), maxheight);
        split = std::max(0, std::min(split, frameH));
    }

    // Get child IDs for first and second halves
    std::string firstId, secondId;
    if (orientation == "v") {
        firstId  = getAttr(elem, "left", "");
        secondId = getAttr(elem, "right", "");
    } else {
        firstId  = getAttr(elem, "top", "");
        secondId = getAttr(elem, "bottom", "");
    }

    // Define rectangles for each half
    SDL_FRect firstRect, secondRect;
    if (orientation == "v") {
        firstRect  = { float(frameRect.x), float(frameRect.y), float(split), float(frameRect.h) };
        secondRect = { float(frameRect.x + split), float(frameRect.y), float(frameRect.w - split), float(frameRect.h) };
    } else {
        firstRect  = { float(frameRect.x), float(frameRect.y), float(frameRect.w), float(split) };
        secondRect = { float(frameRect.x), float(frameRect.y + split), float(frameRect.w), float(frameRect.h - split) };
    }

    auto renderGroupById = [&](const std::string& id, SDL_FRect area) {
        if (id.empty()) return;

        // Search for UIElement child with id in this frame element's children
        for (const auto& child : elem.children) {
            if (child->attributes.count("id") && child->attributes.at("id") == id) {
                renderElement(renderer, skin, *child, int(area.x), int(area.y), int(area.w), int(area.h));
                return;
            }
        }

        // Fallback: render from global groupDefs
        // this needs to be reworked
        auto it = skin.groupDefs.find(id);
        if (it != skin.groupDefs.end()) {
            for (const auto& child : it->second.elements) {
                renderElement(renderer, skin, *child, int(area.x), int(area.y), int(area.w), int(area.h));
            }
        }
    };

    renderGroupById(firstId, firstRect);
    renderGroupById(secondId, secondRect);

    // Draw divider bitmaps (vbitmap, vgrabber)
    auto getBitmap = [&](const std::string& bmpId) -> SkinBitmap* {
        if (bmpId.empty()) return nullptr;
        auto it = skin.bitmaps.find(bmpId);
        return it != skin.bitmaps.end() ? &it->second : nullptr;
    };

    SkinBitmap* vbitmap = getBitmap(getAttr(elem, "vbitmap", ""));
    SkinBitmap* vgrabber = getBitmap(getAttr(elem, "vgrabber", ""));

    if (vbitmap) {
        SDL_Texture* tex = getOrLoadTexture(renderer, skin, *vbitmap);
        if (tex) {
            if (orientation == "v") {
                SDL_FRect src = { float(vbitmap->x), float(vbitmap->y), float(vbitmap->w), float(vbitmap->h) };
                SDL_FRect dst = { float(frameRect.x + split), float(frameRect.y), float(vbitmap->w), float(frameRect.h) };
                SDL_RenderTexture(renderer, tex, &src, &dst);
            } else {
                SDL_FRect src = { float(vbitmap->x), float(vbitmap->y), float(vbitmap->w), float(vbitmap->h) };
                SDL_FRect dst = { float(frameRect.x), float(frameRect.y + split), float(frameRect.w), float(vbitmap->h) };
                SDL_RenderTexture(renderer, tex, &src, &dst);
            }
        }
    }

    if (vgrabber) {
        SDL_Texture* tex = getOrLoadTexture(renderer, skin, *vgrabber);
        if (tex) {
            if (orientation == "v") {
                int centerY = frameRect.y + (frameRect.h - vgrabber->h) / 2;
                SDL_FRect src = { float(vgrabber->x), float(vgrabber->y), float(vgrabber->w), float(vgrabber->h) };
                SDL_FRect dst = { float(frameRect.x + split), float(centerY), float(vgrabber->w), float(vgrabber->h) };
                SDL_RenderTexture(renderer, tex, &src, &dst);
            } else {
                int centerX = frameRect.x + (frameRect.w - vgrabber->w) / 2;
                SDL_FRect src = { float(vgrabber->x), float(vgrabber->y), float(vgrabber->w), float(vgrabber->h) };
                SDL_FRect dst = { float(centerX), float(frameRect.y + split), float(vgrabber->w), float(vgrabber->h) };
                SDL_RenderTexture(renderer, tex, &src, &dst);
            }
        }
    }

    SDL_SetRenderClipRect(renderer, nullptr);
    return true;
}
