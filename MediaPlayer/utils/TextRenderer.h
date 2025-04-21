#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class TextRenderer {
private:
    TTF_Font* font;
    SDL_Color color;

public:
    TextRenderer(const std::string& fontPath, int fontSize, SDL_Color color);
    ~TextRenderer();

    void renderText(SDL_Renderer* renderer, const std::string& text, int x, int y);
};

#endif
