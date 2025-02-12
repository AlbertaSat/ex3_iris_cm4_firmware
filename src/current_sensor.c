#include "current_sensor.h"
#include "i2c.h"
#include "main.h"
#include "gpio.h"
#include "logger.h"
#include "error_handler.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @file current_sensor.c
 * @author Noah Klager
 * @brief Current Sensor Driver for Theia CM4 
 *        Provides functions to...
 *         - Configure Current Sensors
 *         - Verifies functionality of Current Sensors
 *         - Resets Current Sensors
 *         ! ADD THE ADDITIONAL FUNCTIONS OF THE CURRENT SENSOR DRIVER
 * 
 * @version 0.1
 * @date 2024-11-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */


/**
 * @brief Converts a general error code into a specific one used to indicate which
 *        current sensor had the error
 * 
 * @param currAddr I2C Address of Current sensor
 * @param errorType General Iris error detected
 * @return Specific Iris error code indicating which current sensor had the error
 */
enum IRIS_ERROR current_error_code(uint8_t currAddr, enum IRIS_ERROR errorType){

    switch (errorType){
        case CURR1_SETUP_ERROR:
            switch (currAddr){
                case CURRENT_SENSOR_ADDR_3V3:
                    return CURR1_SETUP_ERROR;
                case CURRENT_SENSOR_ADDR_5V:
                    return CURR2_SETUP_ERROR;
                case CURRENT_SENSOR_ADDR_CAM:
                    return CURR3_SETUP_ERROR;
                default:
                    // This will only happen if there is a coding error when using this function
                    log_write(LOG_ERROR, "Invalid Current Sensor ERROR");
                    exit(EXIT_FAILURE);
            }

        case CURR1_VERIFICATION_ERROR:
            switch (currAddr){
                case CURRENT_SENSOR_ADDR_3V3:
                    return CURR1_VERIFICATION_ERROR;
                case CURRENT_SENSOR_ADDR_5V:
                    return CURR2_VERIFICATION_ERROR;
                case CURRENT_SENSOR_ADDR_CAM:
                    return CURR3_VERIFICATION_ERROR;
                default:
                    // This will only happen if there is a coding error when using this function
                    log_write(LOG_ERROR, "Invalid Current Sensor ERROR");
                    exit(EXIT_FAILURE);
            }
        
        case CURR1_RESET_ERROR:
            switch (currAddr){
                case CURRENT_SENSOR_ADDR_3V3:
                    return CURR1_RESET_ERROR;
                case CURRENT_SENSOR_ADDR_5V:
                    return CURR2_RESET_ERROR;
                case CURRENT_SENSOR_ADDR_CAM:
                    return CURR3_RESET_ERROR;
                default:
                    // This will only happen if there is a coding error when using this function
                    log_write(LOG_ERROR, "Invalid Current Sensor ERROR");
                    exit(EXIT_FAILURE);
            }

        case CURR1_VAL_READ_ERROR_16BIT:
            switch (currAddr){
                case CURRENT_SENSOR_ADDR_3V3:
                    return CURR1_VAL_READ_ERROR_16BIT;
                case CURRENT_SENSOR_ADDR_5V:
                    return CURR2_VAL_READ_ERROR_16BIT;
                case CURRENT_SENSOR_ADDR_CAM:
                    return CURR3_VAL_READ_ERROR_16BIT;
                default:
                    // This will only happen if there is a coding error when using this function
                    log_write(LOG_ERROR, "Invalid Current Sensor ERROR");
                    exit(EXIT_FAILURE);
            }

        case CURR1_LIMIT_ERROR:
            switch (currAddr){
                case CURRENT_SENSOR_ADDR_3V3:
                    return CURR1_LIMIT_ERROR;
                case CURRENT_SENSOR_ADDR_5V:
                    return CURR2_LIMIT_ERROR;
                case CURRENT_SENSOR_ADDR_CAM:
                    return CURR3_LIMIT_ERROR;
                default:
                    // This will only happen if there is a coding error when using this function
                    log_write(LOG_ERROR, "Invalid Current Sensor ERROR");
                    exit(EXIT_FAILURE);
            }

        default:
            // This will only happen if there is a coding error when using this function
            log_write(LOG_ERROR, "Invalid Current Sensor ERROR");
            exit(EXIT_FAILURE);

    }
}

