# Umami

Umami (/uːˈmɑːmi/ from Japanese: うま味 Japanese pronunciation: [ɯmami]) is a current Work In Progress that aims to be compatible with Winamp3 and up skins.

Currently only partially processes the included ``skin.xml`` file to gather information about the ``WinampAbstractionLayer`` version, though it is planned that the complete file and subsequent ``<include file=""\>`` tags are parsed.

### TODO:
Skin Engine:
- [x] Compile to Linux
- [ ] Compile to Windows using MinGW
- [ ] Compile to Windows using MSVC
- [ ] Get a basic Winamp/WACUP general purpose plug-in going
- skin.xml:
  - [x] Parse the contents of the file to gather information (partially done)
  - [ ] Perform version checking
  - [ ] Load and parse the other required XML files
  - [ ] Print the information gathered from the elements and their attributes to console
