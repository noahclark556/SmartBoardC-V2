#include <SDL2/SDL.h>
#include "animations.h"
#include "../../config/cfg.h"
#include "../../logic/api/news_api.h"

SDL_TimerID quoteTextTimer;
SDL_TimerID newsTextTimer;
SDL_TimerID newsDataTimer;

Uint32 updateQuoteText(Uint32 interval, void *param)
{
    int *quoteIndexPtr = (int *)param;
    int quoteIndexVal = *quoteIndexPtr;
    quoteIndex = (quoteIndexVal + 1) % QUOTE_COUNT;
    if (quoteIndex > QUOTE_COUNT)
    {
        quoteIndex = 0;
    }
    quoteText = allQuotes[quoteIndex];
    return interval; // Return the same interval to keep the timer running
}

Uint32 updateNewsText(Uint32 interval, void *param)
{
    if (newsIndex == 1)
    {
        newsIndex = 2;
    }
    else
    {
        newsIndex = 1;
    }
    return interval; // Return the same interval to keep the timer running
}

Uint32 updateNewsData(Uint32 interval, void *param)
{
    if (newsDataIndex < 5)
    {
        newsDataIndex++;
    }
    else
    {
        newsDataIndex = 0;
    }
    free(globalNewsData);
    initializeNews();
    
    return interval; // Return the same interval to keep the timer running
}

void initAnimations(void)
{
    SDL_TimerID quoteTextTimer = SDL_AddTimer(quoteAnimationInterval, updateQuoteText, &quoteIndex);
    SDL_TimerID newsTextTimer = SDL_AddTimer(NEWS_SWITCH_INTERVAL, updateNewsText, &newsIndex);
    SDL_TimerID newsDataTimer = SDL_AddTimer(NEWS_DATA_SWITCH_INTERVAL, updateNewsData, &newsDataIndex);
}

void destroyAnimations(void)
{
    SDL_RemoveTimer(quoteTextTimer);
    SDL_RemoveTimer(newsTextTimer);
    SDL_RemoveTimer(newsDataTimer);
}