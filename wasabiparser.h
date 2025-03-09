#ifndef UMAMI_WASABIPARSER
#define UMAMI_WASABIPARSER

#include <iostream>
#include <string>
#include <filesystem>
#include "tinyxml2/tinyxml2.h"

using namespace tinyxml2;

bool WALvalidator(const char* skinXML);
void parseSkinXML(std::string filepath, bool includeElements);
std::string stripXMLFileName(const std::string& filepath);
void loadFreeform(const std::filesystem::path& directory);
void parseGroupWithinGroup(XMLElement* children_of_Layout, XMLDocument& xml_doc);

#endif
