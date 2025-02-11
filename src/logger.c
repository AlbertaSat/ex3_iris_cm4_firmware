#include "logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

int LOG_FILE_LOOPS = 0; 

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
 * @brief Checks if the Log File Size is larger than the Limit Set
 * 
 * @param fp Log File instance
 * @return Boolean variable indicating if the file size has been surpassed (True = File Size Reached | False = File Size NOT Reached)
 */
bool check_log_file_size(FILE *fp){

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);

    if (file_size > MAX_FILE_SIZE_MB * 1e6){
        return true;
    }
    return false;
}


/**
 * @brief Write a message to the terminal and log file to record any information or events
 * 
 * @param logLev The level indicating what type of log to record (Error, Info, Warning, Debug)
 * @param msg Pointer to character array containing message that will be logged
 */
void log_write(enum LOG_LEVEL logLev, const char *msg){

    FILE *fp;
    char filepath[512];
    snprintf(filepath, sizeof(filepath), "%s/%s", LOG_DIRECTORY, LOG_FILENAME);

    //Check if any Logging is Active
    if(!(INFO_ACTIVE || DEBUG_ACTIVE || ERROR_ACTIVE ))
        return;

    time_t curr_time = time(NULL);
    struct tm tm = *localtime(&curr_time);

    //Check if we are logging to a file
    if(LOG_TO_FILE){
        fp = fopen(filepath, "a");

        if(fp == NULL){
            printf("ERROR: UNABLE TO OPEN LOG FILE.\n");
            return;
        }

        //Checks that if Log Size is to LARGE
        if(check_log_file_size(fp)){

            fclose(fp);
            fp = fopen(filepath, "w");

            if(fp == NULL){
                printf("ERROR: UNABLE TO OPEN LOG FILE.\n");
                return;
            }
            LOG_FILE_LOOPS++;
            printf("%d-%02d-%02d %02d:%02d:%02d | " KMAG "ERROR: LOG FILE OVERFLOWED #%d\n" KNRM, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, LOG_FILE_LOOPS + 1);
            fprintf(fp, "%d-%02d-%02d %02d:%02d:%02d | " "ERROR: LOG FILE OVERFLOWED #%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, LOG_FILE_LOOPS + 1);
        }

    }

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

/**
 * @brief Sets up the blank Log File
 */
void log_file_init(void){

    char filepath[512];
    snprintf(filepath, sizeof(filepath), "%s/%s", LOG_DIRECTORY, LOG_FILENAME);
    FILE *file = fopen(filepath, "w"); // Open file for writing (creates if it doesn't exist)

    if (file == NULL) {
        printf("ERROR: Unable to create the Log File.\n");
        return;
    }

    fclose(file);
}