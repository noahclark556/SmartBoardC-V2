#ifndef LISTENERS_H
#define LISTENERS_H

#include <stdio.h>

#define COMMAND_FILE "./daemon/vdout.qdll"
#define RESPONSE_FILE "./daemon/response.qdll"
#define MODE_FILE "./daemon/status.qdll"

#define BUFFER_SIZE 256
#define CLEAR_FILE 1
#define DAEMON_INTERVAL 500

extern char *responseText;
int mainEventListener(void);
void initListeners(void);
void destroyListeners(void);
#endif