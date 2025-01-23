#include <SDL2/SDL.h>
#include "animations.h"
#include "../../config/cfg.h"

SDL_TimerID quoteTextTimer;

Uint32 updateQuoteText(Uint32 interval, void *param) {
    int *quoteIndexPtr = (int*)param;
    int quoteIndexVal = *quoteIndexPtr;
    quoteIndex = (quoteIndexVal + 1) % QUOTE_COUNT;
    if (quoteIndex > QUOTE_COUNT) {
        quoteIndex = 0;
    }
    quoteText = allQuotes[quoteIndex];
    return interval; // Return the same interval to keep the timer running
}

void initAnimations(void)
{
    SDL_TimerID quoteTextTimer = SDL_AddTimer(quoteAnimationInterval, updateQuoteText, &quoteIndex);
}

void destroyAnimations(void)
{
    SDL_RemoveTimer(quoteTextTimer);
}