@echo off
REM for %%a in (*.m) do "D:\Dev\cvs_winamp\Wasabi\mc.exe" /debug /df /d /dumpsym %%a
for %%a in (*.m) do "D:\Dev\cvs_winamp\Wasabi\mc.exe" %%a
@echo on