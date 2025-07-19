#include "skin.h"
#include "render_shared.h"
#include <vector>
#include <algorithm>

int VISWIDTH = 72;
int VISHEIGHT = 16;

int SHIFT_AMOUNT = 0;
int DIRECTION = 1;

typedef struct {
    Uint8 r, g, b;
} Color;

static Color osc_colors[16];

Color colors[] = {
    {255, 255, 255},  // colorallbands
    // dunno why but its swapped around
    {24, 132, 8},     // 1 = bottom of spec
    {41, 148, 0},     // 2
    {49, 156, 8},     // 3
    {57, 181, 16},    // 4
    {50, 190, 16},    // 5
    {41, 206, 16},    // 6
    {148, 222, 33},   // 7
    {189, 222, 41},   // 8
    {214, 181, 33},   // 9
    {222, 165, 24},   // 10
    {198, 123, 8},    // 11
    {214, 115, 0},    // 12
    {214, 102, 0},    // 13
    {214, 90, 0},     // 14
    {206, 41, 16},    // 15
    {239, 49, 16},    // color 16 = top of spec
    {255, 255, 255},  // colorallosc
    {255, 255, 255},  // 18 = osc 1
    {214, 214, 222},  // 19 = osc 2 (slightly dimmer)
    {181, 189, 189},  // 20 = osc 3
    {160, 170, 175},  // 21 = osc 4
    {148, 156, 165},  // 22 = osc 5
    {150, 150, 150}   // 23 = analyzer peak dots
};

Color* osccolors(const Color* colors) {

    osc_colors[0] = colors[22];
    osc_colors[1] = colors[22];
    osc_colors[2] = colors[21];
    osc_colors[3] = colors[21];
    osc_colors[4] = colors[20];
    osc_colors[5] = colors[20];
    osc_colors[6] = colors[19];
    osc_colors[7] = colors[19];
    osc_colors[8] = colors[20];
    osc_colors[9] = colors[20];
    osc_colors[10] = colors[21];
    osc_colors[11] = colors[21];
    osc_colors[12] = colors[22];
    osc_colors[13] = colors[22];
    osc_colors[14] = colors[23];
    osc_colors[15] = colors[23];

    return osc_colors;
}

// Enum for spectroscope bands (1-16) and oscilloscope segments (1-5), plus "all" and "peak"
enum VisColorType {
    COLOR_ALL_BANDS,
    COLOR_BAND_1, COLOR_BAND_2, COLOR_BAND_3, COLOR_BAND_4,
    COLOR_BAND_5, COLOR_BAND_6, COLOR_BAND_7, COLOR_BAND_8,
    COLOR_BAND_9, COLOR_BAND_10, COLOR_BAND_11, COLOR_BAND_12,
    COLOR_BAND_13, COLOR_BAND_14, COLOR_BAND_15, COLOR_BAND_16,
    COLOR_BAND_PEAK,
    COLOR_ALL_OSC,
    COLOR_OSC_1, COLOR_OSC_2, COLOR_OSC_3, COLOR_OSC_4, COLOR_OSC_5,
    COLOR_TOTAL
};

void shift_vector_to_right(std::vector<double>& vec) {
    int size = vec.size();
    // Update direction if necessary
    if (SHIFT_AMOUNT >= size - 1 && DIRECTION == 1) {
        // Reached the end, change direction to negative
        DIRECTION = -1;
    } else if (SHIFT_AMOUNT <= 0 && DIRECTION == -1) {
        // Reached the beginning, change direction to positive
        DIRECTION = 1;
    }

    // Update shift amount based on direction
    SHIFT_AMOUNT += DIRECTION;

    // Ensure SHIFT_AMOUNT stays within valid range
    if (SHIFT_AMOUNT >= size) {
        SHIFT_AMOUNT = size - 1;
    } else if (SHIFT_AMOUNT < 0) {
        SHIFT_AMOUNT = 0;
    }

    // Perform rotation based on direction
    if (DIRECTION == 1) {
        std::rotate(vec.rbegin(), vec.rbegin() + 1, vec.rend());
    } else if (DIRECTION == -1) {
        std::rotate(vec.begin(), vec.begin() + 1, vec.end());
    }
    // Uncomment the following line for debugging purposes
    // std::cout << SHIFT_AMOUNT << std::endl;
}

