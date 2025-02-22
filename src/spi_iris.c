
/**
 * @file spi_iris.c
 * @author Noah Klager
 * @brief SPI Driver for Theia CM4 
 *        Provides functions to...
 *         - Configure a SPI Interface on the CM4
 *         - Configure CS to detect any event on the GPIO line
 *         - Write a file to SPI Peripherals
 *         - Test functionality of the SPI Interface
 *         - Write 8-bit data packets to SPI Peripherals
 *         - Read 8-bit data packets from SPI Peripherals
 * 
 * @version 0.1
 * @date 2024-11-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//---- Headers ----//
#include "cmd_controller.h"
#include "error_handler.h"
#include "gpio.h"
#include "file_operations.h"
#include "logger.h"
#include "main.h"
#include "spi_iris.h"
#include "timing.h"

#include <fcntl.h>
#include <gpiod.h>
#include <linux/spi/spidev.h>
#include <openssl/sha.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>


/**
 * @brief Configures the selected SPI Interface to communicate with SPI Peripherals
 * 
 * @param device Pointer to character array selecting SPI bus
 * @param config 'spi_config_t' structure containing configuration information for the interface
 * @return Either ID of SPI bus instance or Error Code if unsuccessful
 */
int spi_open(const char *device, spi_config_t config) {

    int fileDesc = 0;
    fileDesc = open(device, O_RDWR);
    if (fileDesc < 0) {
        return SPI_SETUP_ERROR;
    }

    // Set SPI_POL and SPI_PHA
    if (ioctl(fileDesc, SPI_IOC_WR_MODE, &config.mode) < 0) {
        log_write(LOG_ERROR, "SPI Bus - Failed to Set WR IOC");
        return SPI_SETUP_ERROR;
    }
    if (ioctl(fileDesc, SPI_IOC_RD_MODE, &config.mode) < 0) {
        log_write(LOG_ERROR, "SPI Bus - Failed to Set RD IOC");
        return SPI_SETUP_ERROR;
    }

    // Set bits per word
    if (ioctl(fileDesc, SPI_IOC_WR_BITS_PER_WORD, &config.bits_per_word) < 0) {
        log_write(LOG_ERROR, "SPI Bus - Failed to Set WR Bits-per-Word");
        return SPI_SETUP_ERROR;
    }
    if (ioctl(fileDesc, SPI_IOC_RD_BITS_PER_WORD, &config.bits_per_word) < 0) {
        log_write(LOG_ERROR, "SPI Bus - Failed to Set RD Bits-per-Word");
        return SPI_SETUP_ERROR;
    }

    // Set SPI speed
    if (ioctl(fileDesc, SPI_IOC_WR_MAX_SPEED_HZ, &config.speed) < 0) {
        log_write(LOG_ERROR, "SPI Bus - Failed to Set WR Speed");
        return SPI_SETUP_ERROR;
    }
    if (ioctl(fileDesc, SPI_IOC_RD_MAX_SPEED_HZ, &config.speed) < 0) {
        log_write(LOG_ERROR, "SPI Bus - Failed to Set RD Speed");
        return SPI_SETUP_ERROR;
    }

    // Return file descriptor
    return fileDesc;
}

/**
 * @brief Closes instance of SPI Interface
 * 
 * @param fileDesc Configured SPI bus instance
 */
int spi_close(int fileDesc) {
    return close(fileDesc);
}


//! ADD DESCRIPTIONNNNNNNNNNNNNNNNNNNNNNNNNNNNN
enum IRIS_ERROR spi_init(int *spi_dev, struct gpiod_line_request **spi_cs_request, struct gpiod_edge_event_buffer **event_buffer){

    int errorCheck = NO_ERROR;
    int loopCounter = 0;

    log_write(LOG_INFO, "SPI-INIT: Start setup of SPI interface with OBC");
    
