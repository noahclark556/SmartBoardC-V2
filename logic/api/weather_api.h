// WEATHER_API_H
#ifndef WEATHER_API_H
#define WEATHER_API_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

typedef struct {
    double temperature;
    double weather_code;
    char *date;
} HourlyWeather;

typedef struct {
    double current_temperature;
    double current_weather_code;
    HourlyWeather *hourly;
    size_t hourly_count;
} WeatherData;

typedef struct {
    char date[11]; // YYYY-MM-DD
    double high;
    double low;
} WeatherHighLows;

// WeatherHighLows *globalWeatherData = {0};
extern double currentTemperature;
extern double currentWeatherCode;
extern WeatherHighLows *globalWeatherData;
extern size_t highLowsCount;
extern size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
extern WeatherData requestWeather(void);
extern void initializeWeather(void);
void destroyWeatherData(void);
char *translateWeatherCode(double weatherCode);
SDL_Texture * translateWeatherCodeIcon(double weatherCode);


#endif // WEATHER_API_H