/**
 * @brief Configures the selected Current Sensor using pre-defined settings.
 * 
 * @param currAddr I2C Address of Current senors user wants to configure
 * @return Iris error code indicating the success or failure of function
 */
enum IRIS_ERROR current_setup(uint8_t currAddr){


    //! SHOULD MAKE IT SO IT VERIFIES REGISTER VALUES BEFORE ENABLED FLAGS
    //! SINCE IF THE LIMIT REGS ARE NOT CORRECT IT CAN LOCK THE ENTIRE DEVICE
    int bus; 
    int tempErrorCheck;
    enum IRIS_ERROR error = NO_ERROR;
    char logBuffer[255];

    uint16_t regConfig[13] = {0};
    uint8_t regAddr[13] = { CURR_REG_CFG,
                            CURR_REG_SHT_VOLT_WRN_P,
                            CURR_REG_SHT_VOLT_WRN_N,
                            CURR_REG_POWER_WRN,
                            CURR_REG_BUS_OVVOLT_WRN,
                            CURR_REG_BUS_UNVOLT_WRN,
                            CURR_REG_PWR_OVERLIMIT,
                            CURR_REG_BUS_OVERLIMIT,
                            CURR_REG_BUS_UNDERLIMIT,
                            CURR_REG_DAC_POS,
                            CURR_REG_DAC_NEG,
                            CURR_REG_CALIBRATION,
                            CURR_REG_FLAG_CFG
                          };

    snprintf(logBuffer, sizeof(logBuffer), "CURRENT-SENSOR-SETUP: Begin functional verification of Current Sensor 0x%02x", currAddr);
    log_write(LOG_INFO, logBuffer);

    //Sets up what configuration settings to use on Current Sensor
    if (currAddr == CURRENT_SENSOR_ADDR_3V3){
        uint16_t regConfigTemp[13] = CURR_SENSOR_REG_DEFAULT_3V3;
        for(int x = 0; x < sizeof(regConfigTemp); x++){
            regConfig[x] = regConfigTemp[x];
        }

    }else if(currAddr == CURRENT_SENSOR_ADDR_5V){
        uint16_t regConfigTemp[13] = CURR_SENSOR_REG_DEFAULT_5V;
        for(int x = 0; x < sizeof(regConfigTemp); x++){
            regConfig[x] = regConfigTemp[x];
        }

    }else if(currAddr == CURRENT_SENSOR_ADDR_CAM){
        uint16_t regConfigTemp[13] = CURR_SENSOR_REG_DEFAULT_CAM;
        for(int x = 0; x < sizeof(regConfigTemp); x++){
            regConfig[x] = regConfigTemp[x];
        }
    }else{
        // This will only happen if there is a coding error when using this function
        log_write(LOG_ERROR, "Invalid Current Sensor Address");
        exit(EXIT_FAILURE);
    }

    //Setup I2C Interface with Temp Sensor
    bus = i2c_setup(I2C_BUS_INDEX, currAddr);
    if (bus == I2C_SETUP_ERROR){
        snprintf(logBuffer, sizeof(logBuffer), "Current Sensor 0x%02x - I2C Bus Failed to Open", currAddr);
        log_write(LOG_ERROR, logBuffer);
        return current_error_code(currAddr, CURR1_SETUP_ERROR);
    }

    //Write the configuration registers of the Current Sensor
    for(int index = 0; index < sizeof(regAddr); index++){
        tempErrorCheck = i2c_write_reg16(bus, 2, (regAddr + index), (regConfig + index));
        if (tempErrorCheck == I2C_WR_R_ERROR){
            snprintf(logBuffer, sizeof(logBuffer), "Current Sensor 0x%02x - I2C Reg 0x%02x Write Failed", currAddr, regAddr[index]);
            log_write(LOG_ERROR, logBuffer);
            error = current_error_code(currAddr, CURR1_SETUP_ERROR);
        }
    }

