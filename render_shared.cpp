// Explicit instantiation for getAttr<UIElement>
// Explicit instantiation for getAttr<UIElement>
#include "skin.h"
#include <SDL2/SDL.h>
#include <string>
#include "render_shared.h"

template std::string getAttr<UIElement>(const UIElement&, const std::string&, const std::string&);
