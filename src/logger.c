#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "logger.h"

/**
 * @file logger.c
 * @author Noah Klager
 * @brief Logger for Theia CM4 
 *        Provides functions to...
 *         - Log Information / Errors to terminal
 *         - Log Information / Errors to a file
 * 
 * @version 0.1
 * @date 2024-11-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */


/**
 * @brief Write a message to the terminal and log file to record any information or events
 * 
 * @param logLev The level indicating what type of log to record (Error, Info, Warning, Debug)
 * @param msg Pointer to character array containing message that will be logged
 */
//! NEED TO ADD IT SO IT APPENDS FILE
void log_write(enum LOG_LEVEL logLev, const char *msg){

    char *logMsg = NULL;
    FILE *fp;

    //Check if any Logging is Active
    if(!(INFO_ACTIVE || DEBUG_ACTIVE || ERROR_ACTIVE ))
        return;

    //Check if we are logging to a file
    if(LOG_TO_FILE){
        fp = fopen(LOG_FILE_PATH, "a");
    }

    time_t curr_time = time(NULL);
    struct tm tm = *localtime(&curr_time);

    //Log to terminal or file
    switch (logLev){
    case LOG_ERROR:
        if (ERROR_ACTIVE){
            printf("%d-%02d-%02d %02d:%02d:%02d | " KRED "ERROR: %s\n" KNRM, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, msg);
            if (LOG_TO_FILE)
                fprintf(fp, "%d-%02d-%02d %02d:%02d:%02d | " "ERROR: %s\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, msg);
        }
        break;
    case LOG_WARNING:
        if (ERROR_ACTIVE){
            printf("%d-%02d-%02d %02d:%02d:%02d | " KYEL "WARN : %s\n" KNRM, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, msg);
            if (LOG_TO_FILE)
                fprintf(fp, "%d-%02d-%02d %02d:%02d:%02d | " "WARN : %s\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, msg);
        }
        break;
    case LOG_DEBUG:
        if (DEBUG_ACTIVE){
            printf("%d-%02d-%02d %02d:%02d:%02d | " KGRN "DEBUG: %s\n" KNRM, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, msg);
            if (LOG_TO_FILE)
                fprintf(fp, "%d-%02d-%02d %02d:%02d:%02d | " "DEBUG: %s\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, msg);
        }
        break;
    default:
        if (INFO_ACTIVE){
            printf("%d-%02d-%02d %02d:%02d:%02d | " KWHT "INFO : %s\n" KNRM, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, msg);
            if (LOG_TO_FILE)
                fprintf(fp, "%d-%02d-%02d %02d:%02d:%02d | " "INFO : %s\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, msg);
        }
    }

    fclose(fp);
    return;
}