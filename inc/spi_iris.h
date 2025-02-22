#ifndef SPI_IRIS_H
#define SPI_IRIS_H

#include <gpiod.h>
#include <stdint.h>

enum CS_STATE{
    CS_MONITOR = 1,
    CS_RW      = 2
};

typedef struct {
    uint8_t mode;
    uint8_t bits_per_word;
    uint32_t speed;
    uint16_t delay;
} spi_config_t;

#define SPI_MODE_TYP_0 0
#define SPI_SPEED 1000000
#define SPI_DELAY 100
#define SPI_BITS_PER_WORD 8
#define SPI_DEVICE "/dev/spidev0.0"
//#define SPI_DEVICE "/dev/spidev1.0"

#define SPI_FILE_BUFFER_LEN 4096//4095
#define SPI_TEST_TIMEOUT 0.5 //0.5s Timeout
#define SPI_TEST_CMD 0x6A //! This command is determined at a later date
#define SPI_BUFFER_LEN 512

#define END_SPI_CMD 0xFF
#define SPI_ERROR_BUFFER_LEN 4096

#define SPI_TEST_MSG {SPI_TEST_CMD, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE};
#define SPI_TEST_MSG_SIZE 8

int spi_open(const char *device, spi_config_t config);
int spi_bus_setup(void);
int spi_close(int fileDesc);
struct gpiod_line_request *spi_cs_setup(void);

enum IRIS_ERROR spi_init(int *spi_dev, struct gpiod_line_request **spi_cs_request, struct gpiod_edge_event_buffer **event_buffer);
enum IRIS_ERROR spi_reinit(int *spi_dev, struct gpiod_line_request **spi_cs_request, struct gpiod_edge_event_buffer **event_buffer);

enum IRIS_ERROR spi_read(int fileDesc, uint8_t *rx_buffer, uint16_t rx_len, struct gpiod_line_request *cs_request);
enum IRIS_ERROR spi_write(int fileDesc, const uint8_t *tx_buffer, uint16_t tx_len, struct gpiod_line_request *cs_request);

enum IRIS_ERROR spi_file_read(int spi_dev, struct gpiod_line_request **spi_cs_request, struct gpiod_edge_event_buffer **event_buffer, const char *file_path);
enum IRIS_ERROR spi_file_write(int spi_dev, struct gpiod_line_request **spi_cs_request, char *file_path, struct gpiod_edge_event_buffer **event_buffer);
enum IRIS_ERROR spi_bus_test(int spi_dev, struct gpiod_line_request *spi_cs_request, struct gpiod_edge_event_buffer *event_buffer);
enum IRIS_ERROR spi_bus_test_compare(const uint8_t *buffer);

#endif //SPI_IRIS_H