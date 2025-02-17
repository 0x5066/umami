#include <iostream>
#include "tinyxml2/tinyxml2.h"

// there are definitely more attributes
struct umamiXML
{
    double WALversion {};
    double version {};
    char name {};
    char comment {};
    char author {};
    char email {};
    char screenshot {};
};

using namespace tinyxml2;

bool Test()
{
    umamiXML skin {};
    XMLDocument xml_doc;
    XMLError eResult = xml_doc.LoadFile("skin/skin.xml");

    if (eResult != XML_SUCCESS) return false;

    // can i just adjust the root/FirstChildElement willynilly like that?
    XMLElement* WAL = xml_doc.FirstChildElement("WinampAbstractionLayer");
    if (WAL == nullptr) return false;

    // figure this out later
    /*skin.umamiXMLversion = (double)WAL->Attribute("version");
    std::cout << skin.umamiXMLversion << '\n';*/

    XMLElement* skininfo = WAL->FirstChildElement("skininfo");
    if (skininfo == nullptr) return false;

    // extensively document this
    for( XMLElement* children_of_skininfo = skininfo->FirstChildElement();
    children_of_skininfo != NULL;
    children_of_skininfo = children_of_skininfo->NextSiblingElement() )
    {
        const char* tagName = children_of_skininfo->Name();
        const char* information = children_of_skininfo->GetText();
        std::cout << tagName << ": " << information << '\n';
    }

    return true;
}
int main()
{
    Test();
    return 0;
}
