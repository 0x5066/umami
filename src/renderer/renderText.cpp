#include "../skin/skin.h"
#include "render_shared.h"
#include <algorithm>

const char *charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZ\"@   0123456789….:()-'!_+\\/[]^&%,=$#ÂÖÄ?* ";

extern PlayerCore* PlayerCore;

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

float total_time = 0;

void format_time(unsigned int time, char *buffer, int width) {
    unsigned int display_time = time;

    unsigned int minutes = display_time / 60;
    unsigned int seconds = display_time % 60;

    sprintf(buffer, "%2u:%02u", minutes, seconds);

    // Ensure null-termination
    buffer[width] = '\0';
}

std::wstring formatTime(int milliseconds, int tos = 0) {
    // Convert milliseconds to seconds
    int totalSeconds = milliseconds / 1000;

    // Extract minutes and seconds
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    // Format as m:ss (because modern skins fucking SUCK)
    std::wstringstream ss;
    if (PlayerCore->isPlaying() == 1 || PlayerCore->isPlaying() == 3) {
        ss << minutes << L":"
            << std::setw(2) << std::setfill(L'0') << seconds;
    } else {
        switch(tos){
            case 0: ss << L"  :  ";
            case 1: ss << L"00:00"; // yet here, setting timeroffstyle to 1 makes it display mm:ss??? thanks for nothing nullsoft
            case 2: ss << L"";
            default: ss << L"  :  "; // i dont know what the default here is
        }
    }

    return ss.str();
}

std::wstring CreateSongTickerText() {
    // Convert the time to MM:SS format
    std::wstring getSongTime = formatTime(static_cast<int>(PlayerCore->getLength()));

    // Combine the strings
    if (PlayerCore->isPlaying() == 1 || PlayerCore->isPlaying() == 3) {
        return PlayerCore->getPlayingTitle() + L" (" + getSongTime + L")";
    } else if (PlayerCore->getKhz() == 0 && PlayerCore->getKbps() == 0 && PlayerCore->getChannels() == 0) {
        return PlayerCore->getPlayingTitle();
    }
    return PlayerCore->getPlayingTitle();
}

std::string wstring_to_string(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.to_bytes(wstr);
}

