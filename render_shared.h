// Render function declarations for registry
#pragma once
#include "skin.h"
#include <string>
#include <SDL2/SDL.h>

// Utility to get attribute with fallback
template<typename ElemType>
std::string getAttr(const ElemType& elem, const std::string& key, const std::string& fallback) {
    auto it = elem.attributes.find(key);
    return it != elem.attributes.end() ? it->second : fallback;
}

// Explicit instantiation for UIElement
extern template std::string getAttr<UIElement>(const UIElement&, const std::string&, const std::string&);

// Computes the rectangle for a UI element
SDL_Rect computeElementRect(const UIElement& elem, int parentX, int parentY, int parentW, int parentH);

// Computes a dimension with relativity
int compute_dimension(const std::string& value, int relatMode, int parent);

// Entrypoint for rendering a UI element
bool renderElement(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int x, int y, int w, int h);

bool renderGrid(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH);
bool renderProgressGrid(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH);
bool renderGroup(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH);
bool renderFrame(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH);
bool renderStatus(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH);
bool renderText(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH);
bool renderLayer(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH);
bool renderVis(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH);
