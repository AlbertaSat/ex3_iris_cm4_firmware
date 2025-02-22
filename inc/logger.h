#ifndef LOGGER_H
#define LOGGER_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum LOG_LEVEL{
    LOG_INFO,
    LOG_DEBUG,
    LOG_WARNING,
    LOG_ERROR
} LOG_LEVEL;

//! NEED TO AUTOMATE MAKING LOG FILE NAME
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

//Used to either ENABLE (1) or DISABLE (0) logging style
typedef enum LOG_STATES{
    INFO_ACTIVE  = 1,
    DEBUG_ACTIVE = 1,
    ERROR_ACTIVE = 1,
    LOG_TO_FILE  = 1,
}LOG_STATES;

#define LOG_DIRECTORY "/home/iris/ex3_iris_cm4_firmware"
#define LOG_FILENAME  "Iris_Log.txt"

#define MAX_FILE_SIZE_MB 2000
#define LOG_BUFFER_SIZE  255
#define LOG_FILE_PATH_LEN 512

void log_file_init(void);
bool check_log_file_size(FILE *filePtr);
void log_write(enum LOG_LEVEL logLev, const char *msg);

#endif /* LOGGER_H */