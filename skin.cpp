// skin.cpp
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
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
        result[attr->Name()] = attr->Value();
        attr = attr->Next();
    }
    return result;
}

std::unique_ptr<UIElement> parseUIElement(const XMLElement* elem) {
    std::string tag = elem->Name();

    // Handle xuitag remapping
    if (g_targetSkin && g_targetSkin->xuiTagMap.count(tag)) {
        std::string groupId = g_targetSkin->xuiTagMap[tag];

#if defined(__linux__)
        std::cout << "xuitag matched: " << tag << " → group id: " << groupId << "\n";
#else 
        std::cout << "xuitag matched: " << tag << " -> group id: " << groupId << "\n";         // the arrow is not rendered properly in windows so replace with something that looks like it
#endif

        auto ui = std::make_unique<UIElement>();
        ui->tag = "group";
        ui->attributes = collectAttributes(elem);
        ui->attributes["id"] = groupId;
        return ui;
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

    // Load actual image to fallback on dimensions if w/h not given
    if ((bmp.w <= 0 || bmp.h <= 0) && !bmp.file.empty()) {
        std::string fullPath = g_skinPath + bmp.file;
        SDL_Surface* surface = IMG_Load(fullPath.c_str());

        if (!surface) {
            SDL_Log("Could not find bitmap %s", fullPath.c_str());

            std::filesystem::path xmlDir = std::filesystem::path(xmlPath).parent_path();
            std::filesystem::path fallbackPath = xmlDir / bmp.file;

            std::cout << "DEBUG: Trying fallback in XML dir: " << fallbackPath << std::endl;
            surface = IMG_Load(fallbackPath.string().c_str());

            if (!surface) {
                std::string wasabiPath = std::filesystem::relative("freeform/xml/wasabi/" + bmp.file).string();
                SDL_Log("Trying fallback in freeform/xml/wasabi: %s", wasabiPath.c_str());
                surface = IMG_Load(wasabiPath.c_str());
            }
            if (!surface) {
                std::string wasabiPath = std::filesystem::relative("freeform/" + bmp.file).string();
                SDL_Log("Trying fallback in freeform: %s", wasabiPath.c_str());
                surface = IMG_Load(wasabiPath.c_str());
            }
            if (!surface) {
                SDL_Log("Fatal: Could not find bitmap file: %s", bmp.file.c_str());
            }
        }

        if (surface) {
            if (bmp.w <= 0) bmp.w = surface->w;
            if (bmp.h <= 0) bmp.h = surface->h;
            SDL_FreeSurface(surface);
        }
    }

    if (std::string(elem->Name()) == "bitmapfont") {
        bmp.isFont = true;
        elem->QueryIntAttribute("charwidth", &bmp.charWidth);
        elem->QueryIntAttribute("charheight", &bmp.charHeight);
        elem->QueryIntAttribute("hspacing", &bmp.hspacing);
        elem->QueryIntAttribute("vspacing", &bmp.vspacing);
    }

    g_targetSkin->bitmaps[bmp.id] = std::move(bmp);
}


void tryRegisterGroupDef(const XMLElement* elem) {
    if (!g_targetSkin) return;
    GroupDef group;
    group.id = elem->Attribute("id") ? elem->Attribute("id") : "";

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
    // Collect layout attributes
    layout->attributes = collectAttributes(elem);

    const XMLElement* child = elem->FirstChildElement();
    while (child) {
        layout->elements.push_back(parseUIElement(child));
        child = child->NextSiblingElement();
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

    g_targetSkin = nullptr;
    g_currentContainerId.clear();
    g_pendingLayouts.clear();
    return result;
}

void registerElementHook(const XMLElement* elem, const std::string& xmlPath) {
    if (!elem || !g_targetSkin) return;
    std::string tag = elem->Name();
    if (tag == "bitmap" || tag == "bitmapfont") {
        tryRegisterBitmap(elem, xmlPath);
    } else if (tag == "groupdef") {
        tryRegisterGroupDef(elem);
    } else if (tag == "layout") {
        tryRegisterLayout(elem);
    } else if (tag == "container") {
        tryRegisterContainer(elem);
    }
}
