#ifndef UMAMI_UMAMISTRUCTS
#define UMAMI_UMAMISTRUCTS

#include <vector>

// there's more but that'll be enough for now
struct umamiContainer {
    std::string containerID;
    std::string containerMinimumW;
    std::string containerMinimumH;
    std::string containerMaximumW;
    std::string containerMaximumH;
    std::string containerDefaultVisible;
    std::string containerDefaultX;
    std::string containerDefaultY;
    // this is wrong but i am also lazy
    std::string containerSizeH;
    std::string containerSizeW;
};

struct umamiLayout {
    std::string layoutID;
    std::string layoutName;
    std::string layoutDefaultX;
    std::string layoutDefaultY;
    std::string layoutDefaultW;
    std::string layoutDefaultH;
    std::string layoutMinimumX;
    std::string layoutMinimumY;
    std::string layoutMaximumX;
    std::string layoutMaximumY;
};

struct umamiElement {
    std::string type;
    std::string id;
    std::string file;
    std::string hotx;
    std::string hoty;
    std::string imagew;
    std::string imageh;
};

extern struct umamiContainer umamiMain;
extern struct umamiContainer PL;
extern struct umamiContainer EQ;
extern struct umamiContainer MLibrary;

extern std::vector<umamiElement> elements;

#endif // UMAMI_UMAMISTRUCTS
