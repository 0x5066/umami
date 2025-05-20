#ifndef UMAMI_UMAMISTRUCTS
#define UMAMI_UMAMISTRUCTS

#include <vector>
#include <string>

struct umamiContainer {
    std::string containerID;
    std::string containerMinimumW;
    std::string containerMinimumH;
    std::string containerMaximumW;
    std::string containerMaximumH;
    std::string containerDefaultVisible;
    std::string containerDefaultX;
    std::string containerDefaultY;
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

struct umamiGuiObjects {
    // All the objects to be discussed in this chapter are GuiObjects. This means, among other things, all such objects have the same basic set of parameters which are valid for all of them. 
    // All GuiObjects are objects that live with a set of spatial coordinates within a groupdef (or layout). 
    // A button, a piece of text, a flat bitmap image, a tree of items, and an edit box would all be different kinds of GuiObjects, and all of the parameters listed here work on all of them. 
    std::string id;                 // The ID of the object
    std::string x;                  // (int) The X position of the object in its group or layout
    std::string y;                  // (int) The Y position of the object in its group or layout
    std::string width;              // (int) The width of the object in its group or layout
    std::string height;             // (int) The height of the object in its group or layout
    std::string alpha = "255";      // (int) An integer [0,255] specifying the alpha blend mode of the object (0 is transparent, 255 is opaque). Default is 255.
    std::string activealpha;        // (int) A different alpha value [0,255] for the object in its active state.
    std::string inactivealpha;      // (int) A different alpha value [0,255] for the object in its inactive state (if you set both activealpha and inactivealpha, it is meaningless to set alpha).
    std::string cursor;             // The id of the cursor element to be displayed when the mouse is over this object.
    std::string tooltip;            // A human readable string to display as a tooltip for when the mouse is hovered over this object.
    std::string move;               // (bool) Setting this flag causes any dragging of this object to actually drag the entire layout within which the object resides.
    std::string renderbasetexture;  // (bool) A flag to tell the system to always use the system's base texture as the background of this object.
    std::string cfgattrib;          // A configattrib string assigned to this GuiObject.
    std::string visible;            // (bool) Whether or not the object should be displayed in its group or layout.
    std::string relatx;             // (int) This [0,2] integer determines if the X position should be calculated by normal ("0"), relative ("1") or proportional ("2") units.
    std::string relaty;             // (int) This [0,2] integer determines if the Y position should be calculated by normal ("0"), relative ("1") or proportional ("2") units.
    std::string relatw;             // (int) This [0,2] integer determines if the width should be calculated by normal ("0"), relative ("1") or proportional ("2") units.
    std::string relath;             // (int) This [0,2] integer determines if the height should be calculated by normal ("0"), relative ("1") or proportional ("2") units.
    std::string fitparent;          // (bool) This parameter is used instead of x;y;w;h parameters and means that the object covers all the area of the parent object.
    std::string x1;                 // (int) The X position of the left edge of the object in its group or layout. Only works inside definitions with design_w parameters set.
    std::string y1;                 // (int) The Y position of the top edge of the object in its group or layout. Only works inside definitions with design_h parameters set.
    std::string x2;                 // (int) The X position of the right edge of the object in its group or layout. Only works inside definitions with design_w parameters set.
    std::string y2;                 // (int) The Y position of the bottom edge of the object in its group or layout. Only works inside definitions with design_h parameters set.
    std::string anchor;             // A list of one or more of the following keywords, delimited by the pipe (|) character: "top|bottom|left|right".
    std::string sysmetricsx;        // (bool) This flag will cause this object to be resized by having its X position multiplied by the OS scalar coefficient.
    std::string sysmetricsy;        // (bool) This flag will cause this object to be resized by having its Y position multiplied by the OS scalar coefficient.
    std::string sysmetricsw;        // (bool) This flag will cause this object to be resized by having its width multiplied by the OS scalar coefficient.
    std::string sysmetricsh;        // (bool) This flag will cause this object to be resized by having its height multiplied by the OS scalar coefficient.
    std::string rectrgn;            // (bool) Setting this flag causes the engine to ignore the object's region for handling "mouse over object" detection and only use the bounding rectangle.
    std::string regionop;           // (int) An integer enumeration [-2,1] for what kind of region-operation to perform with this object's calculated region against its parent's region.
    std::string wantfocus;          // (bool) This flag determines whether or not an object ever gets the focus at all.
    std::string focusonclick;       // (bool) This flag determines whether or not an object gets the focus when clicked.
    std::string taborder;           // (int) This value indicates the "tab order" of the object within the group or layout in which it resides.
    std::string nodblclick;         // (bool) This flag blocks doubleclick events from being received by this object.
    std::string noleftclick;        // (bool) This flag blocks left click events from being received by this object.
    std::string norightclick;       // (bool) This flag blocks right click events from being received by this object.
    std::string nomousemove;        // (bool) This flag blocks mouse move events from being received by this object.
    std::string nocontextmenu;      // (bool) This flag blocks the main context menu from being displayed for rightclick.
    std::string ghost;              // (bool) This flag causes the object to be transparent to all user interface events such that all the mouse messages pass to the object(s) visibly underneath it.
    std::string notify[10];         // A string to be used as a parameter to the onNotify event for the object upon instantiation. The notify strings 0-9 will be sent in order.
    std::string droptarget;         // The id of an object to which all of this object's drag and drop events will be redirected.
};

struct umamiButton : public umamiGuiObjects {
    std::string label;
    std::string action;
};

struct umamiLayer : public umamiGuiObjects {
    std::string image; // (id) The id of the bitmap element to be used for display. 
    std::string inactiveimage; // (id) A secondary id to be displayed if this guiobject is set to "inactive" status. 
    std::string region; // (id) The id of a different bitmap element to use as the source of this object's region. 
    std::string tile; // (bool) Setting this flag will tile the image if the rectangular size of the layer is different than the rectangular size of the bitmap. The default is to stretch. 
    std::string resize; // (str) A specific string enumerating which direction one should allow the resizing of the parent layout if this object's region is gripped and dragged by the user. Incompatible with the move parameter on guiobject. See below. 
    std::string scale; // (str) A specific string enumerating which direction one should allow the scaling of the parent layout if this object's region is gripped and dragged by the user. Incompatible with the move parameter on guiobject. See below. 
    std::string dblclickaction; // (str) The string of an action to be sent into the system if this object is doubleclicked within its region. 
    // The resize and scale parameters transform the simple layer graphic into interactive gui objects in the system by enabling them to be dragged by the user to resize or change the scale of the parent layout. 
    // The eight possible values for these parameters are "top" "left" "right" "bottom" "topleft" "topright" "bottomleft" and "bottomright." 
    // If both resize and scale are set on a single layer, the object will act as a resizer by default but as a scaler if the ALT key is held down. 
};

extern struct umamiContainer umamiMain;
extern struct umamiContainer PL;
extern struct umamiContainer EQ;
extern struct umamiContainer MLibrary;

extern std::vector<umamiElement> elements;

#endif // UMAMI_UMAMISTRUCTS