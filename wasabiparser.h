#ifndef UMAMI_WASABIPARSER
#define UMAMI_WASABIPARSER

#include <iostream>
#include <string>
#include <filesystem>
#include <unordered_map>
#include "tinyxml2/tinyxml2.h"
#include "umamistructs.h" // Include the umamistructs.h header file

using namespace tinyxml2;

extern std::unordered_map<std::string, umamiContainer*> containerMap;

bool WALvalidator(const char* skinXML);
void parseSkinXML(std::string filepath, bool includeElements);
std::string stripXMLFileName(const std::string& filepath);
void loadFreeform(const std::filesystem::path& directory);
void parseGroupWithinGroup(XMLElement* children_of_Layout, XMLDocument& xml_doc, const std::string& containerID);

#endif
