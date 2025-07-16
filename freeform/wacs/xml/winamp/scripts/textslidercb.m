#include <lib/std.mi>

Global Slider slidercb;
Global Text textobj;
Global String prefix, one, many;
Global Group pgroup;
Global GuiObject targetbase;

System.onScriptLoaded() {

  // we only run in groups!
  pgroup = getScriptGroup();
  if (pgroup == NULL) return;

  String par = getParam();

  String tid = getToken(par, ";", 0);
  String gid = getToken(par, ";", 1);

  // if the text object isnt there abort
  textobj = pgroup.findObject(tid);
  if (textobj == NULL) {
    messageBox("textslidercm.maki : text not found (param 0 = " + tid + ")", "Error", 0, "");
    return;
  }

  // if the base group isnt there or doesn't have a titlebox.text object, abort
  if (gid != "") {
    targetbase = pgroup.findObject(gid);
    if (targetbase == NULL) {
      messageBox("textslidercm.maki : text not found (param 0 = " + gid + ")", "Error", 0, "");
      return;
    }
  }

  String id = getToken(par, ";", 2);
  prefix = getToken(par, ";", 3);
  one = getToken(par, ";", 4);
  many = getToken(par, ";", 5);
  if (many == "") many = one;

  if (targetbase != NULL)
    slidercb = targetbase.findObject(id);
  else 
    slidercb = pgroup.findObject(id);
  if (slidercb == NULL) {
    messageBox("titleboxslidercm.make : slidercb not found (param 1 = " + id + ")", "Error", 0, "");
    return;
  }

  slidercb.onSetPosition(slidercb.getPosition());

}

slidercb.onSetPosition(int val) {
  String s = prefix + IntegerToString(val);
  if (val > 1) s = s + many; else s = s + one;
  textobj.setText(s);
}
