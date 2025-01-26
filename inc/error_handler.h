#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

//----- Temperature Sensor -----//
// Temperature General ERROR Flags
#define TMP1_MAX_ERROR_FLG (1 << 0)
#define TMP2_MAX_ERROR_FLG (1 << 1)
#define TMP3_MAX_ERROR_FLG (1 << 2)
#define TMP4_MAX_ERROR_FLG (1 << 3)
#define TMP1_GEN_ERROR_FLG (1 << 4)
#define TMP2_GEN_ERROR_FLG (1 << 5)
#define TMP3_GEN_ERROR_FLG (1 << 6)
#define TMP4_GEN_ERROR_FLG (1 << 7)

// Temperature Specific Error Flags
#define TMP_I2C_BUS_ERROR_FLG 0
#define TMP_CFG1_ERROR_FLG    1
#define TMP_CFG2_ERROR_FLG    2
#define TMP_DIODE_ERROR_FLG   3


typedef enum IRIS_ERROR{
    NO_ERROR,

    I2C_SETUP_ERROR,
    I2C_WRITE_ERROR,
    I2C_READ_ERROR,
    I2C_WR_R_ERROR,

    SPI_SETUP_ERROR = 5000, //Need it to out of range for spi_dev
    SPI_WRITE_ERROR,
    SPI_READ_ERROR,
    SPI_TEST_ERROR,
    SPI_FILE_WRITE_ERROR,

    GPIO_SETUP_ERROR,

    TEMP1_SETUP_ERROR,
    TEMP2_SETUP_ERROR,
    TEMP3_SETUP_ERROR,
    TEMP4_SETUP_ERROR,
    TEMP1_VERIFICATION_ERROR,
    TEMP2_VERIFICATION_ERROR,
    TEMP3_VERIFICATION_ERROR,
    TEMP4_VERIFICATION_ERROR,
    TEMP1_RESET_ERROR,
    TEMP2_RESET_ERROR,
    TEMP3_RESET_ERROR,
    TEMP4_RESET_ERROR,
    TEMP1_TEMP_READ_ERROR = 1000,
    TEMP2_TEMP_READ_ERROR = 1001,
    TEMP3_TEMP_READ_ERROR = 1002,
    TEMP4_TEMP_READ_ERROR = 1003,

    TEMP1_LIMIT_ERROR,
    TEMP2_LIMIT_ERROR,
    TEMP3_LIMIT_ERROR,
    TEMP4_LIMIT_ERROR,

    CURR1_SETUP_ERROR,
    CURR2_SETUP_ERROR,
    CURR3_SETUP_ERROR,
    CURR1_VERIFICATION_ERROR,
    CURR2_VERIFICATION_ERROR,
    CURR3_VERIFICATION_ERROR,
    CURR1_RESET_ERROR,
    CURR2_RESET_ERROR,
    CURR3_RESET_ERROR,
    CURR1_VAL_READ_ERROR,
    CURR2_VAL_READ_ERROR,
    CURR3_VAL_READ_ERROR,

    CURR1_LIMIT_ERROR,
    CURR2_LIMIT_ERROR,
    CURR3_LIMIT_ERROR,

    USB_HUB_SETUP_ERROR,
    USB_HUB_VERIFICATION_ERROR,
    USB_HUB_RESET_ERROR
    
} IRIS_ERROR;


//------------ GPIO ------------//






//---------- I2C BUS -----------//


//---------- SPI BUS -----------//

//---------- USB HUB -----------//

//------- CURRENT SENSOR -------//

//-------- IRIS GENERAL --------//

#endif //ERROR_HANDLER