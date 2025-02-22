
// #include <linux/spi/spidev.h>

// #include <stdio.h>
// #include <stdint.h>
// #include <fcntl.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <errno.h>
// #include <sys/ioctl.h>
// #include <string.h>
// #include <time.h>
// #include <openssl/sha.h>

// #include "spi_iris.h"
// #include "main.h"
// #include "gpio.h"
// #include "error_handler.h"
// #include "logger.h"
// #include "cmd_controller.h"
// #include <sys/mman.h>

// #define BCM2835_PERI_BASE        0x3F000000  // Change to 0xFE000000 for Raspberry Pi 4
// #define GPIO_BASE                (BCM2835_PERI_BASE + 0x200000)

// // GPIO register offsets
// #define GPSET0                   7  // GPIO Set (high)
// #define GPCLR0                   10 // GPIO Clear (low)
// #define GPFSEL0                  0  // GPIO Function Select

// volatile unsigned *gpio;
// int mem_fd;
// //                        MISO       MOSI      CLK
// #define BOOTGPIOOFFSET {    9    ,    10   ,    11   }
// #define BOOTGPIOBIAS   { BIAS_PU , BIAS_PU , BIAS_PU }
// #define BOOTGPIOVAL    { VAL_HI  , VAL_HI ,  VAL_HI  }
// #define BOOTGPIODRV    { DRV_PP  , DRV_PP  , DRV_PP  }
// #define BOOTGPIODIR    { DIR_OUT , DIR_IN ,  DIR_IN }

// #define BOOTNUMOFFSET 4

// void setup_gpio() {
//     mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
//     if (mem_fd < 0) {
//         perror("Failed to open /dev/mem");
//         return;
//     }

//     gpio = (volatile unsigned *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, GPIO_BASE);
//     if (gpio == MAP_FAILED) {
//         perror("Failed to mmap");
//         close(mem_fd);
//         return;
//     }

//     printf("OPENED GPIO\n");
//     // Set GPIO 9 (BCM) as output
//     gpio[GPFSEL0] &= ~(7 << (9 * 3)); // Clear bits for GPIO 9
//     gpio[GPFSEL0] |= (1 << (9 * 3));  // Set bits to 001 (output)

// }

// setup_spi_interface(){

//     struct gpiod_line_request *clk_pin = NULL;
//     struct gpiod_line_request *cs_pin = NULL;
//     struct gpiod_line_request *miso_pin = NULL;
//     struct gpiod_line_request *mosi_pin = NULL;
//     struct gpiod_edge_event_buffer *cs_event_buffer = NULL;
//     struct gpiod_edge_event_buffer *clk_event_buffer = NULL;

//     uint8_t miso_buffer[20] = {170,85,230,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
//     uint8_t mosi_buffer[20] = {0};

//     char *gpioDev = GPIOCHIP;
//     int misonum = 9;
//     int mosinum = 10;

//     int dirout = DIR_OUT;
//     int dirin = DIR_IN;

//     int outhigh = VAL_HI;
//     int outLow = VAL_LOW;

//     int drive = DRV_PP;

//     int bias = BIAS_PU;

//     setup_gpio();

//     //clk_pin = gpio_config_input_detect(gpioDev, 11, EDGE_RIS, "IRIS_SPI_INTERFACE");
//     cs_pin = gpio_config_input_detect(gpioDev, SPI_CE_N, EDGE_BOTH, "IRIS_SPI_INTERFACE");
//     //miso_pin = gpio_config_group(gpioDev, 1, &misonum, &dirout, &outhigh, &drive, &bias, "IRIS_SPI_INTERFACE");
//     //mosi_pin = gpio_config_group(gpioDev, 1, &mosinum, &dirin, &outLow, &drive, &bias, "IRIS_SPI_INTERFACE");


//     log_write(LOG_INFO, "SPI-BUS-TEST: Begin SPI bus test transfer too OBC");

//     clock_t start_time = clock();
//     double elaspsedTime = 0;
//     bool cs_edge = false;
//     bool datafound = false;
//     bool spi_active = true;

//     log_write(LOG_INFO, "SPI-BUS-TEST: Begin SPI bus test read from OBC");
//     uint8_t counter = 7;
//     uint8_t buff_count = 0;

//     cs_event_buffer = gpiod_edge_event_buffer_new(EDGE_EVENT_BUFF_SIZE);
//     //clk_event_buffer = gpiod_edge_event_buffer_new(EDGE_EVENT_BUFF_SIZE);

//     do{
//         cs_edge = signal_edge_detect(cs_pin, cs_event_buffer);

//         if(cs_edge == true){
//             log_write(LOG_INFO, "SPI-BUS-TEST: ENTERRRRRRRRRRR");
//             while(spi_active){
                
//                 //signal_edge_detect(clk_pin, clk_event_buffer)
//                 //errorCheck = gpiod_line_request_set_value(gpio_request, HUB_RST_L, GPIOD_LINE_VALUE_ACTIVE);
//                 if(true){

//                     if((miso_buffer[buff_count] >> counter) & 0x01){
//                         printf("HIGH\n");
//                         gpio[GPSET0] = (1 << 9); // Set pin high
//                     }else{
//                         printf("LOW\n");
//                         gpio[GPCLR0] = (1 << 9); // Set pin high
//                     }
//                     if (counter == 0){
//                         counter = 7;
//                         buff_count += 1;
//                     }
//                     counter -= 1;
//                 }

//                 if(buff_count == 10){
//                     spi_active = false;
//                 }
//             }
//         }
//         elaspsedTime = ((double)(clock() - start_time) / CLOCKS_PER_SEC);
//     }while((elaspsedTime < 100) && (!datafound));

// }