// NEWS_API_H
#ifndef NEWS_API_H
#define NEWS_API_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

typedef struct
{
    char *title;
    char *description;
    // char *date;
} NewsItem;

typedef struct
{
    NewsItem *articles;
    size_t article_count;
} NewsData;


extern NewsData *globalNewsData;
extern size_t articleCount;
extern size_t WriteNewsCallback(void *contents, size_t size, size_t nmemb, void *userp);
extern NewsData requestNews(void);
extern char currentNewsTitleString[2048];
extern char currentNewsDescriptionString[2048];
extern void initializeNews(void);
void destroyNewsData(void);

#endif // NEWS_API_H
