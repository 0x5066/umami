#include "skin.h"
#include "render_shared.h"

SDL_Rect computeElementRect(const UIElement& elem, int parentX, int parentY, int parentW, int parentH) {
    int x = 0, y = 0, w = parentW, h = parentH;

    // fitparent: fill parent rect
    if (elem.attributes.count("fitparent") && elem.attributes.at("fitparent") == "1") {
        return {parentX, parentY, parentW, parentH};
    }

    // x/y/w/h with relativity
    if (elem.attributes.count("x")) x = std::stoi(elem.attributes.at("x"));
    if (elem.attributes.count("y")) y = std::stoi(elem.attributes.at("y"));
    if (elem.attributes.count("w")) w = std::stoi(elem.attributes.at("w"));
    if (elem.attributes.count("h")) h = std::stoi(elem.attributes.at("h"));

    if (elem.attributes.count("relatx") && elem.attributes.at("relatx") == "1") x = parentW + x;
    if (elem.attributes.count("relaty") && elem.attributes.at("relaty") == "1") y = parentH + y;
    if (elem.attributes.count("relatw") && elem.attributes.at("relatw") == "1") w = parentW + w;
    if (elem.attributes.count("relath") && elem.attributes.at("relath") == "1") h = parentH + h;

    // Clamp to parent bounds
    if (w < 0) w = 0;
    if (h < 0) h = 0;

    return {parentX + x, parentY + y, w, h};
}

// Utility for computing dimension
int compute_dimension(const std::string& value, int relatMode, int parent) {
    int val = std::stoi(value);
    switch (relatMode) {
        case 0: return val; // absolute
        case 1: return parent + val; // relative (offset from edge)
        case 2: return (val * parent) / 100; // percentage
        default: return val;
    }
}