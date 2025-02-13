#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "dashboard.h"
#include "../logic/rendering/rendering.h"
#include "../logic/rendering/style.h"
#include "../config/cfg.h"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <time.h>
#include <stdio.h>
#include "../logic/api/weather_api.h"
#include "../logic/api/news_api.h"
#include "../logic/network/time_config.h"
#include "../logic/api/database.h"
#include "talkpage.h"

int cardWidth = 500;
int cardHeight = 300;
int cardSpacing = 100; 
char username[32] = USER_FNAME_DEFAULT;

void weatherCard(SDL_Renderer *renderer);
void agendaCard(SDL_Renderer *renderer);
void notesCard(SDL_Renderer *renderer);
void newsCard(SDL_Renderer *renderer);

void dashboardWindow(SDL_Renderer *renderer)
{
    if (currentWindow == 1)
    {
        talkPage(renderer);
        return;
    }
    
    snprintf(welcomeText, 256, "Good %s, %s", todText, username);
    renderText(welcomeText, 0, 50, renderer, fontXXLarge, textColorBlack, 0, 0);
    renderText(timeText, 0, 130, renderer, fontXLarge, textColorBlack, 0, 0);
    renderText(dateText, 0, 190, renderer, fontLarge, textColorBlack, 0, 0);

    // I set this up so that if either x or y is 0, the starting point is center (h or v).
    // So i am using the x and y pad (last params) to offset from the center. Dont forget that.
    renderText(quoteText, 0, 0, renderer, fontLarge, textColorBlack, 0, -150);
    // renderText(currentNewsTitleString, 0, (windowHeight - cardHeight) / 1.3, renderer, fontMedium, textColorBlack, 0, 0);

    // printf("Drawing weather card\n");
    weatherCard(renderer);
    // printf("Drawing agenda card\n");
    agendaCard(renderer);
    // printf("Drawing notes card\n");

    // printf("Drawing news card\n");
    newsCard(renderer);

    renderText("\"Hey Smartboard\"", 0, windowHeight - 50, renderer, fontSmallMedium, textColorBlack, 0, 0);
}

void renderWeatherIcon(SDL_Renderer *renderer, int x, int y, int w, int h, double wcode)
{
    SDL_Texture *texture = translateWeatherCodeIcon(wcode);
    renderIcon(renderer, texture, x, y, w, h);
}

void renderDailyWeather(SDL_Renderer *renderer, int x, int y)
{
    char dateBuffer[11];
    getCurrentDate(dateBuffer, sizeof(dateBuffer));
    int rc = 0;
    int originalY = y;
    int nx = x;
    
    for (size_t i = 0; i < highLowsCount; i++)
    {
        if (strcmp(globalWeatherData[i].date, dateBuffer) != 0)
        {

            const char *dayOfWeek = getDayOfWeek(globalWeatherData[i].date);
            char combinedText[100];
            snprintf(combinedText, sizeof(combinedText), "%.1f °F", globalWeatherData[i].high);
            renderText(dayOfWeek, x, y, renderer, fontMedium, textColorBlack, 0, 0);
            renderWeatherIcon(renderer, x + 50, y, 30, 30, globalWeatherData[i].high);
            renderText(combinedText, x + 90, y, renderer, fontMedium, textColorBlack, 0, 0);
            y += 50;
            if (rc == 3)
            {
                y = originalY;
                x = x + 210;
            }
        }
        rc++;
    }
}


char *insertNewlines(const char *text, int x) {
    if (!text || x <= 0) return NULL;

    int len = strlen(text);
    int estimatedNewLines = len / x + 1;  // Conservative estimate for added newlines
    int newLen = len + estimatedNewLines + 1;
    char *buffer = malloc(newLen);
    if (!buffer) return NULL;

    int j = 0, lastSpace = -1;
    int count = 0;

    for (int i = 0; i < len; i++) {
        buffer[j++] = text[i];
        count++;

        if (text[i] == ' ') {
            lastSpace = j - 1;  // Track last space index
        }

        if (count >= x && i + 1 < len) {
            if (lastSpace != -1) {
                buffer[lastSpace] = '\n';  // Replace space with newline
                count = j - lastSpace - 1; // Reset count correctly
                lastSpace = -1; // Reset last space tracking
            } else {
                buffer[j++] = '\n'; // Force newline if no space found
                count = 0;
            }
        }
    }

    buffer[j] = '\0';  // Null-terminate the new string
    return buffer;
}

void increase_index_callback(int *index, int increment) {
    *index += increment;
}

void renderMultilineNewsText(const char *text, int x, int y, SDL_Renderer *renderer, TTF_Font *font, SDL_Color color, int cutoff, int *index)
{
    char *textCopy = insertNewlines(text, cutoff);
    
    char *line = strtok(textCopy, "\n");

    int lineHeight = TTF_FontHeight(font);
    int currentY = y;

    while (line != NULL)
    {
        renderText(line, x, currentY, renderer, font, color, 0, 0);
        currentY += lineHeight;
        line = strtok(NULL, "\n");
        if (index != NULL) {
            increase_index_callback(index, 1);
        }
    }

    free(textCopy);
    
}


