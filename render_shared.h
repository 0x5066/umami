// Render function declarations for registry
#pragma once
#include "skin.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

// texture management
SDL_Texture* getOrLoadTexture(SDL_Renderer* renderer, Skin& skin, SkinBitmap& bmp);

// Explicit instantiation for UIElement
extern template std::string getAttr<UIElement>(const UIElement&, const std::string&, const std::string&);

// Computes the rectangle for a UI element
SDL_Rect computeElementRectSDL(const UIElement& elem, int parentX, int parentY, int parentW, int parentH);

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
bool renderNStatesButton(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH);
bool renderVis(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH);
bool renderSlider(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH);
bool renderAnimatedLayer(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH);