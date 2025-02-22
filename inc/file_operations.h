#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include <stdint.h>

enum IRIS_ERROR sha256_checksum(const char *filename, uint8_t *checksum);

#endif //FILE_OPERATIONS_H