    snprintf(logBuffer, sizeof(logBuffer), "CURRENT-SENSOR-SETUP: Completed attempt for setup of Current Monitor Sensor 0x%02x", currAddr);
    log_write(LOG_INFO, logBuffer);

    i2c_close(bus);
    return error;
}


//! STILL WORK IN PROGRESS
/**
 * @brief Verifies the selected Current Sensor configuration by checking register values.
 * 
 * @param currAddr I2C Address of Current senors user wants to verify
 * @return Iris error code indicating the success or failure of function
 */
//! ADD CODE FOR RESETING DEVICE AND CHECKING FOR FAULTS IE OPEN CIRCUIT
//! ADD CODE FOR DEALING WITH DIFFERNET ERROR (WRONG CONFIG REG, INVALID TEMP MEAS)
//! ADD CODE TO CHECK FOR PVLD ERROR
enum IRIS_ERROR current_func_validate(uint8_t currAddr){
    
    int bus; 
    int tempErrorCheck;
    enum IRIS_ERROR error = NO_ERROR;
    char logBuffer[255];

    uint16_t regData[1] = {0};
    uint16_t regConfig[13] = {0};
    uint8_t regAddr[13] = { CURR_REG_CFG,
                            CURR_REG_SHT_VOLT_WRN_P,
                            CURR_REG_SHT_VOLT_WRN_N,
                            CURR_REG_POWER_WRN,
                            CURR_REG_BUS_OVVOLT_WRN,
                            CURR_REG_BUS_UNVOLT_WRN,
                            CURR_REG_PWR_OVERLIMIT,
                            CURR_REG_BUS_OVERLIMIT,
                            CURR_REG_BUS_UNDERLIMIT,
                            CURR_REG_DAC_POS,
                            CURR_REG_DAC_NEG,
                            CURR_REG_CALIBRATION,
                            CURR_REG_FLAG_CFG
      };
    
    snprintf(logBuffer, sizeof(logBuffer), "CURRENT-FUNC-VALIDATE: Begin functional verification of Current Sensor 0x%02x", currAddr);
    log_write(LOG_INFO, logBuffer);

    //Sets up what configuration settings to use on Current Sensor
    if (currAddr == CURRENT_SENSOR_ADDR_3V3){
        uint16_t regConfigTemp[13] = CURR_SENSOR_REG_DEFAULT_3V3;
        for(int x = 0; x < sizeof(regConfigTemp); x++){
            regConfig[x] = regConfigTemp[x];
        }
    }else if(currAddr == CURRENT_SENSOR_ADDR_5V){
        uint16_t regConfigTemp[13] = CURR_SENSOR_REG_DEFAULT_5V;
        for(int x = 0; x < sizeof(regConfigTemp); x++){
            regConfig[x] = regConfigTemp[x];
        }
    }else if(currAddr == CURRENT_SENSOR_ADDR_CAM){
        uint16_t regConfigTemp[13] = CURR_SENSOR_REG_DEFAULT_CAM;
        for(int x = 0; x < sizeof(regConfigTemp); x++){
            regConfig[x] = regConfigTemp[x];
        }
    }else{
        // This will only happen if there is a coding error when using this function
        log_write(LOG_ERROR, "Invalid Current Sensor Address");
        exit(EXIT_FAILURE);
    }

    bus = i2c_setup(I2C_BUS_INDEX, currAddr);
    if (bus == I2C_SETUP_ERROR){
        snprintf(logBuffer, sizeof(logBuffer), "Current Sensor 0x%02x - I2C Bus Failed to Open", currAddr);
        log_write(LOG_ERROR, logBuffer);
        return current_error_code(currAddr, CURR1_VERIFICATION_ERROR);
    }

