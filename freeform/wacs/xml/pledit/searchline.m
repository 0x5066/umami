
#include "..\..\..\lib\std.mi"

Global Text label;
Global Edit searchtext;

System.onScriptLoaded() {
  Group g = getScriptGroup();
  searchtext = g.findObject("pledit.search.text");
  label = g.findObject("pledit.search.label");
}

searchtext.onEnter() {
  onIdleEditUpdate();
}

searchtext.onIdleEditUpdate() {
  Group g = getScriptGroup();
  GuiObject ed = g.getObject("pledit.editor");
  ed.setXmlParam("select", getText());
}

label.onLeftButtonDblClk(int x, int y) {
  searchtext.setText("");
  searchtext.onIdleEditUpdate();
}
