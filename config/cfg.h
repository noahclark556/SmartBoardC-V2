#ifndef CFG_H
#define CFG_H

#include <stdio.h>
#include "../logic/rendering/rendering.h"

// These are okay to change -----------------------------

#define QUOTE_COUNT 40 // The number of quotes in the quote array
#define USER_FNAME_DEFAULT "Gentlemen" // The name uses in the welcome message
#define WINDOW_WIDTH 1920 // The width of the window
#define WINDOW_HEIGHT 1080 // The height of the window
#define QUOTE_ANIMATION_INTERVAL 10000 // The interval of the quote animation (ms)
#define NEWS_SWITCH_INTERVAL 30000 // The interval of the news switch between title and description (ms) 15 seconds
#define NEWS_DATA_SWITCH_INTERVAL 1200000 // The interval of the news data switch (ms) 20 minutes
// -----------------------------------------------


// Dont change these -----------------------------

extern int windowWidth;
extern int windowHeight;
extern char welcomeText[256];
extern char * todText;
extern char timeText[32];
extern char dateText[32];
extern char *quoteText;
extern int quoteIndex;
extern int newsIndex;
extern int newsDataIndex;
extern const char *allQuotes[QUOTE_COUNT];
extern int quoteAnimationInterval;
extern int currentWindow;
extern int isSpeaking;
// -----------------------------------------------

#endif