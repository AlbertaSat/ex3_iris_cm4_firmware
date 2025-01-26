
#include "i2c.h"
#include "logger.h"
#include "error_handler.h"

#include <linux/i2c-dev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <errno.h>
#include <fcntl.h>

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

/**
 * @file i2c.c
 * @author Noah Klager
 * @brief I2C Driver for Theia CM4 
 *        Provides functions to...
 *         - Configure a I2C Interface on the CM4
 *         - Write 8-bit and 16-bit data packets to I2C Peripherals
 *         - Read 8-bit and 16-bit data packets from I2C Peripherals
 * 
 * @version 0.1
 * @date 2024-11-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/**
 * @brief Configures the selected I2C interface to communicate with inputed I2C address
 * 
 * @param i2cBus Pointer to character array selecting I2C bus
 * @param devID I2C address of peripheral on bus that user wants to access
 * @return Either ID of I2C bus instance or Error Code if unsuccessful
 */
int i2c_setup_interface(char *i2cBus, uint8_t devID) {

    int fd;
    char logBuffer[255];

    //Open I2C bus linux interface
    fd = open(i2cBus, O_RDWR);
    if (fd < 0){
        snprintf(logBuffer, sizeof(logBuffer), "Unable to open I2C Bus: %s", strerror(errno));
        log_write(LOG_ERROR, logBuffer);
        return I2C_SETUP_ERROR;
    }

    //Configure Bus Slave Addr
    if (ioctl(fd, I2C_SLAVE, devID) < 0){
        snprintf(logBuffer, sizeof(logBuffer), "Unable to configure peripheral I2C Address: %s", strerror(errno));
        log_write(LOG_ERROR, logBuffer);
        return I2C_SETUP_ERROR;
    }
    return fd;
}

/**
 * @brief High level function used to configure an I2C interface to communicate with inputed device
 * 
 * @param i2cBus I2C Interface Number being used
 * @param devID I2C address of peripheral user wants to access
 * @return Either ID of I2C bus instance or Error Code if unsuccessful
 */
int i2c_setup(int i2cBus, uint8_t devID){

    char *device;
    switch (i2cBus){
        case 0:
            device = "/dev/i2c-0" ;
            break;
        case 1:
            device = "/dev/i2c-1" ;
            break;
        case 2:
            device = "/dev/i2c-2" ;
            break;
        case 3:
            device = "/dev/i2c-3" ;
            break;
        default:
            // This will only happen if there is a coding error when using this function
            log_write(LOG_ERROR, "Invalid I2C Interface ID");
            exit(EXIT_FAILURE);
    }
    return i2c_setup_interface(device, devID); 
}

/**
 * @brief Main I2C function used to read / write data to and from the configured bus.
 * 
 * @param fd Configured I2C bus instance
 * @param rwType Type of I2C operation to be completed (1 = I2C_READ, 0 = I2C_WRITE)
 * @param sizeByte The size of data array being sent or received, in number of bytes
 * @param data Pointer to data array that stores data
 * @return Iris error code indicating the success or failure of function
 */
enum IRIS_ERROR i2c_interface (int fd, int rwType, int sizeByte, uint8_t *data){
    
    int error;
    if (rwType == I2C_READ){
        error = read(fd, data, sizeByte);
    }else if (rwType == I2C_WRITE){
        error = write(fd, data, sizeByte);
    }else{
        // This will only happen if there is a coding error when using this function
        log_write(LOG_ERROR, "Invalid I2C Interface Operation (R/W)");
        exit(EXIT_FAILURE);
    }

    //Check if I2C operation was successful
    if (error == sizeByte){
        return 0;
    }else{
        log_write(LOG_ERROR, "Invalid I2C Interface Operation (R/W)");
        switch (rwType){
            case I2C_READ:
                return I2C_READ_ERROR;
            case I2C_WRITE:
                return I2C_WRITE_ERROR;
        }
    }
    return NO_ERROR;
}

/**
 * @brief Write data to I2C peripheral's 8-bit register.
 * 
 * @param fd Configured I2C bus instance
 * @param writeNum Number of 8-bit packets to write to I2C peripheral
 * @param data Pointer to array that stores data being written, first byte should be address of register user
 *             wants to write too
 * @return Iris error code indicating the success or failure of function
 */
enum IRIS_ERROR i2c_write_reg8 (int fd, int writeNum, uint8_t *data){

    if (writeNum > I2C_MAX_WRITE){
        // This will only happen if there is a coding error when using this function
        log_write(LOG_ERROR, "I2C Command Fail: To many write requests");
        exit(EXIT_FAILURE);
    }

