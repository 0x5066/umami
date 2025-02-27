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

std::string stripXMLFileName(const std::string& filepath) {
    size_t pos = filepath.find_last_of("/\\"); // Find last '/' or '\'
    if (pos != std::string::npos) {
        return filepath.substr(0, pos + 1); // Keep the path, including the last '/'
    }
    return filepath; // Return the original if no slashes were found
}

void loadFreeform(const std::filesystem::path& directory) {
    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
        std::cerr << "Invalid directory: " << directory << std::endl;
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_directory()) {
            if (entry.path().filename() == "wasabi") {
                std::cout << "Found 'wasabi' folder at: " << entry.path() << "\nStopping search.\n";
                std::string wasabixml = "/wasabi.xml";
                Test((entry.path().string() + wasabixml).c_str());
                std::cout << (entry.path().string() + wasabixml).c_str() << '\n';
                return;
            }
            loadFreeform(entry.path());
        } else {
            if (entry.path().extension() == ".xml") {
                //std::cout << "XML File: " << entry.path() << std::endl;
                if (Test(entry.path().c_str()) != 1){
                    parseSkinXML((entry.path().c_str()), 0);
                }
            }
        }
    }
}

// doesnt yet handle groupdefs inside a layout
void parseGroupWithinGroup(XMLElement* children_of_Layout, XMLDocument& xml_doc){
    XMLElement* Groupdef = xml_doc.FirstChildElement("groupdef");
        if (Groupdef != nullptr){
            for( XMLElement* children_of_Groupdef = Groupdef;
            children_of_Groupdef != NULL;
            children_of_Groupdef = children_of_Groupdef->NextSiblingElement("groupdef") )
            {
            if (std::string(children_of_Groupdef->Attribute("id")) == std::string(children_of_Layout->Attribute("id")) ) {
                std::cout << children_of_Groupdef->Name() << ": " << children_of_Groupdef->Attribute("id") << '\n';
                for( XMLElement* inside_Groupdef = children_of_Groupdef->FirstChildElement();
                    inside_Groupdef != NULL; inside_Groupdef = inside_Groupdef->NextSiblingElement() ) {
                    const char* tagName = inside_Groupdef->Name();
                    const char* id = (inside_Groupdef->Attribute("id") != nullptr) ? inside_Groupdef->Attribute("id") : "No ID";
                    const char* wx = (inside_Groupdef->Attribute("x") != nullptr) ? inside_Groupdef->Attribute("x") : "0 (default)";
                    const char* wy = (inside_Groupdef->Attribute("y") != nullptr) ? inside_Groupdef->Attribute("y") : "0 (default)";
                    const char* ww = (inside_Groupdef->Attribute("w") != nullptr) ? inside_Groupdef->Attribute("w") : "N/A";
                    const char* wh = (inside_Groupdef->Attribute("h") != nullptr) ? inside_Groupdef->Attribute("h") : "N/A";
                    std::cout << tagName << ": " << id << ", x:" << wx << ", y:" << wy << ", w:" << ww << ", h:" << wh << '\n';
                    if (std::string(tagName) == "group"){
                        std::cout << '\n' << "GROUP FOUND WITHIN GROUP, PARSING..." << '\n';
                        //std::cout << inside_Groupdef->Name() << ": " << inside_Groupdef->Attribute("id") << '\n';
                        parseGroupWithinGroup(inside_Groupdef, xml_doc);
                        std::cout << "Finished, returning to groupdef" << children_of_Groupdef->Attribute("id") << "." << '\n' << '\n';
                    }
                }
                if (Groupdef == nullptr) {
                    std::cout << "Done." << '\n' << '\n';
                }
            }
        }
    }
}