    //! SHOULD I SPLIT EACH ATTEMPT UP INTO SEPARATE LOOPS
    do{
        loopCounter++;
        *spi_dev = spi_bus_setup();
        if (*spi_dev == SPI_SETUP_ERROR){
            errorCheck = SPI_SETUP_ERROR;
            continue;
        } 
        *spi_cs_request = spi_cs_setup();
        if (*spi_cs_request == NULL){
            spi_close(*spi_dev);
            errorCheck = SPI_SETUP_ERROR;
            continue;
        }
        *event_buffer = gpiod_edge_event_buffer_new(EDGE_EVENT_BUFF_SIZE);
        if (*event_buffer == NULL){
            spi_close(*spi_dev);
            gpiod_line_request_release(*spi_cs_request);
            errorCheck = SPI_SETUP_ERROR;
            continue;
        }
        errorCheck = NO_ERROR;
    }while((errorCheck == SPI_SETUP_ERROR) && (loopCounter < MAX_SPI_INIT_ATTEMPTS));
    
    loopCounter = 0;
    // if(errorCheck == NO_ERROR){
    //     do{
    //         errorCheck = spi_bus_test(*spi_dev, *spi_cs_request, *event_buffer);
    //         loopCounter++;
    //     }while((errorCheck == SPI_TEST_ERROR) && (loopCounter < MAX_SPI_INIT_ATTEMPTS));

    //     if(errorCheck != NO_ERROR){
    //         spi_close(*spi_dev);
    //         gpiod_edge_event_buffer_free(*event_buffer);
    //         gpiod_line_request_release(*spi_cs_request);
    //     }
    // }

    log_write(LOG_INFO, "SPI-INIT: Finished setup attempt of SPI interface with OBC");

    return errorCheck;
}


/**
 * @brief Read data as 8-bit packets from SPI Peripheral
 * 
 * @param fileDesc Configured SPI bus instance
 * @param rx_buffer Pointer to array buffer that will store data being read
 * @param rx_len Number of 8-bit packets to read from SPI Peripheral
 * @param cs_request Pointer to structure that contains the CS instance
 * @return Iris error code indicating the success or failure of function
 */
enum IRIS_ERROR spi_read(int fileDesc, uint8_t *rx_buffer, uint16_t rx_len, struct gpiod_line_request *cs_request){

    int retVal = 0;
    struct spi_ioc_transfer spi_msg[1];
    memset(spi_msg, 0, sizeof(spi_msg));

    spi_msg[0].rx_buf = (unsigned long)rx_buffer;
    spi_msg[0].len = rx_len;

    cs_request = cs_toggle(cs_request, CS_RW);
    retVal = ioctl(fileDesc, SPI_IOC_MESSAGE(1), spi_msg);
    cs_request = cs_toggle(cs_request, CS_MONITOR);

    if(retVal == rx_len){
        return NO_ERROR;
    }
    return SPI_READ_ERROR;
}

/**
 * @brief Write data as 8-bit packets to SPI Peripheral
 * 
 * @param fileDesc Configured SPI bus instance
 * @param tx_buffer Pointer to array buffer storing data to be written
 * @param tx_len Number of 8-bit packets to write to SPI Peripheral
 * @param cs_request Pointer to structure that contains the CS instance
 * @return Iris error code indicating the success or failure of function
 */
enum IRIS_ERROR spi_write(int fileDesc, const uint8_t *tx_buffer, uint16_t tx_len, struct gpiod_line_request *cs_request){

    int retVal = 0;
    struct spi_ioc_transfer spi_msg[1];
    memset(spi_msg, 0, sizeof(spi_msg));

    spi_msg[0].tx_buf = (unsigned long)tx_buffer;
    spi_msg[0].len = tx_len;

    cs_request = cs_toggle(cs_request, CS_RW);
    retVal = ioctl(fileDesc, SPI_IOC_MESSAGE(1), spi_msg);
    cs_request = cs_toggle(cs_request, CS_MONITOR);

    if(retVal == tx_len){
        return NO_ERROR;
    }
    return SPI_WRITE_ERROR;

}


/**
 * @brief Configures the GPIO used for SPI CS as an input to detect any events on the line
 * 
 * @return Structure containing CS request, can be used to reconfigure the CS GPIO
 */
struct gpiod_line_request *spi_cs_setup(void){

    struct gpiod_line_request *request = NULL;
    const char *gpioDev = GPIOCHIP;
    //create request for SPI CS configuration
    request = gpio_config_input_detect(gpioDev, SPI_CE_N, EDGE_FALL, "IRIS SPI CS");

    return request;

}


