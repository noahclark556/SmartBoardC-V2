#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include "database.h"
#include "../../config/api.h"
#include "../../config/cfg.h"

#define INITIAL_BUFFER_SIZE 8096

DatabaseData databaseData = {0};
typedef struct {
    char *data;
    size_t size;
} ResponseBuffer;

// Function to handle the response data
size_t DBWriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t totalSize = size * nmemb;
    ResponseBuffer *buffer = (ResponseBuffer *)userp;

    // Reallocate memory to fit new data
    char *newData = realloc(buffer->data, buffer->size + totalSize + 1);
    if (newData == NULL) {
        fprintf(stderr, "Memory allocation failed in callback\n");
        return 0;  // Returning 0 will cause curl to stop the request
    }

    buffer->data = newData;
    memcpy(&(buffer->data[buffer->size]), contents, totalSize);
    buffer->size += totalSize;
    buffer->data[buffer->size] = '\0';  // Null-terminate the string

    return totalSize;
}

DatabaseData parseDatabaseData(const char *json)
{
    cJSON *root = cJSON_Parse(json);
    if (!root)
    {
        fprintf(stderr, "Error parsing JSON\n");
        return databaseData;
    }
   
    cJSON *noteItem = cJSON_GetObjectItem(root, "note");
    if (noteItem && cJSON_IsString(noteItem))
    {
        databaseData.note = strdup(noteItem->valuestring);
    }
    else
    {
        printf("Note key not found or is not a string.\n");
    }
    cJSON *agendas = cJSON_GetObjectItem(root, "agendas");
    if (agendas && cJSON_IsObject(agendas))
    {
        databaseData.agenda_count = cJSON_GetArraySize(agendas);
        databaseData.agenda = malloc(databaseData.agenda_count * sizeof(AgendaData));
        if (databaseData.agenda == NULL)
        {
            fprintf(stderr, "Memory allocation failed\n");
            cJSON_Delete(root);
            return databaseData;
        }

        size_t index = 0;
        cJSON *dateItem;
        cJSON_ArrayForEach(dateItem, agendas)
        {
            const char *date = dateItem->string;

            cJSON *timeItem = dateItem->child;
            while (timeItem != NULL)
            {
                const char *time = timeItem->string;
                const char *note = timeItem->valuestring;

                // printf("  Time: %s, Note: %s\n", time, note);

                databaseData.agenda[index].date = strdup(date);
                databaseData.agenda[index].hour = strdup(time);
                databaseData.agenda[index].note = strdup(note);
                index++;

                timeItem = timeItem->next;
            }
        }
        databaseData.agenda_count = index;
    }

    // cJSON *history = cJSON_GetObjectItem(root, "history");
    // if (history && cJSON_IsArray(history))
    // {
    //     databaseData.history_count = cJSON_GetArraySize(history);
    //     size_t maxHistory = 2;
    //     printf("History count: %zu\n", databaseData.history_count);
    //     databaseData.history = malloc(databaseData.history_count * sizeof(char *));
    //     if (databaseData.history == NULL)
    //     {
    //         fprintf(stderr, "Memory allocation failed\n");
    //         cJSON_Delete(root);
    //         return databaseData;
    //     }
    //     size_t index = 0;
    //     cJSON *historyItem;
    //     size_t bufferSize = 2048;
    //     char *buffer = malloc(bufferSize);
    //     buffer[0] = '\0';
    //     int trimHistory = 0; // index to start trimming from if over max
    //     if(databaseData.history_count > maxHistory){
    //         trimHistory = databaseData.history_count - maxHistory;
    //     }
    //     printf("History count: %d", databaseData.history_count);
    //     cJSON_ArrayForEach(historyItem, history)
    //     {
    //         char entry[256];
    //         printf("Index: %zu\n", index);
    //         if(trimHistory > 0 && index <= trimHistory){
    //             printf("Skipping history item %zu\n", index);
    //             index++;
    //             continue;
    //         }
    //         if (cJSON_IsString(historyItem))
    //         {                                                         // Check if the item is a string
    //             const char *historyString = historyItem->valuestring; // Get the string value           // Print the history item
    //             snprintf(entry, sizeof(entry), "%s", historyString);
    //             // Store the string in the databaseData.history array
    //             databaseData.history[index] = strdup(historyString); // Duplicate the string
    //             if (strlen(buffer) + strlen(entry) + 1 > bufferSize) {
    //                 bufferSize *= 2;
    //                 char *newBuffer = realloc(buffer, bufferSize);
    //                 if (newBuffer == NULL) {
    //                     fprintf(stderr, "Failed to reallocate memory for buffer.\n");
    //                     free(buffer); // Free the old buffer
    //                 }
    //                 buffer = newBuffer; // Update the buffer pointer
    //             }
    //             strcat(buffer, entry);
    //         }
    //         index++;
    //     }
    //     writeHistoryToFile(buffer);
    //     databaseData.history_count = index;
    // }

    cJSON_Delete(root);
    return databaseData;
}



int syncDatabase() {
    CURL *curl;
    CURLcode res;

    ResponseBuffer response = {malloc(INITIAL_BUFFER_SIZE), 0};
    if (!response.data) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    response.data[0] = '\0';  // Ensure it's null-terminated

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, READ_DB_FUNCTION_URL);

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Create JSON request body
        cJSON *jsonBody = cJSON_CreateObject();
        cJSON_AddStringToObject(jsonBody, "userId", DB_USER_ID);
        char *jsonString = cJSON_PrintUnformatted(jsonBody);  // More compact JSON output
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonString);

        // Set up response handling
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, DBWriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            cJSON *json = cJSON_Parse(response.data);
            if (json) {
                parseDatabaseData(response.data);
                cJSON_Delete(json);
            } else {
                printf("JSON parsing error\n");
            }
        }

        // Cleanup
        free(response.data);
        free(jsonString);
        cJSON_Delete(jsonBody);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    } else {
        free(response.data);
    }

    curl_global_cleanup();
    return 0;
}