    return i2c_interface(fd, I2C_WRITE, writeNum, data);

}

/**
 * @brief Write data to I2C peripheral's 16-bit register.
 * 
 * @param fd Configured I2C bus instance
 * @param writeNum Number of 16-bit packets to write to I2C peripheral
 * @param reg Pointer to register address value
 * @param data Pointer to array that stores data being written
 * @return Iris error code indicating the success or failure of function
 */
enum IRIS_ERROR i2c_write_reg16 (int fd, int writeNum, uint8_t *reg, uint16_t *data){
    
    uint8_t tempData[I2C_MAX_WRITE];
    if ((2*writeNum - 1) > I2C_MAX_WRITE){
        // This will only happen if there is a coding error when using this function
        log_write(LOG_ERROR, "I2C Command Fail: Too many write requests");
        exit(EXIT_FAILURE);
    }

    // Combines 8-bit packets from I2C into 16-bit array
    tempData[0] = *reg;
    for(int x = 1; x < writeNum; x++){
        tempData[2*x - 1] = (data[x-1] >> 8) & 0xFF;
        tempData[2*x] = data[x-1] & 0xFF; 
    }
    return i2c_interface(fd, I2C_WRITE, (2*writeNum - 1), tempData);

}

/**
 * @brief Read data from I2C peripheral's 8-bit register. Need to first 
 *        confiugre what register to read using a write command.
 * 
 * @param fd Configured I2C bus instance
 * @param readNum Number of 8-bit packets to read from I2C peripheral
 * @param data Pointer to array that will store data being read
 * @return Iris error code indicating the success or failure of function
 */
enum IRIS_ERROR i2c_read_reg8 (int fd, int readNum, uint8_t *data){

    if (readNum > I2C_MAX_READ){
        // This will only happen if there is a coding error when using this function
        log_write(LOG_ERROR, "I2C Command Fail: Too many read requests");
        exit(EXIT_FAILURE);
    }

    return i2c_interface(fd, I2C_READ, readNum, data);

}

/**
 * @brief Read data from I2C peripheral's 16-bit register. Need to first 
 *        configure what register to read using a write command.
 * 
 * @param fd Configured I2C bus instance
 * @param readNum Number of 16-bit packets to read from I2C peripheral
 * @param data Pointer to array that will store data being read
 * @return Iris error code indicating the success or failure of function
 */
enum IRIS_ERROR i2c_read_reg16 (int fd, int readNum, uint16_t *data){

    int errorCheck;
    uint8_t tempData[I2C_MAX_READ];
    if ((2*readNum) > I2C_MAX_READ){
        // This will only happen if there is a coding error when using this function
        log_write(LOG_ERROR, "I2C Command Fail: Too many read requests");
        exit(EXIT_FAILURE);
    }
    errorCheck = i2c_interface(fd, I2C_READ, (2*readNum), tempData);
    for(int x = 0; x < readNum; x++){
        data[x] = (tempData[x] << 8) | tempData[x+1]; 
    }

    return errorCheck;

}

/**
 * @brief Read data from I2C peripheral's 8-bit register using provided address. Will 
 *        configure peripheral to read out inputed address.
 * 
 * @param fd Configured I2C bus instance
 * @param reg Pointer to register address value
 * @param readNum Number of 8-bit packets to read from I2C peripheral
 * @param data Pointer to array that will store data being read
 * @return Iris error code indicating the success or failure of function
 */
enum IRIS_ERROR i2c_reg8_write_read (int fd, uint8_t *reg, int readNum, uint8_t *data){

    i2c_write_reg8 (fd, 1, reg);
    return i2c_read_reg8 (fd, readNum, data);

}

/**
 * @brief Read data from I2C peripheral's 16-bit register using provided address. Will 
 *        configure peripheral to read out inputed address.
 * 
 * @param fd Configured I2C bus instance
 * @param reg 8-Bit Register Address
 * @param readNum Number of 16-bit packets to read from I2C peripheral
 * @param data Pointer to array that will store data being read
 * @return Iris error code indicating the success or failure of function
 */
enum IRIS_ERROR i2c_reg16_write_read (int fd, uint8_t *reg, int readNum, uint16_t *data){

    i2c_write_reg8 (fd, 1, reg);
    return i2c_read_reg16 (fd, readNum, data);

}

/**
 * @brief Closes instance of I2C Interface
 * 
 * @param fd Configured I2C bus instance
 */
int i2c_close(int fd) {
    return close(fd);
}