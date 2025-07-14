#include "skin.h"
#include "render_shared.h"
#include <algorithm>

// doesnt do TTFs for now
bool renderText(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH) {
    // Resolve display content (new addition)
    std::string content;
    std::string display = getAttr(elem, "display", "");
    std::string defaultText = getAttr(elem, "default", getAttr(elem, "text", ""));

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

    auto it = skin.bitmaps.find(fontId);
    if (it == skin.bitmaps.end()) return false;

    const SkinBitmap& font = it->second;
    bool isBitmapFont = font.isFont;

    std::string path = g_skinPath + font.file;
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) return false;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) return false;

    int charW = font.charWidth;
    int charH = font.charHeight;
    int spacingX = font.hspacing;
    int spacingY = font.vspacing;
    int charsPerRow = (font.w > 0) ? font.w / charW : 1;

    const wchar_t *charset = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ\"@   0123456789….:()-'!_+\\/[]^&%,=$#ÂÖÄ?* ";

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
