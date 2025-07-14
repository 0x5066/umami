#include <lib/std.mi>

Function onVideoResize(int w);

Global Group pGroup;
Global Group visGroup;
Global Int skipNextResizeEvent;

System.onScriptLoaded() {
  pgroup = getScriptGroup();
  visGroup = pgroup.findObject("video.visualization");
  onVideoResize(pgroup.getWidth());
}

pgroup.onResize(int x, int y, int w, int h) {
  onVideoResize(w);
}

onVideoResize(int w) {
  if (w < 318) {
    visGroup.hide();
    return;
  } else {
    visGroup.show();
    return;
  }
}

