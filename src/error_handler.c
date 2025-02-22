
#include "cmd_controller.h"
#include "error_handler.h"
#include "gpio.h"
#include "spi_iris.h"


#include <gpiod.h>
#include <stdint.h>

//! ADD ERROR DETECTIONS FOR SPI WHERE IT ONLY RESET ERROR COUNTER IF THE TRANSFER WAS SUCCESSFUL
//! NEED TO MAKE IT SO IT CONVERTS ANY 16-BIT ERRORS TO 8-BIT REPRESENTATIVE
//! NEED TO FIGURE OUT HOW TO AVOID RACE CONDITION
    //! COULD HAVE SITUATION WHERE WE LOOK AT csVal AND ITS HIGH BUT ONCE
    //! WE GET TO TRANSFERING DATA THE OBC HAS PULLED LINE LOW
    //! MAYBE HAVE IT SO DIRECTLY BEFORE TRANSFER WE CHECK CSVAL
enum IRIS_ERROR iris_error_transfer(int spi_dev, struct gpiod_line_request *spi_cs_request, const enum IRIS_ERROR *errorBuffer, uint8_t *errorCount){

    IRIS_ERROR spiError = NO_ERROR;
    uint8_t dataBuffer[ERROR_BUFFER_SIZE + 2] = {0};
    enum gpiod_line_value csVal = GPIOD_LINE_VALUE_INACTIVE;

    csVal = gpiod_line_request_get_value(spi_cs_request, SPI_CE_N);

    // Only Proceed with transfer if CS Line is inactive
    if (csVal == GPIOD_LINE_VALUE_ACTIVE){

        // Number of Error Packets to Send
        dataBuffer[0] = ERROR_TRANSFER;
        dataBuffer[1] = *errorCount;

        for (int index = 0; index < *errorCount; index++){
            dataBuffer[index + 2] = errorBuffer[index];
        }

        spiError = spi_write(spi_dev, dataBuffer, *errorCount + 2, spi_cs_request);

        // Checks if the Error Transfer was SUCCESSFUL
        if (spiError == NO_ERROR){
            *errorCount = 0;
            return spiError;
        }
        return spiError;
    }
    return NO_ERROR;
}