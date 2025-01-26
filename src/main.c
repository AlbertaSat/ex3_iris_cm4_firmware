#include "gpio.h"
#include "spi_iris.h"
#include "main.h"
#include "logger.h"
#include "cmd_controller.h"
#include "temp_read.h"
#include "error_handler.h"
#include "current_sensor.h"
#include "usb_hub.h"
#include "watchdog.h"

#include <linux/spi/spidev.h>
#include <pthread.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <gpiod.h>
#include <unistd.h>
#include <errno.h>

#include <stdlib.h>
#include <string.h>

#define MAX_ITERATIONS 5
#define EDGE_EVENT_BUFF_SIZE 5
#define SPI_MAX_LOOP_AMT 65535
#define SPI_RX_LEN 255
#define MAX_TEMP_INIT_ATTEMPTS 5
#define MAX_CURR_INIT_ATTEMPTS 5
#define MAX_SPI_INIT_ATTEMPTS  5
#define MAX_GPIO_INIT_ATTEMPTS 5
#define MAX_TEMP_HOUSE_KEEPING_ATTEMPTS 5
#define MAX_CURR_HOUSE_KEEPING_ATTEMPTS 5
#define MAX_USBHUB_INIT_ATTEMPTS 5
#define SPI_ERROR_TRANSFER_CMD 100


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


// void main(){
//     printf("HELLO WORLD!");
//     struct gpiod_request_config *req_cfg = NULL;
// 	struct gpiod_line_request *request = NULL;
// 	struct gpiod_line_settings *settings= NULL;
// 	struct gpiod_line_config *line_cfg= NULL;
// 	struct gpiod_chip *chip= NULL;
// 	int ret;

// 	settings = gpiod_line_settings_new();

//     log_write(LOG_ERROR, "WHAT IS UP1");
//     log_write(LOG_WARNING, "WHAT IS UP2");
//     log_write(LOG_DEBUG, "WHAT IS UP3");
//     log_write(LOG_INFO, "WHAT IS UP4");


//     int val = 0;
//     int spiDev;

//     spi_config_t spi_config_test;
//     struct gpiod_line_request *spiCSRequest = NULL;


//     //log_write(1, "WHAT IS UP");
//    // spiCSRequest = spi_cs_setup();


// // //     int spiDev;

// // //     spi_config_t spi_config_test;
// // //     struct gpiod_line_request *spiCSRequest = NULL;


// //     //log_write(1, "WHAT IS UP");
//     spiCSRequest = spi_cs_setup();
//     spi_config_test.mode= SPI_MODE_0 ;
//     spi_config_test.speed=1000000;
//     spi_config_test.delay=0;
//     spi_config_test.bits_per_word=8;

//     char *device = "/dev/spidev0.0";


//     spiDev = spi_open(device, spi_config_test);
//     uint8_t rx_buffer[10];
//     uint8_t tx_buffer[10] = {0,1,2,3,4,5,6,7,8,9};
//     uint8_t rx_len = 10;

//     // !CAN GET "main: line-request.c:257: gpiod_line_request_reconfigure_lines: Assertion `request' failed."
//     // !WHERE THE SPI LINES ARE BEING "CONTROLLED" BY SOMTHING ELSE NEED TO REBOOT BOARD OR FIGURE OUT A BETTER
//     // !WAY TOO RELEASE THE GPIO
//     while (1){
//         spi_read(spiDev, rx_buffer, rx_len, spiCSRequest);
//         //spi_write(spiDev, tx_buffer, 8, spiCSRequest);
//     }
//     return 0;

// }
// void main(int argc, char *argv[]) {
//     FILE *logfile = fopen("/home/iris/myprogram.log", "w");
//     if (logfile) {
//         fprintf(logfile, "Program started with arguments:\n");
//         for (int i = 0; i < argc; i++) {
//             fprintf(logfile, "argv[%d] = %s\n", i, argv[i]);
//         }

//         while (1) {
//             fprintf(logfile, "WORKING %d...\n", ++argc);
//             fflush(logfile);  // Make sure the output is written to the file immediately
//             sleep(3);
//         }
//         fclose(logfile);
//     }
// }


////////////////////////////////////////////////////////////////////////////////////



void usb_hub_init(enum IRIS_ERROR *errorBuffer, struct gpiod_line_request *gpio_request){

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
        errorBuffer[(1 + errorBuffer[0]++)] = errorCheck;
    }
}

