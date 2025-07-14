#include "../../../lib/std.mi"

Global Text txt;
Global Edit editor;
Global CfgGroup cgroup;

System.onScriptLoaded() {
  cgroup = getScriptGroup();
  txt = cgroup.getObject("cfg.txt");
  editor = cgroup.getObject("cfg.edit");
}

editor.onEnter() {
  cgroup.cfgSetString(getText());
}

cgroup.onCfgChanged() {
  txt.setText(cfgGetName());
  editor.setText(cfgGetString());
}
