

#include "error_handler.h"
#include "ipc_iris.h"
#include "logger.h"
#include "main.h"
#include "timing.h"

#include <stdint.h>

#include <sys/ipc.h>
#include <sys/msg.h>





//! NEED TO MAKE IT SO IT CAN HANDLE LARGER NUMBER OF TOTAL ERROR CODES
//! CURRENTLY ITS LIMITED TO MSG_SIZE
//! MAKE IT SO IT CAN HANDLE 16-BIT ERROR CODES
enum IRIS_ERROR iris_error_transfer_spi_service(int ipcMsgID, const enum IRIS_ERROR *errorBuffer, uint8_t *errorCount){

        struct msg_buffer ipc_message;
        ipc_message.msg_type    = ERROR_MAIN_TO_SPI;
        ipc_message.msg_type_rx = ERROR_SPI_TO_MAIN;
        
        for (int index = 0; index < *errorCount; index ++){
            ipc_message.msg_text[index] = errorBuffer[index];
        }

        //Send data to Queue
        if(msgsnd(ipcMsgID, &ipc_message, sizeof(ipc_message.msg_text), IPC_NOWAIT) == -1){
            // If fails that means Queue is full, therefore leave and try again later
            return NO_ERROR; 
        }

        int start_time = get_time_seconds();
        //log_write(LOG_INFO, "SPI-BUS-TEST: Begin SPI bus test read from OBC");

        // Wait for a Period of Time to get confirmation on Error Transfer to OBC
        do{
            if(msgrcv(ipcMsgID, &ipc_message, sizeof(ipc_message.msg_text), ipc_message.msg_type_rx, IPC_NOWAIT) != -1){
                
                if(ipc_message.msg_text[0] == ERROR_TRANSFER_FAIL){
                    return NO_ERROR;
                }
                *errorCount = 0;
                return NO_ERROR;

            }
        }while((get_time_seconds() - start_time) < ERROR_TRANSFER_TIMEOUT_S);

        return NO_ERROR;
}



enum IRIS_ERROR ipc_setup(key_t *key, int *msgid){

    log_write(LOG_INFO, "IPC-SETUP: Start IPC connection setup.");

    *key = ftok(IPC_REF_FILE_PATH, IPC_REF_VAL); // Create a unique key
    *msgid = msgget(*key, 0660 | IPC_CREAT);      // Create message queue with restricted permissions
    
    if (*msgid == -1) {
        log_write(LOG_ERROR, "IPC-SETUP: Failed to establish IPC Connection between SPI and Main Services");
        return IPC_ERROR;
    }

    log_write(LOG_INFO, "IPC-SETUP: Successfully completed setup of IPC connection");
    return NO_ERROR;
}