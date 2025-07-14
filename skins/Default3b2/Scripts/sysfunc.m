/*--------------------------------------------------------------------
 System functions.

 This scripts handles various system functions, like toggling of some
 configuration options (crossfading for example), and temporary output
 in the song ticker (volume, seek, clicks on the player buttons).
--------------------------------------------------------------------*/

#include "../../../lib/std.mi"

Function setTempText(String txt);
Function emptyTempText();

Global Wac MainComp, Playlist;
Global Text Songticker;
Global Slider Volbar, Seeker, SeekGhost;
Global Timer Songtickertimer, Wobbler;
Global Int WobblerWay;

Class GuiObject HintObject;
Class ToggleButton HintToggleButton;

Global HintObject Play, Stop, Previous, Next, Pause, Thinger, Open, Eq, Ml, Pl;
Global HintToggleButton ToggleXFade, ToggleShuffle, ToggleRepeat;

System.onScriptUnloading() {
  delete Songtickertimer;
  delete Wobbler;
}

System.onScriptLoaded() {

  // Timers won't start until .start(); is called
  Songtickertimer = new Timer;
  Songtickertimer.setDelay(1000);

  // Get Main and playlist components
  MainComp = getWac("{3CBD4483-DC44-11d3-B608-000086340885}");
  Playlist = getWac("{45F3F7C1-A6F3-4EE6-A15E-125E92FC3F8D}");

  Layout mainnormal = getContainer("Main").getLayout("Normal");

  // Get buttons
  ToggleXFade = mainnormal.getObject("Crossfade");
  ToggleShuffle = mainnormal.getObject("Shuffle");
  ToggleRepeat = mainnormal.getObject("Repeat");

  // Get songticker, Volbar & Seeker
  Songticker = mainnormal.getObject("Songticker");
  Volbar = mainnormal.getObject("Volume");
  Seeker = mainnormal.getObject("Seeker");
  SeekGhost = mainnormal.getObject("SeekerGhost");

  // Get Various buttons
  Play = mainnormal.getObject("Play");
  Pause = mainnormal.getObject("Pause");
  Stop = mainnormal.getObject("Stop");
  Next = mainnormal.getObject("Next");
  Previous = mainnormal.getObject("Previous");
  Thinger = mainnormal.getObject("Thinger");
  Open = mainnormal.getObject("Eject");

  Eq = mainnormal.getObject("Eq");
  Ml = mainnormal.getObject("Ml");
  Pl = mainnormal.getObject("Pl");

  // Set buttons status
  if (ToggleXFade != NULL && MainComp != NULL) ToggleXFade.setActivated(MainComp.sendCommand("get_crossfade_enabled", 0, 0,""));
  if (ToggleShuffle != NULL && Playlist != NULL) ToggleShuffle.setActivated(Playlist.sendCommand("get_shuffle", 0, 0,""));
  if (ToggleRepeat != NULL && Playlist != NULL) ToggleRepeat.setActivated(Playlist.sendCommand("get_repeat", 0, 0,""));

  if (SeekGhost != NULL)
    SeekGhost.setAlpha(0);

  if (Thinger != NULL) {
    Wobbler = new Timer;
    Wobbler.setDelay(50);
  }

}

Songtickertimer.onTimer() {
  Songticker.setText("");
  stop();
}

Volbar.onSetPosition(int p) {
  Float f;
  f = p;
  f = f / 255 * 100;
  setTempText("Volume: " + System.integerToString(f) + "%");
}

Volbar.onSetFinalPosition(int p) {
  Songticker.setText("");
}

Seeker.onSetPosition(int p) {
  if (!SeekGhost) {
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

SeekGhost.onSetPosition(int p) {
  if (getalpha() == 0) setAlpha(128);
  Float f;
  f = p;
  f = f / 255 * 100;
  Float len = getPlayItemLength();
  if (len != 0) {
    int np = len * f / 100;
    setTempText("Seek to " + integerToTime(np) + " / " + integerToTime(len) + " (" + integerToString(f) + "%)");
  }
}

SeekGhost.onsetfinalposition(int p) {
  Songticker.setText("");
  SeekGhost.setAlpha(0);
}

HintToggleButton.onLeftButtonDown(int x, int y) {
  if (HintToggleButton == ToggleXFade) setTempText("Toggle crossfade");
  else if (HintToggleButton == ToggleRepeat) setTempText("Toggle repeat");
  else if (HintToggleButton == ToggleShuffle) setTempText("Toggle shuffle");
}

HintToggleButton.onLeftButtonUp(int x, int y) {
  emptyTempText();
}

HintToggleButton.onToggle(int onoff) {
  String cmd, txt;
  Wac dest;
  if (HintToggleButton == ToggleXFade) {
    cmd = "crossfade_enable";
    txt = "Crossfade";
    dest = MainComp;
  } else if (HintToggleButton == ToggleRepeat) {
    cmd = "repeat";
    txt = "Repeat";
    dest = Playlist;
  } else if (HintToggleButton == ToggleShuffle) {
    cmd = "shuffle";
    txt = "Shuffle";
    dest = Playlist;
  }
  if (dest) {
    dest.sendCommand(cmd, onoff, 0,"");
    String s;
    if (onoff) s = "on"; else s = "off";
    setTempText(txt + " now " + s);
  }
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
 WobblerWay = 1;
 Wobbler.start();
}

Thinger.onLeaveArea() {
 Wobbler.stop();
 setAlpha(255);
}

Wobbler.onTimer() {
  int curalpha = Thinger.getAlpha() + WobblerWay * 24;
  if (curalpha <= 96) { curalpha = 96; WobblerWay = -WobblerWay; }
  if (curalpha > 255) { curalpha = 255; WobblerWay = -WobblerWay; }
  Thinger.setAlpha(curalpha);
}

setTempText(String txt) {
  Songtickertimer.stop();
  Songticker.setText(txt);
  Songtickertimer.start();
}

emptyTempText() {
  Songticker.setText("");
  Songtickertimer.stop();
}

Songticker.onNotify(String s1, String s2, int i1, int i2) {
  setTempText(s1);
}