void weatherCard(SDL_Renderer *renderer)
{
    int padV = 15;
    int padH = 0;
    int index = 0; 
    int totalWidth = 3 * cardWidth + 2 * cardSpacing;
    int startX = (windowWidth - totalWidth) / 2;
    int startY = (windowHeight - cardHeight) / 1.2;
    SDL_Rect card;
    card.x = startX + index * (cardWidth + cardSpacing);
    card.y = startY;
    card.w = cardWidth;
    card.h = cardHeight;

    double currentTemp = currentTemperature;

    SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);

    int radius = 10; 
    roundedBoxRGBA(renderer, card.x, card.y, card.x + card.w, card.y + card.h, radius, 220, 220, 220, 80);

    char *locationText = "Mt. Pleasant";
    char *weatherText = "Clear";
    weatherText = translateWeatherCode(currentWeatherCode);
    char tempText[20];
    snprintf(tempText, sizeof(tempText), "%.1f °F", currentTemp);

    renderText(locationText, card.x + 50, card.y + padV, renderer, fontLargeReg, textColorBlack, 40, 0);
    renderText(weatherText, card.x + 50, card.y + padV + 35, renderer, fontMedium, textColorBlack, 40, 0);
    renderText(tempText, card.x + 50, card.y + padV + 67, renderer, fontMedium, textColorBlack, 40, 0);

    renderDailyWeather(renderer, card.x + 65, card.y + padV + 120);

    renderWeatherIcon(renderer, card.x + 10, card.y + padV, 60, 60, currentWeatherCode);
}

void agendaCard(SDL_Renderer *renderer)
{
    int padV = 15;
    int padH = 0;
    int index = 1; 
    int totalWidth = 3 * cardWidth + 2 * cardSpacing;
    int startX = (windowWidth - totalWidth) / 2;
    int startY = (windowHeight - cardHeight) / 1.2;
    SDL_Rect card;
    card.x = startX + index * (cardWidth + cardSpacing);
    card.y = startY;
    card.w = cardWidth;
    card.h = cardHeight;

    SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);

    int radius = 10;
    roundedBoxRGBA(renderer, card.x, card.y, card.x + card.w, card.y + card.h, radius, 220, 220, 220, 80);

    char *titleText = "Today's Agenda";
    

    renderText(titleText, card.x + 50, card.y + padV, renderer, fontLargeReg, textColorBlack, 40, 0);

    renderIcon(renderer, agendaIconTexture, card.x + 10, card.y + padV, 60, 60);
    padV += 5;

    char *agendaText = malloc(256);
    char *cd = malloc(256);
    getCurrentDate(cd, 256);
    int ri = 0;
    for (size_t i = 0; i < databaseData.agenda_count; i++)
    {
        if (strcmp(databaseData.agenda[i].date, cd) == 0)
        {
            agendaText[0] = '\0';
            
            snprintf(agendaText, 256, "%s -", databaseData.agenda[i].hour);
            
            renderText(agendaText, card.x + 10, card.y + padV + 65 + (ri * 31), renderer, fontSmallMediumReg, textColorBlack, 40, 0);
            //renderText(databaseData.agenda[i].note, card.x + 90, card.y + padV + 65 + (ri * 38), renderer, fontSmallMedium, textColorBlack, 40, 0);
            renderMultilineNewsText(databaseData.agenda[i].note, card.x + 110, card.y + padV + 65 + (ri * 31), renderer, fontSmallMedium, textColorBlack, 44, &ri);
        }
    }
    if (ri == 0)
    {
        renderText("No Agenda Today", card.x + 50, card.y + padV + 65, renderer, fontSmallMedium, textColorBlack, 40, 0);
    }
    free(agendaText);
    free(cd);
}

void newsCard(SDL_Renderer *renderer)
{
    int padV = 15;
    int padH = 0;
    int index = 2;
    int totalWidth = 3 * cardWidth + 2 * cardSpacing;
    int startX = (windowWidth - totalWidth) / 2;
    int startY = (windowHeight - cardHeight) / 1.2;
    SDL_Rect card;
    card.x = startX + index * (cardWidth + cardSpacing);
    card.y = startY;
    card.w = cardWidth;
    card.h = cardHeight;

    SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);

    int radius = 10;
    roundedBoxRGBA(renderer, card.x, card.y, card.x + card.w, card.y + card.h, radius, 220, 220, 220, 80);

    char *titleText = "Daily News";
    renderIcon(renderer, notesIconTexture, card.x + 10, card.y + padV, 60, 60);
    renderText(titleText, card.x + 50, card.y + padV, renderer, fontLargeReg, textColorBlack, 40, 0);
    if (newsIndex == 1) {
        renderMultilineNewsText(currentNewsTitleString, card.x + 30, card.y + padV + 75, renderer, fontSmallMid, textColorBlack, 44, NULL);
    }
    else {
        renderMultilineNewsText(currentNewsDescriptionString, card.x + 30, card.y + padV + 75, renderer, fontSmallMid, textColorBlack, 44, NULL);
    }
}

void notesCard(SDL_Renderer *renderer)
{
    int padV = 15;
    int padH = 0;
    int index = 2;
    int totalWidth = 3 * cardWidth + 2 * cardSpacing;
    int startX = (windowWidth - totalWidth) / 2;
    int startY = (windowHeight - cardHeight) / 1.2;
    SDL_Rect card;
    card.x = startX + index * (cardWidth + cardSpacing);
    card.y = startY;
    card.w = cardWidth;
    card.h = cardHeight;

    SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);

    int radius = 10;
    roundedBoxRGBA(renderer, card.x, card.y, card.x + card.w, card.y + card.h, radius, 220, 220, 220, 255);

    char *titleText = "Notes to Self";
    renderIcon(renderer, notesIconTexture, card.x + 10, card.y + padV, 60, 60);
    renderText(titleText, card.x + 50, card.y + padV, renderer, fontLargeReg, textColorBlack, 40, 0);
    renderText(databaseData.note, card.x + 53, card.y + padV + 70, renderer, fontSmallMedium, textColorBlack, 40, 0);
}