// does TTFs now
bool renderText(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH) {
    std::string content;
    std::string display = getAttr(elem, "display", "");
    std::string defaultText = getAttr(elem, "default", getAttr(elem, "text", "This is a test string"));
    std::transform(display.begin(), display.end(), display.begin(), ::tolower);

    int timeroffstyle;

    if (elem.attributes.count("timeroffstyle")) timeroffstyle = std::stoi(elem.attributes.at("timeroffstyle"));

    if (display == "songname") content = wstring_to_string(CreateSongTickerText());
    else if (display == "songinfo"){
        char channelstate[64];
        char buf[255];
        if ((PlayerCore->isPlaying() == 1 || PlayerCore->isPlaying() == 3) &&
           (PlayerCore->getKbps() == 0 || PlayerCore->getKhz() == 0 || PlayerCore->getChannels() == 0)) {
            snprintf(buf, sizeof(buf), ""); // setting NULL is probably a bad idea
           }
        else if (PlayerCore->isPlaying() == 1 || PlayerCore->isPlaying() == 3) {
            switch(PlayerCore->getChannels()) {
                case 1: snprintf(channelstate, sizeof(channelstate), "Mono"); break;
                case 2: snprintf(channelstate, sizeof(channelstate), "Stereo"); break;
                case 4: snprintf(channelstate, sizeof(channelstate), "Quad"); break;
                case 6: snprintf(channelstate, sizeof(channelstate), "5.1 Surround"); break;
                case 8: snprintf(channelstate, sizeof(channelstate), "7.1 Surround"); break;
                default: snprintf(channelstate, sizeof(channelstate), "Surround"); break;
            }
            snprintf(buf, sizeof(buf), "%dkbps %s %.1fkHz", PlayerCore->getKbps(), channelstate, PlayerCore->getRealKhz() / 1000.0f);
        } else {
            snprintf(buf, sizeof(buf), "");
        }
        content = buf;
    }
    else if (display == "songartist") content = "DJ Mike Llama";
    else if (display == "songtitle")  content = "Llama Whippin' Intro";
    else if (display == "songalbum")  content = "Beats of Burdon";
    else if (display == "songlength") content = "0:05";
    else if (display == "time") content = wstring_to_string(formatTime(PlayerCore->getOutputTime(), timeroffstyle));
    else if (display == "timeelapsed") content = "0:02";
    else if (display == "timeremaining") content = "-0:05";
    else if (display == "componentbucket") content = "media.library";
    else if (display == "songbitrate") content = std::to_string(PlayerCore->getKbps());
    else if (display == "songsamplerate") content = std::to_string(PlayerCore->getKhz());
    else if (display == "normalizer:status") content = "enabled";
    else content = defaultText;

    std::string text = content;
    //std::cout << "DEBUG: Rendering text: '" << text << "' with display mode: '" << display << "'\n";

    int x = 0, y = 0, w = parentW, h = parentH;
    if (elem.attributes.count("x")) x = std::stoi(elem.attributes.at("x"));
    if (elem.attributes.count("y")) y = std::stoi(elem.attributes.at("y"));
    if (elem.attributes.count("w")) w = std::stoi(elem.attributes.at("w"));
    if (elem.attributes.count("h")) h = std::stoi(elem.attributes.at("h"));
    if (elem.attributes.count("relatx") && elem.attributes.at("relatx") == "1") x = parentW + x;
    if (elem.attributes.count("relaty") && elem.attributes.at("relaty") == "1") y = parentH + y;
    if (elem.attributes.count("relatw") && elem.attributes.at("relatw") == "1") w = parentW + w;
    if (elem.attributes.count("relath") && elem.attributes.at("relath") == "1") h = parentH + h;

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

    int relatx = elem.attributes.count("relatx") ? std::stoi(elem.attributes.at("relatx")) : 0;
    int relaty = elem.attributes.count("relaty") ? std::stoi(elem.attributes.at("relaty")) : 0;
    int relatw = elem.attributes.count("relatw") ? std::stoi(elem.attributes.at("relatw")) : 0;
    int relath = elem.attributes.count("relath") ? std::stoi(elem.attributes.at("relath")) : 0;

    x = compute_dimension(elem.attributes.count("x") ? elem.attributes.at("x") : "0", relatx, parentW);
    y = compute_dimension(elem.attributes.count("y") ? elem.attributes.at("y") : "0", relaty, parentH);

    w = font.w;
    if (elem.attributes.count("w")) {
        w = compute_dimension(elem.attributes.at("w"), relatw, parentW);
    } else if (relatw == 2) {
        w = compute_dimension(std::to_string(font.w), relatw, parentW);
    }

    h = font.h;
    if (elem.attributes.count("h")) {
        h = compute_dimension(elem.attributes.at("h"), relath, parentH);
    } else if (relath == 2) {
        h = compute_dimension(std::to_string(font.h), relath, parentH);
    }

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
            SDL_Log("Failed to load TTF font %s: %s", path.c_str(), SDL_GetError());
            return false;
        }

        SDL_Color color = {255, 255, 255, 255}; // white text
        SDL_Surface* surface = TTF_RenderText_Blended(ttfFont, text.c_str(), text.size(), color);
        if (!surface) {
#ifdef DEBUG
            SDL_Log("Failed to render text: %s", SDL_GetError());
#endif // DEBUG
            TTF_CloseFont(ttfFont);
            return false;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);
        if (!texture) {
            TTF_CloseFont(ttfFont);
            return false;
        }

        int tw, th;
        //SDL_QueryTexture(texture, nullptr, nullptr, &tw, &th);

        int offsetX = 0;
        if (align == "center") offsetX = x + (w - tw) / 2;
        else if (align == "right") offsetX = x + w - tw;
        else offsetX = x; // left

        int offsetY = 0;
        if (valign == "center") offsetY = y + (h - th) / 2;
        else if (valign == "bottom") offsetY = y + h - th;
        else offsetY = y; // top

        SDL_FRect dst = { static_cast<float>(parentX + offsetX), static_cast<float>(parentY + offsetY), static_cast<float>(tw), static_cast<float>(th)};
        SDL_RenderTexture(renderer, texture, nullptr, &dst);
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

        SDL_FRect src = {
            static_cast<float>(font.x + col * charW),
            static_cast<float>(font.y + row * charH),
            static_cast<float>(charW),
            static_cast<float>(charH)
        };

        SDL_FRect dst = {
            static_cast<float>(parentX + x + offsetX + (int)i * (charW + spacingX)),
            static_cast<float>(parentY + y + offsetY),
            static_cast<float>(charW),
            static_cast<float>(charH)
        };
        if (dst.x + charW > parentX + x + w || dst.y + charH > parentY + y + h) continue;

        SDL_RenderTexture(renderer, texture, &src, &dst);
    }

    //SDL_DestroyTexture(texture);
    return true;
}

