#include "temp_read.h"
#include "i2c.h"
#include "main.h"
#include "gpio.h"
#include "logger.h"
#include "error_handler.h"

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

/**
 * @file temp_read.c
 * @author Noah Klager
 * @brief Temperature Sensor Driver for Theia CM4 
 *        Provides functions to...
 *         - Configure temperature sensors
 *         - Verifies functionality of temperature sensors
 *         - Resets temperature sensors
 *         - Reads the temperature measurement of sensors
 *         - Detects if any temperature measurements are out of specifications
 * 
 * @version 0.1
 * @date 2024-11-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//Used to convert Temperature sensor register value into actual measurement.
//Can be altered to help calibrate the sensor
#define CONVERSION_FACTOR 1 

/**
 * @brief Converts a general error code into a specific one used to indicate which
 *        temperature sensor had the error
 * 
 * @param tempAddr I2C Address of Temperature sensor
 * @param errorType General Iris error detected
 * @return Specific Iris error code indicating which temperature sensor had the error
 */
enum IRIS_ERROR temp_error_code(uint8_t tempAddr, enum IRIS_ERROR errorType){

    switch (errorType){
        case TEMP1_SETUP_ERROR:
            switch (tempAddr){
                case TEMP_SENSOR_1_ADDR:
                    return TEMP1_SETUP_ERROR;
                case TEMP_SENSOR_2_ADDR:
                    return TEMP2_SETUP_ERROR;
                case TEMP_SENSOR_3_ADDR:
                    return TEMP3_SETUP_ERROR;
                case TEMP_SENSOR_4_ADDR:
                    return TEMP4_SETUP_ERROR;
                default:
                    // This will only happen if there is a coding error when using this function
                    log_write(LOG_ERROR, "Invalid Temp Sensor ERROR");
                    exit(EXIT_FAILURE);
            }

        case TEMP1_VERIFICATION_ERROR:
            switch (tempAddr){
                case TEMP_SENSOR_1_ADDR:
                    return TEMP1_VERIFICATION_ERROR;
                case TEMP_SENSOR_2_ADDR:
                    return TEMP2_VERIFICATION_ERROR;
                case TEMP_SENSOR_3_ADDR:
                    return TEMP3_VERIFICATION_ERROR;
                case TEMP_SENSOR_4_ADDR:
                    return TEMP4_VERIFICATION_ERROR;
                default:
                    // This will only happen if there is a coding error when using this function
                    log_write(LOG_ERROR, "Invalid Temp Sensor ERROR");
                    exit(EXIT_FAILURE);
            }

        case TEMP1_RESET_ERROR:
            switch (tempAddr){
                case TEMP_SENSOR_1_ADDR:
                    return TEMP1_RESET_ERROR;
                case TEMP_SENSOR_2_ADDR:
                    return TEMP2_RESET_ERROR;
                case TEMP_SENSOR_3_ADDR:
                    return TEMP3_RESET_ERROR;
                case TEMP_SENSOR_4_ADDR:
                    return TEMP4_RESET_ERROR;
                default:
                    // This will only happen if there is a coding error when using this function
                    log_write(LOG_ERROR, "Invalid Temp Sensor ERROR");
                    exit(EXIT_FAILURE);
            }

        case TEMP1_TEMP_READ_ERROR:
            switch (tempAddr){
                case TEMP_SENSOR_1_ADDR:
                    return TEMP1_TEMP_READ_ERROR;
                case TEMP_SENSOR_2_ADDR:
                    return TEMP2_TEMP_READ_ERROR;
                case TEMP_SENSOR_3_ADDR:
                    return TEMP3_TEMP_READ_ERROR;
                case TEMP_SENSOR_4_ADDR:
                    return TEMP4_TEMP_READ_ERROR;
                default:
                    // This will only happen if there is a coding error when using this function
                    log_write(LOG_ERROR, "Invalid Temp Sensor ERROR");
                    exit(EXIT_FAILURE);
            }

        case TEMP1_LIMIT_ERROR:
            switch (tempAddr){
                case TEMP_SENSOR_1_ADDR:
                    return TEMP1_LIMIT_ERROR;
                case TEMP_SENSOR_2_ADDR:
                    return TEMP2_LIMIT_ERROR;
                case TEMP_SENSOR_3_ADDR:
                    return TEMP3_LIMIT_ERROR;
                case TEMP_SENSOR_4_ADDR:
                    return TEMP4_LIMIT_ERROR;
                default:
                    // This will only happen if there is a coding error when using this function
                    log_write(LOG_ERROR, "Invalid Temp Sensor ERROR");
                    exit(EXIT_FAILURE);
            }
        default:
            // This will only happen if there is a coding error when using this function
            log_write(LOG_ERROR, "Invalid Temp Sensor ERROR");
            exit(EXIT_FAILURE);
    }
}

