// skin.cpp
#include "skin.h"
#include "wasabiparser.h"

Skin* g_targetSkin = nullptr;
Container* g_currentContainer = nullptr;
std::string g_currentContainerId;
std::unordered_map<std::string, std::vector<std::unique_ptr<Layout>>> g_pendingLayouts;

namespace {

std::unordered_map<std::string, std::string> collectAttributes(const XMLElement* elem) {
    std::unordered_map<std::string, std::string> result;
    const XMLAttribute* attr = elem->FirstAttribute();
    while (attr) {
        std::string key = attr->Name();
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        result[key] = attr->Value();
        attr = attr->Next();
    }
    return result;
}

std::unique_ptr<UIElement> cloneElement(const UIElement* src) {
    auto copy = std::make_unique<UIElement>();
    copy->tag = src->tag;
    copy->attributes = src->attributes;
    copy->syntheticId = src->syntheticId;

    for (const auto& child : src->children) {
        copy->children.push_back(cloneElement(child.get()));
    }

    return copy;
}

std::unique_ptr<UIElement> parseUIElement(const XMLElement* elem) {
    std::string tag = elem->Name();

    // Handle xuitag remapping
    if (g_targetSkin && g_targetSkin->xuiTagMap.count(tag)) {
        std::string groupId = g_targetSkin->xuiTagMap[tag];

        auto groupDefIt = g_targetSkin->groupDefs.find(groupId);
        if (groupDefIt == g_targetSkin->groupDefs.end()) {
            std::cerr << "Missing groupdef: " << groupId << "\n";
            return nullptr;
        }

        // Create a wrapper group
        auto wrapper = std::make_unique<UIElement>();
        wrapper->tag = "group";
        wrapper->attributes = collectAttributes(elem);

        if (wrapper->attributes.count("id") == 0) {
            wrapper->attributes["id"] = groupId;
            wrapper->syntheticId = true;
        }

        // Deep-copy groupdef contents into wrapper
        for (const auto& child : groupDefIt->second.elements) {
            wrapper->children.push_back(cloneElement(child.get()));
        }

        return wrapper;
    }

    auto ui = std::make_unique<UIElement>();
    ui->tag = tag;
    ui->attributes = collectAttributes(elem);

    const XMLElement* child = elem->FirstChildElement();
    while (child) {
        ui->children.push_back(parseUIElement(child));
        child = child->NextSiblingElement();
    }
    return ui;
}

void collectElementsById(UIElement* elem, std::unordered_map<std::string, UIElement*>& idMap) {
    // If the element has an ID, register it
    std::string id = getAttr(*elem, "id", "");
    if (!id.empty()) {
        idMap[id] = elem;
    }

    // Recurse into immediate children
    for (auto& child : elem->children) {
        collectElementsById(child.get(), idMap);
    }

    // If this element is a group referencing a groupdef, also collect its contents
    if (elem->tag == "group" && g_targetSkin) {
        std::string refId = getAttr(*elem, "id", "");
        auto groupIt = g_targetSkin->groupDefs.find(refId);
        if (groupIt != g_targetSkin->groupDefs.end()) {
            for (auto& groupElem : groupIt->second.elements) {
                collectElementsById(groupElem.get(), idMap);
            }
        }
    }
}

static void applyPostLayoutMutations(Skin& skin) {
    for (auto& [_, container] : skin.containers) {
        for (auto& layout : container.layouts) {
            std::unordered_map<std::string, UIElement*> idMap;

            // Recursively collect all elements with IDs
            for (auto& elem : layout->elements) {
                collectElementsById(elem.get(), idMap);
            }

            // First pass: sendparams
            for (auto& elem : layout->elements) {
                if (elem->tag != "sendparams") continue;
                std::string targets = getAttr(*elem, "target", "");
                std::stringstream ss(targets);
                std::string id;
                while (std::getline(ss, id, ';')) {
                    id.erase(std::remove_if(id.begin(), id.end(), ::isspace), id.end());
                    if (auto it = idMap.find(id); it != idMap.end()) {
                        UIElement* target = it->second;
                        for (const auto& [key, value] : elem->attributes) {
                            if (key != "target") {
                                target->attributes[key] = value;
                            }
                        }
                    }
                }
            }

            // Second pass: hideobject
            for (auto& elem : layout->elements) {
                if (elem->tag != "hideobject") continue;
                std::string targets = getAttr(*elem, "target", "");
                std::stringstream ss(targets);
                std::string id;
                while (std::getline(ss, id, ';')) {
                    id.erase(std::remove_if(id.begin(), id.end(), ::isspace), id.end());
                    if (auto it = idMap.find(id); it != idMap.end()) {
                        it->second->attributes["visible"] = "0";
                    }
                }
            }
        }
    }
}

void tryRegisterBitmap(const XMLElement* elem, const std::string& xmlPath) {
    if (!g_targetSkin) return;

    SkinBitmap bmp;
    bmp.id = elem->Attribute("id") ? elem->Attribute("id") : "";
    bmp.file = elem->Attribute("file") ? elem->Attribute("file") : "";
    bmp.gammaGroup = elem->Attribute("gammagroup") ? elem->Attribute("gammagroup") : "";
    elem->QueryIntAttribute("x", &bmp.x);
    elem->QueryIntAttribute("y", &bmp.y);
    elem->QueryIntAttribute("w", &bmp.w);
    elem->QueryIntAttribute("h", &bmp.h);

    // Set up the fallback paths
    std::vector<std::string> searchPaths;

    if (!bmp.file.empty()) {
        searchPaths.push_back(g_skinPath + bmp.file);

        std::filesystem::path xmlDir = std::filesystem::path(xmlPath).parent_path();
        searchPaths.push_back((xmlDir / bmp.file).string());

        searchPaths.push_back("freeform/xml/wasabi/" + bmp.file);
        searchPaths.push_back("freeform/" + bmp.file);
    }

    SDL_Surface* surface = nullptr;

    if (bmp.file != "$solid" && bmp.file != "$polygon" && bmp.file != "$gradient") {
        for (const auto& path : searchPaths) {
            surface = IMG_Load(path.c_str());
            if (surface) {
#ifdef DEBUG
                std::cout << "Loaded bitmap for '" << bmp.id << "' from: " << path << std::endl;
#endif
                break;
            } else {
#ifdef DEBUG
                std::cout << "Failed to load '" << bmp.file << "' from: " << path << std::endl;
#endif
            }
        }

        if (surface) {
            if (bmp.w <= 0) bmp.w = surface->w;
            if (bmp.h <= 0) bmp.h = surface->h;
            SDL_DestroySurface(surface);
        } else {
            SDL_Log("Fatal: Could not find bitmap file: %s", bmp.file.c_str());
        }
    }

    if (std::string(elem->Name()) == "bitmapfont") {
        bmp.isFont = true;
        elem->QueryIntAttribute("charwidth", &bmp.charWidth);
        elem->QueryIntAttribute("charheight", &bmp.charHeight);
        elem->QueryIntAttribute("hspacing", &bmp.hspacing);
        elem->QueryIntAttribute("vspacing", &bmp.vspacing);

        // handle alias indirection: bitmapfont file refers to another bitmap's ID
        auto aliasIt = g_targetSkin->bitmaps.find(bmp.file);
        if (aliasIt != g_targetSkin->bitmaps.end()) {
            const SkinBitmap& aliased = aliasIt->second;

            // only copy over bitmap metadata, not font metadata
            bmp.file = aliased.file;
            bmp.x = aliased.x;
            bmp.y = aliased.y;
            bmp.w = aliased.w;
            bmp.h = aliased.h;

            // also inherit texture load state and texture itself
            bmp.texture = aliased.texture;
            bmp.triedLoading = aliased.triedLoading;
            bmp.loadFailed = aliased.loadFailed;
        }
    }

    if (std::string(elem->Name()) == "truetypefont") {
        std::cout << "Info: Registering TTF font: " << bmp.id << " from file: " << bmp.file << std::endl;
        bmp.isFont = false; // TTF fonts are not bitmap fonts
    }

    if (bmp.file == "$solid") {
        if (const char* colorAttr = elem->Attribute("color")) {
            bmp.attributes["color"] = colorAttr; // Store for deferred use
            std::cout << "Info: Registering solid color bitmap: " << bmp.id << " with color: " << colorAttr << std::endl;
        }
    }

    if (bmp.file == "$polygon") {
        if (const char* pointsAttr = elem->Attribute("points")) {
            bmp.attributes["points"] = pointsAttr; // Store for deferred use
            std::cout << "Info: Registering $polygon color bitmap: " << bmp.id << " with points: " << pointsAttr << std::endl;
        }
    }

    if (bmp.file == "$gradient") {
        if (const char* pointsAttr = elem->Attribute("points")) {
            bmp.attributes["points"] = pointsAttr;
        }
        bmp.attributes["gradient_x1"] = getAttr(*elem, "gradient_x1", "0");
        bmp.attributes["gradient_y1"] = getAttr(*elem, "gradient_y1", "0");
        bmp.attributes["gradient_x2"] = getAttr(*elem, "gradient_x2", "0");
        bmp.attributes["gradient_y2"] = getAttr(*elem, "gradient_y2", "1");

        std::cout << "Info: Registering gradient bitmap: " << bmp.id
                << " from " << bmp.attributes["gradient_x1"] << "," << bmp.attributes["gradient_y1"]
                << " to " << bmp.attributes["gradient_x2"] << "," << bmp.attributes["gradient_y2"]
                << " with points: " << bmp.attributes["points"] << std::endl;
    }

    g_targetSkin->bitmaps[bmp.id] = std::move(bmp);
}

void tryRegisterGroupDef(const XMLElement* elem) {
    if (!g_targetSkin) return;
    GroupDef group;
    group.id = elem->Attribute("id") ? elem->Attribute("id") : "";
    group.attributes = collectAttributes(elem);

    // Check and register xuitag
    if (const char* xtag = elem->Attribute("xuitag")) {
        g_targetSkin->xuiTagMap[xtag] = group.id;
    }

    const XMLElement* child = elem->FirstChildElement();
    while (child) {
        group.elements.push_back(parseUIElement(child));
        child = child->NextSiblingElement();
    }

    g_targetSkin->groupDefs[group.id] = std::move(group);
}

void tryRegisterLayout(const XMLElement* elem) {
    if (!g_targetSkin) return;
    std::string targetId = g_currentContainerId;
    if (targetId.empty()) return;

    auto layout = std::make_unique<Layout>();
    layout->id = elem->Attribute("id") ? elem->Attribute("id") : "";
    layout->attributes = collectAttributes(elem);

    const XMLElement* child = elem->FirstChildElement();
    while (child) {
        layout->elements.push_back(parseUIElement(child));
        child = child->NextSiblingElement();
    }

    // Per-layout mutation logic here
    std::unordered_map<std::string, UIElement*> idMap;

    for (auto& elem : layout->elements) {
        collectElementsById(elem.get(), idMap);
    }

    for (auto& elem : layout->elements) {
        if (elem->tag == "sendparams") {
            std::string targets = getAttr(*elem, "target", "");
            std::stringstream ss(targets);
            std::string id;
            while (std::getline(ss, id, ';')) {
                id.erase(std::remove_if(id.begin(), id.end(), ::isspace), id.end());
                if (auto it = idMap.find(id); it != idMap.end()) {
                    for (const auto& [key, value] : elem->attributes) {
                        if (key != "target") {
                            it->second->attributes[key] = value;
                        }
                    }
                }
            }
        } else if (elem->tag == "hideobject") {
            std::string targets = getAttr(*elem, "target", "");
            std::stringstream ss(targets);
            std::string id;
            while (std::getline(ss, id, ';')) {
                id.erase(std::remove_if(id.begin(), id.end(), ::isspace), id.end());
                if (auto it = idMap.find(id); it != idMap.end()) {
                    it->second->attributes["visible"] = "0";
                }
            }
        }
    }

    g_pendingLayouts[targetId].push_back(std::move(layout));
}

void tryRegisterContainer(const XMLElement* elem) {
    if (!g_targetSkin) return;
    Container container;
    container.id = elem->Attribute("id") ? elem->Attribute("id") : "";
    g_currentContainerId = container.id;
    container.name = elem->Attribute("name") ? elem->Attribute("name") : "";
    elem->QueryIntAttribute("default_x", &container.defaultX);
    elem->QueryIntAttribute("default_y", &container.defaultY);
    int vis = 0;
    elem->QueryIntAttribute("default_visible", &vis);
    container.defaultVisible = vis != 0;
    // Collect container attributes
    container.attributes = collectAttributes(elem);

    g_targetSkin->containers[container.id] = std::move(container);

}

} // namespace

