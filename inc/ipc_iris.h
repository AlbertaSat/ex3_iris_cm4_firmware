#ifndef IPC_IRIS_H
#define IPC_IRIS_H

#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_SIZE             1024
#define IPC_REF_FILE_PATH    "~/theia_ipc_ref"
#define IPC_REF_VAL          53

typedef enum IPC_LABEL{

    ERROR_MAIN_TO_SPI = 1,
    ERROR_SPI_TO_MAIN = 2,

    CMD_MAIN_TO_SPI = 3,
    CMD_SPI_TO_MAIN = 4

}IPC_LABEL;

struct msg_buffer {
    long msg_type;
    long msg_type_rx;
    uint8_t msg_text[MSG_SIZE];
};

enum IRIS_ERROR iris_error_transfer_spi_service(int ipcMsgID, const enum IRIS_ERROR *errorBuffer, uint8_t *errorCount);
enum IRIS_ERROR ipc_setup(key_t *key, int *msgid);

#endif //IPC_IRIS_H