#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "listeners.h"
#include "../../config/cfg.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "../../windows/talkpage.h"

SDL_Event event;
SDL_TimerID commandTimer;
SDL_TimerID responseTimer;
pid_t voice_daemon_pid;
char *responseText = NULL;

int mainEventListener(void)
{
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            return 0;
        }

        if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
        {
            int mouseX = event.button.x;
            int mouseY = event.button.y;
        }
        return 1;
    }
    return 1;
}

Uint32 checkForCommand(Uint32 interval, void *param)
{
    FILE *file;
    char buffer[BUFFER_SIZE];
    int *cw = (int *)param;

    if (access(COMMAND_FILE, F_OK) != -1)
    {
        file = fopen(COMMAND_FILE, "r");
        if (file)
        {
            while (fgets(buffer, sizeof(buffer), file) != NULL)
            {
                if (strcmp(buffer, "speechmode") == 0)
                {
                    printf("Init Detected. Changing window\n");
                    *cw = 1;
                }
                if (strcmp(buffer, "silentmode") == 0)
                {
                    printf("Silent Detected. Changing window\n");
                    *cw = 0;
                }
            }
            fclose(file);

            if (CLEAR_FILE == 1)
            {
                file = fopen(COMMAND_FILE, "w");
                if (file)
                {
                    fclose(file);
                }
                else
                {
                    perror("Failed to open command file");
                }
            }
        }
        else
        {
            perror("Failed to open command file");
        }
    }
    return interval;
}

Uint32 checkForResponse(Uint32 interval, void *param)
{
    FILE *file;
    char buffer[BUFFER_SIZE];
    char **responseTextPtr = (char **)param;

    if (access(RESPONSE_FILE, F_OK) != -1)
    {

        file = fopen(RESPONSE_FILE, "r");
        if (file)
        {

            fseek(file, 0, SEEK_END);
            long fileSize = ftell(file);
            fseek(file, 0, SEEK_SET);
            if (fileSize == 0)
            {

                fclose(file);
                return interval;
            }

            if (*responseTextPtr != NULL)
            {
                free(*responseTextPtr);
                *responseTextPtr = NULL;
            }

            *responseTextPtr = malloc(BUFFER_SIZE);
            if (*responseTextPtr == NULL)
            {
                perror("Failed to allocate memory for response text");
                fclose(file);
                return interval;
            }

            size_t totalLength = 0;

            while (fgets(buffer, sizeof(buffer), file) != NULL)
            {
                printf("Read line: %s", buffer);
                size_t bufferLength = strlen(buffer);

                if (totalLength + bufferLength >= BUFFER_SIZE)
                {
                    char *temp = realloc(*responseTextPtr, totalLength + bufferLength + 1);
                    if (temp == NULL)
                    {
                        perror("Failed to reallocate memory for response text");
                        free(*responseTextPtr);
                        fclose(file);
                        return interval;
                    }
                    *responseTextPtr = temp;
                }

                strcpy(*responseTextPtr + totalLength, buffer);
                totalLength += bufferLength;
            }
            fclose(file);

            file = fopen(RESPONSE_FILE, "w");
            if (file)
            {
                fclose(file);
            }
            else
            {
                perror("Failed to open response file for clearing");
            }
        }
        else
        {
            perror("Failed to open response file");
        }
    }
    return interval;
}

void initVoiceListener(void)
{
    printf("Initializing voice listener\n");

    SDL_TimerID commandTimer = SDL_AddTimer(DAEMON_INTERVAL, checkForCommand, &currentWindow);
    SDL_TimerID responseTimer = SDL_AddTimer(DAEMON_INTERVAL, checkForResponse, &responseText);
}

void start_voice_daemon(void)
{
    printf("Starting voice daemon\n");
    remove(RESPONSE_FILE);
    remove(COMMAND_FILE);
    voice_daemon_pid = fork();
    if (voice_daemon_pid < 0)
    {
        perror("Failed to fork voice daemon");
        exit(EXIT_FAILURE);
    }

    if (voice_daemon_pid == 0)
    {
        execl("./daemon/voice_daemon", "voice_daemon", (char *)NULL);

        perror("Failed to exec voice_daemon");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Voice daemon started with PID: %d\n", voice_daemon_pid);
    }
}

void stop_voice_daemon(void)
{
    printf("Stopping voice daemon\n");
    if (voice_daemon_pid > 0)
    {
        kill(voice_daemon_pid, SIGTERM);
        printf("Voice daemon stopped\n");
    }
    else
    {
        printf("Voice daemon not running\n");
    }
}

void initListeners(void)
{
    initVoiceListener();
    start_voice_daemon();
}

void destroyListeners(void)
{
    SDL_RemoveTimer(commandTimer);
    SDL_RemoveTimer(responseTimer);
    stop_voice_daemon();
}