void parseSkinXML(std::string filepath, bool includeElements){
    XMLDocument xml_doc;
    XMLError eResult = xml_doc.LoadFile(filepath.c_str());

    // this is really unhinged, but needed
    // we're going to create a new root just so quirky skins' elements files can still be parsed
    if (includeElements){
        //std::cout << filepath << '\n';
        XMLElement* elements = xml_doc.NewElement("elements");

        XMLNode* root = xml_doc.RootElement();
        while (root) {
            XMLNode* next = root->NextSibling();
            //xml_doc.DeleteNode(root);
            elements->InsertEndChild(root);
            root = next;
        }
        xml_doc.InsertFirstChild(elements);
    }

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
            // and it is getting worse
            if (std::string(children_of_Elements->Name()) == "bitmap" || std::string(children_of_Elements->Name()) == "color" ||
                std::string(children_of_Elements->Name()) == "cursor" || std::string(children_of_Elements->Name()) == "elementalias"){
                std::cout << children_of_Elements->Name() << ": " << children_of_Elements->Attribute("id") << '\n';
            } else {
                std::cout << children_of_Elements->Name() << ": " << children_of_Elements->Attribute("file") << '\n';
            }
            if (children_of_Elements && std::string(children_of_Elements->Name()) == "include") {
                //std::cout << stripXMLFileName(filepath) + children_of_Elements->Attribute("file") << '\n';
                parseSkinXML(stripXMLFileName(filepath) + children_of_Elements->Attribute("file"), 1);
            }
        }
    }

    if (Elements == nullptr) {
        //std::cout << "Elements not found! Continuing." << '\n';
    }

    XMLElement* Container = xml_doc.FirstChildElement("container");
    if (Container != nullptr){
        const char* lid = (Container->Attribute("id") != nullptr) ? Container->Attribute("id") : "No ID";
        const char* miniw = (Container->Attribute("minimum_w") != nullptr) ? Container->Attribute("minimum_w") : "0";
        const char* minih = (Container->Attribute("minimum_h") != nullptr) ? Container->Attribute("minimum_h") : "0";
        const char* maxiw = (Container->Attribute("maximum_w") != nullptr) ? Container->Attribute("maximum_w") : "0";
        const char* maxih = (Container->Attribute("maximum_h") != nullptr) ? Container->Attribute("maximum_h") : "0";
        const char* dt = (Container->Attribute("droptarget") != nullptr) ? Container->Attribute("droptarget") : "???";
        const char* da = (Container->Attribute("desktopalpha") != nullptr) ? Container->Attribute("desktopalpha") : "0";
        std::cout << Container->Name() << ": " << lid
        << ", minimum_w:" << miniw
        << ", minimum_h:" << minih << '\n'
        << ", maximum_w:" << maxiw
        << ", maximum_h:" << maxih << '\n'
        << ", droptarget:" << dt
        << ", desktopalpha:" << da
        << '\n' << '\n';
    }
    if (Container == nullptr) {
            //if (Container == nullptr && Groupdef != nullptr) return;
        std::cout << "Container not found! Exiting." << '\n' << '\n';
        return;
    }

    XMLElement* Layout = Container->FirstChildElement("layout");
    if (Layout != nullptr){
        const char* lid = (Layout->Attribute("id") != nullptr) ? Layout->Attribute("id") : "No ID";
        const char* ln = (Layout->Attribute("name") != nullptr) ? Layout->Attribute("name") : "None";
        const char* dy = (Layout->Attribute("default_y") != nullptr) ? Layout->Attribute("default_y") : "0";
        const char* dx = (Layout->Attribute("default_x") != nullptr) ? Layout->Attribute("default_x") : "0";
        const char* dv = (Layout->Attribute("default_visible") != nullptr) ? Layout->Attribute("default_visible") : "0";
        const char* miniw = (Layout->Attribute("minimum_w") != nullptr) ? Layout->Attribute("minimum_w") : "0";
        const char* minih = (Layout->Attribute("minimum_h") != nullptr) ? Layout->Attribute("minimum_h") : "0";
        const char* maxiw = (Layout->Attribute("maximum_w") != nullptr) ? Layout->Attribute("maximum_w") : "0";
        const char* maxih = (Layout->Attribute("maximum_h") != nullptr) ? Layout->Attribute("maximum_h") : "0";
        std::cout << Layout->Name() << ": " << lid
        << ", name:" << ln << '\n'
        << ", default_y:" << dy
        << ", default_x:" << dx
        << ", default_visible:" << dv << '\n'
        << ", minimum_w:" << miniw
        << ", minimum_h:" << minih << '\n'
        << ", maximum_w:" << maxiw
        << ", maximum_h:" << maxih
        << '\n' << '\n';
    }

    for( XMLElement* children_of_Layout = Layout->FirstChildElement();
        children_of_Layout != NULL;
        children_of_Layout = children_of_Layout->NextSiblingElement() )
    {
        const char* tagName = children_of_Layout->Name();
        const char* id = (children_of_Layout->Attribute("id") != nullptr) ? children_of_Layout->Attribute("id") : "No ID";
        std::cout << tagName << ": " << id << '\n';
        if (std::string(children_of_Layout->Name()) == "group"){
            //std::cout << children_of_Layout->Name() << ": " << children_of_Layout->Attribute("id") << '\n';
            parseGroupWithinGroup(children_of_Layout, xml_doc);
        }
    }
    if (Layout == nullptr) {
        std::cout << "Layout not found! Exiting. (That would be exceptionally weird.)" << '\n';
        return;
    }
}

bool WALvalidator(const char* skinXML)
{
    XMLDocument xml_doc;
    XMLError eResult = xml_doc.LoadFile(skinXML);

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
        parseSkinXML(fileAttribute, 0);
    }
    return true;
}
int main()
{
    loadFreeform("freeform");
    WALvalidator("skin/skin.xml");
    return 0;
}
