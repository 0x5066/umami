
#include "skin.h"
#include "render_shared.h"
#include <unordered_map>
#include <string>
#include <functional>
#include <algorithm>

// Forward declarations and getAttr now in render_shared.h

// Registry must match new signature (x, y, w, h)
using RenderFn = std::function<bool(SDL_Renderer*, Skin&, const UIElement&, int, int, int, int)>;

static std::unordered_map<std::string, RenderFn> renderRegistry;

static void initializeRenderRegistry() {
    renderRegistry["layer"] = renderLayer;
    renderRegistry["button"] = renderLayer;
    renderRegistry["group"] = renderGroup;
    renderRegistry["togglebutton"] = renderLayer;
    renderRegistry["nstatesbutton"] = renderNStatesButton;
    renderRegistry["wasabi:frame"] = renderFrame;
    renderRegistry["animatedlayer"] = renderAnimatedLayer; 
    renderRegistry["grid"] = renderGrid;
    renderRegistry["progressgrid"] = renderProgressGrid;
    renderRegistry["status"] = renderStatus;
    renderRegistry["text"] = renderText;
    renderRegistry["vis"] = renderVis; 
    renderRegistry["slider"] = renderSlider; 
}

static bool initialized = false;
// Entrypoint: always takes x, y, w, h
bool renderElement(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int x, int y, int w, int h) {
    std::string tag = elem.tag;
    std::transform(tag.begin(), tag.end(), tag.begin(), ::tolower);
    if (!initialized) {
        initializeRenderRegistry();
        initialized = true;
    }
    if (tag == "script") return false;
    if (tag == "sendparams") return false;
    if (elem.attributes.count("visible") && elem.attributes.at("visible") == "0") return false;

    auto it = renderRegistry.find(tag);
    if (it != renderRegistry.end()) {
        return it->second(renderer, skin, elem, x, y, w, h);
    }
    return false;
}