    for(int index = 0; index < sizeof(regAddr); index++){
        tempErrorCheck = i2c_reg16_write_read(bus, (regAddr + index), 1, regData);
        if (tempErrorCheck == I2C_WR_R_ERROR){
            snprintf(logBuffer, sizeof(logBuffer), "Current Sensor 0x%02x - I2C Reg 0x%02x Read Failed", currAddr, regAddr[index]);
            log_write(LOG_ERROR, logBuffer);
            error = current_error_code(currAddr, CURR1_VERIFICATION_ERROR);
        }
        if (regData[0] != regConfig[index]){
            snprintf(logBuffer, sizeof(logBuffer), "Current Sensor 0x%02x - I2C Reg 0x%02x Value Incorrect 0x%02x", currAddr, regAddr[index], regData[0]);
            log_write(LOG_ERROR, logBuffer);
            error = current_error_code(currAddr, CURR1_VERIFICATION_ERROR);
        }
    }

    snprintf(logBuffer, sizeof(logBuffer), "CURRENT-FUNC-VALIDATE: Completed attempt for functional verification of Current Sensor 0x%02x", currAddr);
    log_write(LOG_INFO, logBuffer);

    i2c_close(bus);
    return error;
}

/**
 * @brief Triggers a reset of selected Current sensors. 
 *        Note: This will NOT reconfigure the sensor.
 * 
 * @param currAddr I2C Address of Current senors user wants to reset
 * @return Iris error code indicating the success or failure of function
 */
enum IRIS_ERROR current_monitor_reset_trig(uint8_t currAddr){

    int bus;
    int errorCheck;
    char logBuffer[255];
    uint8_t reg = CURR_REG_CFG;

    snprintf(logBuffer, sizeof(logBuffer), "CURRENT-SENSOR-RESET-TRIG: Begin triggering reset of Current Sensor 0x%02x", currAddr);
    log_write(LOG_INFO, logBuffer);

    //Setup I2C Interface with Temp Sensor
    bus = i2c_setup(I2C_BUS_INDEX, currAddr);
    if (bus == I2C_SETUP_ERROR){
        snprintf(logBuffer, sizeof(logBuffer), "Current Sensor 0x%02x - I2C Bus Failed to Open", currAddr);
        log_write(LOG_ERROR, logBuffer);
        return current_error_code(currAddr, CURR1_RESET_ERROR);
    }

    //Sets Register reset bit in Current sensor
    errorCheck = i2c_write_reg16(bus, 1, &reg, (uint16_t)0xB99F);
    if (errorCheck == I2C_WR_R_ERROR){
        snprintf(logBuffer, sizeof(logBuffer), "Current Sensor 0x%02x - I2C Reg 0x%02x Write Failed", currAddr, 0xB99F);
        log_write(LOG_ERROR, logBuffer);
        i2c_close(bus);
        return current_error_code(currAddr, CURR1_RESET_ERROR);
    }

    snprintf(logBuffer, sizeof(logBuffer), "CURRENT-SENSOR-RESET-TRIG: Successfully triggered reset of Current Sensor 0x%02x", currAddr);
    log_write(LOG_INFO, logBuffer);

    i2c_close(bus);
    return NO_ERROR;

}


/**
 * @brief Completes a reset of selected Current sensors. 
 *        Note: This WILL reconfigure the sensor.
 * 
 * @param currAddr I2C Address of Current senors user wants to reset
 * @return Iris error code indicating the success or failure of function
 */
enum IRIS_ERROR current_monitor_reset(uint8_t currAddr){

    int errorCheck;
    int loopCounter = 0;
    char logBuffer[255];

    snprintf(logBuffer, sizeof(logBuffer), "CURRENT-SENSOR-RESET: Begin reset of Current Sensor 0x%02x", currAddr);
    log_write(LOG_INFO, logBuffer);

    errorCheck = NO_ERROR;

    // Attempt to trigger reset of current sensor
    do{
        errorCheck = current_monitor_reset_trig(currAddr);
        loopCounter++;
    }while((errorCheck != NO_ERROR) && (loopCounter < MAX_CURR_INIT_ATTEMPTS));

    // Return error code if unable to trigger 'reset' of current sensor
    if(errorCheck != NO_ERROR){
        return errorCheck;
    }

