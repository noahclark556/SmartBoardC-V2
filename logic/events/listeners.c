#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "listeners.h"
#include "../../config/cfg.h"
#include "../../config/api.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "../../windows/talkpage.h"
#include "../api/database.h"

SDL_Event event;
SDL_TimerID commandTimer;
SDL_TimerID responseTimer;
SDL_TimerID modeTimer;
pid_t voice_daemon_pid;
pid_t ir_daemon_pid;
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

Uint32 checkMode(Uint32 interval, void *param)
{
    FILE *file;
    char buffer[BUFFER_SIZE];
    int *iss = (int *)param;

    if(currentWindow == 0)
    {
        return interval;
    }

    if (access(MODE_FILE, F_OK) != -1)
    {
        file = fopen(MODE_FILE, "r");
        if (file)
        {
            while (fgets(buffer, sizeof(buffer), file) != NULL)
            {
                if (strcmp(buffer, "true") == 0)
                {
                    *iss = 1;
                }
                if (strcmp(buffer, "false") == 0)
                {
                    *iss = 0;
                }
            }
            fclose(file);
        }
        else
        {
            perror("Failed to open command file");
        }
    }
    return interval;
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
                    syncDatabase();
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
                // printf("Read line: %s", buffer);
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
    SDL_TimerID modeTimer = SDL_AddTimer(DAEMON_INTERVAL, checkMode, &isSpeaking);
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
        // execl("./daemon/voice_daemon", "voice_daemon", (char *)NULL);
        execl("./daemon/voice_daemon", "voice_daemon", "--openai-api-key", OPENAI_API_KEY, "--google-api-key", GOOGLE_API_KEY, "--db-read-function", READ_DB_FUNCTION_URL, "--db-update-function", UPDATE_DB_FUNCTION_URL, "--db-user-id", DB_USER_ID, "--db-agenda-function", UPDATE_AGENDA_FUNCTION_URL, (char *)NULL);
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

void start_ir_daemon(void)
{
    ir_daemon_pid = fork();
    if (ir_daemon_pid < 0)
    {
        perror("Failed to fork ir daemon");
        exit(EXIT_FAILURE);
    }

    if (ir_daemon_pid == 0)
    {
        printf("Starting IR daemon\n");
        execl("./daemon/ir_daemon", "ir_daemon", (char *)NULL);
        perror("Failed to exec ir_daemon");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("IR daemon started with PID: %d\n", ir_daemon_pid);
    }
}

void stop_ir_daemon(void)
{
    printf("Stopping IR daemon\n");
    if (ir_daemon_pid > 0)
    {
        kill(ir_daemon_pid, SIGTERM);
        printf("IR daemon stopped\n");
    }
    else
    {
        printf("IR daemon not running\n");
    }
}

void initListeners(void)
{
    initVoiceListener();
    start_voice_daemon();
    start_ir_daemon();
}

void destroyListeners(void)
{
    SDL_RemoveTimer(commandTimer);
    SDL_RemoveTimer(responseTimer);
    SDL_RemoveTimer(modeTimer);
    stop_voice_daemon();
    stop_ir_daemon();
}