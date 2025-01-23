#ifndef CFG_H
#define CFG_H

#include <stdio.h>
#include "../logic/rendering/rendering.h"

#define QUOTE_COUNT 40


extern int windowWidth;
extern int windowHeight;
extern int buttonsEnabled;
extern char username[32];
extern char welcomeText[256];
extern char * todText;
extern char timeText[32];
extern char dateText[32];
extern char *quoteText;
extern int quoteIndex;
extern const char *allQuotes[QUOTE_COUNT];
extern int quoteAnimationInterval;
extern int currentWindow;

#endif