    // Attempt to setup current sensor
    do{
        errorCheck = current_setup(currAddr);
        if (errorCheck == NO_ERROR){
            errorCheck = current_func_validate(currAddr);
        }
        loopCounter++;
    }while((errorCheck != NO_ERROR) && (loopCounter < MAX_CURR_INIT_ATTEMPTS));


    snprintf(logBuffer, sizeof(logBuffer), "CURRENT-SENSOR-RESET: Finished reset attempt of Current Sensor 0x%02x", currAddr);
    log_write(LOG_INFO, logBuffer);

    return errorCheck;

}

//! RETURNS ERROR CODE WHERE
//! errorCode[3:0] = [temp4_MAX, temp3_MAX, temp2_MAX, temp1_MAX]
//! errorCode[7:4] = [temp4_RD_ERR, temp3_RD_ERR, temp2_RD_ERR, temp1_RD_ERR]
int current_monitor_status(uint8_t currAddr, uint8_t *errorCount){
    
    int errorCheck = 0;
    int monitorStatus = 0;
    int bus;
    char logBuffer[255];
    uint8_t reg = CURR_REG_STATUS;

    bus = i2c_setup(I2C_BUS_INDEX, currAddr);
    if (bus == I2C_SETUP_ERROR){
        snprintf(logBuffer, sizeof(logBuffer), "Current Sensor 0x%02x - I2C Bus Failed to Open", currAddr);
        log_write(LOG_ERROR, logBuffer);
        //error = current_error_code(currAddr, CURR1_VERIFICATION_ERROR);
    }
    errorCheck = i2c_reg16_write_read(bus, &reg, 2, monitorStatus);
    if (errorCheck == I2C_WR_R_ERROR){
        log_write(LOG_ERROR, "Current Sensor - I2C reg read/write Failed");
        i2c_close(bus);
        return 0xFFFF;
    }

    i2c_close(bus);
    return (monitorStatus >> 3);

}

/**
 * @brief Reads the Current stored in the Sensors Register
 * 
 * @param currAddr I2C Address of Current sensors
 * @return Measured Current in mili-Amps
 */
uint16_t read_current(uint8_t currAddr){

    int errorCheck = 0;
    int currReg = 0;
    int bus;
    float currBuf = 0;
    char logBuffer[255];
    uint8_t reg = CURR_REG_CURRENT;

    bus = i2c_setup(I2C_BUS_INDEX, currAddr);
    if (bus == I2C_SETUP_ERROR){
        snprintf(logBuffer, sizeof(logBuffer), "Current Sensor 0x%02x - I2C Bus Failed to Open", currAddr);
        log_write(LOG_ERROR, logBuffer);
        return current_error_code(currAddr, CURR1_VAL_READ_ERROR_16BIT);
    }

    errorCheck = i2c_reg16_write_read(bus, &reg, 1, &currReg);
    if (errorCheck == I2C_WR_R_ERROR){
        snprintf(logBuffer, sizeof(logBuffer), "Current Sensor 0x%02x - I2C Reg 0x%02x Read Failed", currAddr, reg);
        log_write(LOG_ERROR, logBuffer);
        return current_error_code(currAddr, CURR1_VAL_READ_ERROR_16BIT);
    }

    i2c_close(bus);

    switch (currAddr){
        case CURRENT_SENSOR_ADDR_3V3:
            currBuf = currReg * CURR_LSB_VAL_3V3;
            break;
        case CURRENT_SENSOR_ADDR_5V:
            currBuf = currReg * CURR_LSB_VAL_5V;
            break;
        case CURRENT_SENSOR_ADDR_CAM:
            currBuf = currReg * CURR_LSB_VAL_CAM;
            break;
        default:
            // This will only happen if there is a coding error when using this function
            log_write(LOG_ERROR, "Invalid Current Sensor Address");
            exit(EXIT_FAILURE);
    }

    // Convert to mA
    return currBuf / 1000;

}


/**
 * @brief Reads the Power stored in the Sensors Register
 * 
 * @param currAddr I2C Address of Current sensors
 * @return Measured Power in mili-Watts
 */
