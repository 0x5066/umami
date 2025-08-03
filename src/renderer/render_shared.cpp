#include <string>
#include <string_view>
#include "render_shared.h"

template std::string getAttr<UIElement>(const UIElement&, const std::string&, const std::string&);

SDL_Texture* loadSkinTexture(SDL_Renderer* renderer, const std::string& imageFile, SDL_Surface** outSurface = nullptr) {
    std::vector<std::string> searchPaths = {
        g_skinPath + imageFile,
        "freeform/xml/wasabi/" + imageFile,
        "freeform/" + imageFile
    };

    for (const std::string& path : searchPaths) {
        SDL_Log("Trying to load image: %s", path.c_str());

        if (!std::filesystem::exists(path)) {
            SDL_Log("Path does not exist: %s", path.c_str());
            continue;
        }

        SDL_Surface* surface = IMG_Load(path.c_str());
        if (!surface) {
            SDL_Log("Failed to load %s: %s", path.c_str(), SDL_GetError());
            continue;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            SDL_Log("Failed to create texture from %s: %s", path.c_str(), SDL_GetError());
            SDL_DestroySurface(surface);
            continue;
        }

        SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

        SDL_Log("Successfully loaded texture: %s", path.c_str());

        if (outSurface) {
            *outSurface = surface;
        } else {
            SDL_DestroySurface(surface);
        }

        return texture;
    }

    SDL_Log("All attempts to load image '%s' failed.", imageFile.c_str());
    return nullptr;
}

