// rendering.h
#ifndef RENDERING_H
#define RENDERING_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

typedef struct {
    SDL_Rect rect; // Position and size of the button
    const char *label; // Text label for the button
} Button;

/**
 * @brief Center item horizontally
 *
 * @param itemWidth The width of item to center
 */
double getHorizontalCenter(double itemWidth);
/**
 * @brief Center item vertically
 *
 * @param itemWidth The height of item to center
 */
double getVerticalCenter(double itemHeight);
/**
 * @brief Render text to screen
 *
 * @param txt *char - Pointer to label value
 * @param x double - X coord location (pass 0 for center)
 * @param y double - Y coord location (pass 0 for center)
 * @param renderer *SDL_Renderer - pointer to renderer object
 * @param font *TTF_Font - pointer to font
 * @param color SDL_Color - color of text 
 * @param xb double - Buffer for x coord (+/- horizontal of x param, pass 0 for nothing)
 * @param yb double - Buffer for y coord (+/- vertical of y param, pass 0 for nothing)
 */
void renderText(char *txt, double x, double y, SDL_Renderer *renderer, TTF_Font *font, SDL_Color color, double xb, double yb);

SDL_Texture* loadTexture(const char* file, SDL_Renderer* renderer);
void renderIcon(SDL_Renderer* renderer, SDL_Texture* iconTexture, int x, int y, int w, int h);
#endif // RENDERING_H
