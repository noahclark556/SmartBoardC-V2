#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include "./config/cfg.h"
#include "./logic/rendering/rendering.h"
#include "./logic/events/listeners.h"
#include "./logic/rendering/style.h"
#include "./windows/dashboard.h"
#include "./logic/network/time_config.h"
#include "./logic/api/weather_api.h"
#include "./logic/rendering/animations.h"
#include "./logic/api/database.h"
#include "./logic/events/listeners.h"

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        fprintf(stderr, "Could not initialize SDL_image: %s\n", IMG_GetError());
        return 1;
    }

    if (TTF_Init() == -1)
    {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Smart Board",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        windowWidth, windowHeight,
        SDL_WINDOW_BORDERLESS);

    printf("Creating window\n");

    if (!window)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    printf("Window created successfully\n");

    printf("Initializing date time\n");
    initializeDateTime();
    printf("Setting window to fullscreen\n");
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    printf("Getting window size\n");
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    printf("Window size: %d, %d\n", windowWidth, windowHeight);
    // Create SDL Renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    printf("Renderer created successfully\n");

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
    printf("Setting renderer to linear scaling\n");
    SDL_RenderSetLogicalSize(renderer, windowWidth, windowHeight); // Logical resolution for scaling
    printf("Setting renderer to 4x MSAA\n");
    // These two lines are causing the issue of the window not showing up on linux
    //  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    // printf("Setting renderer to 4x MSAA\n");
    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // 4x MSAA (anti-aliasing)
    printf("Loading styles\n");
    loadStyles(renderer);
    printf("Initializing weather\n");
    initializeWeather();
    printf("Initializing animations\n");
    initAnimations();
    printf("Syncing database\n");
    syncDatabase();
    printf("Initializing Listeners and Daemons\n");
    initListeners();

    if (!fontLarge || !fontMedium)
    {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    printf("Starting main event loop\n");
    // Main event loop
    SDL_Event event;
    int running = 1;
    while (running)
    {
        if (mainEventListener() == 0)
        {
            running = 0;
        }
        // Set renderer draw color for background
        // SDL_SetRenderDrawColor(renderer, 173, 216, 230, 255); // Light blue
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White

        SDL_RenderClear(renderer); // Clear with the current draw color
        // printf("Drawing dashboard\n");
        dashboardWindow(renderer);

        // Update the screen
        SDL_RenderPresent(renderer);
        // printf("Presenting screen\n");

        // Delay to cap the frame rate (60 FPS)
        SDL_Delay(16);

        // Calculate frame rate cap delay, where x = target delay and 30 = target frame rate. 
        // 60/16 is base. Inverse for FPS calculation.
        // 60/16 = 30/x
    }

    // Clean up

    destroyListeners();
    destroyDateTime();
    destroyStyles();
    destroyWeatherData();
    destroyAnimations();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
