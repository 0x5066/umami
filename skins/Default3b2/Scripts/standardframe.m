#include "../../../lib/std.mi"

Global Group frameGroup, content;
Global String x, y, w, h, rx, ry, rw, rh;

Function setNewGroup(String groupid);

System.onScriptLoaded() {
  frameGroup = getScriptGroup();
  String param = getParam();
  x = getToken(param, ",", 0);
  y = getToken(param, ",", 1);
  w = getToken(param, ",", 2);
  h = getToken(param, ",", 3);
  rx = getToken(param, ",", 4);
  ry = getToken(param, ",", 5);
  rw = getToken(param, ",", 6);
  rh = getToken(param, ",", 7);
}

frameGroup.onNotify(String cmd, String param, int a, int b) {
  if (getToken(cmd, ",", 0) == "content") {
    setNewGroup(getToken(cmd, ",", 1));
  }
}

setNewGroup(String groupid) {
  content = newGroup(groupid);
  if (!content) {
    messagebox("group \"" + groupid + "\" not found", "ButtonGroup", 0, "");
    return;
  }
  content.setXmlParam("x", x);
  content.setXmlParam("y", y);
  content.setXmlParam("w", w);
  content.setXmlParam("h", h);
  content.setXmlParam("relatx", rx);
  content.setXmlParam("relaty", ry);
  content.setXmlParam("relatw", rw);
  content.setXmlParam("relath", rh);
  content.init(frameGroup);
}