/**
 * @brief Configures the selected Temperature Sensor uses pre-defined settings.
 * 
 * @param tempAddr I2C Address of Temperature senors user wants to configure
 * @return Iris error code indicating the success or failure of function
 */
enum IRIS_ERROR temp_setup(uint8_t tempAddr){

    int bus; 
    int tempErrorCheck;
    char logBuffer[255];
    enum IRIS_ERROR error = NO_ERROR;

    uint8_t regAddr[2] = TEMP_REG_ADDR;
    uint8_t regConfig[2] = TEMP_REG_DEFAULT; 
    uint8_t regData[2] = {0};

    snprintf(logBuffer, sizeof(logBuffer), "TEMP-SETUP: Begin setup of Temperature Sensor 0x%02x",tempAddr);
    log_write(LOG_INFO, logBuffer);

    //Setup I2C Interface with Temp Sensor
    bus = i2c_setup(I2C_BUS_INDEX, tempAddr);
    if (bus == I2C_SETUP_ERROR){
        snprintf(logBuffer, sizeof(logBuffer), "Temp Sensor 0x%02x - I2C Bus Failed to Open", tempAddr);
        log_write(LOG_ERROR, logBuffer);
        return temp_error_code(tempAddr, TEMP1_SETUP_ERROR);
    }

    //Write the configuration registers of the Temp Sensor
    for(int index = 0; index < sizeof(regAddr); index++){
        regData[0] = regAddr[index];
        regData[1] = regConfig[index];
        tempErrorCheck = i2c_write_reg8(bus, 2, regData);
        if (tempErrorCheck == I2C_WRITE_ERROR){
            snprintf(logBuffer, sizeof(logBuffer), "Temp Sensor 0x%02x - I2C Reg 0x%02x Write Failed", tempAddr, regAddr[index]);
            log_write(LOG_ERROR, logBuffer);
            error = temp_error_code(tempAddr, TEMP1_SETUP_ERROR);
        }
    }

    snprintf(logBuffer, sizeof(logBuffer), "TEMP-SETUP: Completed attempt for setup of Temperature Sensor 0x%02x",tempAddr);
    log_write(LOG_INFO, logBuffer);

    i2c_close(bus);
    return error;
}

/**
 * @brief Verifies the selected Temperature Sensor configuration by checking register values.
 * 
 * @param tempAddr I2C Address of Temperature senors user wants to verify
 * @return Iris error code indicating the success or failure of function
 */
//! ADD CODE FOR RESETING DEVICE AND CHECKING FOR FAULTS IE OPEN CIRCUIT
//! ADD CODE FOR DEALING WITH DIFFERNET ERROR (WRONG CONFIG REG, INVALID TEMP MEAS)
enum IRIS_ERROR temp_func_validate(uint8_t tempAddr){
    
    int bus;
    int tempErrorCheck;
    enum IRIS_ERROR error = NO_ERROR;
    char logBuffer[255];

    uint8_t regAddr[2] = TEMP_REG_ADDR;
    uint8_t regConfig[2] = TEMP_REG_DEFAULT; 
    uint8_t regData[2] = {0};

    snprintf(logBuffer, sizeof(logBuffer), "TEMP-FUNC-VALIDATE: Begin functional verification of Temperature Sensor 0x%02x",tempAddr);
    log_write(LOG_INFO, logBuffer);

    //Setup I2C Interface with Temp Sensor
    bus = i2c_setup(I2C_BUS_INDEX, tempAddr);
    if (bus == I2C_SETUP_ERROR){
        snprintf(logBuffer, sizeof(logBuffer), "Temp Sensor 0x%02x - I2C Bus Failed to Open", tempAddr);
        log_write(LOG_ERROR, logBuffer);
        return temp_error_code(tempAddr, TEMP1_VERIFICATION_ERROR);
    }

    //Reads and verifies the configuration registers of the Temp Sensor
    for(int index = 0; index < sizeof(regAddr); index++){
        tempErrorCheck = i2c_reg8_write_read(bus, (&regAddr + index), 1, regData);
        if (tempErrorCheck == I2C_WR_R_ERROR){
            snprintf(logBuffer, sizeof(logBuffer), "Temp Sensor 0x%02x - I2C Reg 0x%02x Read Failed", tempAddr, regAddr[index]);
            log_write(LOG_ERROR, logBuffer);
            error = temp_error_code(tempAddr, TEMP1_VERIFICATION_ERROR);
        }
        if (regData[0] != regConfig[index]){
            snprintf(logBuffer, sizeof(logBuffer), "Temp Sensor 0x%02x - I2C Reg 0x%02x Value Incorrect 0x%02x", tempAddr, regAddr[index], regData[0]);
            log_write(LOG_ERROR, logBuffer);
            error = temp_error_code(tempAddr, TEMP1_VERIFICATION_ERROR);
        }
    }

