#include <lib/std.mi>

Global Boolean startit;
Global GuiObject ThingerIcon;
Global Timer timor;

System.onScriptLoaded() {
  startit = 0;
  if (getPrivateInt("firststart", "done", 0) == 1) return;
  startit = 1;
}

System.onScriptUnloading() {
  if (timor != NULL) delete timor;
}

System.onPlay() {
  if (!startit) return;
  startit = 0;
  Layout l = getContainer("main").getLayout("normal");
  if (l == NULL) return;
  ThingerIcon = l.findObject("Thinger");
  if (ThingerIcon == NULL) return;
  ThingerIcon.onNotify("forcedwobbling", "", 1, 0);
  setPrivateInt("firststart", "done", 1);
  timor = new Timer;
  timor.setDelay(3000);
  timor.start();
}

timor.onTimer() {
  ThingerIcon.onNotify("forcedwobbling", "", 0, 0);
  timor.stop();
}