void current_monitor_init(enum IRIS_ERROR *errorBuffer){

    enum IRIS_ERROR errorCheck = NO_ERROR;
    int loopCounter = 0;
    uint8_t currAddr[3] = {CURRENT_SENSOR_ADDR_3V3, 
                           CURRENT_SENSOR_ADDR_5V,
                           CURRENT_SENSOR_ADDR_CAM};

    log_write(LOG_INFO, "CURRENT-MONITOR-INIT: Started Current Monitor Initialization");

    for (int x = 0; x < sizeof(currAddr); x++){
        errorCheck = NO_ERROR;
        do{
            errorCheck = current_setup(currAddr[x]);
            if (errorCheck == NO_ERROR){
                errorCheck = current_func_validate(currAddr[x]);
            }
            loopCounter++;
        }while((errorCheck != NO_ERROR) && (loopCounter < MAX_CURR_INIT_ATTEMPTS));
        
        if(errorCheck != NO_ERROR){
            errorBuffer[(1 + errorBuffer[0]++)] = errorCheck;
        }
    }

}

void temp_sensor_init(enum IRIS_ERROR *errorBuffer){

    enum IRIS_ERROR errorCheck = NO_ERROR;
    int loopCounter = 0;
    uint8_t tempAddr[4] = {TEMP_SENSOR_1_ADDR, TEMP_SENSOR_2_ADDR,
                           TEMP_SENSOR_3_ADDR, TEMP_SENSOR_4_ADDR};

    log_write(LOG_INFO, "TEMP-SENSOR-INIT: Started Temperature Sensor Initialization");

    for (int x = 0; x < sizeof(tempAddr); x++){
        errorCheck = NO_ERROR;
        do{
            errorCheck = temp_setup(tempAddr[x]);
            if (errorCheck == NO_ERROR){
                errorCheck = temp_func_validate(tempAddr[x]);
            }
            loopCounter++;
        }while((errorCheck != NO_ERROR) && (loopCounter < MAX_TEMP_INIT_ATTEMPTS));
        
        if(errorCheck != NO_ERROR){
            errorBuffer[(1 + errorBuffer[0]++)] = errorCheck;
        }
    }

}


void temp_sensor_house_keeping(enum IRIS_ERROR *errorBuffer){

    enum IRIS_ERROR errorCheck = NO_ERROR;
    int loopCounter = 0;
    uint8_t tempAddr[4] = {TEMP_SENSOR_1_ADDR, TEMP_SENSOR_2_ADDR,
                           TEMP_SENSOR_3_ADDR, TEMP_SENSOR_4_ADDR};

    log_write(LOG_INFO, "TEMP-SENSOR-HOUSE-KEEPING: Started Temperature Sensor Verification");

    for (int x = 0; x < sizeof(tempAddr); x++){
        errorCheck = NO_ERROR;
        do{
            errorCheck = temp_func_validate(tempAddr[x]);
            if (errorCheck != NO_ERROR){
                errorCheck = temp_reset_trig(tempAddr[x]);
                if(errorCheck == NO_ERROR){
                    errorCheck = temp_setup(tempAddr[x]);
                }
            }
            loopCounter++;
        }while((errorCheck != NO_ERROR) && (loopCounter < MAX_TEMP_HOUSE_KEEPING_ATTEMPTS));
        
        if(errorCheck != NO_ERROR){
            errorBuffer[(1 + errorBuffer[0]++)] = errorCheck;
        }
    }

    temperature_limit(errorBuffer);
}


void curr_sensor_house_keeping(enum IRIS_ERROR *errorBuffer){

    enum IRIS_ERROR errorCheck = NO_ERROR;
    int loopCounter = 0;
    uint8_t currAddr[3] = {CURRENT_SENSOR_ADDR_3V3, CURRENT_SENSOR_ADDR_5V,
                           CURRENT_SENSOR_ADDR_CAM};

    log_write(LOG_INFO, "CURR-SENSOR-HOUSE-KEEPING: Started Current Sensor Verification");

    for (int x = 0; x < sizeof(currAddr); x++){
        errorCheck = NO_ERROR;
        do{
            errorCheck = current_func_validate(currAddr[x]);
            if (errorCheck != NO_ERROR){
                errorCheck = current_monitor_reset_trig(currAddr[x]);
                if(errorCheck == NO_ERROR){
                    errorCheck = current_setup(currAddr[x]);
                }
            }
            loopCounter++;
        }while((errorCheck != NO_ERROR) && (loopCounter < MAX_CURR_HOUSE_KEEPING_ATTEMPTS));
        
        if(errorCheck != NO_ERROR){
            errorBuffer[(1 + errorBuffer[0]++)] = errorCheck;
        }
    }

    log_write(LOG_INFO, "CURR-SENSOR-HOUSE-KEEPING: Finished Current Sensor Verification");

    //current_limit(errorBuffer);

}