bool renderSongTicker(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH) {

    std::string text = "DJ Mike Llama - Llama Whippin' Intro (0:05)";

    int x = 0, y = 0, w = parentW, h = parentH;
    if (elem.attributes.count("x")) x = std::stoi(elem.attributes.at("x"));
    if (elem.attributes.count("y")) y = std::stoi(elem.attributes.at("y"));
    if (elem.attributes.count("w")) w = std::stoi(elem.attributes.at("w"));
    if (elem.attributes.count("h")) h = std::stoi(elem.attributes.at("h"));
    if (elem.attributes.count("relatx") && elem.attributes.at("relatx") == "1") x = parentW + x;
    if (elem.attributes.count("relaty") && elem.attributes.at("relaty") == "1") y = parentH + y;
    if (elem.attributes.count("relatw") && elem.attributes.at("relatw") == "1") w = parentW + w;
    if (elem.attributes.count("relath") && elem.attributes.at("relath") == "1") h = parentH + h;

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

    int relatx = elem.attributes.count("relatx") ? std::stoi(elem.attributes.at("relatx")) : 0;
    int relaty = elem.attributes.count("relaty") ? std::stoi(elem.attributes.at("relaty")) : 0;
    int relatw = elem.attributes.count("relatw") ? std::stoi(elem.attributes.at("relatw")) : 0;
    int relath = elem.attributes.count("relath") ? std::stoi(elem.attributes.at("relath")) : 0;

    x = compute_dimension(elem.attributes.count("x") ? elem.attributes.at("x") : "0", relatx, parentW);
    y = compute_dimension(elem.attributes.count("y") ? elem.attributes.at("y") : "0", relaty, parentH);

    w = font.w;
    if (elem.attributes.count("w")) {
        w = compute_dimension(elem.attributes.at("w"), relatw, parentW);
    } else if (relatw == 2) {
        w = compute_dimension(std::to_string(font.w), relatw, parentW);
    }

    h = font.h;
    if (elem.attributes.count("h")) {
        h = compute_dimension(elem.attributes.at("h"), relath, parentH);
    } else if (relath == 2) {
        h = compute_dimension(std::to_string(font.h), relath, parentH);
    }
    
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
            SDL_Log("Failed to load TTF font %s: %s", path.c_str(), SDL_GetError());
            return false;
        }

        SDL_Color color = {255, 255, 255, 255}; // white text
        SDL_Surface* surface = TTF_RenderText_Blended(ttfFont, text.c_str(), text.size(), color);
        if (!surface) {
#ifdef DEBUG
            SDL_Log("Failed to render text: %s", SDL_GetError());
#endif // DEBUG
            TTF_CloseFont(ttfFont);
            return false;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);
        if (!texture) {
            TTF_CloseFont(ttfFont);
            return false;
        }

        int tw, th;
        //SDL_QueryTexture(texture, nullptr, nullptr, &tw, &th);

        int offsetX = 0;
        if (align == "center") offsetX = x + (w - tw) / 2;
        else if (align == "right") offsetX = x + w - tw;
        else offsetX = x; // left

        int offsetY = 0;
        if (valign == "center") offsetY = y + (h - th) / 2;
        else if (valign == "bottom") offsetY = y + h - th;
        else offsetY = y; // top

        SDL_FRect dst = { static_cast<float>(parentX + offsetX), static_cast<float>(parentY + offsetY), static_cast<float>(tw), static_cast<float>(th)};
        SDL_RenderTexture(renderer, texture, nullptr, &dst);
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

        SDL_FRect src = {
            static_cast<float>(font.x + col * charW),
            static_cast<float>(font.y + row * charH),
            static_cast<float>(charW),
            static_cast<float>(charH)
        };

        SDL_FRect dst = {
            static_cast<float>(parentX + x + offsetX + (int)i * (charW + spacingX)),
            static_cast<float>(parentY + y + offsetY),
            static_cast<float>(charW),
            static_cast<float>(charH)
        };
        if (dst.x + charW > parentX + x + w || dst.y + charH > parentY + y + h) continue;

        SDL_RenderTexture(renderer, texture, &src, &dst);
    }

    //SDL_DestroyTexture(texture);
    return true;
}