    //Check Temp Diode Fault / Low Supply Voltage
    regAddr[0] = TMP_REG_RMT_1_LOW; 
    tempErrorCheck = i2c_reg8_write_read(bus, regAddr, 1, regData);
    if (tempErrorCheck == I2C_WR_R_ERROR){
        snprintf(logBuffer, sizeof(logBuffer), "Temp Sensor 0x%02x - I2C Reg 0x%02x Read Failed", tempAddr, regAddr[0]);
        log_write(LOG_ERROR, logBuffer);
        error = temp_error_code(tempAddr, TEMP1_VERIFICATION_ERROR);
    }

    // Check Bit[0:1] of Register
    if ((regData[0] & (0x01)) == 0x01){
        snprintf(logBuffer, sizeof(logBuffer), "Temp Sensor 0x%02x - Open Circuit Flag Asserted in Reg 0x%02x", tempAddr, regAddr[0]);
        log_write(LOG_ERROR, logBuffer);
        error = temp_error_code(tempAddr, TEMP1_VERIFICATION_ERROR);
    }
    if ((regData[0] & (0x02)) == 0x02){
        snprintf(logBuffer, sizeof(logBuffer), "Temp Sensor 0x%02x - Low Supply Voltage Flag Asserted in Reg 0x%02x", tempAddr, regAddr[0]);
        log_write(LOG_ERROR, logBuffer);
        error = temp_error_code(tempAddr, TEMP1_VERIFICATION_ERROR);
    }

    snprintf(logBuffer, sizeof(logBuffer), "TEMP-FUNC-VALIDATE: Completed attempt for functional verification of Temperature Sensor 0x%02x",tempAddr);
    log_write(LOG_INFO, logBuffer);

    i2c_close(bus);
    return error;
}


/**
 * @brief Triggers a reset of selected temperature sensors. 
 *        Note: This will NOT reconfigure the sensor.
 * 
 * @param tempAddr I2C Address of Temperature senors user wants to reset
 * @return Iris error code indicating the success or failure of function
 */
enum IRIS_ERROR temp_reset_trig(uint8_t tempAddr){

    int bus;
    int errorCheck;
    char logBuffer[255];
    uint8_t regData[2] = {TMP_REG_SW_RST, 0x01};

    snprintf(logBuffer, sizeof(logBuffer), "TEMP-RESET: Begin reset of Temperature Sensor 0x%02x",tempAddr);
    log_write(LOG_INFO, logBuffer);

    //Setup I2C Interface with Temp Sensor
    bus = i2c_setup(I2C_BUS_INDEX, tempAddr);
    if (bus == I2C_SETUP_ERROR){
        snprintf(logBuffer, sizeof(logBuffer), "Temp Sensor 0x%02x - I2C Bus Failed to Open", tempAddr);
        log_write(LOG_ERROR, logBuffer);
        return temp_error_code(tempAddr, TEMP1_RESET_ERROR);
    }

    //Sets Register reset bit in temperature sensor
    errorCheck = i2c_write_reg8(bus, 2, regData);
    if (errorCheck == I2C_WRITE_ERROR){
        snprintf(logBuffer, sizeof(logBuffer), "Temp Sensor 0x%02x - I2C Reg 0x%02x Write Failed", tempAddr, regData[0]);
        log_write(LOG_ERROR, logBuffer);
        i2c_close(bus);
        return temp_error_code(tempAddr, TEMP1_RESET_ERROR);
    }

    snprintf(logBuffer, sizeof(logBuffer), "TEMP-RESET: Successful reset trigger of Temperature Sensor 0x%02x",tempAddr);
    log_write(LOG_INFO, logBuffer);

    i2c_close(bus);
    return NO_ERROR;
}


/**
 * @brief Checks if temperature limit is reached on any of the sensors 
 * 
 * @param errorBuffer Pointer to Buffer containing any errors that occur during operation
 */
