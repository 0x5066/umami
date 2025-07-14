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
    std::ifstream file(filepath, std::ios::binary);
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
    // Wrap only if rawXml doesn't have a single root element already
    // But safer to wrap, just ensure it won't break XML declaration
    std::string wrappedXml = "<root>" + rawXml + "</root>";

    // Step 3: Parse wrapped XML
    XMLDocument xml_doc;

    // tinyxml2 cannot parse multiple XML declarations or multiple roots.
    // Wrapping might cause an extra XML declaration inside <root> which breaks parsing.
    // Remove XML declaration from rawXml before wrapping:
    size_t declEnd = rawXml.find("?>");
    if (declEnd != std::string::npos && rawXml.find("<?xml") == 0) {
        rawXml.erase(0, declEnd + 2);
    }
    wrappedXml = "<root>" + rawXml + "</root>";

    XMLError result = xml_doc.Parse(wrappedXml.c_str());
    if (result != XML_SUCCESS) {
        std::cerr << "Failed to parse XML: " << xml_doc.ErrorStr() << " Line: " << xml_doc.ErrorLineNum() << std::endl;
        return;
    }

    // Step 4: Traverse all elements recursively
    std::function<void(XMLElement*)> traverse;
    traverse = [&](XMLElement* elem) {
        while (elem) {
            //std::cout << "Tag: " << elem->Name() << std::endl;

            registerElementHook(elem);

            if (std::string(elem->Name()) == "include" && elem->Attribute("file")) {
                std::string dir = stripXMLFileName(filepath);
                std::string includePath = dir + elem->Attribute("file");
                parseSkinXML(includePath, true);
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

    std::ifstream file(skinXML, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Failed to open file: " << skinXML << '\n';
        return false;
    }

    std::string xml_content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
    file.close();

    if (xml_content.size() >= 3 &&
        (unsigned char)xml_content[0] == 0xEF &&
        (unsigned char)xml_content[1] == 0xBB &&
        (unsigned char)xml_content[2] == 0xBF)
    {
        xml_content.erase(0, 3);
    }

    XMLError eResult = xml_doc.Parse(xml_content.c_str(), xml_content.size());

    std::cout << "WALvalidator: " << skinXML << '\n';

    if (eResult != XML_SUCCESS) {
        return false;
    }

    XMLElement* WAL = xml_doc.FirstChildElement("WinampAbstractionLayer");
    if (WAL != nullptr) std::cout << WAL->Name() << '\n';

    if (WAL == nullptr) {
        WAL = xml_doc.FirstChildElement("WasabiXML");
        if (WAL != nullptr) std::cout << WAL->Name() << '\n';
    }

    if (WAL == nullptr) {
        std::cout << "BAILED!!! Not a Modern Skin" << '\n';
        return false;
    }

    const char* ver = WAL->Attribute("version");
    if (ver == nullptr) ver = "0.0";

    char finalver[5] = {0};
    int len = (int)strlen(ver);
    if (len >= 3) {
        finalver[0] = ver[0];
        finalver[1] = ver[2];
        finalver[2] = ver[3] ? ver[3] : '\0';
    } else {
        strncpy(finalver, ver, 4);
    }
    for (int i = 0; i < 3 && finalver[i] != '\0'; i++) {
        std::cout << finalver[i] << '\n';
    }

    XMLElement* skininfo = WAL->FirstChildElement("skininfo");
    if (skininfo != nullptr) {
        for (XMLElement* child = skininfo->FirstChildElement();
             child != nullptr;
             child = child->NextSiblingElement()) {
            const char* tagName = child->Name();
            const char* information = child->GetText();
            std::cout << tagName << ": " << (information ? information : "") << '\n';
        }
    } else {
        std::cout << "No <skininfo> found, continuing without it.\n";

        // ✅ FIX: call parseSkinXML with absolute path to ensure includes resolve relative to this file
        parseSkinXML(std::filesystem::absolute(skinXML).string(), false);
    }

    for (XMLElement* includeElem = WAL->FirstChildElement("include");
         includeElem != nullptr;
         includeElem = includeElem->NextSiblingElement("include")) {
        const char* fileAttr = includeElem->Attribute("file");
        if (fileAttr) {
            std::filesystem::path base = std::filesystem::path(skinXML).parent_path();
            std::filesystem::path includePath = base / fileAttr;

            if (!std::filesystem::exists(includePath)) {
                includePath = std::filesystem::path("skin") / fileAttr;
            }

            parseSkinXML(includePath.lexically_normal().string(), true);
        }
    }

    return true;
}