bool Skin::loadFromXML(const std::string& skinXmlPath) {
    g_targetSkin = this;
    g_currentContainerId.clear();
    g_pendingLayouts.clear();

    bool result = WALvalidator(skinXmlPath.c_str());

    // After parsing, attach pending layouts to containers
    for (auto& [id, layouts] : g_pendingLayouts) {
        auto it = containers.find(id);
        if (it != containers.end()) {
            for (auto& layout : layouts) {
                it->second.layouts.push_back(std::move(layout));
            }
        }
    }

    applyPostLayoutMutations(*this);

    g_targetSkin = nullptr;
    g_currentContainerId.clear();
    g_pendingLayouts.clear();
    return result;
}

void Skin::unload() {
    // Free all textures
    for (auto& [id, bmp] : bitmaps) {
        if (bmp.texture) {
            SDL_DestroyTexture(bmp.texture);
            bmp.texture = nullptr;
        }
    }

    bitmaps.clear();
    containers.clear();
    groupDefs.clear();
    globalElements.clear();
    xuiTagMap.clear();
    name.clear();
    version.clear();
    clearVisStates();
}

void registerElementHook(const XMLElement* elem, const std::string& xmlPath) {
    if (!elem || !g_targetSkin) return;
    std::string tag = elem->Name();
    if (tag == "bitmap" || tag == "bitmapfont" || tag == "truetypefont") {
        tryRegisterBitmap(elem, xmlPath);
    } else if (tag == "groupdef") {
        tryRegisterGroupDef(elem);
    } else if (tag == "layout") {
        tryRegisterLayout(elem);
    } else if (tag == "container") {
        tryRegisterContainer(elem);
    }
}
