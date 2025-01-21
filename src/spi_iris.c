
#include <linux/spi/spidev.h>

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <string.h>
#include <time.h>

#include "spi_iris.h"
#include "main.h"
#include "gpio.h"
#include "error_handler.h"
#include "logger.h"


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


/**
 * @brief Configures the selected SPI Interface to communicate with SPI Peripherals
 * 
 * @param device Pointer to character array selecting SPI bus
 * @param config 'spi_config_t' structure containing configuration information for the interface
 * @return Either ID of SPI bus instance or Error Code if unsuccessful
 */
int spi_open(char *device, spi_config_t config) {

    int fd;
    // Open block device
    fd = open(device, O_RDWR);
    if (fd < 0) {
        return SPI_SETUP_ERROR;
    }

    // Set SPI_POL and SPI_PHA
    if (ioctl(fd, SPI_IOC_WR_MODE, &config.mode) < 0) {
        log_write(LOG_ERROR, "SPI Bus - Failed to Set WR IOC");
        return SPI_SETUP_ERROR;
    }
    if (ioctl(fd, SPI_IOC_RD_MODE, &config.mode) < 0) {
        log_write(LOG_ERROR, "SPI Bus - Failed to Set RD IOC");
        return SPI_SETUP_ERROR;
    }

    // Set bits per word
    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &config.bits_per_word) < 0) {
        log_write(LOG_ERROR, "SPI Bus - Failed to Set WR Bits-per-Word");
        return SPI_SETUP_ERROR;
    }
    if (ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &config.bits_per_word) < 0) {
        log_write(LOG_ERROR, "SPI Bus - Failed to Set RD Bits-per-Word");
        return SPI_SETUP_ERROR;
    }

    // Set SPI speed
    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &config.speed) < 0) {
        log_write(LOG_ERROR, "SPI Bus - Failed to Set WR Speed");
        return SPI_SETUP_ERROR;
    }
    if (ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &config.speed) < 0) {
        log_write(LOG_ERROR, "SPI Bus - Failed to Set RD Speed");
        return SPI_SETUP_ERROR;
    }

    // Return file descriptor
    return fd;
}

/**
 * @brief Closes instance of SPI Interface
 * 
 * @param fd Configured SPI bus instance
 */
int spi_close(int fd) {
    return close(fd);
}

/**
 * @brief Write than Read data as 8-bit packets from SPI Peripheral
 * 
 * @param fd Configured SPI bus instance
 * @param tx_buffer Pointer to array buffer storing data to be written
 * @param tx_len Number of 8-bit packets to write to SPI Peripheral
 * @param rx_buffer Pointer to array buffer that will store data being read
 * @param rx_len Number of 8-bit packets to read from SPI Peripheral
 * @param cs_request Pointer to structure that contains the CS instance
 * @return Iris error code indicating the success or failure of function
 */
//! NEED TO ADD ERROR CHECKING
int spi_xfer(int fd, uint8_t *tx_buffer, uint8_t tx_len, uint8_t *rx_buffer, uint8_t rx_len, struct gpiod_line_request *cs_request){
    struct spi_ioc_transfer spi_msg[1];
    memset(spi_msg, 0, sizeof(spi_msg));

    spi_msg[0].rx_buf = (unsigned long)rx_buffer;
    spi_msg[0].tx_buf = (unsigned long)tx_buffer;
    spi_msg[0].len = tx_len;


    return ioctl(fd, SPI_IOC_MESSAGE(1), spi_msg);
}

/**
 * @brief Read data as 8-bit packets from SPI Peripheral
 * 
 * @param fd Configured SPI bus instance
 * @param rx_buffer Pointer to array buffer that will store data being read
 * @param rx_len Number of 8-bit packets to read from SPI Peripheral
 * @param cs_request Pointer to structure that contains the CS instance
 * @return Iris error code indicating the success or failure of function
 */
