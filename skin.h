// skin.h
#pragma once

#include <string>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <memory>
#include <optional>
#include <iostream>
#include <filesystem>
#include "tinyxml2/tinyxml2.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

using namespace tinyxml2;

extern std::string g_skinPath;
extern std::vector<double> sample; // temp

// Represents a bitmap or bitmapfont resource
struct SkinBitmap {
    std::string id; // unique id, e.g., "player.button.play"
    std::string file; // file path relative to skin
    int x = 0, y = 0, w = 0, h = 0; // cropping rectangle
    std::string gammaGroup;

    // font-specific
    bool isFont = false;
    int charWidth = 0, charHeight = 0;
    int hspacing = 0, vspacing = 0;

    SDL_Texture* texture = nullptr;
};

// Generic UI element, e.g., <button>, <sendparams>, etc.
struct UIElement {
    std::string tag; // element name
    std::unordered_map<std::string, std::string> attributes; // all XML attributes
    std::vector<std::unique_ptr<UIElement>> children; // for nested elements

    bool syntheticId = false;
};

// Represents a <layout> block
struct Layout {
    std::string id;
    std::unordered_map<std::string, std::string> attributes;
    std::vector<std::unique_ptr<UIElement>> elements;
};

// Represents a reusable groupdef definition
struct GroupDef {
    std::string id;
    std::unordered_map<std::string, std::string> attributes;
    std::vector<std::unique_ptr<UIElement>> elements;
};

// Represents a <container>
struct Container {
    std::string id;
    std::string name;
    int defaultX = 0, defaultY = 0;
    bool defaultVisible = false;
    std::unordered_map<std::string, std::string> attributes;
    std::vector<std::unique_ptr<Layout>> layouts;
};

// Forward declaration for element hook
void registerElementHook(const tinyxml2::XMLElement* elem, const std::string& xmlPath);

// Main skin object
class Skin {
public:
    std::unordered_map<std::string, SkinBitmap> bitmaps; // id -> bitmap
    std::unordered_map<std::string, Container> containers; // id -> container
    std::unordered_map<std::string, GroupDef> groupDefs; // id -> group definition

    std::vector<std::unique_ptr<UIElement>> globalElements; // optional raw UI elements
    
    std::unordered_map<std::string, std::string> xuiTagMap; // e.g. "Wasabi:MainFrame:NoStatus" → "wasabi.mainframe.nostatusbar"

    std::string name; // from skininfo
    std::string version; // from WAL root attribute

    // Loads a skin given a path to skin.xml
    bool loadFromXML(const std::string& skinXmlPath);
};
