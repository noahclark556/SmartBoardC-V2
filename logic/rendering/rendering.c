#include "rendering.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include "../../config/cfg.h"

double getHorizontalCenter(double itemWidth) {
    return (windowWidth - itemWidth) / 2;
}
double getVerticalCenter(double itemHeight) {
    return (windowHeight - itemHeight) / 2;
}

SDL_Texture* loadTexture(const char* file, SDL_Renderer* renderer) {
    SDL_Texture* texture = NULL;
    SDL_Surface* loadedImage = IMG_Load(file);
    if (loadedImage != NULL) {
        texture = SDL_CreateTextureFromSurface(renderer, loadedImage);
        SDL_FreeSurface(loadedImage);
    } else {
        fprintf(stderr, "Failed to load image: %s\n", IMG_GetError());
    }
    return texture;
}

void renderIcon(SDL_Renderer* renderer, SDL_Texture* iconTexture, int x, int y, int w, int h) {
    SDL_Rect destRect = {x, y, w, h}; // Set the position and size of the icon
    SDL_RenderCopy(renderer, iconTexture, NULL, &destRect);
}

void renderText(char *txt, double x, double y, SDL_Renderer *renderer, TTF_Font *font, SDL_Color color, double xb, double yb){
        if (txt == NULL || strlen(txt) == 0) {
           fprintf(stderr, "Warning: Attempted to render NULL or empty text.\n");
           return; // Early return if txt is NULL or empty
       }
        SDL_Surface *surface = TTF_RenderUTF8_Blended(font, txt, color); // quality
        // SDL_Surface *surface = TTF_RenderText_Blended(font, txt, color); // quality
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

        // Calculate centered position
        SDL_Rect textRect;
        textRect.w = surface->w; // Width of the text
        textRect.h = surface->h; // Height of the text
        if(x == 0){
            textRect.x = getHorizontalCenter(surface->w) + xb;
        }else{
            textRect.x = x + xb;
        }
        if(y == 0){
            textRect.y = getVerticalCenter(surface->h) + yb;
        }else{
            textRect.y = y + yb; 
        }
        // Render the text
        SDL_RenderCopy(renderer, texture, NULL, &textRect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
}