SDL_Texture* getOrLoadTexture(SDL_Renderer* renderer, Skin& skin, SkinBitmap& bmp) {
    if (bmp.triedLoading && bmp.loadFailed) return nullptr;
    if (bmp.texture) return bmp.texture;

    bmp.triedLoading = true;

    // Handle $solid bitmap
    if (bmp.file == "$solid") {
        // Default color if not present
        int r = 0, g = 0, b = 0;
        auto it = bmp.attributes.find("color");
        if (it != bmp.attributes.end()) {
            sscanf(it->second.c_str(), "%d,%d,%d", &r, &g, &b);
        }

        SDL_Surface* surf = SDL_CreateSurface(bmp.w, bmp.h, SDL_PIXELFORMAT_RGBA32);
        if (!surf) {
            SDL_Log("Failed to create $solid surface: %s", SDL_GetError());
            bmp.loadFailed = true;
            return nullptr;
        }

        SDL_FillSurfaceRect(surf, nullptr, SDL_MapRGB(SDL_GetPixelFormatDetails(surf->format), SDL_GetSurfacePalette(surf), r, g, b));
        bmp.texture = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_DestroySurface(surf);

        if (!bmp.texture) {
            SDL_Log("Failed to create $solid texture: %s", SDL_GetError());
            bmp.loadFailed = true;
            return nullptr;
        }

        bmp.loadFailed = false;
        return bmp.texture;
    }

    if (bmp.file == "$polygon") {
        auto it = bmp.attributes.find("points");
        if (it == bmp.attributes.end()) {
            SDL_Log("Missing points for $polygon bitmap: %s", bmp.id.c_str());
            bmp.loadFailed = true;
            return nullptr;
        }

        SDL_Surface* surf = SDL_CreateSurface(bmp.w, bmp.h, SDL_PIXELFORMAT_RGBA32);
        if (!surf) {
            SDL_Log("Failed to create $polygon surface: %s", SDL_GetError());
            bmp.loadFailed = true;
            return nullptr;
        }

        SDL_FillSurfaceRect(surf, nullptr, SDL_MapRGBA(SDL_GetPixelFormatDetails(surf->format), SDL_GetSurfacePalette(surf), 0, 0, 0, 0));  // clear to transparent

        // naive flat fill polygon drawing
        std::vector<SDL_FPoint> points;
        std::stringstream ss(it->second);
        std::string token;
        while (std::getline(ss, token, ';')) {
            int x, y;
            if (sscanf(token.c_str(), "%f,%f", &x, &y) == 2) {
                points.push_back({static_cast<float>(x), static_cast<float>(y)});
            }
        }

        if (points.size() >= 3) {
            SDL_SetSurfaceBlendMode(surf, SDL_BLENDMODE_BLEND);
            SDL_Renderer* tempRenderer = SDL_CreateSoftwareRenderer(surf);
            SDL_SetRenderDrawColor(tempRenderer, 255, 255, 255, 255); // white
            SDL_RenderLines(tempRenderer, points.data(), points.size());
            //SDL_RenderFillPoly(tempRenderer, points.data(), points.size()); // SDL_gfx-style API, or implement custom fill
            SDL_DestroyRenderer(tempRenderer);
        }

        bmp.texture = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_DestroySurface(surf);

        if (!bmp.texture) {
            SDL_Log("Failed to create $polygon texture: %s", SDL_GetError());
            bmp.loadFailed = true;
            return nullptr;
        }

        bmp.loadFailed = false;
        return bmp.texture;
    }

    if (bmp.file == "$gradient") {
        auto it = bmp.attributes.find("points");
        if (it == bmp.attributes.end()) {
            SDL_Log("Missing points for $gradient bitmap: %s", bmp.id.c_str());
            bmp.loadFailed = true;
            return nullptr;
        }

        SDL_Surface* surf = SDL_CreateSurface(bmp.w, bmp.h, SDL_PIXELFORMAT_RGBA32);
        if (!surf) {
            SDL_Log("Failed to create $gradient surface: %s", SDL_GetError());
            bmp.loadFailed = true;
            return nullptr;
        }

        float x1 = std::stof(bmp.attributes["gradient_x1"]);
        float y1 = std::stof(bmp.attributes["gradient_y1"]);
        float x2 = std::stof(bmp.attributes["gradient_x2"]);
        float y2 = std::stof(bmp.attributes["gradient_y2"]);

        struct GradientStop {
            float pos;
            Uint8 r, g, b, a;
        };

        std::vector<GradientStop> stops;
        std::stringstream ss(it->second);
        std::string token;
        while (std::getline(ss, token, ';')) {
            float pos;
            int r, g, b, a = 255;
            size_t eq = token.find('=');
            if (eq == std::string::npos) continue;

            pos = std::stof(token.substr(0, eq));
            std::string color = token.substr(eq + 1);
            if (color.starts_with("#")) {
                unsigned int rgba;
                sscanf(color.c_str(), "#%08x", &rgba);
                r = (rgba >> 24) & 0xFF;
                g = (rgba >> 16) & 0xFF;
                b = (rgba >> 8)  & 0xFF;
                a = rgba & 0xFF;
            } else {
                sscanf(color.c_str(), "%d,%d,%d,%d", &r, &g, &b, &a);
            }
            stops.push_back({pos, (Uint8)r, (Uint8)g, (Uint8)b, (Uint8)a});
        }

        std::sort(stops.begin(), stops.end(), [](const auto& a, const auto& b) {
            return a.pos < b.pos;
        });

        // Compute direction
        float dx = x2 - x1;
        float dy = y2 - y1;
        float lenSq = dx * dx + dy * dy;

        // For each pixel, interpolate color
        Uint32* pixels = (Uint32*)surf->pixels;
        for (int y = 0; y < bmp.h; ++y) {
            for (int x = 0; x < bmp.w; ++x) {
                float u = 0.0f;
                if (lenSq > 0.0001f) {
                    float relx = (float)x / bmp.w;
                    float rely = (float)y / bmp.h;
                    float proj = (relx - x1) * dx + (rely - y1) * dy;
                    u = std::clamp(proj / lenSq, 0.0f, 1.0f);
                }

                GradientStop c1 = stops.front(), c2 = stops.back();
                for (size_t i = 1; i < stops.size(); ++i) {
                    if (u < stops[i].pos) {
                        c1 = stops[i - 1];
                        c2 = stops[i];
                        break;
                    }
                }

                float t = (u - c1.pos) / std::max(0.001f, c2.pos - c1.pos);
                Uint8 r = (Uint8)(c1.r + (c2.r - c1.r) * t);
                Uint8 g = (Uint8)(c1.g + (c2.g - c1.g) * t);
                Uint8 b = (Uint8)(c1.b + (c2.b - c1.b) * t);
                Uint8 a = (Uint8)(c1.a + (c2.a - c1.a) * t);

                pixels[y * surf->pitch / 4 + x] = SDL_MapRGBA(SDL_GetPixelFormatDetails(surf->format), SDL_GetSurfacePalette(surf), r, g, b, a);
            }
        }

        bmp.texture = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_DestroySurface(surf);

        if (!bmp.texture) {
            SDL_Log("Failed to create $gradient texture: %s", SDL_GetError());
            bmp.loadFailed = true;
            return nullptr;
        }

        bmp.loadFailed = false;
        return bmp.texture;
    }

    // Fallback to file-based loading
    bmp.texture = loadSkinTexture(renderer, bmp.file);
    if (!bmp.texture) {
        SDL_Log("Failed to load texture for SkinBitmap id: %s, and file: %s", bmp.id.c_str(), bmp.file.c_str());
        bmp.loadFailed = true;
        return nullptr;
    }

    bmp.loadFailed = false;
    return bmp.texture;
}