//! NEED TO ADD ERROR CHECKING
int spi_read(int fd, uint8_t *rx_buffer, uint8_t rx_len, struct gpiod_line_request *cs_request){
    struct spi_ioc_transfer spi_msg[1];
    int val;
    memset(spi_msg, 0, sizeof(spi_msg));

    spi_msg[0].rx_buf = (unsigned long)rx_buffer;
    spi_msg[0].len = rx_len;

    cs_request = cs_toggle(cs_request, CS_RW);
    val = ioctl(fd, SPI_IOC_MESSAGE(1), spi_msg);
    cs_request = cs_toggle(cs_request, CS_MONITOR);

    return val;
}

/**
 * @brief Write data as 8-bit packets to SPI Peripheral
 * 
 * @param fd Configured SPI bus instance
 * @param tx_buffer Pointer to array buffer storing data to be written
 * @param tx_len Number of 8-bit packets to write to SPI Peripheral
 * @param cs_request Pointer to structure that contains the CS instance
 * @return Iris error code indicating the success or failure of function
 */
//! NEED TO ADD ERROR CHECKING
int spi_write(int fd, uint8_t *tx_buffer, uint16_t tx_len, struct gpiod_line_request *cs_request){
    struct spi_ioc_transfer spi_msg[1];
    memset(spi_msg, 0, sizeof(spi_msg));
    int val;

    spi_msg[0].tx_buf = (unsigned long)tx_buffer;
    spi_msg[0].len = tx_len;

    cs_request = cs_toggle(cs_request, CS_RW);
    val = ioctl(fd, SPI_IOC_MESSAGE(1), spi_msg);
    cs_request = cs_toggle(cs_request, CS_MONITOR);

    return val;
}


/**
 * @brief Configures the GPIO used for SPI CS as an input to detect any events on the line
 * 
 * @return Structure containing CS request, can be used to reconfigure the CS GPIO
 */
struct gpiod_line_request *spi_cs_setup(void){

    struct gpiod_line_request *request = NULL;
    char *gpioDev = GPIOCHIP;
    //create request for SPI CS configuration
    request = gpio_config_input_detect(GPIOCHIP, SPI_CE_N, EDGE_FALL, "IRIS SPI CS");

    return request;

}


/**
 * @brief High level function used to configure the SPI Interface
 * 
 * @return Instance of SPI Interface
 */
int spi_bus_setup(void){

    spi_config_t spi_config_test;
    int spi_dev;

    spi_config_test.mode = SPI_MODE_0;
    spi_config_test.speed = SPI_SPEED;
    spi_config_test.delay = SPI_DELAY;
    spi_config_test.bits_per_word = SPI_BITS_PER_WORD;

    char *spi_device_name = SPI_DEVICE;

    spi_dev = spi_open(spi_device_name, spi_config_test);

    return spi_dev;

}

uint8_t checksum_calc(uint8_t *tx_buffer, uint8_t bytesRead){

    uint16_t sum = 0; // Use 16 bits to prevent overflow during summation
    
    for (int i = 1; i < bytesRead + 1; i++) {
        sum += tx_buffer[i]; // Add each packet to the sum
    }

    return (uint8_t)(sum & 0xFF); // Reduce sum to 8 bits

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

    uint8_t txBuffer[SPI_FILE_BUFFER_LEN * 3];
    uint8_t checksum;
    char logBuffer[255];
    int bytesRead = 0;
    int writeNum = 0;

    log_write(LOG_INFO, "SPI-FILE-WRITE: Begin file write onto SPI bus too OBC");

    FILE *file = fopen(file_path, "rb");

    if (file) {

        bytesRead = fread(txBuffer, 1, SPI_FILE_BUFFER_LEN, file);
        do {
            writeNum = spi_write(spi_dev, txBuffer, bytesRead, spi_cs_request);
            if (writeNum != bytesRead){
                log_write(LOG_ERROR, "SPI-FILE-WRITE: Failed to write file onto SPI bus due to 'spi_write' FAIL");
                return SPI_FILE_WRITE_ERROR;
            }
            //usleep(15);

            bytesRead = fread(txBuffer, 1, SPI_FILE_BUFFER_LEN, file);

        } while (bytesRead > 0);
        fclose(file);

        log_write(LOG_INFO, "SPI-FILE-WRITE: Completed file write onto SPI bus too OBC");
        return NO_ERROR;
    }

    snprintf(logBuffer, sizeof(logBuffer), "SPI-FILE-WRITE: Failed to open file %s", file_path);
    log_write(LOG_ERROR, logBuffer);
    return SPI_FILE_WRITE_ERROR;
}



