#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "talkpage.h"
#include "../config/cfg.h"
#include "../logic/rendering/style.h"
#include "../logic/rendering/rendering.h"
#include "../logic/events/listeners.h"

void renderMultilineText(const char *text, int x, int y, SDL_Renderer *renderer, TTF_Font *font, SDL_Color color)
{
    char *textCopy = strdup(text);
    char *line = strtok(textCopy, "\n");

    int lineHeight = TTF_FontHeight(font);
    int currentY = y;

    while (line != NULL)
    {

        renderText(line, x, currentY, renderer, font, color, 0, 0);
        currentY += lineHeight;
        line = strtok(NULL, "\n");
    }

    free(textCopy);
}

void talkPage(SDL_Renderer *renderer)
{
    if (responseText != NULL)
    {
        renderMultilineText(responseText, 0, 200, renderer, fontSmallMedium, textColorBlack); //fontXSmall in dev, fontSmallMedium on rpi
    }
    else
    {
        renderText("No response available", 0, 200, renderer, fontSmallMedium, textColorBlack, 0, 0);
    }
}