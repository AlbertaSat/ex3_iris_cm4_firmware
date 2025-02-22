
#include "cmd_controller.h"
#include "current_sensor.h"
#include "error_handler.h"
#include "spi_iris.h"
#include "temp_read.h"

#include <gpiod.h>
#include <stdint.h>
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

IRIS_ERROR current_val_error_16bit_to_8bit(IRIS_ERROR error){

    switch (error){
        case CURR1_VAL_READ_ERROR_16BIT:
            return CURR1_VAL_READ_ERROR_8BIT;
        case CURR2_VAL_READ_ERROR_16BIT:
            return CURR2_VAL_READ_ERROR_8BIT;
        case CURR3_VAL_READ_ERROR_16BIT:
            return CURR3_VAL_READ_ERROR_8BIT;
        default:
            return CMD_FORMAT_ERROR;
    }

}

IRIS_ERROR cmd_return(int spi_dev, struct gpiod_line_request **spi_cs_request, uint8_t *buffer, uint8_t numWrites){
    return spi_write(spi_dev, buffer, numWrites, *spi_cs_request);
}

IRIS_ERROR cmd_center(uint8_t cmd, uint8_t *args, int nargs, int spi_dev, struct gpiod_line_request **spi_cs_request){

    int ncmdArg = 0;
    uint8_t addr = 0;
    uint16_t dataBuf = 0;
    enum IRIS_ERROR error = NO_ERROR;

    uint8_t cmdReturn[RETURN_CMD_SIZE] = {NO_ERROR};
    cmdReturn[0] = CMD_RETURN;

    uint8_t errorCount = 0;
    enum IRIS_ERROR errorBuffer[RETURN_CMD_SIZE] = {NO_ERROR};

    switch(cmd){

        case CURR_SENSOR_SETUP:
            addr = cmd_to_current_addr(args[0]);
            if (addr == CMD_FORMAT_ERROR){
                cmdReturn[1] = CMD_FORMAT_ERROR;
                error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 2);
                break;
            }
            cmdReturn[1] = current_setup(addr);
            error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 2);
            break;

        case CURR_SENSOR_VALIDATE:
            addr = cmd_to_current_addr(args[0]);
            if (addr == CMD_FORMAT_ERROR){
                cmdReturn[1] = CMD_FORMAT_ERROR;
                error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 2);
                break;
            }
            cmdReturn[1] = current_func_validate(addr);
            error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 2);
            break;

        case CURR_SENSOR_STATUS:
            //ADD CODE
        case CURR_SENSOR_RESET:
            addr = cmd_to_current_addr(args[0]);
            if (addr == CMD_FORMAT_ERROR){
                cmdReturn[1] = CMD_FORMAT_ERROR;
                error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 2);
                break;
            }
            cmdReturn[1] = current_monitor_reset(addr);
            error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 2);
            break;

        case CURR_SENSOR_READ_CURRENT:
            addr = cmd_to_current_addr(args[0]);
            if (addr == CMD_FORMAT_ERROR){
                cmdReturn[1] = CMD_FORMAT_ERROR;
                error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 2);
                break;
            }
            dataBuf = read_current(addr);

            if ((dataBuf == CURR1_VAL_READ_ERROR_16BIT) ||
                (dataBuf == CURR2_VAL_READ_ERROR_16BIT) ||
                (dataBuf == CURR3_VAL_READ_ERROR_16BIT)){

                    cmdReturn[1] = current_val_error_16bit_to_8bit(dataBuf);
                    error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 2);
                    break;
                }
            cmdReturn[1] = NO_ERROR;
            cmdReturn[2] = (dataBuf >> 8) & 0xFF;  // MSB
            cmdReturn[3] =  dataBuf & 0xFF;        // LSB
            error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 4);
            break;

        case CURR_SENSOR_READ_VOLTAGE:
            addr = cmd_to_current_addr(args[0]);
            if (addr == CMD_FORMAT_ERROR){
                cmdReturn[1] = CMD_FORMAT_ERROR;
                error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 2);
                break;
            }
            dataBuf = read_bus_voltage(addr);

            if ((dataBuf == CURR1_VAL_READ_ERROR_16BIT) ||
                (dataBuf == CURR2_VAL_READ_ERROR_16BIT) ||
                (dataBuf == CURR3_VAL_READ_ERROR_16BIT)){

                    cmdReturn[1] = current_val_error_16bit_to_8bit(dataBuf);
                    error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 2);
                    break;
                }
            cmdReturn[1] = NO_ERROR;
            cmdReturn[2] = (dataBuf >> 8) & 0xFF;  // MSB
            cmdReturn[3] =  dataBuf & 0xFF;        // LSB
            error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 4);
            break;

        case CURR_SENSOR_READ_POWER:
            addr = cmd_to_current_addr(args[0]);
            if (addr == CMD_FORMAT_ERROR){
                cmdReturn[1] = CMD_FORMAT_ERROR;
                error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 2);
                break;
            }
            dataBuf = read_power(addr);

            if ((dataBuf == CURR1_VAL_READ_ERROR_16BIT) ||
                (dataBuf == CURR2_VAL_READ_ERROR_16BIT) ||
                (dataBuf == CURR3_VAL_READ_ERROR_16BIT)){

                    cmdReturn[1] = current_val_error_16bit_to_8bit(dataBuf);
                    error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 2);
                    break;
                }
            cmdReturn[1] = NO_ERROR;
            cmdReturn[2] = (dataBuf >> 8) & 0xFF;  // MSB
            cmdReturn[3] =  dataBuf & 0xFF;        // LSB
            error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 4);
            break;

        case CURR_SENSOR_READ_PK_POWER:
            addr = cmd_to_current_addr(args[0]);
            if (addr == CMD_FORMAT_ERROR){
                cmdReturn[1] = CMD_FORMAT_ERROR;
                error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 2);
                break;
            }
            dataBuf = read_pk_power(addr);

            if ((dataBuf == CURR1_VAL_READ_ERROR_16BIT) ||
                (dataBuf == CURR2_VAL_READ_ERROR_16BIT) ||
                (dataBuf == CURR3_VAL_READ_ERROR_16BIT)){

                    cmdReturn[1] = current_val_error_16bit_to_8bit(dataBuf);
                    error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 2);
                    break;
                }
            cmdReturn[1] = NO_ERROR;
            cmdReturn[2] = (dataBuf >> 8) & 0xFF;  // MSB
            cmdReturn[3] =  dataBuf & 0xFF;        // LSB
            error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 4);
            break;

        case CURR_SENSOR_READ_LIMIT:

            current_limit(errorBuffer, &errorCount);

            if(errorCount == 0){
                cmdReturn[2] = NO_ERROR;
                errorCount = 1;
            }else{
                for(int index = 0; index < errorCount; index++){
                    cmdReturn[index+1] = errorBuffer[index];
                }
            }
            error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 1 + errorCount);
            break;

        case TEMP_SENSOR_SETUP:
            addr = cmd_to_temp_addr(args[0]);
            if (addr == CMD_FORMAT_ERROR){
                cmdReturn[1] = CMD_FORMAT_ERROR;
                error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 2);
                break;
            }
            cmdReturn[1] = temp_setup(addr);
            error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 2);
            break;

        case TEMP_SENSOR_VALIDATE:
            addr = cmd_to_temp_addr(args[0]);
            if (addr == CMD_FORMAT_ERROR){
                cmdReturn[1] = CMD_FORMAT_ERROR;
                error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 2);
                break;
            }
            cmdReturn[1] = temp_func_validate(addr);
            error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 2);
            break;

        case TEMP_SENSOR_RESET:
            addr = cmd_to_temp_addr(args[0]);
            if (addr == CMD_FORMAT_ERROR){
                cmdReturn[1] = CMD_FORMAT_ERROR;
                error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 2);
                break;
            }
            cmdReturn[1] = temp_reset(addr);
            error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 2);
            break;

        case TEMP_SENSOR_READ:
            addr = cmd_to_temp_addr(args[0]);
            if (addr == CMD_FORMAT_ERROR){
                cmdReturn[1] = CMD_FORMAT_ERROR;
                error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 2);
                break;
            }

            cmdReturn[2] = read_temperature(addr);

            if ((cmdReturn[2] == TEMP1_TEMP_READ_ERROR) ||
                (cmdReturn[2] == TEMP2_TEMP_READ_ERROR) ||
                (cmdReturn[2] == TEMP3_TEMP_READ_ERROR) ||
                (cmdReturn[2] == TEMP4_TEMP_READ_ERROR)){

                cmdReturn[1] = cmdReturn[2];
                error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 2);
                break;
            }

            cmdReturn[1] = NO_ERROR;
            error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 3);
            break;

        case TEMP_SENSOR_READ_LIMIT:
            temperature_limit(errorBuffer, &errorCount);

            for(int x = 0; x < errorCount; x++){
                cmdReturn[x+1] = errorBuffer[x];
            }
            error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 1 + errorCount);
            break;

        case USB_HUB_SETUP:
            //ADD CODE
        case USB_HUB_VALIDATE:
            //ADD CODE
        case USB_HUB_RESET:
            //ADD CODE
        case ERROR_TRANSFER:
            error = iris_error_transfer(spi_dev, spi_cs_request, errorBuffer, errorCount);
            break;
        case IMAGE_CONFIG:
            //ADD CODE
        case IMAGE_CAPTURE:
            //ADD CODE
        case FILE_TRANSFER:
            //ADD CODE
        default:
            cmdReturn[1] = CMD_FORMAT_ERROR;
            error = cmd_return(spi_dev, spi_cs_request, cmdReturn, 2);
    }
    return error;
}

//! ISSUES WITH ARG DECODING
int cmd_extracter(uint8_t *cmd, uint8_t *arg, const uint8_t *rx_buffer, uint8_t rx_len){

    uint8_t current_byte = 0;
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