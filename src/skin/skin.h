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
#include "../../tinyxml2/tinyxml2.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

using namespace tinyxml2;

extern std::string g_skinPath;
extern char *plugdir_raw;
extern std::string plugdir;
#if defined(__LINUX__)
extern std::vector<double> sample; // temp
#endif
extern char sample[75 * 2]; 
extern void shift_vector_to_right(std::vector<double>& vec);

// Utility to get attribute with fallback
template<typename ElemType>
std::string getAttr(const ElemType& elem, const std::string& key, const std::string& fallback) {
    auto it = elem.attributes.find(key);
    return it != elem.attributes.end() ? it->second : fallback;
}

// Overload for tinyxml2::XMLElement&
inline std::string getAttr(const tinyxml2::XMLElement& elem, const std::string& key, const std::string& fallback) {
    const char* val = elem.Attribute(key.c_str());
    return val ? val : fallback;
}

// Overload for tinyxml2::XMLElement*
inline std::string getAttr(const tinyxml2::XMLElement* elem, const std::string& key, const std::string& fallback) {
    if (!elem) return fallback;
    const char* val = elem->Attribute(key.c_str());
    return val ? val : fallback;
}

// Represents a bitmap or bitmapfont resource
struct SkinBitmap {
    std::string id; // unique id, e.g., "player.button.play"
    std::string file; // file path relative to skin
    int x = 0, y = 0, w = 0, h = 0; // cropping rectangle
    std::string gammaGroup;

    std::unordered_map<std::string, std::string> attributes; // additional attributes from XML

    // font-specific
    bool isFont = false;
    int charWidth = 0, charHeight = 0;
    int hspacing = 0, vspacing = 0;

    bool triedLoading = false;
    bool loadFailed = false;

    SDL_Texture* texture = nullptr;
};

// Generic UI element, e.g., <button>, etc.
struct UIElement {
    std::string name;
    std::string tag;
    std::unordered_map<std::string, std::string> attributes;
    std::vector<std::unique_ptr<UIElement>> children;
    bool syntheticId = false;
};

// Represents a <layout> block
struct Layout {
    std::string id;
    std::unordered_map<std::string, std::string> attributes;
    std::vector<std::unique_ptr<UIElement>> elements;
    std::unordered_map<std::string, UIElement*> idMap; 
};

// Represents a reusable groupdef definition
struct GroupDef {
    std::string id;
    std::string xuitag;
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

    void clearVisStates();
    void unload();
};
