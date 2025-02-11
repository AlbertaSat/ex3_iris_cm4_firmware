
#include "gpio.h"
#include "spi_iris.h"
#include "cmd_controller.h"
#include "error_handler.h"
#include "current_sensor.h"
#include "temp_read.h"

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

uint8_t cmd_to_current_addr(uint8_t arg){

    switch (arg){

        case 1:
            return CURRENT_SENSOR_ADDR_3V3;
        case 2:
            return CURRENT_SENSOR_ADDR_5V;
        case 3:
            return CURRENT_SENSOR_ADDR_CAM;
        default:
            return CMD_FORMAT_ERROR;
    }
}

uint8_t cmd_to_temp_addr(uint8_t arg){

    switch (arg){

        case 1:
            return TEMP_SENSOR_1_ADDR;
        case 2:
            return TEMP_SENSOR_2_ADDR;
        case 3:
            return TEMP_SENSOR_3_ADDR;
        case 4:
            return TEMP_SENSOR_4_ADDR;
        default:
            return CMD_FORMAT_ERROR;
    }
}

void cmd_return(int spi_dev, struct gpiod_line_request **spi_cs_request, uint8_t *buffer, uint8_t numWrites){

    spi_write(spi_dev, buffer, numWrites, spi_cs_request);
// RETURN DATA PACKET

}

void cmd_center(uint8_t cmd, 
                uint8_t *args,
                int nargs,
                int spi_dev, 
                struct gpiod_line_request **spi_cs_request){

    int ncmdArg;
    uint8_t addr = 0;
    enum IRIS_ERROR error = NO_ERROR;
    uint8_t cmdReturn[2] = {CMD_RETURN, NO_ERROR};

    switch(cmd){

        case CURR_SENSOR_SETUP:
            addr = cmd_to_current_addr(args[0]);
            if (addr == CMD_FORMAT_ERROR){
                cmdReturn[1] = CMD_FORMAT_ERROR;
                cmd_return(spi_dev, spi_cs_request, cmdReturn, 2);
                break;
            }
            cmdReturn[1] = current_setup(addr);
            cmd_return(spi_dev, spi_cs_request, cmdReturn, 2);
            break;
            //ADD CODE
        case CURR_SENSOR_VALIDATE:
            //ADD CODE
        case CURR_SENSOR_STATUS:
            //ADD CODE
        case CURR_SENSOR_RESET:
            //ADD CODE
        case CURR_SENSOR_READ:
            //ADD CODE
        case CURR_SENSOR_READ_LIMIT:
            //ADD CODE
        case TEMP_SENSOR_SETUP:
            //ADD CODE
        case TEMP_SENSOR_VALIDATE:
            //ADD CODE
        case TEMP_SENSOR_STATUS:
            //ADD CODE
        case TEMP_SENSOR_RESET:
            //ADD CODE
        case TEMP_SENSOR_READ:
            //ADD CODE
        case TEMP_SENSOR_READ_LIMIT:
            //ADD CODE
        case USB_HUB_SETUP:
            //ADD CODE
        case USB_HUB_VALIDATE:
            //ADD CODE
        case USB_HUB_RESET:
            //ADD CODE
        case ERROR_TRANSFER:
            //ADD CODE
        case IMAGE_CONFIG:
            //ADD CODE
        case IMAGE_CAPTURE:
            //ADD CODE
        case FILE_TRANSFER:
            //ADD CODE
        default:
            //cmd_return(CMD_FORMAT_ERROR); //ADD ERROR RESPONSE
    }
    return;
}

//! ISSUES WITH ARG DECODING
int cmd_extracter(uint8_t *cmd, uint8_t *arg, uint8_t *rx_buffer, uint8_t rx_len){

    uint8_t current_byte = NULL;
    int arg_num = -1;

    *cmd = rx_buffer[0];
 
    for(int index = 1; (index <= rx_len) && (current_byte != END_SPI_CMD); index++){

        current_byte = rx_buffer[index];

        if (current_byte != END_SPI_CMD){
            arg[++arg_num] = current_byte;
        }
    }

    return arg_num;
}