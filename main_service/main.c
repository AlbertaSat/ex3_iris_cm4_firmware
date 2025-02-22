
//! WORK IN PROGRESS FILE CONTAINING TEST CODE AND UNFINISHED CODE


/**
 * @file main.c
 * @author Noah Klager
 * @brief Main function from controlling Theia CM4 
 *        !WORK IN PROGRESS STILL
 * 
 * @version 0.1
 * @date 2024-11-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */

 //---- Headers -----//
#include "gpio.h"
#include "spi_iris.h"
#include "main.h"
#include "logger.h"
#include "cmd_controller.h"
#include "temp_read.h"
#include "error_handler.h"
#include "current_sensor.h"
#include "usb_hub.h"
#include "timing.h"
#include "ipc_iris.h"

#include <gpiod.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <unistd.h>

void usb_hub_init(enum IRIS_ERROR *errorBuffer, uint8_t *errorCount, struct gpiod_line_request *gpio_request){

    enum IRIS_ERROR errorCheck = NO_ERROR;
    int loopCounter = 0;

    log_write(LOG_INFO, "USB-HUB-INIT: Started USB Hub Initialization");
        
    do{
        errorCheck = usb_hub_setup();
        if (errorCheck == NO_ERROR){
            errorCheck = usb_hub_func_validate(gpio_request);
        }
        loopCounter++;
    }while((errorCheck != NO_ERROR) && (loopCounter < MAX_USBHUB_INIT_ATTEMPTS));

    if(errorCheck != NO_ERROR){
        errorBuffer[(*errorCount)++] = errorCheck;
    }
}

void current_monitor_init(enum IRIS_ERROR *errorBuffer, uint8_t *errorCount){

    enum IRIS_ERROR errorCheck = NO_ERROR;
    int loopCounter = 0;
    uint8_t currAddr[3] = {CURRENT_SENSOR_ADDR_3V3, 
                           CURRENT_SENSOR_ADDR_5V,
                           CURRENT_SENSOR_ADDR_CAM};

    log_write(LOG_INFO, "CURRENT-MONITOR-INIT: Started Current Monitor Initialization");

    for (int x = 0; x < sizeof(currAddr); x++){

        do{
            errorCheck = current_setup(currAddr[x]);
            if (errorCheck == NO_ERROR){
                errorCheck = current_func_validate(currAddr[x]);
            }
            loopCounter++;
        }while((errorCheck != NO_ERROR) && (loopCounter < MAX_CURR_INIT_ATTEMPTS));
        
        loopCounter = 0;
        if(errorCheck != NO_ERROR){
            errorBuffer[(*errorCount)++] = errorCheck;
        }
    }

}

void temp_sensor_init(enum IRIS_ERROR *errorBuffer, uint8_t *errorCount){

    enum IRIS_ERROR errorCheck = NO_ERROR;
    int loopCounter = 0;
    uint8_t tempAddr[4] = {TEMP_SENSOR_1_ADDR, TEMP_SENSOR_2_ADDR,
                           TEMP_SENSOR_3_ADDR, TEMP_SENSOR_4_ADDR};

    log_write(LOG_INFO, "TEMP-SENSOR-INIT: Started Temperature Sensor Initialization");

    for (int x = 0; x < sizeof(tempAddr); x++){

        do{
            errorCheck = temp_setup(tempAddr[x]);
            if (errorCheck == NO_ERROR){
                errorCheck = temp_func_validate(tempAddr[x]);
            }
            loopCounter++;
        }while((errorCheck != NO_ERROR) && (loopCounter < MAX_TEMP_INIT_ATTEMPTS));
        
        loopCounter = 0;
        if(errorCheck != NO_ERROR){
            errorBuffer[(*errorCount)++] = errorCheck;
        }
    }

}

