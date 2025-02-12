#include "error_handler.h"
#include "cmd_controller.h"
#include "spi_iris.h"
#include "gpio.h"

#include <gpiod.h>
#include <stdio.h>
#include <stdint.h>


//! ADD ERROR DETECTIONS FOR SPI WHERE IT ONLY RESET ERROR COUNTER IF THE TRANSFER WAS SUCCESSFUL
enum IRIS_ERROR iris_error_transfer(int spi_dev, 
                                    struct gpiod_line_request *spi_cs_request,
                                    enum IRIS_ERROR *errorBuffer,
                                    uint8_t *errorCount){

    bool cs_edge = false;
    IRIS_ERROR spiError = NO_ERROR;
    uint16_t dataBuffer[ERROR_BUFFER_SIZE + 2] = {NULL};
    enum gpiod_line_value csVal;

    csVal = gpiod_line_request_get_value(spi_cs_request, SPI_CE_N);

    // Only Proceed with transfer if CS Line is inactive
    if (csVal == GPIOD_LINE_VALUE_ACTIVE){

        // Number of Error Packets to Send
        dataBuffer[0] = ERROR_TRANSFER;
        dataBuffer[1] = *errorCount;

        for (int x = 0; x < *errorCount; x++){
            dataBuffer[x + 2] = errorBuffer[x];
        }

        spiError = spi_write(spi_dev, dataBuffer, *errorCount + 2, spi_cs_request);

        // Checks if the Error Transfer was SUCCESSFUL
        if (spiError == NO_ERROR){
            *errorCount = 0;
            return spiError;
        }else{
            return spiError;
        }
    }
}