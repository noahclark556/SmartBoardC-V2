#ifndef TIME_CFG_H
#define TIME_CFG_H

#include <stdio.h>

void initializeDateTime(void);
void destroyDateTime(void);
const char* getDayOfWeek(const char* dateStr);
void getCurrentDate(char *buffer, size_t bufferSize);

#endif