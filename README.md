# Umami

Umami (/uːˈmɑːmi/ from Japanese: うま味 Japanese pronunciation: [ɯmami]) is a current Work In Progress project.

What initially started as a project to reimplement the Wasabi skinning framework (in the sense of understanding and rendering a Modern Skin without the need of the services Wasabi has internally), has now changed to be more of a generic parser. This may change in the future.

Features:

WasabiXML Parser:
- [x] Compile to Linux
- [x] Compile to Windows using MinGW
- [ ] Compile to Windows using MSVC (in progress)
- [ ] Parse and understand the contents of the ``freeform`` folder found within Winamp
- skin.xml:
  - [x] Parse the contents of the file to gather information and print to console
  - [x] Load and parse the other XML files included with the ``<include file="">`` tag and parameter
  - [x] Print the tags parsed to console
