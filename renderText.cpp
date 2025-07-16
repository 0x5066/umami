#include "skin.h"
#include "render_shared.h"
#include <algorithm>

// doesnt do TTFs for now
bool renderText(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH) {
    std::string content;
    std::string display = getAttr(elem, "display", "");
    std::string defaultText = getAttr(elem, "default", getAttr(elem, "text", ""));
    std::transform(display.begin(), display.end(), display.begin(), ::tolower);

    if (display == "songname")        content = "1. DJ Mike Llama - Llama Whippin' Intro (0:05)";
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

    auto it = skin.bitmaps.find(fontId);
    if (it == skin.bitmaps.end()){
        SDL_Log("Font with id '%s' not found in skin", fontId.c_str());
        SDL_Log("Should not happen, here we are though.");
        return false;
    }
    const SkinBitmap& font = it->second;
    std::string path = g_skinPath + font.file;
    bool isBitmapFont = font.isFont;

    SDL_Log("DEBUG: Rendering text '%s' with font '%s' at (%d, %d) size (%d x %d)", 
            text.c_str(), fontId.c_str(), x, y, w, h);

    // TrueType font handling
    std::cout << "DEBUG: Font file: " << font.file << " " << path << std::endl;
    std::cout << "DEBUG: Font path: " << g_skinPath + font.file << std::endl;

    if (!font.isFont && font.file.find(".ttf") != std::string::npos) {
        std::string path = g_skinPath + font.file;
        TTF_Font* ttfFont = TTF_OpenFont(path.c_str(), fontSize);
        SDL_Log("DEBUG: TTF font path: %s", path.c_str());
        if (!ttfFont) {
            SDL_Log("Failed to load TTF font %s: %s", path.c_str(), TTF_GetError());
            return false;
        }

        SDL_Color color = {255, 255, 255, 255}; // white text
        SDL_Surface* surface = TTF_RenderUTF8_Blended(ttfFont, text.c_str(), color);
        if (!surface) {
            SDL_Log("Failed to render text: %s", TTF_GetError());
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

    SDL_Surface* surface;
    
    surface = IMG_Load(path.c_str());
    std::cout << "DEBUG: " << "!: " << path << "" << std::endl;

    if (!surface){ // try redirecting to freeform
        SDL_Log("Could not find file %s - using fallback", path.c_str());
        std::string wasabiPath = "freeform/xml/wasabi/" + font.file;
        std::cout << "DEBUG: new fallback: " << wasabiPath << std::endl;
        surface = IMG_Load(wasabiPath.c_str());
        if (!surface) {
			SDL_Log("Could not find file in fallback %s", wasabiPath.c_str());
			return false;
        }
        if (!surface) {
            SDL_Log("FUCK ERROR: Could not find bitmap file: %s", font.file.c_str());
        }
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) return false;

    int charW = font.charWidth;
    int charH = font.charHeight;
    int spacingX = font.hspacing;
    int spacingY = font.vspacing;
    int charsPerRow = (font.w > 0) ? font.w / charW : 1;

    #if defined(__linux__)
        const wchar_t* charset = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ\"@   0123456789….:()-'!_+\\/[]^&%,=$#ÂÖÄ?* ";

    #else
    const wchar_t* charset = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ\"@   0123456789 .:()-'!_+\\/[]^&%,=$#ÂÖÄ?* "; // windows doesnt like … (u2026) - FUCK WINDOWS
#endif
    // Mapping indices for each character in the charset
    const int charset_indices[] = {
        0, // A
        1, // B
        2, // C
        3, // D
        4, // E
        5, // F
        6, // G
        7, // H
        8, // I
        9, // J
        10, // K
        11, // L
        12, // M
        13, // N
        14, // O
        15, // P
        16, // Q
        17, // R
        18, // S
        19, // T
        20, // U
        21, // V
        22, // W
        23, // X
        24, // Y
        25, // Z
        26, // "
        27, // @
        67, // BLANK
        67, // BLANK
        67, // BLANK
        31, // 0
        32, // 1
        33, // 2
        34, // 3
        35, // 4
        36, // 5
        37, // 6
        38, // 7
        39, // 8
        40, // 9
        41, // …
        42, // .
        43, // =
        44, // (
            45, // )
            46, // -
            47, // '
            48, // !
            49, // _
            50, // +
            51, // backslash
            52, // slash
            53, // [
            54, // ]
            55, // ^
            56, // &
            57, // %
            58, // ,
            59, // =
            60, // $
            61, // #
            62, // Â
            63, // Ö
            64, // Ä
            65, // ?
            66, // *
            67  // BLANK (for remaining undefined characters)
    };

    if (isBitmapFont || forceUpcase)
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
        wchar_t ch = (unsigned char)text[i];
        const wchar_t* found = wcschr(charset, ch);
        int index = (found ? found - charset : 67);

        int mapped = charset_indices[index];
        int col = mapped % charsPerRow;
        int row = mapped / charsPerRow;

        SDL_Rect src = {
            font.x + col * charW,
            font.y + row * charH,
            charW,
            charH
        };

        SDL_Rect dst = {
            x + offsetX + (int)i * (charW + spacingX),
            y + offsetY,
            charW,
            charH
        };
        if (dst.x + charW > x + w || dst.y + charH > y + h) continue;

        SDL_RenderCopy(renderer, texture, &src, &dst);
    }

    SDL_DestroyTexture(texture);
    return true;
}
