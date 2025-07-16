
#include <lib/std.mi>

Global Button searchb, queryb;

System.onScriptLoaded() {
  Group g = getScriptGroup();
  searchb = g.getObject("library.search.label");
  queryb = g.getObject("library.query.label");
  queryb.hide();
  // restore mode queryline was in
  GuiObject line = g.getObject("library.query.line");
  line.setXmlParam("auto", getPrivateString("library", "auto", "1"));
}

System.onScriptUnloading() {
  // remember which mode the queryline was in
  if (searchb.isVisible()) setPrivateString("library", "auto", "1");
  else setPrivateString("library", "auto", "0");
}

searchb.onLeftClick() {
  Group g = getScriptGroup();
  hide();
  queryb.show();
//  GuiObject line = g.getObject("library.query.line");
}

queryb.onLeftClick() {
  Group g = getScriptGroup();
  hide();
  searchb.show();
}
