#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <string.h>
#include <curl/curl.h>
#include "weather_api.h"
#include "../rendering/style.h"


double currentTemperature = 0.0;          
double currentWeatherCode = 0.0;          
WeatherHighLows *globalWeatherData = {0}; 
size_t highLowsCount = 0;                 

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t totalSize = size * nmemb;
    char **responsePtr = (char **)userp;

    *responsePtr = realloc(*responsePtr, strlen(*responsePtr) + totalSize + 1);
    if (*responsePtr == NULL)
    {
        fprintf(stderr, "Failed to allocate memory.\n");
        return 0;
    }

    strncat(*responsePtr, contents, totalSize);

    return totalSize;
}

WeatherData parseWeatherData(const char *json)
{
    WeatherData weatherData = {0};
    cJSON *root = cJSON_Parse(json);
    if (!root)
    {
        fprintf(stderr, "Error parsing JSON\n");
        return weatherData;
    }

    cJSON *current = cJSON_GetObjectItem(root, "current");
    if (current)
    {
        weatherData.current_temperature = cJSON_GetObjectItem(current, "temperature_2m")->valuedouble;
        weatherData.current_weather_code = cJSON_GetObjectItem(current, "weather_code")->valuedouble;
    }

    cJSON *hourly = cJSON_GetObjectItem(root, "hourly");
    if (hourly)
    {
        cJSON *times = cJSON_GetObjectItem(hourly, "time");
        cJSON *temps = cJSON_GetObjectItem(hourly, "temperature_2m");
        cJSON *codes = cJSON_GetObjectItem(hourly, "weather_code");

        size_t count = cJSON_GetArraySize(times);
        weatherData.hourly = malloc(count * sizeof(HourlyWeather));
        weatherData.hourly_count = count;

        for (size_t i = 0; i < count; i++)
        {
            cJSON *timeItem = cJSON_GetArrayItem(times, i);
            weatherData.hourly[i].temperature = cJSON_GetArrayItem(temps, i)->valuedouble;
            weatherData.hourly[i].weather_code = cJSON_GetArrayItem(codes, i)->valuedouble;
            weatherData.hourly[i].date = strdup(timeItem->valuestring); 
        }
    }

    cJSON_Delete(root);
    return weatherData;
}

WeatherData requestWeather(void)
{
    CURL *curl;
    CURLcode res;
    char *response = calloc(1, sizeof(char));
    WeatherData wd = {};

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.open-meteo.com/v1/forecast?latitude=32.82204253623829&longitude=-79.81838788885577&current=temperature_2m,weather_code&hourly=temperature_2m,weather_code&temperature_unit=fahrenheit&wind_speed_unit=mph&precipitation_unit=inch&timezone=America%2FNew_York");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        else
        {
            WeatherData weatherData = parseWeatherData(response);
            return weatherData;
        }

        free(response);
        curl_easy_cleanup(curl);
    }

    return wd;
}
// For passing to GPT API later on
WeatherData trimWeatherData(const WeatherData *original)
{
    size_t newCount = (original->hourly_count + 1) / 2;
    HourlyWeather *trimmedHourly = malloc(newCount * sizeof(HourlyWeather));

    for (size_t i = 0, j = 0; i < original->hourly_count; i += 2, j++)
    {
        trimmedHourly[j].temperature = original->hourly[i].temperature;
        trimmedHourly[j].weather_code = original->hourly[i].weather_code;
        trimmedHourly[j].date = strdup(original->hourly[i].date);
    }

    WeatherData trimmedData = {original->current_temperature, original->current_weather_code, trimmedHourly, newCount};
    return trimmedData;
}

char *weatherDataToString(const WeatherData *data)
{
    size_t bufferSize = 1024;
    char *buffer = malloc(bufferSize);
    buffer[0] = '\0';

    for (size_t i = 0; i < data->hourly_count; i++)
    {
        char entry[128];
        snprintf(entry, sizeof(entry), "Date: %s, Temp: %.2f, Code: %.2f\n",
                 data->hourly[i].date, data->hourly[i].temperature, data->hourly[i].weather_code);

        if (strlen(buffer) + strlen(entry) + 1 > bufferSize)
        {
            bufferSize *= 2;
            buffer = realloc(buffer, bufferSize);
        }

        strcat(buffer, entry);
    }

    return buffer;
}

