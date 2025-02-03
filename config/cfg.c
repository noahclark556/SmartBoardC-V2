#include "cfg.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// DONT EDIT THIS FILE: ONLY EDIT cfg.h and api.h

int windowWidth = WINDOW_WIDTH;
int windowHeight = WINDOW_HEIGHT;
char welcomeText[256];
char * todText = "Afternoon";
char timeText[32] = "2:52 pm";
char dateText[32] = "Fri, Jan 2";
char *quoteText = "Any man who knows a thing, knows he knows nothing at all.";
int quoteIndex = 0;
int quoteAnimationInterval = QUOTE_ANIMATION_INTERVAL;
int currentWindow = 0;

const char *allQuotes[QUOTE_COUNT] = {
    "Any man who knows a thing, knows he knows nothing at all.",
    "The only true wisdom is in knowing you know nothing.",
    "Life is what happens when you're busy making other plans.",
    "To be yourself in a world that is constantly trying to make you something else is the greatest accomplishment.",
    "In three words I can sum up everything I've learned about life: it goes on.",
    "Do not dwell in the past, do not dream of the future, concentrate the mind on the present moment.",
    "Happiness is not something ready-made. It comes from your own actions.",
    "Success is not final, failure is not fatal: It is the courage to continue that counts.",
    "You miss 100% of the shots you don’t take.",
    "In the middle of every difficulty lies opportunity.",
    "The best time to plant a tree was 20 years ago. The second best time is now.",
    "An unexamined life is not worth living.",
    "The journey of a thousand miles begins with one step.",
    "What lies behind us and what lies before us are tiny matters compared to what lies within us.",
    "Do what you can, with what you have, where you are.",
    "If you want to lift yourself up, lift up someone else.",
    "The only limit to our realization of tomorrow will be our doubts of today.",
    "It does not matter how slowly you go as long as you do not stop.",
    "Keep your face always toward the sunshine—and shadows will fall behind you.",
    "What you get by achieving your goals is not as important as what you become by achieving your goals.",
    "Believe you can and you're halfway there.",
    "Act as if what you do makes a difference. It does.",
    "The future belongs to those who believe in the beauty of their dreams.",
    "Hardships often prepare ordinary people for an extraordinary destiny.",
    "It always seems impossible until it's done.",
    "The purpose of our lives is to be happy.",
    "You only live once, but if you do it right, once is enough.",
    "Be the change that you wish to see in the world.",
    "In the end, we will remember not the words of our enemies, but the silence of our friends.",
    "The only way to do great work is to love what you do.",
    "To live is the rarest thing in the world. Most people exist, that is all.",
    "The mind is everything. What you think you become.",
    "Life is really simple, but we insist on making it complicated.",
    "The best way to predict the future is to create it.",
    "Do not go where the path may lead, go instead where there is no path and leave a trail.",
    "You have within you right now, everything you need to deal with whatever the world can throw at you.",
    "Spread love everywhere you go. Let no one ever come to you without leaving happier.",
    "Challenges are what make life interesting and overcoming them is what makes life meaningful.",
    "The greatest glory in living lies not in never falling, but in rising every time we fall.",
    "Life is either a daring adventure or nothing at all."
};
