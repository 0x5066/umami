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

std::unique_ptr<UIElement> parseUIElement(const XMLElement* elem) {
    std::string tag = elem->Name();

    // Handle xuitag remapping
    if (g_targetSkin && g_targetSkin->xuiTagMap.count(tag)) {
        std::string groupId = g_targetSkin->xuiTagMap[tag];

    #if defined(__linux__)
        std::cout << "xuitag matched: " << tag << " → group id: " << groupId << "\n";
    #else 
        std::cout << "xuitag matched: " << tag << " -> group id: " << groupId << "\n";
    #endif

        auto ui = std::make_unique<UIElement>();
        ui->tag = "group";
        ui->attributes = collectAttributes(elem);

        // Only inject "id" if the XML did not specify one
        if (ui->attributes.count("id") == 0) {
            ui->attributes["id"] = groupId;
            ui->syntheticId = true;
        }

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
        SDL_FreeSurface(surface);
    } else {
        SDL_Log("Fatal: Could not find bitmap file: %s", bmp.file.c_str());
    }

    if (std::string(elem->Name()) == "bitmapfont") {
        bmp.isFont = true;
        elem->QueryIntAttribute("charwidth", &bmp.charWidth);
        elem->QueryIntAttribute("charheight", &bmp.charHeight);
        elem->QueryIntAttribute("hspacing", &bmp.hspacing);
        elem->QueryIntAttribute("vspacing", &bmp.vspacing);
    }

    if (std::string(elem->Name()) == "truetypefont") {
        std::cout << "Info: Registering TTF font: " << bmp.id << " from file: " << bmp.file << std::endl;
        bmp.isFont = false; // TTF fonts are not bitmap fonts
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
