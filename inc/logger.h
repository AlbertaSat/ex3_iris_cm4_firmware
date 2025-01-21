#ifndef LOGGER_H
#define LOGGER_H

typedef enum LOG_LEVEL{
    LOG_INFO,
    LOG_DEBUG,
    LOG_WARNING,
    LOG_ERROR
};

//! NEED TO AUTOMATE MAKING LOG FILE NAME
#define LOG_FILE_PATH "TestLog.txt"

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

//Used to either ENABLE (1) or DISABLE (0) logging style
#define INFO_ACTIVE    1
#define DEBUG_ACTIVE   1
#define ERROR_ACTIVE   1
#define LOG_TO_FILE    1

void logger_init();
void log_write(enum LOG_LEVEL logLev, const char *msg);

#endif /* LOG_H */