//! LOOK INTO A DIFFERENT SOLUTIONS FOR ADDING THE DELAY AFTER TEMP_SENSOR RESET
void temp_sensor_house_keeping(enum IRIS_ERROR *errorBuffer, uint8_t *errorCount){

    enum IRIS_ERROR errorCheck = NO_ERROR;
    int loopCounter = 0;
    uint8_t tempAddr[4] = {TEMP_SENSOR_1_ADDR, TEMP_SENSOR_2_ADDR,
                           TEMP_SENSOR_3_ADDR, TEMP_SENSOR_4_ADDR};

    log_write(LOG_INFO, "TEMP-SENSOR-HOUSE-KEEPING: Started Temperature Sensor Housekeeping");
    log_write(LOG_INFO, "TEMP-SENSOR-HOUSE-KEEPING: Started Temperature Sensor Verification");

    for (int x = 0; x < sizeof(tempAddr); x++){

        do{
            errorCheck = temp_func_validate(tempAddr[x]);            //Check for Sensor Functionality
            if (errorCheck != NO_ERROR){                             //If sensors FAILS functionality
                errorCheck = temp_reset_trig(tempAddr[x]);              //Trigger Reset of Temperature Sensor
                if(errorCheck == NO_ERROR){                             //If Reset of Temp Sensor is Successful
                    errorCheck = temp_setup(tempAddr[x]);                   //Setup Temp Sensor
                    if(errorCheck == NO_ERROR){                             //If Setup of Temp Sensor is Successful
                        usleep(250000); //Need to wait some time for sensor to reinitizalize
                        errorCheck = temp_func_validate(tempAddr[x]);           //Check for Sensor Functionality AGAIN
                    }
                }
            }
            loopCounter++;
        }while((errorCheck != NO_ERROR) && (loopCounter < MAX_TEMP_HOUSE_KEEPING_ATTEMPTS));
        
        loopCounter = 0;
        if(errorCheck != NO_ERROR){
            errorBuffer[(*errorCount)++] = errorCheck;
        }
    }

    log_write(LOG_INFO, "TEMP-SENSOR-HOUSE-KEEPING: Finished Temperature Sensor Verification");

    temperature_limit(errorBuffer, errorCount);

    log_write(LOG_INFO, "TEMP-SENSOR-HOUSE-KEEPING: Finished Temperature Sensor Housekeeping");

}


void curr_sensor_house_keeping(enum IRIS_ERROR *errorBuffer, uint8_t* errorCount){

    enum IRIS_ERROR errorCheck = NO_ERROR;
    int loopCounter = 0;
    uint8_t currAddr[3] = {CURRENT_SENSOR_ADDR_3V3, CURRENT_SENSOR_ADDR_5V,
                           CURRENT_SENSOR_ADDR_CAM};

    log_write(LOG_INFO, "CURR-SENSOR-HOUSE-KEEPING: Started Current Sensor Housekeeping");
    log_write(LOG_INFO, "CURR-SENSOR-HOUSE-KEEPING: Started Current Sensor Verification");

    for (int x = 0; x < sizeof(currAddr); x++){

        do{
            errorCheck = current_func_validate(currAddr[x]);            //Check for Sensor Functionality
            if (errorCheck != NO_ERROR){                                //If sensors FAILS functionality
                errorCheck = current_monitor_reset_trig(currAddr[x]);      //Trigger Reset of Current Sensor
                if(errorCheck == NO_ERROR){                                //If Reset of Current Sensor is Successful
                    errorCheck = current_setup(currAddr[x]);                   //Setup Current Sensor
                    if(errorCheck == NO_ERROR){                                //If Setup of Current Sensor is Successful
                        errorCheck = current_func_validate(currAddr[x]);           //Check for Sensor Functionality AGAIN
                    }
                }
            }
            loopCounter++;
        }while((errorCheck != NO_ERROR) && (loopCounter < MAX_CURR_HOUSE_KEEPING_ATTEMPTS));
        
        loopCounter = 0;
        if(errorCheck != NO_ERROR){
            errorBuffer[(*errorCount)++] = errorCheck;
        }
    }

    log_write(LOG_INFO, "CURR-SENSOR-HOUSE-KEEPING: Finished Current Sensor Verification");

    current_limit(errorBuffer, errorCount);

    log_write(LOG_INFO, "CURR-SENSOR-HOUSE-KEEPING: Finished Current Sensor Housekeeping");

}


struct gpiod_line_request *gpio_init(enum IRIS_ERROR *errorBuffer, uint8_t *errorCount) {

    struct gpiod_line_request *request = NULL;
    char *gpioDev = GPIOCHIP;

    log_write(LOG_INFO, "GPIO-INIT: Start setup of GPIO interface");

    int loopCounter = 0;

    unsigned int numOffsets               = BOOTNUMOFFSET;
    unsigned int offset[BOOTNUMOFFSET]    = BOOTGPIOOFFSET;
    unsigned int dir[BOOTNUMOFFSET]       = BOOTGPIODIR;
    unsigned int outputVal[BOOTNUMOFFSET] = BOOTGPIOVAL;
    unsigned int drive[BOOTNUMOFFSET]     = BOOTGPIODRV;
    unsigned int bias[BOOTNUMOFFSET]      = BOOTGPIOBIAS;