WeatherHighLows *parseHighLows(WeatherData weatherData, size_t *outCount)
{
    if (weatherData.hourly_count == 0)
        return NULL;

    WeatherHighLows *highLows = malloc(weatherData.hourly_count * sizeof(WeatherHighLows));
    size_t highLowsCount = 0;

    char currentDay[11];
    double dailyHigh = -1e9;
    double dailyLow = 1e9;

    for (size_t i = 0; i < weatherData.hourly_count; i++)
    {
        char day[11];
        strncpy(day, weatherData.hourly[i].date, 10);
        day[10] = '\0';

        if (i == 0 || strcmp(currentDay, day) != 0)
        {
            if (i != 0)
            {
                strcpy(highLows[highLowsCount].date, currentDay);
                highLows[highLowsCount].high = dailyHigh;
                highLows[highLowsCount].low = dailyLow;
                highLowsCount++;
            }
            strcpy(currentDay, day);
            dailyHigh = weatherData.hourly[i].temperature;
            dailyLow = weatherData.hourly[i].temperature;
        }
        else
        {
            if (weatherData.hourly[i].temperature > dailyHigh)
            {
                dailyHigh = weatherData.hourly[i].temperature;
            }
            if (weatherData.hourly[i].temperature < dailyLow)
            {
                dailyLow = weatherData.hourly[i].temperature;
            }
        }
        
        
    }
    strcpy(highLows[highLowsCount].date, currentDay);
    highLows[highLowsCount].high = dailyHigh;
    highLows[highLowsCount].low = dailyLow;
    highLowsCount++;

    *outCount = highLowsCount;
    return highLows;
}

SDL_Texture *translateWeatherCodeIcon(double weatherCode)
{
    if (weatherCode >= 0 && weatherCode <= 4)
    {
        return iconSunTexture;
    }
    else if (weatherCode >= 5 && weatherCode <= 9)
    {
        return iconCloudTexture;
    }
    else if (weatherCode >= 10 && weatherCode <= 19)
    {
        return iconFogTexture;
    }
    else if (weatherCode >= 20 && weatherCode <= 29)
    {
        return iconRainTexture;
    }
    // else if (weatherCode >= 30 && weatherCode <= 39)
    // {
    //     return "Dust or Sandstorm";
    // }
    // else if (weatherCode >= 40 && weatherCode <= 49)
    // {
    //     return "Fog";
    // }
    // else if (weatherCode >= 50 && weatherCode <= 59)
    // {
    //     return "Drizzle";
    // }
    // else if (weatherCode >= 60 && weatherCode <= 69)
    // {
    //     return "Rain";
    // }
    // else if (weatherCode >= 70 && weatherCode <= 79)
    // {
    //     return "Snow";
    // }
    // else if (weatherCode >= 80 && weatherCode <= 99)
    // {
    //     return "Showers or Thunderstorms";
    // }
    return iconSunTexture;
}

char *translateWeatherCode(double weatherCode)
{
    if (weatherCode >= 0 && weatherCode <= 4)
    {
        return "Clear or Partly Cloudy";
    }
    else if (weatherCode >= 5 && weatherCode <= 9)
    {
        return "Localized Phenomena";
    }
    else if (weatherCode >= 10 && weatherCode <= 19)
    {
        return "Mist or Fog";
    }
    else if (weatherCode >= 20 && weatherCode <= 29)
    {
        return "Drizzle or Rain";
    }
    else if (weatherCode >= 30 && weatherCode <= 39)
    {
        return "Dust or Sandstorm";
    }
    else if (weatherCode >= 40 && weatherCode <= 49)
    {
        return "Fog";
    }
    else if (weatherCode >= 50 && weatherCode <= 59)
    {
        return "Drizzle";
    }
    else if (weatherCode >= 60 && weatherCode <= 69)
    {
        return "Rain";
    }
    else if (weatherCode >= 70 && weatherCode <= 79)
    {
        return "Snow";
    }
    else if (weatherCode >= 80 && weatherCode <= 99)
    {
        return "Showers or Thunderstorms";
    }
    return "Clear";
}

void initializeWeather(void)
{
    // finish parse high lows
    WeatherData weatherData = requestWeather();
    globalWeatherData = parseHighLows(weatherData, &highLowsCount);
    currentTemperature = weatherData.current_temperature;
    currentWeatherCode = weatherData.current_weather_code;

    // char *trimmedString = weatherDataToString(globalWeatherData);
    // make sure to free this because it uses malloc
    // printf("%s", trimmedString);
    //  for (size_t i = 0; i < weatherData.hourly_count; i++)
    // {
    //     printf("Hour %zu: Temp: %.2f, Code: %.2f\n", i, weatherData.hourly[i].temperature, weatherData.hourly[i].weather_code);
    //     printf("Date: %s\n", weatherData.hourly[i].date);
    // }
    // printf("Current Temperature: %.2f\n", weatherData.current_temperature);
    // printf("Current Weather Code: %.2f\n", weatherData.current_weather_code);

    //  free(weatherData.hourly);
}

void destroyWeatherData()
{
    free(globalWeatherData);
    globalWeatherData = NULL;
    free(&currentTemperature);
    free(&currentWeatherCode);
}