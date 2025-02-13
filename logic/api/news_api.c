#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <string.h>
#include <curl/curl.h>
#include "news_api.h"
#include "../rendering/style.h"
#include "../../config/api.h"
#include "../../config/cfg.h"
NewsData *globalNewsData = {0};
size_t articleCount = 0;
char currentNewsTitleString[2048];
char currentNewsDescriptionString[2048];

size_t WriteNewsCallback(void *contents, size_t size, size_t nmemb, void *userp)
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

NewsData parseNewsData(const char *json)
{
    NewsData newsData = {0};
    cJSON *root = cJSON_Parse(json);
    if (!root)
    {
        fprintf(stderr, "Error parsing JSON\n");
        return newsData;
    }

    cJSON *mArticles = cJSON_GetObjectItem(root, "results");
    if (mArticles && cJSON_IsArray(mArticles))
    {
        size_t count = cJSON_GetArraySize(mArticles);
        newsData.articles = malloc(count * sizeof(NewsItem));
        if (!newsData.articles)
        {
            fprintf(stderr, "Memory allocation failed\n");
            cJSON_Delete(root);
            return newsData;
        }
        newsData.article_count = count;

        for (size_t i = 0; i < count; i++)
        {
            cJSON *articleItem = cJSON_GetArrayItem(mArticles, i);
            if (!articleItem)
                continue;

            cJSON *title = cJSON_GetObjectItem(articleItem, "title");
            cJSON *description = cJSON_GetObjectItem(articleItem, "description");
            // newsData.articles[i].title = "test test test test test test test testttttttt ttesttetetdjfjsdfjksdfjgksjfkjsfjsfgkj tet etetetetekdjafkjdkjfkdfkdkfdkjfkt tet etetetettesdiafieurijenrenrjnejnjre et tet et ee tt e";

            newsData.articles[i].title = title && cJSON_IsString(title) ? strdup(title->valuestring) : NULL;
            newsData.articles[i].description = description && cJSON_IsString(description) ? strdup(description->valuestring) : NULL;
        }
    }

    cJSON_Delete(root);
    return newsData;
}

NewsData requestNews(void)
{
    CURL *curl;
    CURLcode res;
    char *response = calloc(1, sizeof(char));
    NewsData nd = {};

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, NEWS_API_URL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteNewsCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        else
        {
            NewsData newsData = parseNewsData(response);
            return newsData;
        }

        free(response);
        curl_easy_cleanup(curl);
    }

    return nd;
}
// -1 specificIndex gets all news, any other value gets that index in the news and item represents the item to get
char *newsDataToString(const NewsData *data, size_t specificIndex, size_t item)
{
    if (data == NULL || data->articles == NULL || data->article_count == 0)
    {
        return NULL;
    }

    size_t bufferSize = 2048;
    char *buffer = malloc(bufferSize);
    if (buffer == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for buffer.\n");
        return NULL;
    }
    buffer[0] = '\0';

    if (specificIndex != -1)
    {
        char entry[2048];

        if (item == 1)
        {
            snprintf(entry, sizeof(entry), "%s", data->articles[specificIndex].title);
        }
        else if (item == 2)
        {
            if (data->articles[specificIndex].description == NULL)
            {
                snprintf(entry, sizeof(entry), "%s", data->articles[specificIndex].title);
            }
            else
            {
                snprintf(entry, sizeof(entry), "%s", data->articles[specificIndex].description);
            }
        }
        else
        {
            snprintf(entry, sizeof(entry), "Title: %s, Description: %s\n",
                     data->articles[specificIndex].title, data->articles[specificIndex].description);
        }

        strcat(buffer, entry);
    }
    else
    {
        for (size_t i = 0; i < data->article_count; i++)
        {
            char entry[256];
            snprintf(entry, sizeof(entry), "Title: %s, Description: %s\n",
                     data->articles[i].title, data->articles[i].description);

            if (strlen(buffer) + strlen(entry) + 1 > bufferSize)
            {
                bufferSize *= 2;
                char *newBuffer = realloc(buffer, bufferSize);
                if (newBuffer == NULL)
                {
                    fprintf(stderr, "Failed to reallocate memory for buffer.\n");
                    free(buffer);
                    return NULL;
                }
                buffer = newBuffer;
            }

            strcat(buffer, entry);
        }
    }
    return buffer;
}

void initializeNews(void)
{
    NewsData newsData = requestNews();
    if (newsData.article_count > 0)
    {
        char *newsTitleString = newsDataToString(&newsData, newsDataIndex, 1);
        char *newsDescriptionString = newsDataToString(&newsData, newsDataIndex, 2);
        // printf("News String: %s\n", newsString);
        strcpy(currentNewsTitleString, newsTitleString);
        strcpy(currentNewsDescriptionString, newsDescriptionString);
        printf("Current News Title String: %s\n", currentNewsTitleString);
        printf("Current News Description String: %s\n", currentNewsDescriptionString);
    }
    else
    {
        char *emptyString = "N/A";
        strcpy(currentNewsTitleString, emptyString);
        strcpy(currentNewsDescriptionString, emptyString);
    }

    // for (size_t i = 0; i < newsData.article_count; i++)
    // {
    //     printf("Title: %s\n", newsData.articles[i].title);
    //     printf("Description: %s\n", newsData.articles[i].description);
    // }

    // globalWeatherData = parseHighLows(weatherData, &highLowsCount);
    // currentTemperature = weatherData.current_temperature;
    // currentWeatherCode = weatherData.current_weather_code;
    // printf("HighLows count: %zu\n", weatherData.hourly_count);
    // WeatherData trimmedData = trimWeatherData(&weatherData, 3);
    // writeWeatherDataToFile(&trimmedData);
}

void destroyNewsData()
{
    free(globalNewsData);
    // free(currentNewsString);
    // currentNewsString = NULL;
    globalNewsData = NULL;
}