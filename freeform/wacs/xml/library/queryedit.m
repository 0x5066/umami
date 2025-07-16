#include <lib/std.mi>

Function sendQuery();

Global Group pgroup;
Global Edit editfield;

Global string lastquery;

Global Button toggle;
Global Boolean searchmode;
Global String prevstr;
Global QueryList qlist;

Function setSearchMode();

System.onScriptLoaded() {
  // we only run in groups!
  pgroup = getScriptGroup();
  if (pgroup == NULL) {
    messageBox("queryedit.maki can only run within a group", "Error", 0, "");
    return;
  }

  qlist = getScriptGroup().findObject("library.query.list");

  String id = getToken(getParam(), ";", 0);
  editfield = pgroup.findObject(id);
  if (editfield == NULL) {
    messageBox("queryedit.maki cannot find the edit field (param 0 = " + id + ")", "Error", 0, "");
  }

  toggle = pgroup.findObject("library.togglesearch");
  searchmode = getPrivateInt("library", "mode", true);
  prevstr = "";
  setSearchMode();
}

editfield.onEnter() {
  sendQuery();
}

editfield.onIdleEditUpdate() {
  if (lastquery == editfield.getText()) return;
  sendQuery();
}

sendQuery() {
  String id = getToken(getParam(), ";", 1);
  GuiObject queryline = getScriptGroup().findObject(id);
  lastquery = editfield.getText();
  if (queryline != NULL) {
    queryline.setXmlParam("auto", integerToString(searchmode));
    queryline.setXmlParam("query", lastquery);
  } else
    messageBox("queryedit.maki cannot find the queryline field (param 1 = " + id + ")", "Error", 0, "");
}

toggle.onLeftClick() {
  Edit e = pgroup.findObject("library.query.edit");
  String tmp = e.getText();
  e.setText(prevstr);
  prevstr = tmp;
  searchmode = !searchmode;
  setPrivateInt("library", "mode", searchmode);
  setSearchMode();
}

setSearchMode() {
  Text t = pgroup.findObject("Search for");
  if (searchmode == 0) {
    t.setText("Query:");
  } else if (searchmode == 1) {
    t.setText("Search For:");
  }
}

System.onAccelerator(String action, String section, String key) {
  if (key == "tab" && action == "jumper" && section == "pledit") {
  }
}

qlist.onResetQuery() {
  editfield.setText("");
  sendQuery();
}

