#include <iostream>
#include <string>
#include "tinyxml2/tinyxml2.h"

using namespace tinyxml2;

void parseSkinXML(std::string filepath);
std::string stripXMLFileName(const std::string& filepath);

std::string stripXMLFileName(const std::string& filepath) {
    size_t pos = filepath.find_last_of("/\\"); // Find last '/' or '\'
    if (pos != std::string::npos) {
        return filepath.substr(0, pos + 1); // Keep the path, including the last '/'
    }
    return filepath; // Return the original if no slashes were found
}

void parseSkinXML(std::string filepath){
    XMLDocument xml_doc;
    XMLError eResult = xml_doc.LoadFile(filepath.c_str());

    if (eResult != XML_SUCCESS){
        std::cout << "File not found, exiting early." << '\n';
        return;
    }

    XMLElement* Elements = xml_doc.FirstChildElement("elements");
    if (Elements != nullptr){
        for( XMLElement* children_of_Elements = Elements->FirstChildElement();
        children_of_Elements != NULL;
        children_of_Elements = children_of_Elements->NextSiblingElement() )
        {
            // this is unhinged
            if (std::string(children_of_Elements->Name()) == "bitmap" || std::string(children_of_Elements->Name()) == "color" || std::string(children_of_Elements->Name()) == "cursor"){
                std::cout << children_of_Elements->Name() << ": " << children_of_Elements->Attribute("id") << '\n';
            } else {
                std::cout << children_of_Elements->Name() << ": " << children_of_Elements->Attribute("file") << '\n';
            }
            if (children_of_Elements && std::string(children_of_Elements->Name()) == "include") {
                // this is even worse when you look in classic-elements.xml and see these <include/>s

                //std::cout << stripXMLFileName(filepath) + children_of_Elements->Attribute("file") << '\n';
                parseSkinXML(stripXMLFileName(filepath) + children_of_Elements->Attribute("file"));
            }
        }
    }
    if (Elements == nullptr) {
        //std::cout << "Elements not found! Continuing." << '\n';
    }

    XMLElement* Groupdef = xml_doc.FirstChildElement("groupdef");
    if (Groupdef != nullptr){
        for( XMLElement* children_of_Groupdef = Groupdef;
        children_of_Groupdef != NULL;
        children_of_Groupdef = children_of_Groupdef->NextSiblingElement("groupdef") )
        {
            std::cout << children_of_Groupdef->Name() << ": " << children_of_Groupdef->Attribute("id") << '\n';
        }
    }
    if (Groupdef == nullptr) {
        //std::cout << "Groupdefs not found! Continuing." << '\n';
    }


    XMLElement* Container = xml_doc.FirstChildElement("container");
    if (Container != nullptr) std::cout << Container->Name() << ": " << Container->Attribute("id") << '\n';
    if (Container == nullptr) {
            //if (Container == nullptr && Groupdef != nullptr) return;
        std::cout << "Container not found! Exiting." << '\n';
        return;
    }

    XMLElement* Layout = Container->FirstChildElement("layout");
    if (Layout != nullptr) std::cout << Layout->Name() << ": " << Layout->Attribute("id") << '\n';
    if (Layout == nullptr) {
        std::cout << "Layout not found! Exiting. (That would be exceptionally weird.)" << '\n';
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
        // update: wow that's awful
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
