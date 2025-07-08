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
        result[attr->Name()] = attr->Value();
        attr = attr->Next();
    }
    return result;
}

std::unique_ptr<UIElement> parseUIElement(const XMLElement* elem) {
    std::string tag = elem->Name();

    if (tag == "group") {
        const char* ref = elem->Attribute("id");
        if (ref) {
            std::cout << "Referenced group: " << ref << "\n";
        }
    }
    if (tag == "elements") {
        int count = 0;
        const XMLElement* child = elem->FirstChildElement();
        while (child) {
            ++count;
            child = child->NextSiblingElement();
        }
        std::cout << "Found <elements> container with " << count << " item(s)\n";
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

void tryRegisterBitmap(const XMLElement* elem) {
    if (!g_targetSkin) return;
    SkinBitmap bmp;
    bmp.id = elem->Attribute("id") ? elem->Attribute("id") : "";
    bmp.file = elem->Attribute("file") ? elem->Attribute("file") : "";
    bmp.gammaGroup = elem->Attribute("gammagroup") ? elem->Attribute("gammagroup") : "";
    elem->QueryIntAttribute("x", &bmp.x);
    elem->QueryIntAttribute("y", &bmp.y);
    elem->QueryIntAttribute("w", &bmp.w);
    elem->QueryIntAttribute("h", &bmp.h);
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

void registerElementHook(const XMLElement* elem) {
    if (!elem || !g_targetSkin) return;
    std::string tag = elem->Name();
    if (tag == "bitmap" || tag == "bitmapfont") {
        tryRegisterBitmap(elem);
    } else if (tag == "groupdef") {
        tryRegisterGroupDef(elem);
    } else if (tag == "layout") {
        tryRegisterLayout(elem);
    } else if (tag == "container") {
        tryRegisterContainer(elem);
    }
}
