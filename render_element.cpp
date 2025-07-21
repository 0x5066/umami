
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
    renderRegistry["NStatesbutton"] = renderLayer;
    renderRegistry["Wasabi:Frame"] = renderFrame;
    renderRegistry["AnimatedLayer"] = renderLayer; 
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
    if (!initialized) {
        initializeRenderRegistry();
        initialized = true;
    }
    if (elem.tag == "script") return false;
    if (elem.tag == "sendparams") return false;

    /* std::cout << "Rendering element: " << elem.tag << " id: " << getAttr(elem, "id", "none") 
              << " at (" << x << ", " << y << ") with size (" << w << "x" << h << ")\n"; */
    auto it = renderRegistry.find(elem.tag);
    if (it != renderRegistry.end()) {
        return it->second(renderer, skin, elem, x, y, w, h);
    }
    return false;
}