    do{
        request = gpio_config_group(gpioDev, numOffsets, offset, dir, outputVal, drive, bias, "IRIS CONTROL");
        loopCounter++;
    }while((request == NULL) && (loopCounter < MAX_GPIO_INIT_ATTEMPTS));

    if(request == NULL){
        errorBuffer[(*errorCount)++] = GPIO_SETUP_ERROR;
    }

    log_write(LOG_INFO, "GPIO-INIT: Completed setup attempt of GPIO interface");
    return request;
}

void system_init(enum IRIS_ERROR *errorBuffer, uint8_t *errorCount, struct gpiod_line_request *gpio_request) {

    log_write(LOG_INFO, "SYSTEM-INIT: Start System intialization process");

    temp_sensor_init(errorBuffer, errorCount);
    current_monitor_init(errorBuffer, errorCount);
    //usb_hub_init(errorBuffer, gpio_request);
    //watchdog_setup();

    log_write(LOG_INFO, "SYSTEM-INIT: Finished System intialization process");

}

bool signal_edge_detect(struct gpiod_line_request *request, struct gpiod_edge_event_buffer *event_buffer){
    
    bool event = false;
    int event_amt = EDGE_EVENT_BUFF_SIZE;

    // Check for signal event
    event = gpiod_line_request_wait_edge_events(request, 0); 
    
    // Dont need to clear event buffer since no event occured
    if(event == false){
        return false;
    }

    // Clear event buffer by reading over it
    //*Limit MAX_ITERATIONS so it can't loop forever
    //! IF THE NUMBER OF EVENTS EQUALS THE BUFFER SIZE (EDGE_EVENT_BUFF_SIZE) THAN ON NEXT gpiod_lione_request_read_edge_events IT WILL GET STUCK
    for(int index = 0; (index < MAX_ITERATIONS) && (event_amt >= EDGE_EVENT_BUFF_SIZE); index++){
        event_amt = gpiod_line_request_read_edge_events(request, event_buffer, EDGE_EVENT_BUFF_SIZE);
    }

    return true;
}

IRIS_ERROR spi_cmd_loop(int spi_dev, 
                  struct gpiod_line_request *spi_cs_request, 
                  struct gpiod_edge_event_buffer *event_buffer) {

    bool cs_edge = false;
    uint8_t rx_buffer[SPI_RX_LEN] = {};
    uint8_t cmd = 0;
    uint8_t arg[SPI_RX_LEN] = {0};
    int narg = 0;
    IRIS_ERROR error = NO_ERROR;

    for(int index = 0; (index < SPI_MAX_LOOP_AMT) && (cs_edge == false); index++){
        cs_edge = signal_edge_detect(spi_cs_request, event_buffer);

        if (cs_edge == true){
            error = spi_read(spi_dev, rx_buffer, SPI_RX_LEN, spi_cs_request);
            if(error != NO_ERROR){
                return error;
            }
            narg = cmd_extracter(&cmd, arg, rx_buffer, SPI_RX_LEN);
            error = cmd_center(cmd, arg, narg, spi_dev, &spi_cs_request);
            return error;
        }
    }
    return error;
}

void system_house_keeping(enum IRIS_ERROR *errorBuffer, uint8_t *errorCount, struct gpiod_line_request *gpio_request){

    // Temperature Sensor 
    temp_sensor_house_keeping(errorBuffer, errorCount);

    // Current Sensor 
    curr_sensor_house_keeping(errorBuffer, errorCount);

    // USB Hub House Keeping
    //usb_hub_func_validate(errorBuffer, errorCount, gpio_request);

    // GPIO House Keeping
    // errorCode = gpio_config_validate();

}

void led_toggle(uint8_t *led_status, struct gpiod_line_request *gpio_request){

    if(*led_status){
        *led_status = 0;
        gpiod_line_request_set_value(gpio_request, LED_ACTI, GPIOD_LINE_VALUE_INACTIVE);
    }else{
        *led_status = 1;
        gpiod_line_request_set_value(gpio_request, LED_ACTI, GPIOD_LINE_VALUE_ACTIVE);
    }
}

//! ADD CLEANUP FUNCTION
void clean_up(void){


}
/////////////////////////////////////////////////////////////////////////


#ifdef ONE_SERVICE