struct gpiod_line_request *gpio_init(enum IRIS_ERROR *errorBuffer) {

    struct gpiod_line_request *request = NULL;
    char *gpioDev = GPIOCHIP;

    log_write(LOG_INFO, "GPIO-INIT: Start setup of GPIO interface");

    int loopCounter = 0;

    int numOffsets               = BOOTNUMOFFSET;
    int offset[BOOTNUMOFFSET]    = BOOTGPIOOFFSET;
    int dir[BOOTNUMOFFSET]       = BOOTGPIODIR;
    int outputVal[BOOTNUMOFFSET] = BOOTGPIOVAL;
    int drive[BOOTNUMOFFSET]     = BOOTGPIODRV;
    int bias[BOOTNUMOFFSET]      = BOOTGPIOBIAS;

    do{
        request = gpio_config_group(gpioDev, numOffsets, offset, dir, outputVal, drive, bias, "IRIS CONTROL");
    }while((request == NULL) && (loopCounter < MAX_GPIO_INIT_ATTEMPTS));

    if(request == NULL){
        errorBuffer[(1 + errorBuffer[0]++)] = GPIO_SETUP_ERROR;
    }

    log_write(LOG_INFO, "GPIO-INIT: Completed setup attempt of GPIO interface");
    return request;
}

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
            errorCheck = SPI_SETUP_ERROR;
            continue;     
        }
        *event_buffer = gpiod_edge_event_buffer_new(EDGE_EVENT_BUFF_SIZE);
        if (*event_buffer == NULL){ 
            errorCheck = SPI_SETUP_ERROR;
            continue;
        }
        errorCheck = NO_ERROR;
    }while((errorCheck == SPI_SETUP_ERROR) && (loopCounter < MAX_SPI_INIT_ATTEMPTS));
    
    loopCounter = 0;
    // if(errorCheck == NO_ERROR){
    //     do{
    //         errorCheck = spi_bus_test(spi_dev, spi_cs_request, event_buffer);
    //         loopCounter++;
    //     }while((errorCheck == SPI_TEST_ERROR) && (loopCounter < MAX_SPI_INIT_ATTEMPTS));
    // }

    log_write(LOG_INFO, "SPI-INIT: Finished setup attempt of SPI interface with OBC");

    return errorCheck;
}

