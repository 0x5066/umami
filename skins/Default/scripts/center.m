#include <lib/std.mi>

Global GuiObject obj;
Global Group parentGroup;

System.onScriptLoaded() {
  parentGroup = getScriptGroup();
  obj = parentGroup.getObject(getParam());
}

parentGroup.onResize(int x, int y, int w, int h) {
  int _w = obj.getWidth();
  int _h = obj.getHeight();
  obj.resize((w-_w)/2, (h-_h)/2, _w, _h);
}

