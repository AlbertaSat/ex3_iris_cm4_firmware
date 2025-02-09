
#ifndef TEMP_READ_H
#define TEMP_READ_H

#include <stdint.h>
#include "i2c.h"

//TMP421 Register Address
#define TMP_REG_LOCAL_HIGH 0x00
#define TMP_REG_RMT_1_HIGH 0x01
#define TMP_REG_RMT_2_HIGH 0x02
#define TMP_REG_RMT_3_HIGH 0x03

#define TMP_REG_LOCAL_LOW  0x10
#define TMP_REG_RMT_1_LOW  0x11
#define TMP_REG_RMT_2_LOW  0x12
#define TMP_REG_RMT_3_LOW  0x13

#define TMP_REG_STATUS     0x08
#define TMP_REG_CFG_1      0x09
#define TMP_REG_CFG_2      0x0A

#define TMP_REG_CNV_RATE   0x0B
#define TMP_ONE_SHT_START  0x0F

#define TMP_REM_N1_CORRECT 0x21
#define TMP_REM_N2_CORRECT 0x22
#define TMP_REM_N3_CORRECT 0x23

#define TMP_REG_SW_RST     0xFC
#define TMP_REG_MCT_ID     0xFE
#define TMP_REG_DVC_ID     0xFF

#define TMP_MCT_ID         0x55

//---- Temp Sensor Register Configurations ----//

//POR Register Values
#define TMP_REG_CFG_1_POR  0x00
#define TMP_REG_CFG_2_POR  0x1C

//--- Temp Sensor Reg Config Defaults ---//
#define TEMP_REG_DEFAULT {TMP_REG_CFG_1_POR, \
                          TMP_REG_CFG_2_POR}

#define TEMP_REG_ADDR {TMP_REG_CFG_1, \
                       TMP_REG_CFG_2}

//Temperature I2C Address
#define TEMP_SENSOR_1_ADDR 0x4C
#define TEMP_SENSOR_2_ADDR 0x4D
#define TEMP_SENSOR_3_ADDR 0x4E
#define TEMP_SENSOR_4_ADDR 0x4F

//Max Temperatures
#define TEMP1_MAX 100
#define TEMP2_MAX 100
#define TEMP3_MAX 100
#define TEMP4_MAX 100

//Min Temperatures
#define TEMP1_MIN -50
#define TEMP2_MIN -50
#define TEMP3_MIN -50
#define TEMP4_MIN -50

enum IRIS_ERROR temp_error_code(uint8_t tempAddr, enum IRIS_ERROR errorType);
enum IRIS_ERROR temp_setup(uint8_t tempAddr);
enum IRIS_ERROR temp_func_validate(uint8_t tempAddr);
enum IRIS_ERROR temp_reset_trig(uint8_t tempAddr);
void temperature_limit(enum IRIS_ERROR *errorBuffer);
int convert_temp_read(uint8_t HighByte);
int read_temperature(uint8_t tempAddr);

#endif //TEMP_READ_H