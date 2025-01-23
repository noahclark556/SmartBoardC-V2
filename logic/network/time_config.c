#include <SDL2/SDL.h>
#include <stdio.h>
#include "time_config.h"
#include <time.h>
#include "../../config/cfg.h"

SDL_TimerID timerID;
SDL_TimerID dateTimerID;

void formatCurrentTime(char *buffer, size_t bufferSize)
{
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    int hour = timeinfo->tm_hour % 12;
    int prehour = timeinfo->tm_hour;

    if(prehour >= 0 && prehour <= 11){
        todText = "Morning";
    }else if(prehour >= 12 && prehour <= 17){
        todText = "Afternoon";
    }else{
        todText = "Evening";
    }
    
    if (hour == 0) hour = 12;
    int minute = timeinfo->tm_min;
    snprintf(buffer, bufferSize, "%d:%02d %s", hour, minute, (timeinfo->tm_hour >= 12) ? "PM" : "AM");
}


Uint32 updateTimeLabel(Uint32 interval, void *param)
{
    char *timeText = (char *)param;
    formatCurrentTime(timeText, 32); 
    return interval;                 
}

void formatCurrentDate(char *buffer, size_t bufferSize)
{
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, bufferSize, "%a, %b %d", timeinfo);
}

Uint32 updateDateLabel(Uint32 interval, void *param)
{
    char *dateText = (char *)param;
    formatCurrentDate(dateText, 32);
    return interval;
}

void initializeDateTime(void)
{
    formatCurrentTime(timeText, sizeof(timeText));
    formatCurrentDate(dateText, sizeof(dateText));

    SDL_TimerID timerID = SDL_AddTimer(10000, updateTimeLabel, timeText);      
    SDL_TimerID dateTimerID = SDL_AddTimer(300000, updateDateLabel, dateText); 
}

void getCurrentDate(char *buffer, size_t bufferSize)
{
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, bufferSize, "%Y-%m-%d", tm_info);
}

const char *getDayOfWeek(const char *dateStr)
{
    struct tm timeStruct = {0};
    if (strptime(dateStr, "%Y-%m-%d", &timeStruct) == NULL)
    {
        return "Invalid date";
    }

    time_t time = mktime(&timeStruct);
    if (time == -1)
    {
        return "Invalid date";
    }
    
    const char *daysOfWeek[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    return daysOfWeek[timeStruct.tm_wday];
}

void destroyDateTime(void)
{
    SDL_RemoveTimer(timerID);
    SDL_RemoveTimer(dateTimerID);
}