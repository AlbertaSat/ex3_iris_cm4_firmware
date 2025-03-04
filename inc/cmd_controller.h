#ifndef CMD_CTRL_H
#define CMD_CTRL_H

#include <gpiod.h>
#include <stdint.h>

typedef enum IRIS_CMD{

	CMD_RETURN = 12,

    CURR_SENSOR_SETUP = 55,
	CURR_SENSOR_VALIDATE,
	CURR_SENSOR_STATUS,
	CURR_SENSOR_RESET,
	CURR_SENSOR_READ_CURRENT,
	CURR_SENSOR_READ_VOLTAGE,
	CURR_SENSOR_READ_POWER,
	CURR_SENSOR_READ_PK_POWER,
	CURR_SENSOR_READ_LIMIT,
	
    TEMP_SENSOR_SETUP,
	TEMP_SENSOR_VALIDATE,
	TEMP_SENSOR_STATUS,
	TEMP_SENSOR_RESET,
	TEMP_SENSOR_READ,
	TEMP_SENSOR_READ_LIMIT,
	
    USB_HUB_SETUP,
    USB_HUB_VALIDATE,
    USB_HUB_RESET,
	
	ERROR_TRANSFER,

    IMAGE_CONFIG,
    IMAGE_CAPTURE,
	
    FILE_TRANSFER,
	SYNC_TIME,
	CHECKSUM,

}IRIS_CMD;

#define RETURN_CMD_SIZE 8

uint8_t cmd_to_current_addr(uint8_t arg);
uint8_t cmd_to_temp_addr(uint8_t arg);
enum IRIS_ERROR current_val_error_16bit_to_8bit(enum IRIS_ERROR error);

enum IRIS_ERROR cmd_return(int spi_dev, struct gpiod_line_request **spi_cs_request, uint8_t *buffer, uint8_t numWrites);

enum IRIS_ERROR cmd_center(uint8_t cmd, uint8_t *args, int nargs, int spi_dev, struct gpiod_line_request **spi_cs_request);
int cmd_extracter(uint8_t *cmd, uint8_t *arg, const uint8_t *rx_buffer, uint8_t rx_len);


#endif //CMD_CTRL