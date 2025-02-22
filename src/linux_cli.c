
#include "error_handler.h"
#include "logger.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int test_cli() {
    char buffer[128];
    char result[1024] = {0};  // Store the full command output
    FILE *pipe = popen("ls -l", "r"); // Run a shell command

    if (!pipe) {
        perror("popen failed");
        return 1;
    }

    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        strcat(result, buffer);
    }

    pclose(pipe);

    printf("Command output:\n%s", result);

    return 0;
}


IRIS_ERROR exec_linux_cli_cmd(char* input_cmd, char* output_response){

    char logBuffer[255];
    char tempBuffer[255];

    FILE *cliPipe = popen(input_cmd, "r");

    if(!cliPipe){
        snprintf(logBuffer, sizeof(logBuffer), "Unable to open Linux CLI Pipe: %s", strerror(errno));
        log_write(LOG_ERROR, logBuffer);
        pclose(cliPipe);
        return LINUX_CLI_ERROR;
    }

    while (fgets(tempBuffer, sizeof(tempBuffer), cliPipe) != NULL) {
        strcat(output_response, tempBuffer);
    }

    pclose(cliPipe);

    return NO_ERROR;

}