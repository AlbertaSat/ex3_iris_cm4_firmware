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

        case CURR1_VAL_READ_ERROR:
            switch (currAddr){
                case CURRENT_SENSOR_ADDR_3V3:
                    return CURR1_VAL_READ_ERROR;
                case CURRENT_SENSOR_ADDR_5V:
                    return CURR2_VAL_READ_ERROR;
                case CURRENT_SENSOR_ADDR_CAM:
                    return CURR3_VAL_READ_ERROR;
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

    int bus; 
    int tempErrorCheck;
    enum IRIS_ERROR error = NO_ERROR;
    char logBuffer[255];

    uint16_t regConfig[11] = {0};
    uint8_t regAddr[11] = { CURR_REG_CFG,
                            CURR_REG_FLAG_CFG,
                            CURR_REG_SHT_VOLT_WRN_P,
                            CURR_REG_SHT_VOLT_WRN_N,
                            CURR_REG_POWER_WRN,
                            CURR_REG_BUS_OVVOLT_WRN,
                            CURR_REG_BUS_UNVOLT_WRN,
                            CURR_REG_PWR_OVERLIMIT,
                            CURR_REG_BUS_OVERLIMIT,
                            CURR_REG_BUS_UNDERLIMIT,
                            CURR_REG_CALIBRATION
                          };

    snprintf(logBuffer, sizeof(logBuffer), "CURRENT-MONITOR-SETUP: Begin functional verification of Current Sensor 0x%02x", currAddr);
    log_write(LOG_INFO, logBuffer);

    //Sets up what configuration settings to use on Current Sensor
    if (currAddr == CURRENT_SENSOR_ADDR_3V3){
        uint16_t regConfigTemp[11] = CURR_SENSOR_REG_DEFAULT_3V3;
        for(int x = 0; x < sizeof(regConfigTemp); x++){
            regConfig[x] = regConfigTemp[x];
        }

    }else if(currAddr == CURRENT_SENSOR_ADDR_5V){
        uint16_t regConfigTemp[11] = CURR_SENSOR_REG_DEFAULT_5V;
        for(int x = 0; x < sizeof(regConfigTemp); x++){
            regConfig[x] = regConfigTemp[x];
        }

    }else if(currAddr == CURRENT_SENSOR_ADDR_CAM){
        uint16_t regConfigTemp[11] = CURR_SENSOR_REG_DEFAULT_CAM;
        for(int x = 0; x < sizeof(regConfigTemp); x++){
            regConfig[x] = regConfigTemp[x];
        }
    }else{
        log_write(LOG_ERROR, "Invalid Current Monitior I2C Address");
        exit(EXIT_FAILURE);
    }

    //Setup I2C Interface with Temp Sensor
    bus = i2c_setup(I2C_BUS_INDEX, currAddr);
    if (bus == I2C_SETUP_ERROR){
        snprintf(logBuffer, sizeof(logBuffer), "Current Monitor 0x%02x - I2C Bus Failed to Open", currAddr);
        log_write(LOG_ERROR, logBuffer);
        return current_error_code(currAddr, CURR1_SETUP_ERROR);
    }

    //Write the configuration registers of the Current Sensor
    for(int index = 0; index < sizeof(regAddr); index++){
        tempErrorCheck = i2c_write_reg16(bus, 2, (regAddr + index), (regConfig + index));
        if (tempErrorCheck == I2C_WR_R_ERROR){
            snprintf(logBuffer, sizeof(logBuffer), "Current Monitor 0x%02x - I2C Reg 0x%02x Write Failed", currAddr, regAddr[index]);
            log_write(LOG_ERROR, logBuffer);
            error = current_error_code(currAddr, CURR1_SETUP_ERROR);
        }
    }

    snprintf(logBuffer, sizeof(logBuffer), "CURRENT-MONITOR-SETUP: Completed attempt for setup of Current Monitor Sensor 0x%02x", currAddr);
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
int current_func_validate(uint8_t currAddr){
    
    int bus; 
    int tempErrorCheck;
    enum IRIS_ERROR error = NO_ERROR;
    char logBuffer[255];

    uint16_t regData[1] = {0};
    uint16_t regConfig[11] = {0};
    uint8_t regAddr[11] = { CURR_REG_CFG,
                            CURR_REG_FLAG_CFG,
                            CURR_REG_SHT_VOLT_WRN_P,
                            CURR_REG_SHT_VOLT_WRN_N,
                            CURR_REG_POWER_WRN,
                            CURR_REG_BUS_OVVOLT_WRN,
                            CURR_REG_BUS_UNVOLT_WRN,
                            CURR_REG_PWR_OVERLIMIT,
                            CURR_REG_BUS_OVERLIMIT,
                            CURR_REG_BUS_UNDERLIMIT,
                            CURR_REG_CALIBRATION
                          };
    
    snprintf(logBuffer, sizeof(logBuffer), "CURRENT-FUNC-VALIDATE: Begin functional verification of Current Sensor 0x%02x", currAddr);
    log_write(LOG_INFO, logBuffer);

    //Sets up what configuration settings to use on Current Sensor
    if (currAddr == CURRENT_SENSOR_ADDR_3V3){
        uint16_t regConfigTemp[11] = CURR_SENSOR_REG_DEFAULT_3V3;
        for(int x = 0; x < sizeof(regConfigTemp); x++){
            regConfig[x] = regConfigTemp[x];
        }

    }else if(currAddr == CURRENT_SENSOR_ADDR_5V){
        uint16_t regConfigTemp[11] = CURR_SENSOR_REG_DEFAULT_5V;
        for(int x = 0; x < sizeof(regConfigTemp); x++){
            regConfig[x] = regConfigTemp[x];
        }

    }else if(currAddr == CURRENT_SENSOR_ADDR_CAM){
        uint16_t regConfigTemp[11] = CURR_SENSOR_REG_DEFAULT_CAM;
        for(int x = 0; x < sizeof(regConfigTemp); x++){
            regConfig[x] = regConfigTemp[x];
        }

    }else{
        log_write(LOG_ERROR, "XXX");
        return 255;
    }

    bus = i2c_setup(I2C_BUS_INDEX, currAddr);
    if (bus == I2C_SETUP_ERROR){
        snprintf(logBuffer, sizeof(logBuffer), "Current Monitor 0x%02x - I2C Bus Failed to Open", currAddr);
        log_write(LOG_ERROR, logBuffer);
        return 255;
    }

    for(int index = 0; index < sizeof(regAddr); index++){
        tempErrorCheck = i2c_reg16_write_read(bus, (regAddr + index), 1, regData);
        if (tempErrorCheck == I2C_WR_R_ERROR){
            snprintf(logBuffer, sizeof(logBuffer), "Current Monitor 0x%02x - I2C Reg 0x%02x Read Failed", currAddr, regAddr[index]);
            log_write(LOG_ERROR, logBuffer);
            error = current_error_code(currAddr, CURR1_VERIFICATION_ERROR);   
        }
        if (regData[0] != regConfig[index]){
            snprintf(logBuffer, sizeof(logBuffer), "Current Monitor 0x%02x - I2C Reg 0x%02x Value Incorrect 0x%02x", currAddr, regAddr[index], regData[0]);
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

    snprintf(logBuffer, sizeof(logBuffer), "CURRENT-MONITOR-RESET: Begin reset of Current Monitor 0x%02x", currAddr);
    log_write(LOG_INFO, logBuffer);

    //Setup I2C Interface with Temp Sensor
    bus = i2c_setup(I2C_BUS_INDEX, currAddr);
    if (bus == I2C_SETUP_ERROR){
        snprintf(logBuffer, sizeof(logBuffer), "Current Monitor 0x%02x - I2C Bus Failed to Open", currAddr);
        log_write(LOG_ERROR, logBuffer);
        return current_error_code(currAddr, CURR1_RESET_ERROR);
    }

    //Sets Register reset bit in Current sensor
    errorCheck = i2c_write_reg16(bus, 1, &reg, (uint16_t)0xB99F);
    if (errorCheck == I2C_WRITE_ERROR){
        snprintf(logBuffer, sizeof(logBuffer), "Current Monitor 0x%02x - I2C Reg 0x%02x Write Failed", currAddr, 0xB99F);
        log_write(LOG_ERROR, logBuffer);
        i2c_close(bus);
        return current_error_code(currAddr, CURR1_RESET_ERROR);
    }

    snprintf(logBuffer, sizeof(logBuffer), "CURRENT-MONITOR-RESET: Successful reset trigger of Current Monitor 0x%02x", currAddr);
    log_write(LOG_INFO, logBuffer);

    i2c_close(bus);
    return NO_ERROR;
    
}

//! RETURNS ERROR CODE WHERE
//! errorCode[3:0] = [temp4_MAX, temp3_MAX, temp2_MAX, temp1_MAX]
//! errorCode[7:4] = [temp4_RD_ERR, temp3_RD_ERR, temp2_RD_ERR, temp1_RD_ERR]
int current_monitor_status(uint8_t currAddr){
    
    int errorCheck = 0;
    int monitorStatus = 0;
    int bus;
    uint8_t reg = CURR_REG_STATUS;

    bus = i2c_setup(I2C_BUS_INDEX, currAddr);
    if (bus == -1){
        log_write(LOG_ERROR, "Current Sensor - I2C Bus Failed to Open");
        return 0xFFFF;
    }
    errorCheck = i2c_reg16_write_read(bus, &reg, 2, monitorStatus);
    if (errorCheck == -1){
        log_write(LOG_ERROR, "Current Sensor - I2C reg read/write Failed");
        i2c_close(bus);
        return 0xFFFF;
    }
    i2c_close(bus);
    return (monitorStatus >> 3);

}

int8_t read_current(uint8_t currAddr){
    return 0;
}

int read_power(){
    return 0;
}

int read_bus_voltage(){
    return 0;
}