SDL_RendererFlip setSDLFlip(SDL_Renderer* renderer, bool flipv, bool fliph) {
    SDL_RendererFlip flip = SDL_FLIP_VERTICAL;

    if (flipv) flip = static_cast<SDL_RendererFlip>(SDL_FLIP_NONE);
    if (fliph) flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_HORIZONTAL);

    return flip;
}

bool sa_thick = true;

int last_y = 0;
int top = 0, bottom = 0, color_index = 0;
//unsigned char sample[150] = {15}; 
static float sapeaks[75];
static float safalloff[75];
int sadata2[75];
static float sadata3[75];

int VisMode;

int bar_falloff[5] = {3, 6, 12, 16, 32};
float peak_falloff[5] = {1.05f, 1.1f, 1.2f, 1.4f, 1.6f};

wchar_t* oscstyle = L"lines"; // lines, dots, solid
wchar_t* bandwidth = L"thick"; // thin, thick
wchar_t* coloring = L"normal"; // normal, lines, fire

Color visColors[COLOR_TOTAL]; // Holds all parsed visual colors

Color parseRGB(const std::string& str) {
    Color c = {0, 0, 0};
    sscanf(str.c_str(), "%hhu,%hhu,%hhu", &c.r, &c.g, &c.b);
    return c;
}

void loadVisColors(const UIElement& elem) {
    const std::string keys[] = {
        "colorallbands",
        "colorband1", "colorband2", "colorband3", "colorband4",
        "colorband5", "colorband6", "colorband7", "colorband8",
        "colorband9", "colorband10", "colorband11", "colorband12",
        "colorband13", "colorband14", "colorband15", "colorband16",
        "colorbandpeak",
        "colorallosc",
        "colorosc1", "colorosc2", "colorosc3", "colorosc4", "colorosc5"
    };

    // fallback defaults from original static `colors` array
    const int fallback_indices[COLOR_TOTAL] = {
        1,  // all bands
        2, 3, 4, 5, 6, 7, 8, 9,
        10, 11, 12, 13, 14, 15, 16, 17,  // band colors
        23,  // peak
        18,  // all osc
        18, 19, 20, 21, 22  // osc 1–5
    };

    for (int i = 0; i < COLOR_TOTAL; ++i) {
        auto it = elem.attributes.find(keys[i]);
        if (it != elem.attributes.end()) {
            visColors[i] = parseRGB(it->second);
        } else {
            visColors[i] = colors[fallback_indices[i]];
            #ifdef DEBUG
            std::cout << "DEBUG: visColors[" << i << "] fallback to " 
                      << static_cast<int>(visColors[i].r) << ", "
                      << static_cast<int>(visColors[i].g) << ", "
                      << static_cast<int>(visColors[i].b) << std::endl;
            #endif // DEBUG
        }
    }
}

// draw a pixel on the surface at (x, y) with a given color
void putPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
    // bounds check
    if (x < 0 || x >= surface->w || y < 0 || y >= surface->h) return;

    // lock surface if needed
    if (SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);

    // get pointer to the pixel buffer
    Uint8 *pixels = (Uint8 *)surface->pixels;
    Uint32 *pixelPtr = (Uint32 *)(pixels + y * surface->pitch + x * sizeof(Uint32));
    *pixelPtr = color;

    if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
}

