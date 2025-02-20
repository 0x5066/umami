#include <iostream>
#include <string>
#include "tinyxml2/tinyxml2.h"

using namespace tinyxml2;

void parseSkinXML(std::string filepath);

void parseSkinXML(std::string filepath){
    XMLDocument xml_doc;
    XMLError eResult = xml_doc.LoadFile(filepath.c_str());

    if (eResult != XML_SUCCESS){
        std::cout << "File not found, exiting early." << '\n';
        return;
    }

    XMLElement* Container = xml_doc.FirstChildElement("container");
    if (Container != nullptr) std::cout << Container->Name() << '\n';
    std::cout << Container->Attribute("id") << '\n';

    XMLElement* Layout = Container->FirstChildElement("layout");
    if (Layout != nullptr) std::cout << Layout->Name() << '\n';
    std::cout << Layout->Attribute("id") << '\n';

    if (Container == nullptr) {
        std::cout << "Container not found! Exiting." << '\n';
        return;
    }

    if (Layout == nullptr) {
        std::cout << "Layout not found! Exiting." << '\n';
        return;
    }
}

bool Test()
{
    XMLDocument xml_doc;
    XMLError eResult = xml_doc.LoadFile("skin/skin.xml");

    if (eResult != XML_SUCCESS) return false;

    // can i just adjust the root/FirstChildElement willynilly like that?
    // yes
    XMLElement* WAL = xml_doc.FirstChildElement("WinampAbstractionLayer");
    // if WAL isnt a null pointer, print the name of root
    if (WAL != nullptr) std::cout << WAL->Name() << '\n';

    if (WAL == nullptr) {
        // set WAL to "WasabiXML" if "WinampAbstractionLayer" doesnt exist
        WAL = xml_doc.FirstChildElement("WasabiXML");
        if (WAL != nullptr) std::cout << WAL->Name() << '\n';
    }

    if (WAL == nullptr) {
        std::cout << "BAILED!!! Not a Modern Skin" << '\n';
        return false;
    }

    // figure this out later
    const char* ver;
    char finalver[5];

    ver = WAL->Attribute("version");

    // this is horrible please ignore me
    finalver[0] = ver[0];
    for (int i = 2; i < 4; i++){
        finalver[i-1] = ver[i];
    }

    for (int i = 0; i < 3; i++){
        std::cout << finalver[i] << '\n';
    }

    XMLElement* skininfo = WAL->FirstChildElement("skininfo");
    if (skininfo == nullptr) return false;

    // extensively document this
    // iterate over skininfo, which is what we determined from WAL->FirstChildElement("skininfo");
    for( XMLElement* children_of_skininfo = skininfo->FirstChildElement();
    children_of_skininfo != NULL;
    children_of_skininfo = children_of_skininfo->NextSiblingElement() )
    {
        const char* tagName = children_of_skininfo->Name();
        const char* information = children_of_skininfo->GetText();
        std::cout << tagName << ": " << information << '\n';
    }
    // iterate over WAL but only look for the includes
    for( XMLElement* children_of_WAL = WAL->FirstChildElement("include");
    children_of_WAL != NULL;
    children_of_WAL = children_of_WAL->NextSiblingElement("include") )
    {
        // this is going to be *very* fun
        std::string fileAttribute = "skin/" + std::string(children_of_WAL->Attribute("file"));
        std::cout << "include file: " << fileAttribute << '\n';
        parseSkinXML(fileAttribute);
    }
    return true;
}
int main()
{
    Test();
    return 0;
}
