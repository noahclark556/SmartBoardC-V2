// rendering.h
#ifndef STYLE_H
#define STYLE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


void loadStyles(SDL_Renderer *renderer);
void destroyStyles(void);
extern TTF_Font *fontXXLarge;
extern TTF_Font *fontXLarge;
extern TTF_Font *fontLarge;
extern TTF_Font *fontMedium;
extern TTF_Font *fontSmallMedium;
extern TTF_Font *fontSmall;
extern TTF_Font *fontXSmall;
extern TTF_Font *fontXXSmall;

extern TTF_Font *fontLargeBold;
extern TTF_Font *fontMediumBold;
extern TTF_Font *fontSmallMediumBold;
extern TTF_Font *fontSmallBold;
extern TTF_Font *fontXSmallBold;

extern TTF_Font *fontLargeReg;
extern TTF_Font *fontMediumReg;
extern TTF_Font *fontSmallMediumReg;
extern TTF_Font *fontSmallReg;
extern TTF_Font *fontXSmallReg;

extern SDL_Color textColorWhite;
extern SDL_Color textColorBlack;
extern SDL_Color textColorBlackFaded;

extern SDL_Texture *iconSunTexture;
extern SDL_Texture *iconCloudTexture;
extern SDL_Texture *iconFogTexture;
extern SDL_Texture *iconRainTexture;
extern SDL_Texture *iconSnowTexture;
extern SDL_Texture *iconThunderTexture;

extern SDL_Texture *agendaIconTexture;
extern SDL_Texture *notesIconTexture;
#endif // STYLE_H