// Renders a "layer" (image) at x, y, width, height relative to parent
// stub for button, togglebutton, NStatesbutton, AnimatedLayer
bool renderVis(SDL_Renderer* renderer, Skin& skin, const UIElement& elem, int parentX, int parentY, int parentW, int parentH) {
    // Log input parameters
    /* SDL_Log("[renderLayer] tag=%s id=%s parent=(%d,%d,%d,%d)", 
        elem.tag.c_str(),
        elem.attributes.count("id") ? elem.attributes.at("id").c_str() : "(none)",
        parentX, parentY, parentW, parentH); */

    /* Extract and log all relevant attributes
    auto get = [&](const char* key) -> const char* {
        auto it = elem.attributes.find(key);
        return it != elem.attributes.end() ? it->second.c_str() : "(none)";
    };
    SDL_Log("[renderLayer] attrs: x=%s y=%s w=%s h=%s relatx=%s relaty=%s relatw=%s relath=%s fitparent=%s image=%s",
        get("x"), get("y"), get("w"), get("h"),
        get("relatx"), get("relaty"), get("relatw"), get("relath"),
        get("fitparent"), get("image")); */

    loadVisColors(elem);
    Color* osc_colors = osccolors(visColors);

    // Existing code for computing x, y, w, h
    int x = 0, y = 0, w = parentW, h = parentH, alpha = 255;
    if (elem.attributes.count("x")) x = std::stoi(elem.attributes.at("x"));
    if (elem.attributes.count("y")) y = std::stoi(elem.attributes.at("y"));
    if (elem.attributes.count("w")) w = std::stoi(elem.attributes.at("w"));
    if (elem.attributes.count("h")) h = std::stoi(elem.attributes.at("h"));
    if (elem.attributes.count("relatx") && elem.attributes.at("relatx") == "1") x = parentW + x;
    if (elem.attributes.count("relaty") && elem.attributes.at("relaty") == "1") y = parentH + y;
    if (elem.attributes.count("relatw") && elem.attributes.at("relatw") == "1") w = parentW + w;
    if (elem.attributes.count("relath") && elem.attributes.at("relath") == "1") h = parentH + h;
    if (elem.attributes.count("alpha")) alpha = std::stoi(elem.attributes.at("alpha"));
#ifdef DEBUG
    SDL_Log("[renderVis] computed: x=%d y=%d w=%d h=%d (final rect: %d,%d,%d,%d)", x, y, w, h, parentX + x, parentY + y, w, h);
#endif // DEBUG
    int relatx = elem.attributes.count("relatx") ? std::stoi(elem.attributes.at("relatx")) : 0;
    int relaty = elem.attributes.count("relaty") ? std::stoi(elem.attributes.at("relaty")) : 0;
    int relatw = elem.attributes.count("relatw") ? std::stoi(elem.attributes.at("relatw")) : 0;
    int relath = elem.attributes.count("relath") ? std::stoi(elem.attributes.at("relath")) : 0;

    VisMode = elem.attributes.count("mode") ? std::stoi(elem.attributes.at("mode")) : 1;

    bool flipv = elem.attributes.count("flipv") && elem.attributes.at("flipv") == "1";
    bool fliph = elem.attributes.count("fliph") && elem.attributes.at("fliph") == "1";

    x = compute_dimension(elem.attributes.count("x") ? elem.attributes.at("x") : "0", relatx, parentW);
    y = compute_dimension(elem.attributes.count("y") ? elem.attributes.at("y") : "0", relaty, parentH);

    w = VISWIDTH;
    if (elem.attributes.count("w")) {
        w = compute_dimension(elem.attributes.at("w"), relatw, parentW);
    } else if (relatw == 2) {
        w = compute_dimension(std::to_string(VISWIDTH), relatw, parentW);
    }

    h = VISHEIGHT;
    if (elem.attributes.count("h")) {
        h = compute_dimension(elem.attributes.at("h"), relath, parentH);
    } else if (relath == 2) {
        h = compute_dimension(std::to_string(VISHEIGHT), relath, parentH);
    }

    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(
    0, VISWIDTH, VISHEIGHT, 32, SDL_PIXELFORMAT_RGBA32);

    // Shift sample data
    shift_vector_to_right(sample);

        for (int vx = 0; vx < 75; vx++) {

        //int vy = sample[vx]; // not right since it uses the same data as the spectrum analyzer
        int vy = sample[vx];

            vy = vy < 0 ? 0 : (vy > 16 - 1 ? 16 - 1 : vy);

            if (vx == 0) {
                last_y = vy;
            }

            top = vy;
            bottom = last_y;
            last_y = vy;

            if (wcscmp(oscstyle, L"lines") == 0 ) {
                if (bottom < top) {
                    int temp = bottom;
                    bottom = top;
                    top = temp + 1;
                }
            } else if (wcscmp(oscstyle, L"solid") == 0 ) {
                if (vy >= 8) {
                    top = 8;
                    bottom = vy;
                } else {
                    top = vy;
                    bottom = 7;
                }
            } else if (wcscmp(oscstyle, L"dots") == 0 ) {
                top = vy;
                bottom = vy;
            }


            if (VisMode == 2){
                for (int dy = top; dy <= bottom; dy++) {
                    int color_index = vy;
                    Color scope_color = osc_colors[color_index];
                    //SDL_SetRenderDrawColor(surfaceRenderer, scope_color.r, scope_color.g, scope_color.b, alpha);
                    //SDL_RenderDrawPoint(surfaceRenderer, vx, dy);
                    putPixel(surface, vx, dy, SDL_MapRGB(surface->format, scope_color.r, scope_color.g, scope_color.b));
                }
            }
        
        // i hate this so much, for some reason it doesnt draw accurately
        // i need to go back into the decompilation mines (ghidra) and maybe
        // copy shit verbatim

        // WHY?! WHY DO I HAVE TO DO THIS?! NOWHERE IS THIS IN THE DECOMPILE
        int i = ((i = vx & 0xfffffffc) < 72) ? i : 71; // Limiting i to prevent out of bounds access
            if (sa_thick == true) {
                    // this used to be unoptimized and came straight out of ghidra
                    // here's what that looked like
                    /* uVar12 =  (int)((u_int)*(byte *)(i + 3 + sadata) +
                                    (u_int)*(byte *)(i + 2 + sadata) +
                                    (u_int)*(byte *)(i + 1 + sadata) +
                                    (u_int)*(byte *)(i + sadata)) >> 2; */

                    int uVar12 = (int)((sample[i+3] + sample[i+2] + sample[i+1] + sample[i]) / 4);

                    // shove the data from uVar12 into sadata2
                    sadata2[vx] = uVar12;
            } else if (sa_thick == false) { // just copy sadata to sadata2
                sadata2[vx] = (int)sample[vx];
            }

        safalloff[vx] = safalloff[vx] - (bar_falloff[2] / 16.0f);

        // okay this is really funny
        // somehow the internal vis data for winamp/wacup can just, wrap around
        // but here it didnt, until i saw my rect drawing *under* its intended area
        // and i just figured out that winamp's vis box just wraps that around
        // this is really funny to me
        if (sadata2[vx] < 0) {
            sadata2[vx] = sadata2[vx] + 127;
        }
        if (sadata2[vx] >= 15) {
            sadata2[vx] = 15;
        }

        if (safalloff[vx] <= sadata2[vx]) {
            safalloff[vx] = sadata2[vx];
        }

    if (sapeaks[vx] <= (int)(safalloff[vx] * 256)) {
        sapeaks[vx] = safalloff[vx] * 256;
        sadata3[vx] = 3.0f;
    }

    int intValue2 = (sapeaks[vx]/256);

    sapeaks[vx] -= (int)sadata3[vx];
    sadata3[vx] *= (peak_falloff[1]);
    
    if (sapeaks[vx] < 0) 
    {
        sapeaks[vx] = 0;
    }
    if (VisMode == 1){
        if ((vx == i + 3 && vx < 72) && sa_thick) {
            // skip rendering
            } else {
                for (int dy = 0; dy < safalloff[vx]; ++dy) {
                    switch (coloring[0]) {
                    case 'n': // normal
                        color_index = dy + 1;
                        break;
                    case 'f': // fire
                        color_index = (-safalloff[vx]+16) + dy + 1;
                        break;
                    case 'l': // line
                        color_index = safalloff[vx] + 1;
                        break;
                    default:
                        color_index = dy + 1; // default to normal
                        break;
                    }
                    Color scope_color = visColors[color_index];
                    putPixel(surface, vx, dy, SDL_MapRGBA(surface->format, scope_color.r, scope_color.g, scope_color.b, alpha));
                }            
            }

        if ((vx == i + 3 && vx < 72) && sa_thick) {
            // skip rendering
            } else {
                Color peaksColor = visColors[17];
                putPixel(surface, vx, intValue2, SDL_MapRGBA(surface->format, peaksColor.r, peaksColor.g, peaksColor.b, alpha));
            }
        }
    }

    SDL_Rect src = { 0, 0, VISWIDTH, VISHEIGHT };
    SDL_Rect dst = { parentX + x, parentY + y, w, h }; // <<--- add parent offset!
    //std::cout << "DEBUG: Rendering vis at (" << parentX + x << ", " << parentY + y << ") with size (" << w << ", " << h << ")" << std::endl;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopyEx(renderer, texture, &src, &dst, 0, 0, setSDLFlip(renderer, flipv, fliph));
    SDL_FreeSurface(surface);

    return true;
}