/**
 * @brief High level function used to configure the SPI Interface
 * 
 * @return Instance of SPI Interface
 */
int spi_bus_setup(void){

    spi_config_t spi_config_test;
    int spi_dev = 0;

    spi_config_test.mode = SPI_MODE_TYP_0;
    spi_config_test.speed = SPI_SPEED;
    spi_config_test.delay = SPI_DELAY;
    spi_config_test.bits_per_word = SPI_BITS_PER_WORD;

    char *spi_device_name = SPI_DEVICE;

    spi_dev = spi_open(spi_device_name, spi_config_test);

    return spi_dev;

}


/**
 * @brief Reinitialize the SPI Interface
 * 
 * @return Instance of SPI Interface
 */
enum IRIS_ERROR spi_reinit(int *spi_dev, struct gpiod_line_request **spi_cs_request, struct gpiod_edge_event_buffer **event_buffer){

    spi_close(*spi_dev);
    gpiod_edge_event_buffer_free(*event_buffer);
    gpiod_line_request_release(*spi_cs_request);

    return spi_init(spi_dev, spi_cs_request, event_buffer);

}


/**
 * @brief Write a file to SPI Peripheral. Currently only simple files such as binary / text have been verified, 
 *        may need to be modified to handle image files.
 * 
 * @param spi_dev Configured SPI bus instance
 * @param spi_cs_request Pointer to structure that contains the CS instance
 * @param file_path Pointer to character array with path to file that will be written
 * @return Iris error code indicating the success or failure of function
 */
//! ADD FUNCTION TO VERIFY DATA OF FILE TRANSFER
//! MODIFY TO WORK WITH IMAGE FILES
enum IRIS_ERROR spi_file_write(int spi_dev, struct gpiod_line_request **spi_cs_request, char *file_path, struct gpiod_edge_event_buffer **event_buffer){

    uint8_t txBuffer[SPI_FILE_BUFFER_LEN * 3] = {0};
    char logBuffer[LOG_BUFFER_SIZE];
    int bytesRead = 0;
    IRIS_ERROR error = NO_ERROR;
    uint8_t checksum[SHA256_DIGEST_LENGTH + 1];
    checksum[0] = FILE_TRANSFER;

    log_write(LOG_INFO, "SPI-FILE-WRITE: Begin file write onto SPI bus too OBC");

    //! ADD ERROR DETECT FOR CHECKSUM
    sha256_checksum(file_path, checksum + 1);

    FILE *file = fopen(file_path, "rb");

    if (file) {
        error = spi_write(spi_dev, checksum, sizeof(checksum), *spi_cs_request);
        if (error == SPI_WRITE_ERROR){
            log_write(LOG_ERROR, "SPI-FILE-WRITE: Failed to write Checksum onto SPI bus due to 'spi_write' FAIL");
            return SPI_FILE_WRITE_ERROR;
        }

        bytesRead = (int)fread(txBuffer, 1, SPI_FILE_BUFFER_LEN, file);
        do {
            error = spi_write(spi_dev, txBuffer, bytesRead, *spi_cs_request);
            if (error == SPI_WRITE_ERROR){
                log_write(LOG_ERROR, "SPI-FILE-WRITE: Failed to write file onto SPI bus due to 'spi_write' FAIL");
                return SPI_FILE_WRITE_ERROR;
            }
            //usleep(15);

            bytesRead = (int)fread(txBuffer, 1, SPI_FILE_BUFFER_LEN, file);

        } while (bytesRead > 0);
        fclose(file);

        log_write(LOG_INFO, "SPI-FILE-WRITE: Completed file write onto SPI bus too OBC");
        return NO_ERROR;
    }

    snprintf(logBuffer, sizeof(logBuffer), "SPI-FILE-WRITE: Failed to open file %s", file_path);
    log_write(LOG_ERROR, logBuffer);
    return SPI_FILE_WRITE_ERROR;
}


//! NEED TO FIX FUNCTION
//! NEED IT TO PROPERLY DETECT WHEN THE FILE READ IS DONE
enum IRIS_ERROR spi_file_read(int spi_dev, struct gpiod_line_request **spi_cs_request, struct gpiod_edge_event_buffer **event_buffer, const char *file_path){

