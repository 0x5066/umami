#include "wasabiparser.h"
#include "skin.h"

std::string stripXMLFileName(const std::string& filepath) {
    size_t pos = filepath.find_last_of("/\\"); // Find last '/' or '\'
    if (pos != std::string::npos) {
        return filepath.substr(0, pos + 1); // Keep the path, including the last '/'
    }
    return filepath; // Return the original if no slashes were found
}

void parseSkinXML(const std::string& filepath, bool recursive) {
    // Step 1: Read file manually
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string rawXml = buffer.str();
    file.close();

    if (recursive) {
        //std::cout << "recursively parsing from include: " << filepath << '\n';
    } else {
        //std::cout << "parsing: " << filepath << '\n';
    }

    // Step 2: Wrap with a dummy root element
    std::string wrappedXml = "<root>" + rawXml + "</root>";

    // Step 3: Parse wrapped XML
    XMLDocument xml_doc;
    XMLError result = xml_doc.Parse(wrappedXml.c_str());
    if (result != XML_SUCCESS) {
        std::cerr << "Failed to parse XML: " << xml_doc.ErrorStr() << std::endl;
        return;
    }

    // Step 4: Traverse all elements recursively
    std::function<void(XMLElement*)> traverse;
    traverse = [&](XMLElement* elem) {
        while (elem) {
            // existing debug log
            //std::cout << "Tag: " << elem->Name() << std::endl;

            // hook into loader
            registerElementHook(elem);

            if (std::string(elem->Name()) == "include" && elem->Attribute("file")) {
                parseSkinXML(stripXMLFileName(filepath) + elem->Attribute("file"), 1);
            }

            if (elem->FirstChildElement()) {
                traverse(elem->FirstChildElement());
            }

            elem = elem->NextSiblingElement();
        }
    };

    XMLElement* root = xml_doc.RootElement();
    traverse(root->FirstChildElement());
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
    if (WAL != nullptr) /*std::cout << WAL->Name() << '\n'*/;

    if (WAL == nullptr) {
        // set WAL to "WasabiXML" if "WinampAbstractionLayer" doesnt exist
        WAL = xml_doc.FirstChildElement("WasabiXML");
        if (WAL != nullptr) /*std::cout << WAL->Name() << '\n'*/ ;
    }

    if (WAL == nullptr) {
        //std::cout << "BAILED!!! Not a Modern Skin" << '\n';
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
        //std::cout << finalver[i] << '\n';
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
        //std::cout << tagName << ": " << information << '\n';
    }
    // iterate over WAL but only look for the includes
    for( XMLElement* children_of_WAL = WAL->FirstChildElement("include");
    children_of_WAL != NULL;
    children_of_WAL = children_of_WAL->NextSiblingElement("include") )
    {
        // this is going to be *very* fun
        // update: wow that's awful
        std::string fileAttribute = "skin/" + std::string(children_of_WAL->Attribute("file"));
        //std::cout << "include file: " << fileAttribute << '\n';
        parseSkinXML(fileAttribute, 0);
    }
    return true;
}