uint16_t read_power(uint8_t currAddr){

    int errorCheck = 0;
    int pwrReg = 0;
    int bus;
    float powerBuf = 0;
    char logBuffer[255];
    uint8_t reg = CURR_REG_POWER;

    bus = i2c_setup(I2C_BUS_INDEX, currAddr);
    if (bus == I2C_SETUP_ERROR){
        snprintf(logBuffer, sizeof(logBuffer), "Current Sensor 0x%02x - I2C Bus Failed to Open", currAddr);
        log_write(LOG_ERROR, logBuffer);
        return current_error_code(currAddr, CURR1_VAL_READ_ERROR_16BIT);
    }

    errorCheck = i2c_reg16_write_read(bus, &reg, 1, &pwrReg);
    if (errorCheck == I2C_WR_R_ERROR){
        snprintf(logBuffer, sizeof(logBuffer), "Current Sensor 0x%02x - I2C Reg 0x%02x Read Failed", currAddr, reg);
        log_write(LOG_ERROR, logBuffer);
        return current_error_code(currAddr, CURR1_VAL_READ_ERROR_16BIT);
    }

    i2c_close(bus);

    switch (currAddr){
        case CURRENT_SENSOR_ADDR_3V3:
            powerBuf = pwrReg * PWR_LSB_VAL_3V3;
            break;
        case CURRENT_SENSOR_ADDR_5V:
            powerBuf = pwrReg * PWR_LSB_VAL_5V;
            break;
        case CURRENT_SENSOR_ADDR_CAM:
            powerBuf = pwrReg * PWR_LSB_VAL_CAM;
            break;
        default:
            // This will only happen if there is a coding error when using this function
            log_write(LOG_ERROR, "Invalid Current Sensor Address");
            exit(EXIT_FAILURE);
    }

    //Convert to mW
    return powerBuf / 1000;

}


/**
 * @brief Reads the Max Power stored in the Sensors Register
 * 
 * @param currAddr I2C Address of Current sensors
 * @return Measured Maximum Power in mili-Watts
 */
uint16_t read_pk_power(uint8_t currAddr){

    int errorCheck = 0;
    int pwrReg = 0;
    int bus;
    float powerBuf = 0;
    char logBuffer[255];
    uint8_t reg = CURR_REG_POWER_PK;

    bus = i2c_setup(I2C_BUS_INDEX, currAddr);
    if (bus == I2C_SETUP_ERROR){
        snprintf(logBuffer, sizeof(logBuffer), "Current Sensor 0x%02x - I2C Bus Failed to Open", currAddr);
        log_write(LOG_ERROR, logBuffer);
        return current_error_code(currAddr, CURR1_VAL_READ_ERROR_16BIT);
    }

    errorCheck = i2c_reg16_write_read(bus, &reg, 1, &pwrReg);
    if (errorCheck == I2C_WR_R_ERROR){
        snprintf(logBuffer, sizeof(logBuffer), "Current Sensor 0x%02x - I2C Reg 0x%02x Read Failed", currAddr, reg);
        log_write(LOG_ERROR, logBuffer);
        return current_error_code(currAddr, CURR1_VAL_READ_ERROR_16BIT);
    }

    i2c_close(bus);

    switch (currAddr){
        case CURRENT_SENSOR_ADDR_3V3:
            powerBuf = pwrReg * PWR_LSB_VAL_3V3;
            break;
        case CURRENT_SENSOR_ADDR_5V:
            powerBuf = pwrReg * PWR_LSB_VAL_5V;
            break;
        case CURRENT_SENSOR_ADDR_CAM:
            powerBuf = pwrReg * PWR_LSB_VAL_CAM;
            break;
        default:
            // This will only happen if there is a coding error when using this function
            log_write(LOG_ERROR, "Invalid Current Sensor Address");
            exit(EXIT_FAILURE);
    }

    // Convert to mW
    return powerBuf / 1000; 

}



/**
 * @brief Reads the Bus Voltage stored in the Sensors Register
 * 
 * @param currAddr I2C Address of Current sensors
 * @return Measured Bus Voltage in mili-Volts
 */