enum IRIS_ERROR spi_file_read(int spi_dev, struct gpiod_line_request **spi_cs_request, struct gpiod_edge_event_buffer **event_buffer, char *file_path){

    uint8_t rxBuffer[SPI_FILE_BUFFER_LEN];
    uint8_t checksum;
    char logBuffer[255];
    int bytesRead = 0;
    int readNum = 0;
    bool cs_edge = false;
    log_write(LOG_INFO, "SPI-FILE-WRITE: Begin file write onto SPI bus too OBC");

    FILE *file = fopen(file_path, "wb");

    if (file) {
        do {
            //cs_edge = signal_edge_detect(*spi_cs_request, *event_buffer);
            //if (cs_edge == true){
                readNum = spi_read(spi_dev, rxBuffer, 255, spi_cs_request);
                //checksum = checksum_calc(txBuffer, bytesRead);
                //txBuffer[0] = checksum;
            if (readNum != 255){
                log_write(LOG_ERROR, "SPI-FILE-WRITE: Failed to write file onto SPI bus due to 'spi_write' FAIL");
                return SPI_FILE_WRITE_ERROR;
            }
            
            bytesRead = fwrite(rxBuffer, 1, SPI_FILE_BUFFER_LEN, file);
            //}
        } while (bytesRead > 0);
        fclose(file);

        log_write(LOG_INFO, "SPI-FILE-WRITE: Completed file write onto SPI bus too OBC");
        return NO_ERROR;
    }

    snprintf(logBuffer, sizeof(logBuffer), "SPI-FILE-WRITE: Failed to open file %s", file_path);
    log_write(LOG_ERROR, logBuffer);
    return SPI_FILE_WRITE_ERROR;
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

    int buffer[9] = SPI_TEST_MSG;
    int rwVal = 0;
    int rwNum = sizeof(buffer);
    enum IRIS_ERROR error = NO_ERROR;

    log_write(LOG_INFO, "SPI-BUS-TEST: Begin SPI bus test transfer too OBC");

    rwVal = spi_write(spi_dev, buffer, rwNum, spi_cs_request);
    if (rwVal != rwNum){
        log_write(LOG_ERROR, "SPI-BUS-TEST: Failed to write test message onto SPI bus");
        error = SPI_TEST_ERROR;
    }

    clock_t start_time = clock();
    double elaspsedTime = 0;
    bool cs_edge = false;

    log_write(LOG_INFO, "SPI-BUS-TEST: Begin SPI bus test read from OBC");

    do{
        cs_edge = signal_edge_detect(spi_cs_request, event_buffer);

        if(cs_edge == true){
            rwVal = spi_read(spi_dev, buffer, (rwNum-1), spi_cs_request);
            if (rwVal != (rwNum - 1)){
                log_write(LOG_ERROR, "SPI-BUS-TEST: Failed to read test message from OBC");
                return SPI_TEST_ERROR;
            }else{
                log_write(LOG_INFO, "SPI-BUS-TEST: Completed read test message from OBC");
                return error;
            }
        }

        elaspsedTime = ((double)(clock() - start_time) / CLOCKS_PER_SEC);
    }while(elaspsedTime < SPI_TEST_TIMEOUT);

    log_write(LOG_ERROR, "SPI-BUS-TEST: TIMEOUT Failed to read test message from OBC");
    return SPI_TEST_ERROR;
}