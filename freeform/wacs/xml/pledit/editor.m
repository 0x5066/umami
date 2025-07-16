#include <lib/std.mi>
//#include "..\..\..\lib\std.mi"

Function toggleSearchLine();
Function toggleSelectionList();
Function openSidecar();
Function closeSidecar();
Function sendSelection(String msg);

Global Button poppler;
Global int in;
Global int sl;
Global Button searchlineclose;
Global Edit editcontrol;
Global Layout pllayout;

GLobal Int sidecar_oldpage, sidecar_wasopen;

System.onScriptLoaded() {
  Group g = getScriptGroup();
  pllayout = g.getParentLayout();
  poppler = g.findObject("pledit.poppler");
  searchlineclose = g.findObject("searchline.close");
  GuiObject searchline = g.findObject("pledit.searchline");
  editcontrol = searchline.findObject("historyeditbox.edit");
  if (editcontrol == NULL) editcontrol = searchline;
  in = 0;
  sl = 0;
  String inout = getPrivateString("pledit.poppler", "inout", "out");
  if (inout == "in") poppler.leftClick();
  inout = getPrivateString("pledit.poppler", "searchline", "out");
  if (inout == "in") toggleSearchline();
}

openSidecar() {
  Group g = getScriptGroup();
  Group l = g.findObject("pledit.left");
  Group r = g.findObject("pledit.right");
  poppler.setXmlParam("x", "-138");
  l.setXmlParam("w", "-138");
  r.show();
  setPrivateString("pledit.poppler", "inout", "out");
  poppler.setXmlParam("image", "wasabi.button.label.arrow.right");
  in = 0;
}

CloseSidecar() {
  Group g = getScriptGroup();
  Group l = g.findObject("pledit.left");
  Group r = g.findObject("pledit.right");
  poppler.setXmlParam("x", "-8");
  l.setXmlParam("w", "-8");
  r.hide();
  setPrivateString("pledit.poppler", "inout", "in");
  poppler.setXmlParam("image", "wasabi.button.label.arrow.left");
  in = 1;
}

poppler.onLeftClick() {
  if (in) {
    // pop to out
    openSidecar();
  } else {
    closeSidecar();
  }
}

pllayout.onAccelerator(String action) {
  if (action == "jumper") {
    toggleSearchline();
    toggleSelectionList();
  }
}

toggleSearchLine() {
  Group g = getScriptGroup();
  GuiObject l = g.findObject("pledit.left");
  GuiObject r = g.findObject("pledit.right");
  GuiObject editor = l.findObject("pledit.editor");
  GuiObject searchline = l.findObject("pledit.searchline");
  if (editor == NULL || searchline == NULL) return;
  TabSheet t = r.findObject("pledit.sidecar");
  if (sl) {
    editor.setXmlParam("h", "-23");  
    searchline.setXmlParam("y", "-23");  
    setPrivateString("pledit.poppler", "searchline", "out");
    searchline.show();
    editcontrol.setFocus();
    if (in) {
      openSidecar();
      sidecar_wasopen = 0;
    } else {
      sidecar_wasopen = 1; 
    }
    if (t.getCurPage() != 1) {
      sidecar_oldpage = t.getCurPage();
      t.setCurPage(1);
    } else {
      sidecar_oldpage = -1;
    }
    sl = 0;
  } else {
    searchline.hide();
    editor.setXmlParam("h", "0");  
    searchline.setXmlParam("y", "0");  
    setPrivateString("pledit.poppler", "searchline", "in");
    searchline.onNotify("in", "", 0, 0);
    int page = t.getCurPage();
    if (!in) {
      if (page == 1) {
        if (sidecar_oldpage != -1) {
          t.setCurPage(sidecar_oldpage);
        }
      }
      if (page == 1 && !sidecar_wasopen) {
          closeSidecar();
      }
    }
    sl = 1;
  }
}

sendSelection(String msg) {
  Group g = getScriptGroup();
  GuiObject r = g.findObject("pledit.right");
  GuiObject selwnd = r.findObject("pledit.selection.window");
  if (selwnd != NULL) selwnd.sendAction(msg, "", 0, 0, 0, 0);
}

searchlineclose.onLeftClick() {
  toggleSearchline();
  messagebox("You have closed the Quick Selection box. This feature helps you\nfind and select specific items within a playlist. To access this\nfeature again, press the F3 key while the Playlist Editor window\nis active.", "Quick Selection", 0, "pledit_searchline_close");
}

editcontrol.onKeyDown(Int vkcode) {
  if (vkcode == VK_DOWN && isKeyDown(VK_CONTROL)) sendSelection("down");
  if (vkcode == VK_UP && isKeyDown(VK_CONTROL)) sendSelection("up");
  if (vkcode == 13 && isKeyDown(VK_CONTROL)) sendSelection("doubleclick");
}


