#ifndef UMAMI_LAYERPARSER
#define UMAMI_LAYERPARSER

#include "tinyxml2/tinyxml2.h"
#include "umamistructs.h"
#include <unordered_map>
#include <vector>
#include <string>

using namespace tinyxml2;

extern std::unordered_map<std::string, std::vector<umamiLayer>> containerLayers;

void parseLayer(XMLElement* layerElement, const std::string& containerID, const std::string& groupID);
void parseLayers(XMLElement* groupElement, const std::string& containerID);

#endif // UMAMI_LAYERPARSER
