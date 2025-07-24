#include "skin.h"
#include "render_shared.h"
#include <algorithm>

const char *charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZ\"@   0123456789….:()-'!_+\\/[]^&%,=$#ÂÖÄ?* ";

// from audacious' winamp skin mode, adjusted to include standard characters
static void lookup_char(char c, int* col, int* row) {
    switch (c) {
        case '"': *col = 26; *row = 0; return;
        case '@': *col = 27; *row = 0; return;
        case ' ': *col = 29; *row = 0; return;
        case ':': case ';': case '|': *col = 12; *row = 1; return;
        case '(': case '{': *col = 13; *row = 1; return;
        case ')': case '}': *col = 14; *row = 1; return;
        case '-': case '~': *col = 15; *row = 1; return;
        case '`': case '\'': *col = 16; *row = 1; return;
        case '!': *col = 17; *row = 1; return;
        case '_': *col = 18; *row = 1; return;
        case '+': *col = 19; *row = 1; return;
        case '\\': *col = 20; *row = 1; return;
        case '/': *col = 21; *row = 1; return;
        case '[': *col = 22; *row = 1; return;
        case ']': *col = 23; *row = 1; return;
        case '^': *col = 24; *row = 1; return;
        case '&': *col = 25; *row = 1; return;
        case '%': *col = 26; *row = 1; return;
        case '.': case ',': *col = 27; *row = 1; return;
        case '=': *col = 28; *row = 1; return;
        case '$': *col = 29; *row = 1; return;
        case '#': *col = 30; *row = 1; return;
        case '?': *col = 3; *row = 2; return;
        case '*': *col = 4; *row = 2; return;
        default: *col = 3; *row = 2; break; /* '?' */
    }
    char upper_char = std::toupper(static_cast<unsigned char>(c));
    const char *char_pos = strchr(charset, upper_char);
    int index = char_pos ? (char_pos - charset) : 67;
    *col = index % 31;
    *row = index / 31;
}

