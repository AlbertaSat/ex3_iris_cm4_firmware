#include "i2c.h"
#include "main.h"
#include "gpio.h"
#include "logger.h"
#include "error_handler.h"
#include "usb_hub.h"

#include <stdio.h>


#include <stdio.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <gpiod.h>

// enum IRIS_ERROR usb_hub_func_TESTING(struct gpiod_line_request *gpio_request){
    
//     int bus;
//     int ErrorCheck;
//     int inputVal;
//     char logBuffer[255];

//                       uint8_t hubAddr = USB_HUB_I2C_ADDR;

//     uint8_t regAddr[3] = {CFG_DATA_BYTE_1, CFG_DATA_BYTE_2, CFG_DATA_BYTE_3};
//     uint8_t regConfig[3] = {CFG_DATA_BYTE_1_POR, CFG_DATA_BYTE_2_POR, CFG_DATA_BYTE_3_POR};
//     uint8_t regData[5] = {NULL};

//     snprintf(logBuffer, sizeof(logBuffer), "USB-HUB-FUNC-VALIDATE: Begin functional verification of USB Hub 0x%02x", hubAddr);
//     log_write(LOG_INFO, logBuffer);

//         //Setup I2C Interface with USB Hub
//         bus = i2c_setup(I2C_BUS_INDEX, hubAddr);
//         if (bus == I2C_SETUP_ERROR){
//             snprintf(logBuffer, sizeof(logBuffer), "USB Hub 0x%02x - I2C Bus Failed to Open", hubAddr);
//             log_write(LOG_ERROR, logBuffer);
//             return USB_HUB_VERIFICATION_ERROR;
//         }

//     uint8_t newReg[27] = {6, 11, 0x9B, 0x20, 0x02, 0, 0, 0, 0x01, 0x32, 0x01, 0x32, 0x32};
//             ErrorCheck = i2c_write_reg8(bus, 13, newReg);

//             i2c_close(bus);

//     int fd = open(I2C_DEV_PATH, O_RDWR);
//     if (fd < 0) {
//         perror("Failed to open I2C device");
//         return 1;
//     }

//     struct i2c_msg msgs[3];
//     struct i2c_rdwr_ioctl_data msgset;

//     uint8_t reg = REG_ADDR;
//     uint8_t data[25] = {0};
//     uint8_t backup = 0;
//     // First message: Send register address (no stop condition)
//     msgs[0].addr  = hubAddr;
//     msgs[0].flags = 0; // Write
//     msgs[0].len   = 1;
//     msgs[0].buf   = &reg;

//     // Second message: Read data (with repeated start)
//     msgs[1].addr  = hubAddr;
//     msgs[1].flags = I2C_M_RD; // Read
//     msgs[1].len   = 25;
//     msgs[1].buf   = data;

//         // // Second message: Read data (with repeated start)
//         // msgs[2].addr  = hubAddr;
//         // msgs[2].flags = I2C_M_RD; // Read
//         // msgs[2].len   = 1;
//         // msgs[2].buf   = &backup;

//     // Prepare transaction
//     msgset.msgs = msgs;
//     msgset.nmsgs = 2;

//     // Perform transaction
//     if (ioctl(fd, I2C_RDWR, &msgset) < 0) {
//         perror("Failed to perform I2C_RDWR");
//         close(fd);
//         return 1;
//     }

//     printf("Register 0x%02X: 0x%02X\n", REG_ADDR, data[0]);
//     printf("Register 0x%02X: 0x%02X\n", REG_ADDR + 1, data[1]);
//     printf("Register 0x%02X: 0x%02X\n", REG_ADDR + 2, data[2]);
//     printf("Register 0x%02X: 0x%02X\n", REG_ADDR + 3, data[3]);
//     printf("Register 0x%02X: 0x%02X\n", REG_ADDR + 4, data[4]);
//     printf("Register 0x%02X: 0x%02X\n", REG_ADDR + 5, data[5]);
//     printf("Register 0x%02X: 0x%02X\n", REG_ADDR + 6, data[6]);
//     printf("Register 0x%02X: 0x%02X\n", REG_ADDR + 7, data[7]);
//     printf("Register 0x%02X: 0x%02X\n", REG_ADDR + 8, data[8]);
//     printf("Register 0x%02X: 0x%02X\n", REG_ADDR + 9, data[9]);
//     close(fd);

// //     //Setup I2C Interface with USB Hub
// //     bus = i2c_setup(I2C_BUS_INDEX, hubAddr);
// //     if (bus == I2C_SETUP_ERROR){
// //         snprintf(logBuffer, sizeof(logBuffer), "USB Hub 0x%02x - I2C Bus Failed to Open", hubAddr);
// //         log_write(LOG_ERROR, logBuffer);
// //         return USB_HUB_VERIFICATION_ERROR;
// //     }