void system_init(enum IRIS_ERROR *errorBuffer, struct gpiod_line_request *gpio_request) {

    
    //! ADD FUNCTIONS TO VERIFY THAT INTERFACE / DEVICES ARE WORKING PROPERLY
    temp_sensor_init(errorBuffer);
    current_monitor_init(errorBuffer);
    usb_hub_init(errorBuffer, gpio_request);
    //watchdog_setup();


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

void spi_cmd_loop(int spi_dev, 
                  struct gpiod_line_request *spi_cs_request, 
                  struct gpiod_edge_event_buffer *event_buffer) {
    
    bool cs_edge = false;
    uint8_t rx_buffer[SPI_RX_LEN] = {};
    char cmd[SPI_RX_LEN] = {0};
    char arg[5][100] = {0};
    int num_arg = 0;
    for(int index = 0; (index < SPI_MAX_LOOP_AMT) && (cs_edge == false); index++){
        cs_edge = signal_edge_detect(spi_cs_request, event_buffer);

        if (cs_edge == true){
            spi_read(spi_dev, rx_buffer, SPI_RX_LEN, spi_cs_request);
            num_arg = cmd_extracter(cmd, arg, rx_buffer, SPI_RX_LEN);
            //cmd = rx_buffer;
            //cmd_extracter(cmd, rx_buffer, SPI_RX_LEN);
            //cmd_center();
        }
    }
    
}

void system_house_keeping(enum IRIS_ERROR *errorBuffer, struct gpiod_line_request *gpio_request){

    int errorCode;
    // Temperature Sensor 
    temp_sensor_house_keeping(errorBuffer);

    // Current Sensor 
    curr_sensor_house_keeping(errorBuffer);

    // USB Hub House Keeping
    //usb_hub_func_validate(errorBuffer, gpio_request);

    // GPIO House Keeping
    // errorCode = gpio_config_validate();

}

void iris_error_transfer(int spi_dev, 
                         struct gpiod_line_request *spi_cs_request, 
                         struct gpiod_edge_event_buffer *event_buffer,
                         enum IRIS_ERROR *errorBuffer){

    bool cs_edge = false;
    int errorAmt = errorBuffer[0];
    int spiError = 0;

    cs_edge = signal_edge_detect(spi_cs_request, event_buffer);

    if (cs_edge == false){
        errorBuffer[0] = SPI_ERROR_TRANSFER_CMD;
        spi_write(spi_dev, errorBuffer, errorAmt  + 1, spi_cs_request);
        if(spiError == -1){
            return 0; //ADD ERROR CODE
        }
    }
}

void printNumber(){

    int x = 0;
    while(true){
        printf("TEST: %d\n", x++);
    }
}


//! THE MAIN MAIN FUNCTION
void main(void){
    struct gpiod_line_request *gpio_request = NULL;
    struct gpiod_line_request *spi_cs_request = NULL;
    struct gpiod_edge_event_buffer *event_buffer = NULL;
    
    struct gpiod_line_request *clk_request = NULL;

    enum IRIS_ERROR spiInitError = NO_ERROR;
    enum IRIS_ERROR errorBuffer[256] = {NO_ERROR};
    
    int spi_dev = 0;
    char arg[5][100] = {0};
    char cmd[100] = {0};
    pthread_t id; 
    int state = NULL;
    char *file_path = "/home/iris/Iris_Firmware/PXL_20250115_191406313.RAW-02.ORIGINAL.dng";
    //char *file_path = "/home/iris/Iris_Firmware/Testing_What.txt";
    //char *file_path = "/home/iris/Iris_Firmware/Testing_Main.txt";
    //! ADD ERROR RESPONSE TO FAILURE TO SETUP SPI_INTERFACE
    //pthread_create(&id, NULL, printNumber, &arg);
    
    spiInitError = spi_init(&spi_dev, &spi_cs_request, &event_buffer);
    gpio_request = gpio_init(event_buffer);

    // System Init
    //system_init(errorBuffer, gpio_request);

    //! MAYBE ADD RESET FOR COLD + HOT
    //! MAYBE ADD AN ERROR STATE WHICH WAIT X AMOUNT OF TIME UNTIL A COMMAND IS RECEIVED FROM OC BEFORE DOING A RESTARBT
    //! ADD WATCHDOG
    while(true){

        if (spiInitError != NO_ERROR) {
            //! MAYBE ADD SOME WATCHDOG ON IRIS THAT RESET ITSELF
            //* If SPI BUS doesn't initialize than we will continue to try until it
            //* either succeeds or OBC resets IRIS
            spiInitError = spi_init(&spi_dev, &spi_cs_request, &event_buffer);
        }else{
            spi_cmd_loop(spi_dev, spi_cs_request, event_buffer);
            //spi_read(spi_dev, cmd, 255, spi_cs_request);
            //system_house_keeping(errorBuffer, gpio_request);
            //iris_error_transfer(spi_dev, spi_cs_request, event_buffer, errorBuffer);
        }
    }

}



///////////////////////////////////////////////////////////////////////////////////////////

// void main(void){
//     struct gpiod_line_request *gpio_request = NULL;
//     struct gpiod_line_request *spi_cs_request = NULL;
//     struct gpiod_edge_event_buffer *event_buffer = NULL;
    
//     struct gpiod_line_request *clk_request = NULL;

//     enum IRIS_ERROR spiInitError = NO_ERROR;
//     enum IRIS_ERROR errorBuffer[256] = {NO_ERROR};
    
//     int spi_dev = 0;
//     char arg[5][100] = {0};
//     char cmd[100] = {0};
    
//     char *file_path = "/home/iris/Iris_Firmware/PXL_20250115_191406313.RAW-02.ORIGINAL.dng";
//     //char *file_path = "/home/iris/Iris_Firmware/Testing_What.txt";
//     //char *file_path = "/home/iris/Iris_Firmware/Testing_Main.txt";
//     //! ADD ERROR RESPONSE TO FAILURE TO SETUP SPI_INTERFACE
//     spiInitError = spi_init(&spi_dev, &spi_cs_request, &event_buffer);
//     //gpio_request = gpio_init(event_buffer);

//     // System Init
//     //* system_init(errorBuffer);
//     //! MAYBE ADD RESET FOR COLD + HOT    
//     //! MAYBE ADD AN ERROR STATE WHICH WAIT X AMOUNT OF TIME UNTIL A COMMAND IS RECEIVED FROM OC BEFORE DOING A RESTARBT
//     //! ADD WATCHDOG
//     while(true){

//         if (spiInitError != NO_ERROR) {
//             //! MAYBE ADD SOME WATCHDOG ON IRIS THAT RESET ITSELF
//             //* If SPI BUS doesn't initialize than we will continue to try until it
//             //* either succeeds or OBC resets IRIS
//             spiInitError = spi_init(&spi_dev, &spi_cs_request, &event_buffer);
//         }else{
//             //spi_cmd_loop(spi_dev, spi_cs_request, event_buffer);
//             spi_file_write(spi_dev, &spi_cs_request, file_path, &event_buffer);
//             //spi_file_read(spi_dev, &spi_cs_request, &event_buffer, file_path);
//             //system_house_keeping();
//             //iris_error_transfer();
//         }
//     }

// }

////////////////////////////////////////////////////////////////////////////////////
// void main(void){

//     struct gpiod_line_request *gpio_request = NULL;
//     struct gpiod_line_request *spi_cs_request = NULL;
//     struct gpiod_edge_event_buffer *event_buffer = NULL;
//     int spi_dev;
//     int y = 0;
//     char arg[5][100] = {0};
//     char cmd[100] = {0};

//     spi_dev = spi_setup();
//     spi_cs_request = spi_cs_setup();
//     event_buffer = gpiod_edge_event_buffer_new(EDGE_EVENT_BUFF_SIZE);

//     uint8_t tx_buffer[255];

//     char *file_path = "/home/iris/Iris_Firmware/spi_write_test.txt";
//     spi_file_write(spi_dev, spi_cs_request, file_path);
    
//     //Init Interfaces
//     gpio_request = gpio_setup();
    
    

//     //temp_setup();
//     //current_sensor_setup();
//     //hub_setup();

    

//     while(true){


//         //spi_cmd_loop(spi_dev, spi_cs_request, event_buffer);
//         //spi_write(spi_dev, tx_buffer, spi_cs_request);
//         log_write(LOG_ERROR, "LOOPED");

//     }

// }

// void main(void){

//     int check = 0;
//     const char *chip_path = "/dev/gpiochip0";
//     const char *consumer = "IRIS_SPI";

//     struct gpiod_edge_event_buffer *event_buffer;
// 	struct gpiod_line_request *request;
// 	struct gpiod_edge_event *event;
// 	int i, ret, event_buf_size;
//     bool detect = false;
//     int offset = 8;
//     int index = 0;
//     int amt = 0;
//     int val = 0;
//     int spiDev;


//     //check = gpio_config_port(chip_path, offset, 0, 1, consumer);
//     request = gpio_config_input_detect(chip_path, offset, EDGE_BOTH, consumer);
//     event_buf_size = 10;
// 	event_buffer = gpiod_edge_event_buffer_new(event_buf_size);

//     FILE *logfile = fopen("/home/iris/myprogram.log", "w");

//     while(true){
//         check = gpiod_line_request_wait_edge_events(request, 0);
//          if(check){
//             fprintf(logfile, "EVENT DETECT %d:\n", ++index);
//             amt = gpiod_line_request_read_edge_events(request, event_buffer, 10);
//             val = gpiod_line_request_get_value(request, offset);
//             detect = true;
//         }else{
//             detect = false;
//         }
//     }
//     fclose(logfile);

// }













// void main() {

//     int check = 0;
//     const char *chip_path = "/dev/gpiochip0";
//     const char *consumer = "IRIS_SPI";

//     struct gpiod_edge_event_buffer *event_buffer;
// 	struct gpiod_line_request *request;
// 	struct gpiod_edge_event *event;
// 	int i, ret, event_buf_size;
//     bool detect = false;
//     int offset = 4;
//     int index = 0;
//     int amt = 0;
//     int val = 0;
//     int spiDev;


//     spi_config_t spi_config_test;

//     spi_config_test.mode=0;
//     spi_config_test.speed=1000000;
//     spi_config_test.delay=0;
//     spi_config_test.bits_per_word=8;

//     char *device = "/dev/spidev0.0";


//     //check = gpio_config_port(chip_path, offset, 0, 1, consumer);
//     request = gpio_config_input_detect(chip_path, offset, 1, consumer);
//     event_buf_size = 10;
// 	event_buffer = gpiod_edge_event_buffer_new(event_buf_size);

//     FILE *logfile = fopen("/home/iris/myprogram.log", "w");

//     while(!detect){
//         check = gpiod_line_request_wait_edge_events(request, 0);
//          if(check){
//             fprintf(logfile, "EVENT DETECT %d:\n", ++index);
//             amt = gpiod_line_request_read_edge_events(request, event_buffer, 10);
//             val = gpiod_line_request_get_value(request, offset);
//             detect = true;
//         }else{
//             detect = false;
//         }
//     }
//     fclose(logfile);

//     spiDev = spi_open(device, spi_config_test);
//     val = gpiod_line_request_get_value(request, offset);
//     uint8_t *rx_buffer;
//     uint8_t rx_len = 10;

//     if (!val){
//         while(!(val = gpiod_line_request_get_value(request, offset))){
//             //spi_read(spiDev, rx_buffer, rx_len);

//         }
//     }

//     printf('WHAT');

// }


// static struct gpiod_line_request *request_input_line(const char *chip_path,
// 						     unsigned int offset,
// 						     const char *consumer)
// {
// 	struct gpiod_request_config *req_cfg = NULL;
// 	struct gpiod_line_request *request = NULL;
// 	struct gpiod_line_settings *settings;
// 	struct gpiod_line_config *line_cfg;
// 	struct gpiod_chip *chip;
// 	int ret;

// 	chip = gpiod_chip_open(chip_path);
// 	if (!chip)
// 		return NULL;

// 	settings = gpiod_line_settings_new();
// 	if (!settings)
// 		goto close_chip;

// 	gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);
// 	gpiod_line_settings_set_edge_detection(settings, GPIOD_LINE_EDGE_RISING);

// 	line_cfg = gpiod_line_config_new();
// 	if (!line_cfg)
// 		goto free_settings;

// 	ret = gpiod_line_config_add_line_settings(line_cfg, &offset, 1,
// 						  settings);
// 	if (ret)
// 		goto free_line_config;

// 	if (consumer) {
// 		req_cfg = gpiod_request_config_new();
// 		if (!req_cfg)
// 			goto free_line_config;

// 		gpiod_request_config_set_consumer(req_cfg, consumer);
// 	}

// 	request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);

// 	gpiod_request_config_free(req_cfg);

// free_line_config:
// 	gpiod_line_config_free(line_cfg);

// free_settings:
// 	gpiod_line_settings_free(settings);

// close_chip:
// 	gpiod_chip_close(chip);

// 	return request;
// }

// static const char *edge_event_type_str(struct gpiod_edge_event *event)
// {
// 	switch (gpiod_edge_event_get_event_type(event)) {
// 	case GPIOD_EDGE_EVENT_RISING_EDGE:
// 		return "Rising";
// 	case GPIOD_EDGE_EVENT_FALLING_EDGE:
// 		return "Falling";
// 	default:
// 		return "Unknown";
// 	}
// }

// int main(void)
// {
// 	/* Example configuration - customize to suit your situation. */
// 	static const char *const chip_path = "/dev/gpiochip0";
// 	static const unsigned int line_offset = 5;

// 	struct gpiod_edge_event_buffer *event_buffer;
// 	struct gpiod_line_request *request;
// 	struct gpiod_edge_event *event;
// 	int i, ret, event_buf_size;

// 	request = request_input_line(chip_path, line_offset,
// 				     "watch-line-value");
// 	if (!request) {
// 		fprintf(stderr, "failed to request line: %s\n",
// 			strerror(errno));
// 		return EXIT_FAILURE;
// 	}

// 	/*
// 	 * A larger buffer is an optimisation for reading bursts of events from
// 	 * the kernel, but that is not necessary in this case, so 1 is fine.
// 	 */
// 	event_buf_size = 1;
// 	event_buffer = gpiod_edge_event_buffer_new(event_buf_size);
// 	if (!event_buffer) {
// 		fprintf(stderr, "failed to create event buffer: %s\n",
// 			strerror(errno));
// 		return EXIT_FAILURE;
// 	}

// 	for (;;) {
// 		/* Blocks until at least one event is available. */
// 		ret = gpiod_line_request_read_edge_events(request, event_buffer,
// 							  event_buf_size);
// 		if (ret == -1) {
// 			fprintf(stderr, "error reading edge events: %s\n",
// 				strerror(errno));
// 			return EXIT_FAILURE;
// 		}
// 		for (i = 0; i < ret; i++) {
// 			event = gpiod_edge_event_buffer_get_event(event_buffer,
// 								  i);
// 			printf("offset: %d  type: %-7s  event #%ld\n",
// 			       gpiod_edge_event_get_line_offset(event),
// 			       edge_event_type_str(event),
// 			       gpiod_edge_event_get_line_seqno(event));
// 		}
// 	}
// }












// void main() {

//     int check = 1;
//     const char *chip_path = "/dev/gpiochip0";
//     const char *consumer = "WHAT IS UPPP";
//     uint8_t index;
//     uint8_t dir;

//     int offset = 4;
//     dir = 2;


//     // struct gpiod_request_config *req_cfg = NULL;
// 	// struct gpiod_line_request *request = NULL;
// 	// struct gpiod_line_settings *settings= NULL;
// 	// struct gpiod_line_config *line_cfg= NULL;
// 	// struct gpiod_chip *chip= NULL;
// 	// int ret;

// 	// // Connect to chip
// 	// chip = gpiod_chip_open(chip_path);
// 	// if (!chip)
// 	// 	return -1;

// 	// // Create settings struct
// 	// settings = gpiod_line_settings_new();
// 	// if (!settings){
// 	// 	gpiod_chip_close(chip);
// 	// 	return -1;
// 	// }

// 	// line_cfg = gpiod_line_config_new();
// 	// if (!line_cfg) {
// 	// 	gpiod_chip_close(chip);
// 	// 	gpiod_line_settings_free(settings);
// 	// 	return -1;
// 	// }

// 	// // Assign Directions to Struct
// 	// ret = gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);
//     // ret = gpiod_line_settings_set_drive(settings, GPIOD_LINE_DRIVE_OPEN_DRAIN);
//     // ret = gpiod_line_settings_set_bias(settings, GPIOD_LINE_BIAS_PULL_UP);
//     // gpiod_line_settings_set_active_low(settings, true);
//     // ret = gpiod_line_settings_set_event_clock(settings, GPIOD_LINE_CLOCK_REALTIME);
// 	// //gpiod_line_settings_set_debounce_period_us(settings, 100);
// 	// //ret = gpiod_line_settings_set_edge_detection(settings, GPIOD_LINE_EDGE_RISING);
// 	// if (ret) {
// 	// 	gpiod_line_config_free(line_cfg);
// 	// 	gpiod_chip_close(chip);
// 	// 	gpiod_line_settings_free(settings);
// 	// 	return -1;
// 	// }

// 	// // Add the direction setting to the configuration
// 	// ret = gpiod_line_config_add_line_settings(line_cfg, &offset, 1, settings);
// 	// if (ret) {
// 	// 	gpiod_line_config_free(line_cfg);
// 	// 	gpiod_chip_close(chip);
// 	// 	gpiod_line_settings_free(settings);
// 	// 	return -1;
// 	// }

// 	// // If 'consumer' label was inputted
// 	// if (consumer) {
// 	// 	req_cfg = gpiod_request_config_new();
// 	// 	if (!req_cfg){
// 	// 		gpiod_request_config_free(req_cfg);
// 	// 		gpiod_line_config_free(line_cfg);
// 	// 		gpiod_line_settings_free(settings);
// 	// 		gpiod_chip_close(chip);
// 	// 		return -1;
// 	// 	}
// 	// 	gpiod_request_config_set_consumer(req_cfg, consumer);
// 	// }

// 	// // Complete request
// 	// request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);

// 	// gpiod_request_config_free(req_cfg);
// 	// gpiod_line_config_free(line_cfg);
// 	// gpiod_line_settings_free(settings);
// 	// gpiod_chip_close(chip);
// 	// gpiod_line_request_release(request);

// 	// return 0;
//     index = 4;
//     check = gpio_config_port(chip_path, index, dir, 1, consumer);
//     index = 2;

//     check = gpio_config_port(chip_path, index, dir, 1, consumer);
//     index = 3;

//     check = gpio_config_port(chip_path, index, dir, 0, NULL);
//     index = 4;

//     check = gpio_config_port(chip_path, index, dir, 0, NULL);
//     index = 5;

//     check = gpio_config_port(chip_path, index, dir, 1, NULL);
//     index = 6;

//     check = gpio_config_port(chip_path, index, DIR_IN, 1, consumer);
//     index = 7;

//     check = gpio_config_port(chip_path, index, DIR_IN, 1, consumer);
//     index = 8;

//     check = gpio_config_port(chip_path, index, DIR_IN, 1, consumer);
//     index = 9; 


// }


// static struct gpiod_line_request *
// request_output_line(const char *chip_path, unsigned int offset,
// 		    enum gpiod_line_value value, const char *consumer)
// {
// 	struct gpiod_request_config *req_cfg = NULL;
// 	struct gpiod_line_request *request = NULL;
// 	struct gpiod_line_settings *settings;
// 	struct gpiod_line_config *line_cfg;
// 	struct gpiod_chip *chip;
// 	int ret;

// 	chip = gpiod_chip_open(chip_path);
// 	if (!chip)
// 		return NULL;

// 	settings = gpiod_line_settings_new();
// 	if (!settings)
// 		goto close_chip;

// 	gpiod_line_settings_set_direction(settings,
// 					  GPIOD_LINE_DIRECTION_OUTPUT);
// 	gpiod_line_settings_set_output_value(settings, value);

// 	line_cfg = gpiod_line_config_new();
// 	if (!line_cfg)
// 		goto free_settings;

// 	ret = gpiod_line_config_add_line_settings(line_cfg, &offset, 1,
// 						  settings);
// 	if (ret)
// 		goto free_line_config;

// 	if (consumer) {
// 		req_cfg = gpiod_request_config_new();
// 		if (!req_cfg)
// 			goto free_line_config;

// 		gpiod_request_config_set_consumer(req_cfg, consumer);
// 	}

// 	request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);

// 	gpiod_request_config_free(req_cfg);

// free_line_config:
// 	gpiod_line_config_free(line_cfg);

// free_settings:
// 	gpiod_line_settings_free(settings);

// close_chip:
// 	gpiod_chip_close(chip);

// 	return request;
// }

// static enum gpiod_line_value toggle_line_value(enum gpiod_line_value value)
// {
// 	return (value == GPIOD_LINE_VALUE_ACTIVE) ? GPIOD_LINE_VALUE_INACTIVE :
// 						    GPIOD_LINE_VALUE_ACTIVE;
// }

// static const char * value_str(enum gpiod_line_value value)
// {
// 	if (value == GPIOD_LINE_VALUE_ACTIVE)
// 		return "Active";
// 	else if (value == GPIOD_LINE_VALUE_INACTIVE) {
// 		return "Inactive";
// 	} else {
// 		return "Unknown";
// 	}
// }

// int main(void)
// {
// 	/* Example configuration - customize to suit your situation. */
// 	static const char *const chip_path = "/dev/gpiochip0";
// 	static const unsigned int line_offset = 5;

// 	enum gpiod_line_value value = GPIOD_LINE_VALUE_ACTIVE;
// 	struct gpiod_line_request *request;

// 	request = request_output_line(chip_path, line_offset, value,
// 				      "toggle-line-value");
// 	if (!request) {
// 		fprintf(stderr, "failed to request line: %s\n",
// 			strerror(errno));
// 		return EXIT_FAILURE;
// 	}

// 	for (;;) {
// 		printf("%d=%s\n", line_offset, value_str(value));
// 		sleep(1);
// 		value = toggle_line_value(value);
// 		gpiod_line_request_set_value(request, line_offset, value);
// 	}

// 	gpiod_line_request_release(request);

// 	return EXIT_SUCCESS;
// }
