#ifndef SPI_SERVICE_H
#define SPI_SERVICE_H

#include <gpiod.h>

//BUS INDEX
#define I2C_BUS_INDEX 1 //Indicates which dev_file the i2c driver uses

#define GPIOCHIP "/dev/gpiochip0"

#define RETYCOUNT 5

// GPIO Initialization Configuration for each GPIO pin
// GPIO # ------------  3V3_EN     5V_EN   5VCAM_EN  HUB_RST   HUB_HS_IND   HUB_STP   PI_ACTI -
#define BOOTGPIOOFFSET {   4    ,    5    ,    6    ,    16   ,    17    ,    18    ,    42    }
#define BOOTGPIOBIAS   {BIAS_PU , BIAS_PU , BIAS_PD , BIAS_PU , BIAS_DIS , BIAS_DIS , BIAS_DIS }
#define BOOTGPIOVAL    {VAL_HI  , VAL_HI  , VAL_LOW , VAL_HI  , VAL_LOW  , VAL_LOW  , VAL_HI  }
#define BOOTGPIODRV    {DRV_PP  , DRV_PP  , DRV_PP  , DRV_PP  , DRV_PP   , DRV_PP   , DRV_PP   }
#define BOOTGPIODIR    {DIR_OUT , DIR_OUT , DIR_OUT , DIR_OUT , DIR_IN   , DIR_IN   , DIR_OUT  }

#define BOOTNUMOFFSET 7

#define MAX_ITERATIONS 100
#define EDGE_EVENT_BUFF_SIZE 255
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
#define HOUSE_KEEPING_DELAY_S 10
#define CMD_TRANSFER_TIMEOUT_S 10

int main(void);
enum IRIS_ERROR spi_cmd_loop(int spi_dev, 
                  struct gpiod_line_request *spi_cs_request, 
                  struct gpiod_edge_event_buffer *event_buffer);
bool signal_edge_detect(struct gpiod_line_request *request, struct gpiod_edge_event_buffer *event_buffer);
struct gpiod_line_request *gpio_setup(enum IRIS_ERROR *errorBuffer);
//void system_house_keeping(void);
#endif //SPI_SERVICE_H
