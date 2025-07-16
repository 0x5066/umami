#include <lib/std.mi>
#include "configinfobox.mi"

Global Text txt;
Global ToggleButton tgbutton;
Global CfgGroup cgroup;
Global Button butt;

System.onScriptLoaded() {
  cgroup = getScriptGroup();
  txt = cgroup.getObject("cfg.txt");
  tgbutton = cgroup.getObject("cfg.toggle");
  butt = cgroup.getObject("cfg.toggle2");
}

tgbutton.onToggle(int on) {
  cgroup.cfgSetInt(on);
}

cgroup.onCfgChanged() {
  txt.setText(cfgGetName());
  tgbutton.setActivated(cfgGetInt());
}

butt.onLeftClick() {
 if (!ConfigInfoBox())
   tgbutton.leftClick();
}