    uint8_t rxBuffer[SPI_FILE_BUFFER_LEN];
    char logBuffer[LOG_BUFFER_SIZE];
    int bytesWrote = 0;
    IRIS_ERROR error = NO_ERROR;
    
    //bool cs_edge = false;
    //uint8_t checksum[SHA256_DIGEST_LENGTH];

    log_write(LOG_INFO, "SPI-FILE-WRITE: Begin file write onto SPI bus too OBC");

    FILE *file = fopen(file_path, "wb");

    if (file) {
        do {
            //cs_edge = signal_edge_detect(*spi_cs_request, *event_buffer);
            //if (cs_edge == true){
                error = spi_read(spi_dev, rxBuffer, SPI_FILE_BUFFER_LEN, *spi_cs_request);
                //checksum = checksum_calc(txBuffer, bytesRead);
                //txBuffer[0] = checksum;
            if (error != NO_ERROR){
                log_write(LOG_ERROR, "SPI-FILE-WRITE: Failed to write file onto SPI bus due to 'spi_write' FAIL");
                return SPI_FILE_WRITE_ERROR;
            }
            
            bytesWrote = (int)fwrite(rxBuffer, 1, SPI_FILE_BUFFER_LEN, file);
            //}
        } while (bytesWrote > 0);
        fclose(file);

        log_write(LOG_INFO, "SPI-FILE-WRITE: Completed file write onto SPI bus too OBC");
        return NO_ERROR;
    }

    fclose(file);

    snprintf(logBuffer, sizeof(logBuffer), "SPI-FILE-WRITE: Failed to open file %s", file_path);
    log_write(LOG_ERROR, logBuffer);

    return SPI_FILE_WRITE_ERROR;
}

enum IRIS_ERROR spi_bus_test_compare(const uint8_t *buffer){

    uint8_t reference[SPI_TEST_MSG_SIZE] = SPI_TEST_MSG;

    for (int index = 0; index < sizeof(reference); index++){
        if(buffer[index] != reference[index]){
            return SPI_TEST_ERROR;
        }
    }
    return NO_ERROR;

}

/**
 * @brief Test functionality of the SPI Interface.
 *        Sends series of known packets to peripheral, next waits for peripheral to send what it received 
 *        back to Host. Compares packets recieved with the ones sent to confirm successful transfer.
 * 
 * @param spi_dev Configured SPI bus instance
 * @param spi_cs_request Pointer to structure that contains the CS instance
 * @param event_buffer Pointer to buffer that stores all events detected on CS
 * @return Iris error code indicating the success or failure of function
 */
enum IRIS_ERROR spi_bus_test(int spi_dev, struct gpiod_line_request *spi_cs_request, struct gpiod_edge_event_buffer *event_buffer){

    uint8_t buffer[SPI_TEST_MSG_SIZE] = SPI_TEST_MSG;
    uint16_t rwNum = sizeof(buffer);
    enum IRIS_ERROR error = NO_ERROR;

    log_write(LOG_INFO, "SPI-BUS-TEST: Begin SPI bus test transfer too OBC");

    error = spi_write(spi_dev, buffer, rwNum, spi_cs_request);
    if (error == SPI_WRITE_ERROR){
        log_write(LOG_ERROR, "SPI-BUS-TEST: Failed to write test message onto SPI bus");
        return SPI_TEST_ERROR;
    }

    int start_time = get_time_seconds();
    bool cs_edge = false;

    log_write(LOG_INFO, "SPI-BUS-TEST: Begin SPI bus test read from OBC");

    do{
        cs_edge = signal_edge_detect(spi_cs_request, event_buffer);

        if(cs_edge == true){
            error = spi_read(spi_dev, buffer, rwNum, spi_cs_request);
            if (error == SPI_READ_ERROR){
                log_write(LOG_ERROR, "SPI-BUS-TEST: Failed to read test message from OBC");
                return SPI_TEST_ERROR;
            }
            log_write(LOG_INFO, "SPI-BUS-TEST: Completed read of test message from OBC");
            return spi_bus_test_compare(buffer);
        }
 
    }while((get_time_seconds() - start_time) < SPI_TEST_TIMEOUT);

    log_write(LOG_ERROR, "SPI-BUS-TEST: TIMEOUT Failed to read test message from OBC");
    return SPI_TEST_ERROR;
}