// //     //Reads and verifies the configuration registers of the USB Hub
// //     for(int index = 0; index < sizeof(regAddr); index++){
// //         ErrorCheck = i2c_reg8_write_read(bus, (regAddr + index), 5, regData);
// //         if (ErrorCheck == I2C_WR_R_ERROR){
// //             snprintf(logBuffer, sizeof(logBuffer), "USB Hub 0x%02x - I2C Reg 0x%02x Read Failed", hubAddr, regAddr[index]);
// //             log_write(LOG_ERROR, logBuffer);
// //             return USB_HUB_VERIFICATION_ERROR;
// //         }
// //         if (regData[0] != regConfig[index]){
// //             snprintf(logBuffer, sizeof(logBuffer), "USB Hub 0x%02x - I2C Reg 0x%02x Value Incorrect 0x%02x", hubAddr, regAddr[index], regData[0]);
// //             log_write(LOG_ERROR, logBuffer);
// //             return USB_HUB_VERIFICATION_ERROR;
// //         }
// //     }

// //     //Check GPIO Indicators for the USB Hub
// //     inputVal = gpiod_line_request_get_value(gpio_request, HUB_HS_IND);
// //     if(inputVal !=  HUB_HS_IND_DEF){
// //         log_write(LOG_ERROR, "USB-HUB-FUNC-VALIDATE: USB Hub HS Indicator GPIO not outputing correct value");
// //         return USB_HUB_VERIFICATION_ERROR;
// //     }

// //     inputVal = gpiod_line_request_get_value(gpio_request, HUB_SETUP_IND);
// //     if(inputVal !=  HUB_SETUP_IND_DEF){
// //         log_write(LOG_ERROR, "USB-HUB-FUNC-VALIDATE: USB Hub Setup Indicator GPIO not outputing correct value");
// //         return USB_HUB_VERIFICATION_ERROR;
// //     }

// //     snprintf(logBuffer, sizeof(logBuffer), "USB-HUB-FUNC-VALIDATE: Completed attempt for functional verification of USB Hub 0x%02x", hubAddr);
// //     log_write(LOG_INFO, logBuffer);

// //     i2c_close(bus);
// //     return NO_ERROR;
// }


/**
 * @brief Configures the USB Hub using pre-defined settings.
 * 
 * @return Iris error code indicating the success or failure of function
 */
enum IRIS_ERROR usb_hub_setup(void){

    int bus = 0; 
    char logBuffer[LOG_BUFFER_SIZE];
    enum IRIS_ERROR error = NO_ERROR;

    uint8_t hubAddr = USB_HUB_I2C_ADDR;

    uint8_t regAddr[3] = {CFG_DATA_BYTE_1, CFG_DATA_BYTE_2, CFG_DATA_BYTE_3};
    uint8_t regConfig[3] = {CFG_DATA_BYTE_1_POR, CFG_DATA_BYTE_2_POR, CFG_DATA_BYTE_3_POR};
    uint8_t regData[2] = {0};

    snprintf(logBuffer, sizeof(logBuffer), "USB-HUB-SETUP: Begin setup of USB Hub 0x%02x", hubAddr);
    log_write(LOG_INFO, logBuffer);

    //Setup I2C Interface with USB Hub
    bus = i2c_setup(I2C_BUS_INDEX, hubAddr);
    if (bus == I2C_SETUP_ERROR){
        snprintf(logBuffer, sizeof(logBuffer), "USB Hub 0x%02x - I2C Bus Failed to Open", hubAddr);
        log_write(LOG_ERROR, logBuffer);
        return USB_HUB_SETUP_ERROR;
    }

    //Write the configuration registers of the Temp Sensor
    for(int index = 0; index < sizeof(regAddr); index++){
        regData[0] = regAddr[index];
        regData[1] = regConfig[index];
        error = i2c_write_reg8(bus, 2, regData);
        if (error == I2C_WRITE_ERROR){
            snprintf(logBuffer, sizeof(logBuffer), "USB Hub 0x%02x - I2C Reg 0x%02x Write Failed", hubAddr, regAddr[index]);
            log_write(LOG_ERROR, logBuffer);
            return USB_HUB_SETUP_ERROR;
        }
    }

    snprintf(logBuffer, sizeof(logBuffer), "USB-HUB-SETUP: Completed attempt for setup of USB Hub 0x%02x",hubAddr);
    log_write(LOG_INFO, logBuffer);

    i2c_close(bus);
    return error;
}

/**
 * @brief Verifies the USB Hub configuration by checking register values.
 * 
 * @return Iris error code indicating the success or failure of function
 */
//! ADD CODE FOR RESETING DEVICE AND CHECKING FOR FAULTS IE OPEN CIRCUIT
//! ADD CODE FOR DEALING WITH DIFFERNET ERROR (WRONG CONFIG REG, INVALID TEMP MEAS)
enum IRIS_ERROR usb_hub_func_validate(struct gpiod_line_request *gpio_request){
    
    int bus = 0;
    int inputVal = 0;
    enum IRIS_ERROR errorCheck = NO_ERROR;
    char logBuffer[LOG_BUFFER_SIZE];

