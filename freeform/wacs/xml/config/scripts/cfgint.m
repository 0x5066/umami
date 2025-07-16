#include <lib/std.mi>
#include "configinfobox.mi"

Global Text txt;
Global Edit editor;
Global CfgGroup cgroup;
Global Button butt;

System.onScriptLoaded() {
  cgroup = getScriptGroup();
  txt = cgroup.getObject("cfg.txt");
  editor = cgroup.getObject("cfg.edit");
  editor.setAutoEnter(true);
  butt = cgroup.getObject("cfg.toggle2");
}

editor.onEnter() {
  cgroup.cfgSetInt(stringToInteger(getText()));
}

cgroup.onCfgChanged() {
  txt.setText(cfgGetName());
  editor.setText(integerToString(cfgGetInt()));
}


butt.onLeftClick() {
 ConfigInfoBox();
}
