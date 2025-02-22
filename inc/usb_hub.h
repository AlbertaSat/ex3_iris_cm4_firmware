#ifndef USB_HUB_H
#define USB_HUB_H




//------------ USB2512B Register Address ------------//
#define VENDOR_ID_LSB      0x00
#define VENDOR_ID_MSB      0x01
#define PRODUCT_ID_LSB     0x02
#define PRODUCT_ID_MSB     0x03
#define DEVICE_ID_LSB      0x04
#define DEVICE_ID_MSB      0x05

#define CFG_DATA_BYTE_1    0x06
#define CFG_DATA_BYTE_2    0x07
#define CFG_DATA_BYTE_3    0x08

#define NON_RMV_DEVICES    0x09

#define PORT_DIS_SELF      0x0A
#define PORT_DIS_BUS       0x0B

#define MAX_POWER_SELF     0x0C
#define MAX_POWER_BUS      0x0D

#define HUB_MAX_CURR_SELF  0x0E
#define HUB_MAX_CURR_BUS   0x0F

#define POWER_ON_TIME      0x10

#define LANGUAGE_ID_HIGH   0x11
#define LANGUAGE_ID_LOW    0x12

#define MFG_STR_LEN        0x13
#define PRODUCT_STR_LEN    0x14
#define SERIAL_STR_LEN     0x15

//#define MFG_STR            0x16-0x53
//#define PRODUCT_STR        0x54-0x91
//#define SERIAL_STR         0x92-0xCF

#define BAT_CHARGE_EN     0xD0

#define BOOST_UP          0xF6
#define BOOST_X           0xF8

#define PORT_SWAP         0xFA
#define PORT_MAP_12       0xFB
#define PORT_MAP_34       0xFC

#define STAT_CMD_REG      0xFF

//------- USB Hub Register Configurations -------//

#define CFG_DATA_BYTE_1_POR    0x9B //0b 1001 1011
#define CFG_DATA_BYTE_2_POR    0x20 //0b 0010 0000
#define CFG_DATA_BYTE_3_POR    0x02 //0b 0000 0010

//Temperature I2C Address
#define USB_HUB_I2C_ADDR       0x2C //0b0101100


enum IRIS_ERROR usb_hub_setup(void);
enum IRIS_ERROR usb_hub_func_validate(struct gpiod_line_request *gpio_request);
enum IRIS_ERROR usb_hub_reset_trig(struct gpiod_line_request *gpio_request);

#endif //USB_HUB_H