    uint8_t hubAddr = USB_HUB_I2C_ADDR;

    uint8_t regAddr[3] = {CFG_DATA_BYTE_1, CFG_DATA_BYTE_2, CFG_DATA_BYTE_3};
    uint8_t regConfig[3] = {CFG_DATA_BYTE_1_POR, CFG_DATA_BYTE_2_POR, CFG_DATA_BYTE_3_POR};
    uint8_t regData[2] = {0};

    snprintf(logBuffer, sizeof(logBuffer), "USB-HUB-FUNC-VALIDATE: Begin functional verification of USB Hub 0x%02x", hubAddr);
    log_write(LOG_INFO, logBuffer);

    //Setup I2C Interface with USB Hub
    bus = i2c_setup(I2C_BUS_INDEX, hubAddr);
    if (bus == I2C_SETUP_ERROR){
        snprintf(logBuffer, sizeof(logBuffer), "USB Hub 0x%02x - I2C Bus Failed to Open", hubAddr);
        log_write(LOG_ERROR, logBuffer);
        return USB_HUB_VERIFICATION_ERROR;
    }

    //Reads and verifies the configuration registers of the USB Hub
    for(int index = 0; index < sizeof(regAddr); index++){
        errorCheck = i2c_reg8_write_read(bus, (regAddr + index), 1, regData);
        if (errorCheck == I2C_WR_R_ERROR){
            snprintf(logBuffer, sizeof(logBuffer), "USB Hub 0x%02x - I2C Reg 0x%02x Read Failed", hubAddr, regAddr[index]);
            log_write(LOG_ERROR, logBuffer);
            return USB_HUB_VERIFICATION_ERROR;
        }
        if (regData[0] != regConfig[index]){
            snprintf(logBuffer, sizeof(logBuffer), "USB Hub 0x%02x - I2C Reg 0x%02x Value Incorrect 0x%02x", hubAddr, regAddr[index], regData[0]);
            log_write(LOG_ERROR, logBuffer);
            return USB_HUB_VERIFICATION_ERROR;
        }
    }

    //Check GPIO Indicators for the USB Hub
    inputVal = gpiod_line_request_get_value(gpio_request, HUB_HS_IND);
    if(inputVal !=  HUB_HS_IND_DEF){
        log_write(LOG_ERROR, "USB-HUB-FUNC-VALIDATE: USB Hub HS Indicator GPIO not outputing correct value");
        return USB_HUB_VERIFICATION_ERROR;
    }

    inputVal = gpiod_line_request_get_value(gpio_request, HUB_SETUP_IND);
    if(inputVal !=  HUB_SETUP_IND_DEF){
        log_write(LOG_ERROR, "USB-HUB-FUNC-VALIDATE: USB Hub Setup Indicator GPIO not outputing correct value");
        return USB_HUB_VERIFICATION_ERROR;
    }

    snprintf(logBuffer, sizeof(logBuffer), "USB-HUB-FUNC-VALIDATE: Completed attempt for functional verification of USB Hub 0x%02x", hubAddr);
    log_write(LOG_INFO, logBuffer);

    i2c_close(bus);
    return NO_ERROR;
}


/**
 * @brief Triggers a reset of USB Hub. 
 *        Note: This will NOT reconfigure the USB Hub.
 * 
 * @param gpio_request GPIO Request structure used to toggle USB Hub Reset GPIO
 * @return Iris error code indicating the success or failure of function
 */
enum IRIS_ERROR usb_hub_reset_trig(struct gpiod_line_request *gpio_request){

    enum IRIS_ERROR errorCheck = NO_ERROR;
    char logBuffer[LOG_BUFFER_SIZE];

    uint8_t hubAddr = USB_HUB_I2C_ADDR;

    snprintf(logBuffer, sizeof(logBuffer), "USB-HUB-RESET: Begin reset of USB Hub 0x%02x", hubAddr);
    log_write(LOG_INFO, logBuffer);

    errorCheck = gpiod_line_request_set_value(gpio_request, HUB_RST_L, GPIOD_LINE_VALUE_INACTIVE);
    if (errorCheck == -1){
        log_write(LOG_ERROR, "USB-HUB-RESET: Failed to assert reset GPIO USB Hub");
        return USB_HUB_RESET_ERROR;
    }
    errorCheck = gpiod_line_request_set_value(gpio_request, HUB_RST_L, GPIOD_LINE_VALUE_ACTIVE);
    if (errorCheck == -1){
        log_write(LOG_ERROR, "USB-HUB-RESET: Failed to deassert reset GPIO USB Hub");
        return USB_HUB_RESET_ERROR;
    }

    snprintf(logBuffer, sizeof(logBuffer), "USB-HUB-RESET: Successful reset trigger of USB Hub 0x%02x", hubAddr);
    log_write(LOG_INFO, logBuffer);

    return NO_ERROR;
}
