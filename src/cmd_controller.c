
#include "gpio.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <gpiod.h>
#include <unistd.h>

#include <errno.h>
#include <gpiod.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void cmd_center(int nargs, char *argv[]){

    char cmd[25];
    char arg[10][25];
    int ncmdArg;

    *cmd = argv[0];

    if(strcmp(cmd, "Command1")){

    }else if(strcmp(cmd, "Command2")){

    }else if(strcmp(cmd, "Command3")){

    }else{
        return -1; // Not a valid command
    }

}

//! ISSUES WITH ARG DECODING
int cmd_extracter(char *cmd, char arg[][100], uint8_t *rx_buffer, uint8_t rx_len){

    char current_char = NULL;
    bool cmd_done = false;
    int arg_index = 0;
    int arg_num = 0;
    
    for(int index = 0; (index < rx_len) && (current_char != '\n'); index++){

        current_char = rx_buffer[index];

        if (cmd_done == false){

            if(current_char == ' '){
                cmd[index] = '\n';
                cmd_done = true;
            }else{
                cmd[index] = current_char;
            }
        
        }else{
            
            if(current_char == ' '){
                arg[arg_num][arg_index] = '\n';
                arg_num++;
                arg_index = 0; 
            }else if(current_char == 0){
                arg[arg_num][arg_index] = '\n';
                break; 
            }else{
                arg[arg_num][arg_index] = current_char;
                arg_index++;
            }
        }
    }

    return arg_num;
}