/*--------------------------------------------------------------------
 Main/Normal player group script.
--------------------------------------------------------------------*/

#include </lib/std.mi>
#include </lib/pldir.mi>

Function setTempText(String txt);
Function emptyTempText();

Global Text Songticker;
Global Slider Volbar, Seeker, SeekGhost;
Global Timer Wobbler;
Global Int WobblerWay, Seeking, forcedwobbling;

Class GuiObject HintObject;
Class ToggleButton HintToggleButton;

Global HintObject Play, Stop, Previous, Next, Pause, Thinger, Open, Eq, Ml, Pl;
Global HintToggleButton ToggleXFade, ToggleShuffle, ToggleRepeat;

System.onScriptUnloading() {
  delete Wobbler;
}

System.onScriptLoaded() {

  Group pgroup = getScriptGroup();

  // Get songticker, Volbar & Seeker
  Songticker = pgroup.findObject("Songticker");
  if (songticker == null) messagebox("ooch!!", "ooch", 0, "");
  Volbar = pgroup.findObject("Volume");
  Seeker = pgroup.findObject("Seeker");
  SeekGhost = pgroup.findObject("SeekerGhost");

  // Get Various buttons
  Play = pgroup.findObject("Play");
  Pause = pgroup.findObject("Pause");
  Stop = pgroup.findObject("Stop");
  Next = pgroup.findObject("Next");
  Previous = pgroup.findObject("Previous");
  Thinger = pgroup.findObject("Thinger");
  Open = pgroup.findObject("Eject");

  Eq = pgroup.findObject("Eq");
  Ml = pgroup.findObject("Ml");
  Pl = pgroup.findObject("Pl");

  if (SeekGhost != NULL)
    SeekGhost.setAlpha(1);

  forcedwobbling = 0;
  if (Thinger != NULL) {
    Wobbler = new Timer;
    Wobbler.setDelay(50);
  }

}

Songticker.onLeftButtonDblClk(int x, int y) {
  PlDir.showCurrentlyPlayingEntry();
}

Volbar.onSetPosition(int p) {
  Float f;
  f = p;
  f = f / 255 * 100;
  setTempText("Volume: " + System.integerToString(f) + "%");
}

Volbar.onSetFinalPosition(int p) {
  Songticker.setAlternateText("");
}

Seeker.onSetPosition(int p) {
  if (!SeekGhost && seeking) {
    Float f;
    f = p;
    f = f / 255 * 100;
    Float len = getPlayItemLength();
    if (len != 0) {
      int np = len * f / 100;
      setTempText("Seek to " + integerToTime(np) + " / " + integerToTime(len) + " (" + integerToString(f) + "%)");
    }
  }
}

Seeker.onLeftButtonDown(int x, int y) {
  seeking = 1;
}

Seeker.onLeftButtonUp(int x, int y) {
  seeking = 0;
  setTempText("");
}

SeekGhost.onSetPosition(int p) {
  if (getalpha() == 1) return;
  Float f;
  f = p;
  f = f / 255 * 100;
  Float len = getPlayItemLength();
  if (len != 0) {
    int np = len * f / 100;
    setTempText("Seek to " + integerToTime(np) + " / " + integerToTime(len) + " (" + integerToString(f) + "%)");
  }
}

SeekGhost.onLeftButtonDown(int x, int y) {
  SeekGhost.setAlpha(128);
}

SeekGhost.onLeftButtonUp(int x, int y) {
  SeekGhost.setAlpha(1);
}

Seeker.onSetFinalPosition(int p) {
  Songticker.setAlternateText("");
}

SeekGhost.onsetfinalposition(int p) {
  Songticker.setAlternateText("");
  SeekGhost.setAlpha(1);
}

HintObject.onLeftButtonDown(int x, int y) {
  if (HintObject == Play) setTempText("Play");
  else if (HintObject == Stop) setTempText("Stop");
  else if (HintObject == Pause) setTempText("Pause");
  else if (HintObject == Next) setTempText("Next");
  else if (HintObject == Previous) setTempText("Previous");
  else if (HintObject == Thinger) setTempText("Thinger");
  else if (HintObject == Open) setTempText("Open");
  else if (HintObject == Eq) setTempText("Equalizer");
  else if (HintObject == ML) setTempText("Media Library");
  else if (HintObject == Pl) setTempText("Playlist Editor");
}

HintObject.onLeftButtonUp(int x, int y) {
  emptyTempText();
}

Thinger.onEnterArea() {
 if (forcedwobbling) return;
 WobblerWay = 1;
 Wobbler.start();
}

Thinger.onLeaveArea() {
 if (forcedwobbling) return;
 Wobbler.stop();
 setAlpha(255);
}

Thinger.onRightButtonUp(int x, int y) {
  windowMenu();
  complete;
}

Wobbler.onTimer() {
  int curalpha = Thinger.getAlpha() + WobblerWay * 24;
  if (curalpha <= 96) { curalpha = 96; WobblerWay = -WobblerWay; }
  if (curalpha > 255) { curalpha = 255; WobblerWay = -WobblerWay; }
  Thinger.setAlpha(curalpha);
}

setTempText(String txt) {
  Songticker.setAlternateText(txt);
}

emptyTempText() {
  Songticker.setAlternateText("");
}

Thinger.onNotify(String msg, String param, int p1, int p2) {
  if (msg == "forcedwobbling") {
    if (p1) {
      forcedwobbling = 1;
      WobblerWay = 1;
      Wobbler.start();
    } else {
      forcedwobbling = 0;
      Wobbler.stop();
      Thinger.setAlpha(255);
    }
  }
}
