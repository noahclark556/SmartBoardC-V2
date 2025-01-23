#ifndef LISTENERS_H
#define LISTENERS_H

#include <stdio.h>

#define COMMAND_FILE "./daemon/vdout.qdll" // My own extension :) (qwerty dll hehe)
#define RESPONSE_FILE "./daemon/response.qdll" // Path to the response file

#define BUFFER_SIZE 256
#define CLEAR_FILE 1
#define DAEMON_INTERVAL 500

extern char *responseText;
int mainEventListener(void);
void initListeners(void);
void destroyListeners(void);
#endif