// does TTFs now
bool renderText(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH) {
    std::string content;
    std::string display = getAttr(elem, "display", "");
    std::string defaultText = getAttr(elem, "default", getAttr(elem, "text", "This is a test string"));
    std::transform(display.begin(), display.end(), display.begin(), ::tolower);

    if (display == "songname")        content = "DJ Mike Llama - Llama Whippin' Intro (0:05)";
    else if (display == "songinfo")   content = "56kbps Stereo 22.0khz";
    else if (display == "songartist") content = "DJ Mike Llama";
    else if (display == "songtitle")  content = "My Song";
    else if (display == "songalbum")  content = "Beats of Burdon";
    else if (display == "songlength") content = "0:05";
    else if (display == "time")       content = "0:02";
    else if (display == "timeelapsed") content = "0:02";
    else if (display == "timeremaining") content = "-0:05";
    else if (display == "componentbucket") content = "media.library";
    else if (display == "songbitrate") content = "56";
    else if (display == "songsamplerate") content = "22";
    else if (display == "normalizer:status") content = "enabled";
    else content = defaultText;

    std::string text = content;

    // Parse element position and size (fallback to parent)
    int x = std::stoi(getAttr(elem, "x", std::to_string(parentX)));
    int y = std::stoi(getAttr(elem, "y", std::to_string(parentY)));
    int w = std::stoi(getAttr(elem, "w", std::to_string(parentW)));
    int h = std::stoi(getAttr(elem, "h", std::to_string(parentH)));

    std::string fontId = getAttr(elem, "font", "");
    std::string align = getAttr(elem, "align", "left");
    std::string valign = getAttr(elem, "valign", "center");

    bool forceUpcase = getAttr(elem, "forceupcase", "0") == "1" || getAttr(elem, "forceuppercase", "0") == "1";
    bool forceLocase = getAttr(elem, "forcelocase", "0") == "1" || getAttr(elem, "forcelowercase", "0") == "1";
    int fontSize = std::stoi(getAttr(elem, "fontsize", "12"));

    // Resolve font bitmap, supporting file attribute as bitmap ID
    auto it = skin.bitmaps.find(fontId);
    if (it == skin.bitmaps.end()){
#ifdef DEBUG
        SDL_Log("Font with id '%s' not found in skin", fontId.c_str());
        SDL_Log("Should not happen, here we are though.");
        SDL_Log("What's the fallback here?");
#endif // DEBUG
        return false;
    }
    SkinBitmap& font = it->second;

    bool isBitmapFont = font.isFont;
#ifdef DEBUG
    SDL_Log("DEBUG: Rendering text '%s' with font '%s' at (%d, %d) size (%d x %d)", 
            text.c_str(), fontId.c_str(), x, y, w, h);
    // TrueType font handling
    //std::cout << "DEBUG: Font file: " << font.file << " " << path << std::endl;
    std::cout << "DEBUG: Font path: " << g_skinPath + font.file << std::endl;
#endif // DEBUG
    if (!font.isFont && font.file.find(".ttf") != std::string::npos) {
        // Convert pixel size to point size for TTF fonts
        const int screenDPI = 96; 
        int fontPtSize = int(fontSize * 72.0f / screenDPI);
        std::string path = g_skinPath + font.file;
        TTF_Font* ttfFont = TTF_OpenFont(path.c_str(), fontPtSize);
#ifdef DEBUG
        SDL_Log("DEBUG: TTF font path: %s", path.c_str());
#endif // DEBUG
        if (!ttfFont) {
            SDL_Log("Failed to load TTF font %s: %s", path.c_str(), TTF_GetError());
            return false;
        }

        SDL_Color color = {255, 255, 255, 255}; // white text
        SDL_Surface* surface = TTF_RenderUTF8_Blended(ttfFont, text.c_str(), color);
        if (!surface) {
#ifdef DEBUG
            SDL_Log("Failed to render text: %s", TTF_GetError());
#endif // DEBUG
            TTF_CloseFont(ttfFont);
            return false;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (!texture) {
            TTF_CloseFont(ttfFont);
            return false;
        }

        int tw, th;
        SDL_QueryTexture(texture, nullptr, nullptr, &tw, &th);

        int offsetX = 0;
        if (align == "center") offsetX = x + (w - tw) / 2;
        else if (align == "right") offsetX = x + w - tw;
        else offsetX = x; // left

        int offsetY = 0;
        if (valign == "center") offsetY = y + (h - th) / 2;
        else if (valign == "bottom") offsetY = y + h - th;
        else offsetY = y; // top

        SDL_Rect dst = { parentX + offsetX, parentY + offsetY, tw, th };
        SDL_RenderCopy(renderer, texture, nullptr, &dst);
        SDL_DestroyTexture(texture);
        TTF_CloseFont(ttfFont);
        return true;
    }

    SDL_Texture* texture = getOrLoadTexture(renderer, skin, font);

    int charW = font.charWidth;
    int charH = font.charHeight;
    int spacingX = font.hspacing;
    int spacingY = font.vspacing;

    int charsPerRow = (font.w > 0) ? font.w / charW : 1;

    if (forceUpcase)
        std::transform(text.begin(), text.end(), text.begin(), ::toupper);
    else if (forceLocase)
        std::transform(text.begin(), text.end(), text.begin(), ::tolower);

    int textW = (int)text.size() * (charW + spacingX) - spacingX;
    int textH = charH;

    int offsetX = 0;
    if (align == "center") offsetX = (w - textW) / 2;
    else if (align == "right") offsetX = w - textW;

    int offsetY = 0;
    if (valign == "center") offsetY = (h - textH) / 2;
    else if (valign == "bottom") offsetY = h - textH;

    for (size_t i = 0; i < text.size(); ++i) {
        char ch = text[i];
        int col = 0, row = 0;
        lookup_char(ch, &col, &row);

        SDL_Rect src = {
            font.x + col * charW,
            font.y + row * charH,
            charW,
            charH
        };

        SDL_Rect dst = {
            parentX + x + offsetX + (int)i * (charW + spacingX),
            parentY + y + offsetY,
            charW,
            charH
        };
        if (dst.x + charW > parentX + x + w || dst.y + charH > parentY + y + h) continue;

        SDL_RenderCopy(renderer, texture, &src, &dst);
    }

    //SDL_DestroyTexture(texture);
    return true;
}
