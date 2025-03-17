# Umami

Umami (/uːˈmɑːmi/ from Japanese: うま味 Japanese pronunciation: [ɯmami]) is a current Work In Progress project that aims to be compatible with Winamp3 and up skins.

Currently only partially processes the included ``skin.xml`` file to gather information about the ``WinampAbstractionLayer`` version, though it is planned that the complete file and subsequent ``<include file=""/>`` tags are parsed.

Currently parses the included ``skin.xml`` file to gather information about the ``WinampAbstractionLayer`` version, as well as information about the author and comments and more from ``<skininfo>``.

``<include file=""/>`` tags are parsed, and the subsequent xml files are then parsed as well, with the container and layout information parsed and stored to later spawn empty windows, ``<group/>`` and ``<groupdef/>`` tags are parsed as they appear in the order they were written down, starting from the main group that is called in the layout, although this behavior is wrong.

### TODO:
Skin Engine:
- [x] Compile to Linux
- [x] Compile to Windows using MinGW
- [ ] Compile to Windows using MSVC (in progress)
- [ ] ~~Get a basic Winamp/WACUP general purpose plug-in going~~
- [ ] Parse and understand the contents of the ``freeform`` folder found within Winamp
- skin.xml:
  - [x] Parse the contents of the file to gather information (done)
  - [ ] Perform version checking
    - How should that even be done? Is backwards compatibility really that necessary? (for supporting WasabiXML 1.35 yes but anything earlier?)
  - [x] Load and parse the other required XML files
  - [x] Print the information gathered from the elements and their attributes to console
  - [x] Spawn windows with the information gathered from the container/layout size and default positioning (Windows only)
    - [ ] Place images from parsed ``<layer/>`` tags and load the specified image gathered from parsing the ``<elements/>`` list