void temperature_limit(enum IRIS_ERROR *errorBuffer){

    int8_t temp1 = 0;
    int8_t temp2 = 0;
    int8_t temp3 = 0;
    int8_t temp4 = 0;

    temp1 = read_temperature(TEMP_SENSOR_1_ADDR);
    temp2 = read_temperature(TEMP_SENSOR_2_ADDR);
    temp3 = read_temperature(TEMP_SENSOR_3_ADDR);
    temp4 = read_temperature(TEMP_SENSOR_4_ADDR);

    //DETERMINE IF TEMPERATURE LIMIT REACHED
    if(temp1 != TEMP1_TEMP_READ_ERROR){
        if((temp1 > TEMP1_MAX) || (temp1 < TEMP1_MIN)){
            errorBuffer[(1 + errorBuffer[0]++)] = TEMP1_LIMIT_ERROR;
            log_write(LOG_WARNING, "Temp Sensor 1 - Temperature Limit Reached");
        }
    }else{
        errorBuffer[(1 + errorBuffer[0]++)] = TEMP1_TEMP_READ_ERROR;
        log_write(LOG_WARNING, "Temp Sensor 1 - Failed to Read Temperature");
    }
    
    if(temp2 != TEMP2_TEMP_READ_ERROR){
        if((temp2 > TEMP2_MAX) || (temp2 < TEMP2_MIN)){
            errorBuffer[(1 + errorBuffer[0]++)] = TEMP2_LIMIT_ERROR;
            log_write(LOG_WARNING, "Temp Sensor 2 - Temperature Limit Reached");
        }
    }else{
        errorBuffer[(1 + errorBuffer[0]++)] = TEMP2_TEMP_READ_ERROR;
        log_write(LOG_WARNING, "Temp Sensor 2 - Failed to Read Temperature");
    }
    
    if(temp3 != TEMP3_TEMP_READ_ERROR){
        if((temp3 > TEMP3_MAX) || (temp3 < TEMP3_MIN)){
            errorBuffer[(1 + errorBuffer[0]++)] = TEMP3_LIMIT_ERROR;
            log_write(LOG_WARNING, "Temp Sensor 3 - Temperature Limit Reached");
        }
    }else{
        errorBuffer[(1 + errorBuffer[0]++)] = TEMP3_TEMP_READ_ERROR;
        log_write(LOG_WARNING, "Temp Sensor 3 - Failed to Read Temperature");
    }
    
    if(temp4 != TEMP4_TEMP_READ_ERROR){
        if((temp4 > TEMP4_MAX) || (temp4 < TEMP4_MIN)){
            errorBuffer[(1 + errorBuffer[0]++)] = TEMP4_LIMIT_ERROR;
            log_write(LOG_WARNING, "Temp Sensor 4 - Temperature Limit Reached");
        }
    }else{
        errorBuffer[(1 + errorBuffer[0]++)] = TEMP4_TEMP_READ_ERROR;
        log_write(LOG_WARNING, "Temp Sensor 4 - Failed to Read Temperature");
    }

}


/**
 * @brief Converts temperature sensor value to a signed integer. 
 * 
 * @param HighByte Content of register from temperature sensor
 * @return Signed temperature value from sensor
 */
int convert_temp_read(uint8_t HighByte){
    return HighByte * CONVERSION_FACTOR;
}


/**
 * @brief Reads temperature of inputed sensor.
 * 
 * @param tempAddr I2C Address of Temperature senors user wants to read
 * @return Signed temperature value from sensor
 */
//! NEED TO MAKE ERROR CODES OUT OF 'READ_TEMPERATURE' RANGE (-40 -> 120)
//! MAYBE CHECK IF THE TEMPERATURE VALUIE WILL BE CORRECT 
int read_temperature(uint8_t tempAddr){

    int bus;
    int errorCheck;
    char logBuffer[255];

    uint8_t reg = TMP_REG_RMT_1_HIGH;
    uint8_t regData[1];

    //Setup I2C Interface with Temp Sensor
    bus = i2c_setup(I2C_BUS_INDEX, tempAddr);
    if (bus == I2C_SETUP_ERROR){
        log_write(LOG_ERROR, "Temp Sensor - I2C Bus Failed to Open");
        return temp_error_code(tempAddr, TEMP1_TEMP_READ_ERROR);
    }

    //Reads temperature register in sensor
    errorCheck = i2c_reg8_write_read(bus, &reg, 2, regData);
    if (errorCheck == I2C_WR_R_ERROR){
        snprintf(logBuffer, sizeof(logBuffer), "Temp Sensor 0x%02x - I2C Temperature Reg 0x%02x Read Failed", tempAddr, regData[0]);
        log_write(LOG_ERROR, logBuffer);
        i2c_close(bus);
        return temp_error_code(tempAddr, TEMP1_TEMP_READ_ERROR);
    }
    
    i2c_close(bus);
    return convert_temp_read(*regData);
}

