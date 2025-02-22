
#include "spi_iris.h"
#include "logger.h"
#include "cmd_controller.h"

#include "error_handler.h"


#include "watchdog.h"
#include "timing.h"
#include "spi_service.h"
#include "ipc_iris.h"
#include "gpio.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <gpiod.h>

#include <stdbool.h>

#include <stdlib.h>

#include <sys/ipc.h>
#include <sys/msg.h>


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

enum IRIS_ERROR ipc_spi_cmd_main(int ipcMsgID, uint8_t *rx_buffer, uint8_t rx_count){

    struct msg_buffer ipc_message;
    ipc_message.msg_type    = CMD_SPI_TO_MAIN;
    ipc_message.msg_type_rx = CMD_MAIN_TO_SPI;

    //Transfer RX_SPI Buffer content to MSG Structure
    for (int index = 0; index < rx_count; index ++){
        ipc_message.msg_text[index] = rx_buffer[index];
    }

    //Send data to CMD IPC Queue
    if(msgsnd(ipcMsgID, &ipc_message, sizeof(ipc_message.msg_text), IPC_NOWAIT) == -1){
        //! If fails that means Queue is full, therefore leave and try again later
        //! NEED TO DETERMINE WHAT TO DO IF THIS OCCURS
        //! MAYBE JUST HAVE IT CONTINUE
        return NO_ERROR; 
    }
    return NO_ERROR;
    // int start_time = get_time_seconds();
    // //log_write(LOG_INFO, "SPI-BUS-TEST: Begin SPI bus test read from OBC");

    // // Wait for a Period of Time to get reply from OBC
    // do{
        // if(msgrcv(ipcMsgID, &ipc_message, sizeof(ipc_message.msg_text), ipc_message.msg_type_rx, IPC_NOWAIT) != -1){
        //     *count = ipc_message.msg_text[0];
        //     for (int index = 0; index < *count; index ++){
        //         rx_buffer[index] = ipc_message.msg_text[index + 1];
        //     }
        //     return NO_ERROR;
        // }
    // }while((get_time_seconds() - start_time) < CMD_TRANSFER_TIMEOUT_S);

    // return NO_ERROR;
}

enum IRIS_ERROR ipc_main_cmd_spi(int ipcMsgID, uint8_t *tx_buffer, uint8_t *tx_count){

    struct msg_buffer ipc_message;
    ipc_message.msg_type    = CMD_SPI_TO_MAIN;
    ipc_message.msg_type_rx = CMD_MAIN_TO_SPI;

    if(msgrcv(ipcMsgID, &ipc_message, sizeof(ipc_message.msg_text), ipc_message.msg_type_rx, IPC_NOWAIT) != -1){
        *tx_count = ipc_message.msg_text[0];
        for (int index = 0; index < *tx_count; index ++){
            tx_buffer[index] = ipc_message.msg_text[index + 1];
        }
        return NO_ERROR;
    }
    *tx_count = 0;
    return NO_ERROR;
}


//! NEED TO DEAL WITH IPC FAIL
IRIS_ERROR spi_read_loop(int ipcMsgID, int spi_dev, struct gpiod_line_request *spi_cs_request, struct gpiod_edge_event_buffer *event_buffer) {

    bool cs_edge = false;
    uint8_t rx_count = SPI_RX_LEN;
    uint8_t rx_buffer[SPI_RX_LEN] = {0};

    IRIS_ERROR error = NO_ERROR;

    for(int index = 0; (index < SPI_MAX_LOOP_AMT) && (cs_edge == false); index++){
        cs_edge = signal_edge_detect(spi_cs_request, event_buffer);

        if (cs_edge == true){
            error = spi_read(spi_dev, rx_buffer, rx_count, spi_cs_request);
            if(error != NO_ERROR){
                return error;
            }
            error = ipc_spi_cmd_main(ipcMsgID, rx_buffer, rx_count);
            return error;
        } 
    }
    return error;
}

//! NEED TO BE ABLE TO DETERMINE WHAT TO DO IF THE BUFFER IS FULL
//! NEED TO DETERMINE WHAT TO DO IF SPI FAILS EVEN WITH DATA

IRIS_ERROR spi_write_loop(int ipcMsgID, int spi_dev, struct gpiod_line_request *spi_cs_request, struct gpiod_edge_event_buffer *event_buffer) {

    uint8_t tx_count = 0;
    uint8_t tx_buffer[SPI_BUFFER_LEN] = {};

    enum gpiod_line_value csVal = GPIOD_LINE_VALUE_INACTIVE;
    IRIS_ERROR error = NO_ERROR;

    csVal = gpiod_line_request_get_value(spi_cs_request, SPI_CE_N);

    // Only Proceed with transfer if CS Line is inactive
    if (csVal == GPIOD_LINE_VALUE_ACTIVE){

        error = ipc_main_cmd_spi(ipcMsgID, tx_buffer, &tx_count);

        if (tx_count != 0){
            error = spi_write(spi_dev, tx_buffer, tx_count, spi_cs_request);
            if(error != NO_ERROR){
                return error;
            }
            return error;
        } 
    }
    return error;
}

//! THE MAIN MAIN FUNCTION

int main(void){

    struct gpiod_line_request *spi_cs_request = NULL;
    struct gpiod_edge_event_buffer *event_buffer = NULL;

    enum IRIS_ERROR spiInitError = NO_ERROR;
    enum IRIS_ERROR spiError = NO_ERROR;
    enum IRIS_ERROR ipcInitError = NO_ERROR;

    uint8_t errorCount = 0;
    enum IRIS_ERROR errorBuffer[ERROR_BUFFER_SIZE] = {NO_ERROR};

    key_t IPCKey = 0;
    int ipcMsgID = 0;

    int spi_dev = 0;

    spiInitError = spi_init(&spi_dev, &spi_cs_request, &event_buffer);

    ipcInitError = ipc_setup(&IPCKey, &ipcMsgID);

    //! MAYBE ADD RESET FOR COLD + HOT
    //! MAYBE ADD AN ERROR STATE WHICH WAIT X AMOUNT OF TIME UNTIL A COMMAND IS RECEIVED FROM OC BEFORE DOING A RESTARBT
    //! ADD WATCHDOG
    struct msg_buffer ipc_message_error;
    ipc_message_error.msg_type    = ERROR_SPI_TO_MAIN;
    ipc_message_error.msg_type_rx = ERROR_MAIN_TO_SPI;
    
    struct msg_buffer ipc_message_cmd;
    ipc_message_cmd.msg_type    = CMD_SPI_TO_MAIN;
    ipc_message_cmd.msg_type_rx = CMD_MAIN_TO_SPI;

    while(true){

        if (spiInitError != NO_ERROR) {
            spiInitError = spi_init(&spi_dev, &spi_cs_request, &event_buffer);
        }else if(spiError != NO_ERROR){

            spiInitError = spi_reinit(&spi_dev, &spi_cs_request, &event_buffer);
            spiError = NO_ERROR;

        }else{

            //spiError = spi_cmd_loop(spi_dev, spi_cs_request, event_buffer);


        // snprintf(message_error.msg_text, sizeof(message_error.msg_text), "Service 2 Sending %d and %d\n", x--, y++);

        // if (msgrcv(ipcMsgID, &message_cmd, sizeof(message_cmd.msg_text), 1, IPC_NOWAIT) != -1) {
        //     printf("Service 2 received: %s\n", message_cmd.msg_text);
        // }

        // if(msgsnd(ipcMsgID, &message_error, sizeof(message_error.msg_text), 0) != -1){
        //     printf("Service 2 sent: %s\n", message_error.msg_text);
        }

        
    }
}
