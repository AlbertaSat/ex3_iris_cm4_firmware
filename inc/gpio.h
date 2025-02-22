#ifndef GPIO_H
#define GPIO_H

#include <gpiod.h>

//GPIO Config Indexes
#define DIR_IN 1
#define DIR_OUT 2

#define VAL_HI 1
#define VAL_LOW 0

#define DRV_PP 1
#define DRV_OD 2
#define DRV_OS 3

#define BIAS_DIS 3
#define BIAS_PU 4
#define BIAS_PD 5

#define EDGE_RIS 2
#define EDGE_FALL 3
#define EDGE_BOTH 4

// IRIS GPIO Indexing
typedef enum IRIS_GPIO_INDEX{
    ID_SD = 0,
    ID_SC = 1,

    I2C_SDA = 2,
    I2C_SCL = 3,

    PWR_3V3_EN = 4,
    PWR_5V_EN = 5,
    PWR_5V_CAM_EN = 6,

    CM4_GPIO_7 = 7,

    SPI_CE_N = 8,
    SPI_MISO = 9,
    SPI_MOSI = 10,
    SPI_SCLK = 11,

    CM4_GPIO_12 = 12,
    CM4_GPIO_13 = 13,

    UART_TX = 14,
    UART_RX = 15,

    HUB_RST_L = 16,
    HUB_HS_IND = 17,
    HUB_SETUP_IND = 18,

    CM4_GPIO_19 = 19,
    CM4_GPIO_20 = 20,
    CM4_GPIO_21 = 21,

    JTAG_TRST = 22,
    JTAG_RTCK = 23,
    JTAG_TDO = 24,
    JTAG_TCK = 25,
    JTAG_TDI = 26,
    JTAG_TMS = 27,

    ID_SCL0 = 45,
    ID_SDA0 = 46,

    LED_ACTI = 42,
}IRIS_GPIO_INDEX;

#define NO_CHANGE 0xFFFF

#define HUB_HS_IND_DEF 0
#define HUB_SETUP_IND_DEF 0

typedef struct {
    uint16_t dir;
    uint16_t edge;
    uint16_t drive;
    uint16_t bias;
    uint16_t active_low;
    uint16_t event_clock;
    uint16_t debounce;
    uint16_t output;
} gpio_config_t;

int gpio_config_port(const char *chip_path, int offset, int dir, int outputVal, const char *consumer);
struct gpiod_line_request *cs_toggle(struct gpiod_line_request *cs_request, int state);
struct gpiod_line_request *gpio_config_group(const char *chip_path, unsigned int numOffsets, unsigned int *offset, unsigned int *dir, unsigned int *outputVal, unsigned int *drive, unsigned int *bias, const char *consumer);
struct gpiod_line_request *gpio_config_input_detect(const char *chip_path, int offset, int edgeDetect, const char *consumer);
#endif //GPIO_H