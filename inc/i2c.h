#ifndef I2C_H
#define I2C_H

#include <stdint.h>

enum I2C_OPERATION{
    I2C_READ  = 1,
    I2C_WRITE = 0,
};

#define I2C_MAX_WRITE 255
#define I2C_MAX_READ  255

int i2c_setup_interface(char *i2cBus, uint8_t devID);
int i2c_setup(int i2cBus, uint8_t devID);
enum IRIS_ERROR i2c_interface (int fileDesc, enum I2C_OPERATION rwType, int sizeByte, uint8_t *data);
enum IRIS_ERROR i2c_write_reg8 (int fileDesc, int writeNum, uint8_t *data);
enum IRIS_ERROR i2c_read_reg8 (int fileDesc, int readNum, uint8_t *data);
enum IRIS_ERROR i2c_reg8_write_read (int fileDesc, uint8_t *reg, int readNum, uint8_t *data);
enum IRIS_ERROR i2c_write_reg16(int fileDesc, int writeNum, const uint8_t *reg, const uint16_t *data);
enum IRIS_ERROR i2c_read_reg16 (int fileDesc, int readNum, uint16_t *data);
enum IRIS_ERROR i2c_reg16_write_read (int fileDesc, uint8_t *reg, int readNum, uint16_t *data);
int i2c_close(int fileDesc);

#endif //I2C_H