#include "layerparser.h"
#include "umamistructs.h"
#include <iostream>
#include <unordered_map>

std::unordered_map<std::string, std::vector<umamiLayer>> containerLayers;

void parseLayer(XMLElement* layerElement, const std::string& containerID, const std::string& groupID) {
#ifdef PRINTSTD
    std::cout << "parseLayer called for containerID: " << containerID << ", groupID: " << groupID << '\n';
#endif
    umamiLayer layer;
    layer.id = (layerElement->Attribute("id") != nullptr) ? layerElement->Attribute("id") : "";
    layer.x = (layerElement->Attribute("x") != nullptr) ? layerElement->Attribute("x") : "0";
    layer.y = (layerElement->Attribute("y") != nullptr) ? layerElement->Attribute("y") : "0";
    layer.width = (layerElement->Attribute("w") != nullptr) ? layerElement->Attribute("w") : "0";
    layer.height = (layerElement->Attribute("h") != nullptr) ? layerElement->Attribute("h") : "0";
    layer.alpha = (layerElement->Attribute("alpha") != nullptr) ? layerElement->Attribute("alpha") : "255";
    layer.activealpha = (layerElement->Attribute("activealpha") != nullptr) ? layerElement->Attribute("activealpha") : "";
    layer.inactivealpha = (layerElement->Attribute("inactivealpha") != nullptr) ? layerElement->Attribute("inactivealpha") : "";
    layer.cursor = (layerElement->Attribute("cursor") != nullptr) ? layerElement->Attribute("cursor") : "";
    layer.tooltip = (layerElement->Attribute("tooltip") != nullptr) ? layerElement->Attribute("tooltip") : "";
    layer.move = (layerElement->Attribute("move") != nullptr) ? layerElement->Attribute("move") : "";
    layer.renderbasetexture = (layerElement->Attribute("renderbasetexture") != nullptr) ? layerElement->Attribute("renderbasetexture") : "";
    layer.cfgattrib = (layerElement->Attribute("cfgattrib") != nullptr) ? layerElement->Attribute("cfgattrib") : "";
    layer.visible = (layerElement->Attribute("visible") != nullptr) ? layerElement->Attribute("visible") : "";
    layer.relatx = (layerElement->Attribute("relatx") != nullptr) ? layerElement->Attribute("relatx") : "";
    layer.relaty = (layerElement->Attribute("relaty") != nullptr) ? layerElement->Attribute("relaty") : "";
    layer.relatw = (layerElement->Attribute("relatw") != nullptr) ? layerElement->Attribute("relatw") : "";
    layer.relath = (layerElement->Attribute("relath") != nullptr) ? layerElement->Attribute("relath") : "";
    layer.fitparent = (layerElement->Attribute("fitparent") != nullptr) ? layerElement->Attribute("fitparent") : "";
    layer.x1 = (layerElement->Attribute("x1") != nullptr) ? layerElement->Attribute("x1") : "";
    layer.y1 = (layerElement->Attribute("y1") != nullptr) ? layerElement->Attribute("y1") : "";
    layer.x2 = (layerElement->Attribute("x2") != nullptr) ? layerElement->Attribute("x2") : "";
    layer.y2 = (layerElement->Attribute("y2") != nullptr) ? layerElement->Attribute("y2") : "";
    layer.anchor = (layerElement->Attribute("anchor") != nullptr) ? layerElement->Attribute("anchor") : "";
    layer.sysmetricsx = (layerElement->Attribute("sysmetricsx") != nullptr) ? layerElement->Attribute("sysmetricsx") : "";
    layer.sysmetricsy = (layerElement->Attribute("sysmetricsy") != nullptr) ? layerElement->Attribute("sysmetricsy") : "";
    layer.sysmetricsw = (layerElement->Attribute("sysmetricsw") != nullptr) ? layerElement->Attribute("sysmetricsw") : "";
    layer.sysmetricsh = (layerElement->Attribute("sysmetricsh") != nullptr) ? layerElement->Attribute("sysmetricsh") : "";
    layer.rectrgn = (layerElement->Attribute("rectrgn") != nullptr) ? layerElement->Attribute("rectrgn") : "";
    layer.regionop = (layerElement->Attribute("regionop") != nullptr) ? layerElement->Attribute("regionop") : "";
    layer.wantfocus = (layerElement->Attribute("wantfocus") != nullptr) ? layerElement->Attribute("wantfocus") : "";
    layer.focusonclick = (layerElement->Attribute("focusonclick") != nullptr) ? layerElement->Attribute("focusonclick") : "";
    layer.taborder = (layerElement->Attribute("taborder") != nullptr) ? layerElement->Attribute("taborder") : "";
    layer.nodblclick = (layerElement->Attribute("nodblclick") != nullptr) ? layerElement->Attribute("nodblclick") : "";
    layer.noleftclick = (layerElement->Attribute("noleftclick") != nullptr) ? layerElement->Attribute("noleftclick") : "";
    layer.norightclick = (layerElement->Attribute("norightclick") != nullptr) ? layerElement->Attribute("norightclick") : "";
    layer.nomousemove = (layerElement->Attribute("nomousemove") != nullptr) ? layerElement->Attribute("nomousemove") : "";
    layer.nocontextmenu = (layerElement->Attribute("nocontextmenu") != nullptr) ? layerElement->Attribute("nocontextmenu") : "";
    layer.ghost = (layerElement->Attribute("ghost") != nullptr) ? layerElement->Attribute("ghost") : "";
    for (int i = 0; i < 10; ++i) {
        std::string notifyAttr = "notify" + std::to_string(i);
        layer.notify[i] = (layerElement->Attribute(notifyAttr.c_str()) != nullptr) ? layerElement->Attribute(notifyAttr.c_str()) : "";
    }
    layer.droptarget = (layerElement->Attribute("droptarget") != nullptr) ? layerElement->Attribute("droptarget") : "";
    layer.image = (layerElement->Attribute("image") != nullptr) ? layerElement->Attribute("image") : "";
    layer.inactiveimage = (layerElement->Attribute("inactiveimage") != nullptr) ? layerElement->Attribute("inactiveimage") : "";
    layer.region = (layerElement->Attribute("region") != nullptr) ? layerElement->Attribute("region") : "";
    layer.tile = (layerElement->Attribute("tile") != nullptr) ? layerElement->Attribute("tile") : "";
    layer.resize = (layerElement->Attribute("resize") != nullptr) ? layerElement->Attribute("resize") : "";
    layer.scale = (layerElement->Attribute("scale") != nullptr) ? layerElement->Attribute("scale") : "";
    layer.dblclickaction = (layerElement->Attribute("dblclickaction") != nullptr) ? layerElement->Attribute("dblclickaction") : "";

    std::string key = containerID + ":" + groupID;
#ifdef PRINTSTD
    std::cout << "Layer key: " << key << '\n';
    std::cout << "Layer id: " << layer.id << " " << layer.x << " " << layer.y << " " << layer.width << " " << layer.height << " " << '\n';
#endif
    containerLayers[key].push_back(layer);
#ifdef PRINTSTD
    std::cout << "Added layer to containerLayers[" << key << "]\n";
#endif
}

void parseLayers(XMLElement* groupElement, const std::string& containerID) {
#ifdef PRINTSTD
    std::cout << "parseLayers called for containerID: " << containerID << '\n';
#endif
    for (XMLElement* layerElement = groupElement->FirstChildElement("layer"); layerElement != nullptr; layerElement = layerElement->NextSiblingElement("layer")) {
        std::string groupID = (groupElement->Attribute("id") != nullptr) ? groupElement->Attribute("id") : "";
        parseLayer(layerElement, containerID, groupID);
    }
}
