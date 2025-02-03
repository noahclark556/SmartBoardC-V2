#ifndef CFG_H
#define CFG_H

#include <stdio.h>
#include "../logic/rendering/rendering.h"

// These are okay to change -----------------------------

#define QUOTE_COUNT 40 // The number of quotes in the quote array
#define USER_FNAME_DEFAULT "Noah" // The name used in the welcome message
#define WINDOW_WIDTH 1920 // The width of the window
#define WINDOW_HEIGHT 1080 // The height of the window
#define QUOTE_ANIMATION_INTERVAL 10000 // The interval of the quote animation (ms)

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
extern const char *allQuotes[QUOTE_COUNT];
extern int quoteAnimationInterval;
extern int currentWindow;

// -----------------------------------------------

#endif