//! THE MAIN MAIN FUNCTION
int main(void){
    struct gpiod_line_request *gpio_request = NULL;
    struct gpiod_line_request *spi_cs_request = NULL;
    struct gpiod_edge_event_buffer *event_buffer = NULL;

    enum IRIS_ERROR spiInitError = NO_ERROR;
    enum IRIS_ERROR spiError = NO_ERROR;

    uint8_t errorCount = 0;
    enum IRIS_ERROR errorBuffer[ERROR_BUFFER_SIZE] = {NO_ERROR};
    
    uint8_t led_status = 1;
    int spi_dev = 0;
    int current_time = get_time_seconds();

    atexit(clean_up);

    log_file_init();

    spiInitError = spi_init(&spi_dev, &spi_cs_request, &event_buffer);
    gpio_request = gpio_init(errorBuffer, &errorCount);

    // System Init
    system_init(errorBuffer, &errorCount, gpio_request);

    while(true){

        if (spiInitError != NO_ERROR) {
            spiInitError = spi_init(&spi_dev, &spi_cs_request, &event_buffer);
        }else if(spiError != NO_ERROR){

            spiInitError = spi_reinit(&spi_dev, &spi_cs_request, &event_buffer);
            spiError = NO_ERROR;
        }else{

            spiError = spi_cmd_loop(spi_dev, spi_cs_request, event_buffer);

            if (get_time_seconds() > current_time + HOUSE_KEEPING_DELAY_S){
                current_time = get_time_seconds();
                led_toggle(&led_status, gpio_request);
                system_house_keeping(errorBuffer, &errorCount, gpio_request);
                spiError = iris_error_transfer(spi_dev, spi_cs_request, errorBuffer, &errorCount);
            }

        }
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#else
//! THE MAIN MAIN FUNCTION
int main(void){
    struct gpiod_line_request *gpio_request = NULL;

    uint8_t errorCount = 0;
    enum IRIS_ERROR errorBuffer[ERROR_BUFFER_SIZE] = {NO_ERROR};

    uint8_t led_status = 1;

    key_t IPCKey = 0;
    int ipcMsgID = 0;
    enum IRIS_ERROR ipcInitError = NO_ERROR;

    int current_time = get_time_seconds();

    log_file_init();

    gpio_request = gpio_init(errorBuffer, &errorCount);

    // System Init
    system_init(errorBuffer, &errorCount, gpio_request);
    
    ipcInitError = ipc_setup(&IPCKey, &ipcMsgID);


    // while (1) {
    //             message.msg_type = 1;
    //             snprintf(message.msg_text, MSG_SIZE, "Hello from Service 1!");
                
    //             if (msgsnd(msgid, &message, sizeof(message.msg_text), 0) == -1) {
    //                 perror("msgsnd failed");
    //                 break; // Handle error
    //             }
        
    //             printf("Service 1 sent: %s\n", message.msg_text);
    //             sleep(1); // Simulate some work
    //         }
    int x = 255;
    int y = 0;

    while(true){

        if (ipcInitError != NO_ERROR) {
            ipcInitError = ipc_setup(&IPCKey, &ipcMsgID);
        }else{

            if (get_time_seconds() > current_time + HOUSE_KEEPING_DELAY_S){
                current_time = get_time_seconds();
                led_toggle(&led_status, gpio_request);
                system_house_keeping(errorBuffer, &errorCount, gpio_request);
                iris_error_transfer_spi_service(ipcMsgID, errorBuffer, &errorCount);

            }
        }
    }

}

#endif
///////////////////////////////////////////////////////////////////////////////////////////


// void main_secondary(void){
//     struct gpiod_line_request *gpio_request = NULL;
//     struct gpiod_line_request *spi_cs_request = NULL;
//     struct gpiod_edge_event_buffer *event_buffer = NULL;
    
//     struct gpiod_line_request *clk_request = NULL;

//     enum IRIS_ERROR spiInitError = NO_ERROR;
//     enum IRIS_ERROR spiError = NO_ERROR;

//     uint8_t errorCount = 0;
//     enum IRIS_ERROR errorBuffer[ERROR_BUFFER_SIZE] = {NO_ERROR};
    
//     uint8_t errorCheck = 0;
//     uint8_t led_status = 0;

//     int spi_dev = 0;
//     char arg[5][100] = {0};
//     char cmd[4096] = {170, 85, 170, 85, 170, 85, 170, 85,};
//     pthread_t id; 
//     int state = NULL;
//     char *file_path = "/home/iris/ex3_iris_cm4_firmware/Photo_Package.tar.bz2";
//     //char *file_path = "/home/iris/ex3_iris_cm4_firmware/IMG_20250219_214450.dng.bz2";
//     //char *file_path = "/home/iris/Iris_Firmware/Testing_What.txt";
//     //char *file_path = "/home/iris/Iris_Firmware/Testing_Main.txt";
//     //! ADD ERROR RESPONSE TO FAILURE TO SETUP SPI_INTERFACE
//     //pthread_create(&id, NULL, printNumber, &arg);
    
//     log_file_init();
//     key_t key = ftok("msgfile", 65); // Create a unique key
//     int msgid = msgget(key, 0660 | IPC_CREAT); // Create message queue with restricted permissions

//     if (msgid == -1) {
//         perror("msgget failed");
//         exit(EXIT_FAILURE);
//     }

//     struct msg_buffer message;

//         //spiInitError = spi_init(&spi_dev, &spi_cs_request, &event_buffer);
//         //gpio_request = gpio_init(errorBuffer, &errorCount);

//     // errorCheck = gpiod_line_request_set_value(gpio_request, PWR_5V_CAM_EN, GPIOD_LINE_VALUE_ACTIVE);
//     // if (errorCheck == -1){
//     //     log_write(LOG_ERROR, "USB-HUB-RESET: Failed to deassert reset GPIO USB Hub");
//     //     return USB_HUB_RESET_ERROR;
//     // }

//     IRIS_ERROR temp = 0;
//     // System Init
//     //usb_hub_func_TESTING(gpio_request);
//         //system_init(errorBuffer, &errorCount, gpio_request);

//     //usb_hub_func_TESTING(gpio_request);
//     //! MAYBE ADD RESET FOR COLD + HOT
//     //! MAYBE ADD AN ERROR STATE WHICH WAIT X AMOUNT OF TIME UNTIL A COMMAND IS RECEIVED FROM OC BEFORE DOING A RESTARBT
//     //! ADD WATCHDOG

//     // while (1) {
//     //     message.msg_type = 1;
//     //     snprintf(message.msg_text, MSG_SIZE, "Hello from Service 1!");
        
//     //     if (msgsnd(msgid, &message, sizeof(message.msg_text), 0) == -1) {
//     //         perror("msgsnd failed");
//     //         break; // Handle error
//     //     }

//     //     printf("Service 1 sent: %s\n", message.msg_text);
//     //     sleep(1); // Simulate some work
//     // }

//     //double the_time = get_time_seconds();

//     // while(true){

//     //     if (spiInitError != NO_ERROR) {
//     //         //! MAYBE ADD SOME WATCHDOG ON IRIS THAT RESET ITSELF
//     //         //* If SPI BUS doesn't initialize than we will continue to try until it
//     //         //* either succeeds or OBC resets IRIS
//     //         spiInitError = spi_init(&spi_dev, &spi_cs_request, &event_buffer);
//     //     }else if(spiError != NO_ERROR){

//     //         spiInitError = spi_reinit(&spi_dev, &spi_cs_request, &event_buffer);
//     //         spiError = NO_ERROR;
//     //     }else{
//     //         //time_sync(spi_cs_request, event_buffer);
//     //         //read_bus_voltage(CURRENT_SENSOR_ADDR_3V3);
//     //         // read_current(CURRENT_SENSOR_ADDR_3V3);
//     //         // read_power(CURRENT_SENSOR_ADDR_3V3);

//     //         // read_bus_voltage(CURRENT_SENSOR_ADDR_5V);
//     //         // read_current(CURRENT_SENSOR_ADDR_5V);
//     //         // read_power(CURRENT_SENSOR_ADDR_5V);
//     //         // read_pk_power(CURRENT_SENSOR_ADDR_5V);

//     //         // read_bus_voltage(CURRENT_SENSOR_ADDR_CAM);
//     //         // read_current(CURRENT_SENSOR_ADDR_CAM);
//     //         // read_power(CURRENT_SENSOR_ADDR_CAM);
//     //         // if(get_time_seconds() - the_time > 10){
//     //         //     printf("\n%f\n", the_time);
//     //         //     set_time_seconds(1000);
//     //         //     the_time = get_time_seconds();
//     //         //     printf("\n%f\n", the_time);
//     //         //     while(true){

//     //         //     }
//     //         // }

//     //         // if(errorCount > 65530){
//     //         //     printf("WHAT");
//     //         // }

//     //         // current_limit(errorBuffer, &errorCount);
//     //         //spi_file_write(spi_dev, &spi_cs_request, file_path, &event_buffer);
//     //         //spi_read(spi_dev, cmd, 255, spi_cs_request);
//     //         setup_spi_interface();
//     //         //spi_write(spi_dev, cmd, 16, spi_cs_request);
//     //         //system_house_keeping(errorBuffer, gpio_request);
//     //         //iris_error_transfer(spi_dev, spi_cs_request, event_buffer, errorBuffer);
//     //     }
//     // }

// }


// void main(){
//     main_secondary();
// }