uint16_t read_bus_voltage(uint8_t currAddr){

    int errorCheck = 0;
    int voltReg = 0;
    int bus;
    float voltBuf = 0;
    char logBuffer[255];
    uint8_t reg = CURR_REG_BUS_VOLT;

    bus = i2c_setup(I2C_BUS_INDEX, currAddr);
    if (bus == I2C_SETUP_ERROR){
        snprintf(logBuffer, sizeof(logBuffer), "Current Sensor 0x%02x - I2C Bus Failed to Open", currAddr);
        log_write(LOG_ERROR, logBuffer);
        return current_error_code(currAddr, CURR1_VAL_READ_ERROR_16BIT);
    }

    errorCheck = i2c_reg16_write_read(bus, &reg, 1, &voltReg);
    if (errorCheck == I2C_WR_R_ERROR){
        snprintf(logBuffer, sizeof(logBuffer), "Current Sensor 0x%02x - I2C Reg 0x%02x Read Failed", currAddr, reg);
        log_write(LOG_ERROR, logBuffer);
        return current_error_code(currAddr, CURR1_VAL_READ_ERROR_16BIT);
    }

    i2c_close(bus);

    voltBuf = (voltReg >> 3) * 0.004; // LSB = 10uV

    // Convert to mV
    return voltBuf / 1000;

}

/**
 * @brief Checks if current limit is reached on any of the sensors. This limit is a warning, since the 
 *        actual limit is programmed into the Current Sensors where it will disable the PSU if reached.
 * 
 * @param errorBuffer Pointer to Buffer containing any errors that occur during operation
 */
void current_limit(enum IRIS_ERROR *errorBuffer, uint8_t *errorCount){

    float curr3v3 = 0;
    float curr5v = 0;
    float currcam = 0;

    char logBuffer[255];

    curr3v3 = read_current(CURRENT_SENSOR_ADDR_3V3);
    curr5v = read_current(CURRENT_SENSOR_ADDR_5V);
    currcam = read_current(CURRENT_SENSOR_ADDR_CAM);

    //DETERMINE IF CURRENT LIMIT REACHED
    if(curr3v3 != CURR1_VAL_READ_ERROR_16BIT){
        if(curr3v3 > CURR_3V3_MAX){
            errorBuffer[(*errorCount)++] = CURR1_LIMIT_ERROR;
            snprintf(logBuffer, sizeof(logBuffer), "CURR-LIMIT: 3V3 Current Limit Reached - Measured %fA", curr3v3);
            log_write(LOG_WARNING, logBuffer);
        }
    }else{
        errorBuffer[(*errorCount)++] = CURR1_VAL_READ_ERROR_16BIT;
        log_write(LOG_WARNING, "3V3 Current Sensor - Failed to Read Current");
    }
    
    if(curr5v != CURR2_VAL_READ_ERROR_16BIT){
        if(curr5v > CURR_5V_MAX){
            errorBuffer[(*errorCount)++] = CURR2_LIMIT_ERROR;
            snprintf(logBuffer, sizeof(logBuffer), "CURR-LIMIT: CM4 Current Limit Reached - Measured %fA", curr5v);
            log_write(LOG_WARNING, logBuffer);
        }
    }else{
        errorBuffer[(*errorCount)++] = CURR2_VAL_READ_ERROR_16BIT;
        log_write(LOG_WARNING, "CM4 Current Sensor - Failed to Read Current");
    }
    
    if(currcam != CURR3_VAL_READ_ERROR_16BIT){
        if(currcam > CURR_CAM_MAX){
            errorBuffer[(*errorCount)++] = CURR3_LIMIT_ERROR;
            snprintf(logBuffer, sizeof(logBuffer), "CURR-LIMIT: Camera Current Limit Reached - Measured %fA", currcam);
            log_write(LOG_WARNING, logBuffer);
        }
    }else{
        errorBuffer[(*errorCount)++] = CURR3_VAL_READ_ERROR_16BIT;
        log_write(LOG_WARNING, "Camera Current Sensor - Failed to Read Current");
    }

}