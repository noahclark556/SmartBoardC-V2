#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "style.h"
#include "rendering.h"

TTF_Font *fontXXLarge = NULL;
TTF_Font *fontXLarge = NULL;
TTF_Font *fontLarge = NULL;
TTF_Font *fontMedium = NULL;
TTF_Font *fontSmallMid = NULL;
TTF_Font *fontSmallMedium = NULL;
TTF_Font *fontSmall = NULL;
TTF_Font *fontXSmall = NULL;
TTF_Font *fontXXSmall = NULL;

TTF_Font *fontLargeBold = NULL;
TTF_Font *fontMediumBold = NULL;
TTF_Font *fontSmallMediumBold = NULL;
TTF_Font *fontSmallBold = NULL;
TTF_Font *fontXSmallBold = NULL;

TTF_Font *fontLargeReg = NULL;
TTF_Font *fontMediumReg = NULL;
TTF_Font *fontSmallMediumReg = NULL;
TTF_Font *fontSmallReg = NULL;
TTF_Font *fontXSmallReg = NULL;

SDL_Color textColorWhite = {255, 255, 255, 255};
SDL_Color textColorBlack = {0, 0, 0, 255, 100};
SDL_Color textColorBlackFaded = {0, 0, 0, 255, 20};

SDL_Texture *iconSunTexture = NULL;
SDL_Texture *iconCloudTexture = NULL;
SDL_Texture *iconFogTexture = NULL;
SDL_Texture *iconRainTexture = NULL;
SDL_Texture *iconSnowTexture = NULL;
SDL_Texture *iconThunderTexture = NULL;

SDL_Texture *agendaIconTexture = NULL;
SDL_Texture *notesIconTexture = NULL;

SDL_Texture *circleRedTexture = NULL;
SDL_Texture *circleGreenTexture = NULL;

void loadFonts(void)
{
    fontXXLarge = TTF_OpenFont("./assets/fonts/Roboto-Light.ttf", 60);
    fontXLarge = TTF_OpenFont("./assets/fonts/Roboto-Light.ttf", 45);
    fontLarge = TTF_OpenFont("./assets/fonts/Roboto-Light.ttf", 30);
    fontMedium = TTF_OpenFont("./assets/fonts/Roboto-Light.ttf", 25);
    fontSmallMedium = TTF_OpenFont("./assets/fonts/Roboto-Light.ttf", 20);
    fontSmallMid = TTF_OpenFont("./assets/fonts/Roboto-Light.ttf", 22);
    fontSmall = TTF_OpenFont("./assets/fonts/Roboto-Light.ttf", 17);
    fontXSmall = TTF_OpenFont("./assets/fonts/Roboto-Light.ttf", 14);
    fontXXSmall = TTF_OpenFont("./assets/fonts/Roboto-Light.ttf", 12);

    fontLargeBold = TTF_OpenFont("./assets/fonts/Roboto-Bold.ttf", 30);
    fontMediumBold = TTF_OpenFont("./assets/fonts/Roboto-Bold.ttf", 25);
    fontSmallMediumBold = TTF_OpenFont("./assets/fonts/Roboto-Bold.ttf", 20);
    fontSmallBold = TTF_OpenFont("./assets/fonts/Roboto-Bold.ttf", 17);
    fontXSmallBold = TTF_OpenFont("./assets/fonts/Roboto-Bold.ttf", 14);

    fontLargeReg = TTF_OpenFont("./assets/fonts/Roboto-Regular.ttf", 30);
    fontMediumReg = TTF_OpenFont("./assets/fonts/Roboto-Regular.ttf", 25);
    fontSmallMediumReg = TTF_OpenFont("./assets/fonts/Roboto-Regular.ttf", 20);
    fontSmallReg = TTF_OpenFont("./assets/fonts/Roboto-Regular.ttf", 17);
    fontXSmallReg = TTF_OpenFont("./assets/fonts/Roboto-Regular.ttf", 14);
}

void loadIconTextures(SDL_Renderer *renderer)
{
    iconSunTexture = loadTexture("./assets/images/sun_icon.png", renderer);
    iconCloudTexture = loadTexture("./assets/images/cloud_icon.png", renderer);
    iconFogTexture = loadTexture("./assets/images/fog_icon.png", renderer);
    iconRainTexture = loadTexture("./assets/images/rain_icon.png", renderer);
    // iconSnowTexture = loadTexture("./assets/images/snow_icon.png", renderer);
    // iconThunderTexture = loadTexture("./assets/images/thunder_icon.png", renderer);
    agendaIconTexture = loadTexture("./assets/images/agenda_icon.png", renderer);
    notesIconTexture = loadTexture("./assets/images/notes_icon.png", renderer);
    circleRedTexture = loadTexture("./assets/images/circle_red.png", renderer);
    circleGreenTexture = loadTexture("./assets/images/circle_green.png", renderer);
}

void loadStyles(SDL_Renderer *renderer)
{
    loadFonts();
    loadIconTextures(renderer);
}

void destroyStyles(void)
{
    SDL_DestroyTexture(iconSunTexture);
    iconSunTexture = NULL;
    SDL_DestroyTexture(iconCloudTexture);
    iconCloudTexture = NULL;
    SDL_DestroyTexture(iconFogTexture);
    iconFogTexture = NULL;
    SDL_DestroyTexture(iconRainTexture);
    iconRainTexture = NULL;
    // SDL_DestroyTexture(iconSnowTexture);
    // iconSnowTexture = NULL;
    // SDL_DestroyTexture(iconThunderTexture);
    // iconThunderTexture = NULL;
    SDL_DestroyTexture(agendaIconTexture);
    agendaIconTexture = NULL;
    SDL_DestroyTexture(notesIconTexture);
    notesIconTexture = NULL;
    SDL_DestroyTexture(circleRedTexture);
    circleRedTexture = NULL;
    SDL_DestroyTexture(circleGreenTexture);
    circleGreenTexture = NULL;

    TTF_CloseFont(fontXXSmall);
    TTF_CloseFont(fontXSmall);
    TTF_CloseFont(fontSmall);
    TTF_CloseFont(fontMedium);
    TTF_CloseFont(fontLarge);

    TTF_CloseFont(fontXSmallReg);
    TTF_CloseFont(fontSmallReg);
    TTF_CloseFont(fontMediumReg);
    TTF_CloseFont(fontLargeReg);

    TTF_CloseFont(fontXSmallBold);
    TTF_CloseFont(fontSmallBold);
    TTF_CloseFont(fontMediumBold);
    TTF_CloseFont(fontLargeBold);
}