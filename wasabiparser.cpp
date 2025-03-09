#include "wasabiparser.h"
#include "umamistructs.h"

struct umamiContainer umamiMain;
struct umamiContainer PL;
struct umamiContainer EQ;
struct umamiContainer MLibrary;

std::vector<umamiElement> elements;

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
                WALvalidator((entry.path().string() + wasabixml).c_str());
                std::cout << (entry.path().string() + wasabixml).c_str() << '\n';
#ifdef _WIN32
                WALvalidator((entry.path().u8string().c_str() + wasabixml).c_str());
                std::cout << (entry.path().u8string().c_str() + wasabixml).c_str() << '\n';
#endif // _WIN32
                return;
            }
            loadFreeform(entry.path());
        } else {
            if (entry.path().extension() == ".xml") {
                //std::cout << "XML File: " << entry.path() << std::endl;
#ifdef __linux__
                if (WALvalidator(entry.path().c_str()) != 1){
                    parseSkinXML((entry.path().c_str()), 0);
                }
#endif // __linux__
#ifdef _WIN32
                if (WALvalidator(entry.path().u8string().c_str()) != 1){
                    parseSkinXML((entry.path().u8string().c_str()), 0);
                }
#endif // _WIN32
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
                        std::cout << "Finished, returning to groupdef " << children_of_Groupdef->Attribute("id") << "." << '\n' << '\n';
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

    const char *cid, *cminiw, *cminih, *cmaxiw, *cmaxih, *cdt, *cda, *cdv, *cdx, *cdy;
    const char *lid, *ln, *ldy, *ldx, *ldv, *ldw, *ldh, *lminiw, *lminih, *lmaxiw, *lmaxih;

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
            umamiElement element;
            element.type = children_of_Elements->Name();
            element.id = (children_of_Elements->Attribute("id") != nullptr) ? children_of_Elements->Attribute("id") : "";
            element.file = (children_of_Elements->Attribute("file") != nullptr) ? children_of_Elements->Attribute("file") : "";
            element.hotx = (children_of_Elements->Attribute("x") != nullptr) ? children_of_Elements->Attribute("x") : "";
            element.hoty = (children_of_Elements->Attribute("y") != nullptr) ? children_of_Elements->Attribute("y") : "";
            element.imagew = (children_of_Elements->Attribute("w") != nullptr) ? children_of_Elements->Attribute("w") : "";
            element.imageh = (children_of_Elements->Attribute("h") != nullptr) ? children_of_Elements->Attribute("h") : "";
            elements.push_back(element);

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

/*    XMLElement* aboutSkin = xml_doc.FirstChildElement("groupdef");
    if (aboutSkin != nullptr) {
        for( XMLElement* children_of_aboutSkin = aboutSkin->FirstChildElement("groupdef");
            children_of_aboutSkin != NULL;
            children_of_aboutSkin = children_of_aboutSkin->NextSiblingElement("groupdef") )
        {
            const char* tagName = children_of_aboutSkin->Name();
            const char* id = (children_of_aboutSkin->Attribute("id") != nullptr) ? children_of_aboutSkin->Attribute("id") : "No ID";
            std::cout << tagName << ": " << id << '\n';
            if (std::string(children_of_aboutSkin->Name()) == "group"){
                //std::cout << children_of_aboutSkin->Name() << ": " << children_of_aboutSkin->Attribute("id") << '\n';
                parseGroupWithinGroup(children_of_aboutSkin, xml_doc);
            }
        }
    }

    if (aboutSkin == nullptr) {
        return;
    }*/

    XMLElement* Container = xml_doc.FirstChildElement("container");
    if (Container != nullptr){
        cid = (Container->Attribute("id") != nullptr) ? Container->Attribute("id") : "NoID";
        cminiw = (Container->Attribute("minimum_w") != nullptr) ? Container->Attribute("minimum_w") : "0";
        cminih = (Container->Attribute("minimum_h") != nullptr) ? Container->Attribute("minimum_h") : "0";
        cmaxiw = (Container->Attribute("maximum_w") != nullptr) ? Container->Attribute("maximum_w") : "0";
        cmaxih = (Container->Attribute("maximum_h") != nullptr) ? Container->Attribute("maximum_h") : "0";
        cdt = (Container->Attribute("droptarget") != nullptr) ? Container->Attribute("droptarget") : "???";
        cda = (Container->Attribute("desktopalpha") != nullptr) ? Container->Attribute("desktopalpha") : "0";
        cdv = (Container->Attribute("default_visible") != nullptr) ? Container->Attribute("default_visible") : "0";
        cdx = (Container->Attribute("default_x") != nullptr) ? Container->Attribute("default_x") : "0";
        cdy = (Container->Attribute("default_y") != nullptr) ? Container->Attribute("default_y") : "0";
        std::cout << Container->Name() << ": " << cid
        << ", minimum_w:" << cminiw
        << ", minimum_h:" << cminih << '\n'
        << ", maximum_w:" << cmaxiw
        << ", maximum_h:" << cmaxih << '\n'
        << ", droptarget:" << cdt
        << ", desktopalpha:" << cda
        << ", default_visible:" << cda
        << '\n' << '\n';
        if (std::string(cid) == "main"){
            std::cout << "HIT!" << cid << '\n';
            umamiMain.containerID = cid;
            umamiMain.containerMinimumW = cminiw;
            umamiMain.containerMinimumH = cminih;
            umamiMain.containerMaximumW = cmaxiw;
            umamiMain.containerMaximumH = cmaxih;
            umamiMain.containerDefaultVisible = cdv;
            umamiMain.containerDefaultX = cdx;
            umamiMain.containerDefaultY = cdy;
        }
        if (std::string(cid) == "PL"){
            std::cout << "HIT!" << cid << '\n';
            PL.containerID = cid;
            PL.containerMinimumW = cminiw;
            PL.containerMinimumH = cminih;
            PL.containerMaximumW = cmaxiw;
            PL.containerMaximumH = cmaxih;
            PL.containerDefaultVisible = cdv;
            PL.containerDefaultX = cdx;
            PL.containerDefaultY = cdy;
        }
        if (std::string(cid) == "EQ"){
            std::cout << "HIT!" << cid << '\n';
            EQ.containerID = cid;
            EQ.containerMinimumW = cminiw;
            EQ.containerMinimumH = cminih;
            EQ.containerMaximumW = cmaxiw;
            EQ.containerMaximumH = cmaxih;
            EQ.containerDefaultVisible = cdv;
            EQ.containerDefaultX = cdx;
            EQ.containerDefaultY = cdy;
        }
        if (std::string(cid) == "MLibrary"){
            std::cout << "HIT!" << cid << '\n';
            MLibrary.containerID = cid;
            MLibrary.containerMinimumW = cminiw;
            MLibrary.containerMinimumH = cminih;
            MLibrary.containerMaximumW = cmaxiw;
            MLibrary.containerMaximumH = cmaxih;
            MLibrary.containerDefaultVisible = cdv;
            MLibrary.containerDefaultX = cdx;
            MLibrary.containerDefaultY = cdy;
        }
    }

    if (Container == nullptr) {
            //if (Container == nullptr && Groupdef != nullptr) return;
        std::cout << "Container not found! Exiting." << '\n' << '\n';
        return;
    }

    XMLElement* Layout = Container->FirstChildElement("layout");
    if (Layout != nullptr){
        lid = (Layout->Attribute("id") != nullptr) ? Layout->Attribute("id") : "No ID";
        ln = (Layout->Attribute("name") != nullptr) ? Layout->Attribute("name") : "None";
        ldy = (Layout->Attribute("default_y") != nullptr) ? Layout->Attribute("default_y") : "0";
        ldx = (Layout->Attribute("default_x") != nullptr) ? Layout->Attribute("default_x") : "0";
        ldw = (Layout->Attribute("default_w") != nullptr) ? Layout->Attribute("default_w") : "0";
        ldh = (Layout->Attribute("default_h") != nullptr) ? Layout->Attribute("default_h") : "0";
        lminiw = (Layout->Attribute("minimum_w") != nullptr) ? Layout->Attribute("minimum_w") : "0";
        lminih = (Layout->Attribute("minimum_h") != nullptr) ? Layout->Attribute("minimum_h") : "0";
        lmaxiw = (Layout->Attribute("maximum_w") != nullptr) ? Layout->Attribute("maximum_w") : "0";
        lmaxih = (Layout->Attribute("maximum_h") != nullptr) ? Layout->Attribute("maximum_h") : "0";
        std::cout << Layout->Name() << ": " << lid
        << ", name:" << ln << '\n'
        << ", default_y:" << ldy
        << ", default_x:" << ldx
        << ", default_w:" << ldw
        << ", default_h:" << ldh << '\n'
        << ", minimum_w:" << lminiw
        << ", minimum_h:" << lminih << '\n'
        << ", maximum_w:" << lmaxiw
        << ", maximum_h:" << lmaxih
        << '\n' << '\n';
    }

#ifdef _WIN32
        //if (ldv && strcmp(ldv, "0") == 0){
            if (Container != nullptr || Layout != nullptr){
                const char* w = cminiw;  // Default fallback
                const char* h = cminih;  // Default fallback

                if (ldw && strcmp(ldw, "0") != 0 && ldw[0] != '\0') {
                    w = ldw;
                } else if (lminiw && strcmp(lminiw, "0") != 0 && lminiw[0] != '\0') {
                    w = lminiw;
                }

                if (ldh && strcmp(ldh, "0") != 0 && ldh[0] != '\0') {
                    h = ldh;
                } else if (lminih && strcmp(lminih, "0") != 0 && lminih[0] != '\0') {
                    h = lminih;
                }
                if (std::string(cid) == "main"){
                    umamiMain.containerSizeH = h;
                    umamiMain.containerSizeW = w;
                }
                if (std::string(cid) == "EQ"){
                    EQ.containerSizeH = h;
                    EQ.containerSizeW = w;
                }
                if (std::string(cid) == "PL"){
                    PL.containerSizeH = h;
                    PL.containerSizeW = w;
                }
                if (std::string(cid) == "MLibrary"){
                    MLibrary.containerSizeH = h;
                    MLibrary.containerSizeW = w;
                }
            }
        //}
#endif // _WIN32

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
