#ifndef I2C_H
#define I2C_H

#include <stdint.h>

#define I2C_READ	   1
#define I2C_WRITE	   0
#define I2C_MAX_WRITE 255
#define I2C_MAX_READ  255

int i2c_setup_interface(char *i2cBus, int devID);
int i2c_setup(int i2cBus, int devID);
enum IRIS_ERROR i2c_interface (int fd, int rwType, int sizeByte, uint8_t *data);
enum IRIS_ERROR i2c_write_reg8 (int fd, int writeNum, uint8_t *data);
enum IRIS_ERROR i2c_read_reg8 (int fd, int readNum, uint8_t *data);
enum IRIS_ERROR i2c_reg8_write_read (int fd, uint8_t *reg, int readNum, uint8_t *data);
enum IRIS_ERROR i2c_write_reg16 (int fd, int writeNum, uint8_t *reg, uint16_t *data);
enum IRIS_ERROR i2c_read_reg16 (int fd, int readNum, uint16_t *data);
enum IRIS_ERROR i2c_reg16_write_read (int fd, uint8_t *reg, int readNum, uint16_t *data);
int i2c_close(int fd);

#endif //I2C_H