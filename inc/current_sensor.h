#ifndef CURRENT_SENSOR_H
#define CURRENT_SENSOR_H

#include <stdint.h>

//INA209 Register Address

#define CURR_REG_CFG            0x00
#define CURR_REG_STATUS         0x01
#define CURR_REG_FLAG_CFG       0x02

#define CURR_REG_SHUNT_VOLT     0x03
#define CURR_REG_BUS_VOLT       0x04
#define CURR_REG_POWER          0x05
#define CURR_REG_CURRENT        0x06

#define CURR_REG_SHT_VOLT_PK_P  0x07
#define CURR_REG_SHT_VOLT_PK_N  0x08
#define CURR_REG_BUS_VOLT_PK_P  0x09
#define CURR_REG_BUS_VOLT_PK_N  0x0A
#define CURR_REG_POWER_PK       0x0B

#define CURR_REG_SHT_VOLT_WRN_P 0x0C
#define CURR_REG_SHT_VOLT_WRN_N 0x0D
#define CURR_REG_POWER_WRN      0x0E
#define CURR_REG_BUS_OVVOLT_WRN 0x0F
#define CURR_REG_BUS_UNVOLT_WRN 0x10

#define CURR_REG_PWR_OVERLIMIT  0x11
#define CURR_REG_BUS_OVERLIMIT  0x12
#define CURR_REG_BUS_UNDERLIMIT 0x13

#define CURR_REG_DAC_POS        0x14
#define CURR_REG_DAC_NEG        0x15
#define CURR_REG_CALIBRATION    0x16

//------- Current Sensor Register Configurations -------//

// Current Sensor Config
// D15  D14  D13       D12-11  D10-7    D6-D3    D2-D0
// RST  N/A  Bus_VRng  VGain   Bus_ADC  Sht_ADC  Mode
#define CURR_REG_CFG_POR_3V3        0b0011000110011111 //(
#define CURR_REG_CFG_POR_5V         0b0011000110011111 //(
#define CURR_REG_CFG_POR_CAM        0b0011000110011111 //(

// Current Sensor Flag Config 
// D15-D05    D4   D3        D2           D1          D0
// Flags_Set  N/A  SMBus_En  Crit_DAC_En  OverLim_En  Warn_En
#define CURR_REG_FLAG_CFG_POR_3V3   0b0010000000000010//0b0010000100000010 //(0x0002)
#define CURR_REG_FLAG_CFG_POR_5V    0b0010000000000010//0b0010000100000010 //(0x0002)
#define CURR_REG_FLAG_CFG_POR_CAM   0b0010000000000010//0b0010000100000010 //(0x0002)

#define SHUNT_RESISTANCE                0.05

// Current Register Conversions Val (A / LSB)
// LSB = round(MAX_CURRENT / 32767)
#define CURR_LSB_VAL_3V3                0.00003   //: 0.98A / 32767 = 0.00003
#define CURR_LSB_VAL_5V                 0.000061  //: 2A / 32767 = 0.000061
#define CURR_LSB_VAL_CAM                0.000061  //: 2A / 32767 = 0.000061

// Power Register Conversion Val (W / LSB)
// LSB = round(CURR_LSB * 20)
#define PWR_LSB_VAL_3V3                 20 * 0.00003   //: 20 * CURR_LSB
#define PWR_LSB_VAL_5V                  20 * 0.000061  //: 20 * CURR_LSB
#define PWR_LSB_VAL_CAM                 20 * 0.000061  //: 20 * CURR_LSB

// Calibration Value
// CAL = round( 0.04096 / (CURR_LSB * 0.05))
#define CURR_REG_CALIBRATION_POR_3V3    (27307 << 1) //: 0.04096 / (CURR_LSB_VAL_3V3 * SHUNT_RESISTANCE)
#define CURR_REG_CALIBRATION_POR_5V     (13430 << 1) //: 0.04096 / (CURR_LSB_VAL_5V  * SHUNT_RESISTANCE)
#define CURR_REG_CALIBRATION_POR_CAM    (13430 << 1) //: 0.04096 / (CURR_LSB_VAL_CAM * SHUNT_RESISTANCE)

// Shunt Voltage Pos Warning
#define CURR_REG_SHT_VOLT_WRN_P_POR_3V3 0x0000
#define CURR_REG_SHT_VOLT_WRN_P_POR_5V  0x0000
#define CURR_REG_SHT_VOLT_WRN_P_POR_CAM 0x0000

// Shunt Voltage Neg Warning
#define CURR_REG_SHT_VOLT_WRN_N_POR_3V3 0x0000
#define CURR_REG_SHT_VOLT_WRN_N_POR_5V  0x0000
#define CURR_REG_SHT_VOLT_WRN_N_POR_CAM 0x0000

// Power Warning
#define CURR_REG_POWER_WRN_POR_3V3      667  // 0.4W 
#define CURR_REG_POWER_WRN_POR_5V       3334 // 4W
#define CURR_REG_POWER_WRN_POR_CAM      4167 // 5W

// Bus OV-Voltage Warning
#define CURR_REG_BUS_OVVOLT_WRN_POR_3V3 0x0000
#define CURR_REG_BUS_OVVOLT_WRN_POR_5V  0x0000
#define CURR_REG_BUS_OVVOLT_WRN_POR_CAM 0x0000

// Bus UV-Voltage Warning
#define CURR_REG_BUS_UVVOLT_WRN_POR_3V3 0x0000
#define CURR_REG_BUS_UVVOLT_WRN_POR_5V  0x0000
#define CURR_REG_BUS_UVVOLT_WRN_POR_CAM 0x0000

// Power Overlimit
#define CURR_REG_PWR_OVERLIMIT_POR_3V3  1250 // 0.75W
#define CURR_REG_PWR_OVERLIMIT_POR_5V   6558 // 4920 // 6W
#define CURR_REG_PWR_OVERLIMIT_POR_CAM  6558 // 8W

// Bus OV-Voltage Limit
#define CURR_REG_BUS_OVERLIMIT_POR_3V3  0xFFF8
#define CURR_REG_BUS_OVERLIMIT_POR_5V   0xFFF8
#define CURR_REG_BUS_OVERLIMIT_POR_CAM  0xFFF8

// BUS UV-Voltage Limit
#define CURR_REG_BUS_UNDERLIMIT_POR_3V3 0x0000
#define CURR_REG_BUS_UNDERLIMIT_POR_5V  0x0000
#define CURR_REG_BUS_UNDERLIMIT_POR_CAM 0x0000

// DAC+ Value
#define CURR_REG_DAC_P_POR_3V3          0xFF00
#define CURR_REG_DAC_P_POR_5V           0xFF00
#define CURR_REG_DAC_P_POR_CAM          0xFF00

// DAC- Value
#define CURR_REG_DAC_N_POR_3V3          0xFFFF
#define CURR_REG_DAC_N_POR_5V           0xFFFF
#define CURR_REG_DAC_N_POR_CAM          0xFFFF


//--- Current Sensor Reg Config Defaults ---//

// 3V3 Current Sensor
#define CURR_SENSOR_REG_DEFAULT_3V3 {CURR_REG_CFG_POR_3V3,            \
                                     CURR_REG_SHT_VOLT_WRN_P_POR_3V3, \
                                     CURR_REG_SHT_VOLT_WRN_N_POR_3V3, \
                                     CURR_REG_POWER_WRN_POR_3V3,      \
                                     CURR_REG_BUS_OVVOLT_WRN_POR_3V3, \
                                     CURR_REG_BUS_UVVOLT_WRN_POR_3V3, \
                                     CURR_REG_PWR_OVERLIMIT_POR_3V3,  \
                                     CURR_REG_BUS_OVERLIMIT_POR_3V3,  \
                                     CURR_REG_BUS_UNDERLIMIT_POR_3V3, \
                                     CURR_REG_DAC_P_POR_3V3,          \
                                     CURR_REG_DAC_N_POR_3V3,          \
                                     CURR_REG_CALIBRATION_POR_3V3,    \
                                     CURR_REG_FLAG_CFG_POR_3V3}

// 5V Current Sensor
#define CURR_SENSOR_REG_DEFAULT_5V  {CURR_REG_CFG_POR_5V,            \
                                     CURR_REG_SHT_VOLT_WRN_P_POR_5V, \
                                     CURR_REG_SHT_VOLT_WRN_N_POR_5V, \
                                     CURR_REG_POWER_WRN_POR_5V,      \
                                     CURR_REG_BUS_OVVOLT_WRN_POR_5V, \
                                     CURR_REG_BUS_UVVOLT_WRN_POR_5V, \
                                     CURR_REG_PWR_OVERLIMIT_POR_5V,  \
                                     CURR_REG_BUS_OVERLIMIT_POR_5V,  \
                                     CURR_REG_BUS_UNDERLIMIT_POR_5V, \
                                     CURR_REG_DAC_P_POR_5V,          \
                                     CURR_REG_DAC_N_POR_5V,          \
                                     CURR_REG_CALIBRATION_POR_5V,    \
                                     CURR_REG_FLAG_CFG_POR_5V}

// CAM Current Sensor
#define CURR_SENSOR_REG_DEFAULT_CAM {CURR_REG_CFG_POR_CAM,            \
                                     CURR_REG_SHT_VOLT_WRN_P_POR_CAM, \
                                     CURR_REG_SHT_VOLT_WRN_N_POR_CAM, \
                                     CURR_REG_POWER_WRN_POR_CAM,      \
                                     CURR_REG_BUS_OVVOLT_WRN_POR_CAM, \
                                     CURR_REG_BUS_UVVOLT_WRN_POR_CAM, \
                                     CURR_REG_PWR_OVERLIMIT_POR_CAM,  \
                                     CURR_REG_BUS_OVERLIMIT_POR_CAM,  \
                                     CURR_REG_BUS_UNDERLIMIT_POR_CAM, \
                                     CURR_REG_DAC_P_POR_CAM,          \
                                     CURR_REG_DAC_N_POR_CAM,          \
                                     CURR_REG_CALIBRATION_POR_CAM,    \
                                     CURR_REG_FLAG_CFG_POR_CAM}

//------- Current Sensor I2C Address -------//
// Note: Error Codes for Sensors are Numerical
//       3V3 ERROR : CURR1
//       5V ERROR  : CURR2
//       CAM ERROR : CURR3

#define CURRENT_SENSOR_ADDR_3V3 0x45
#define CURRENT_SENSOR_ADDR_5V  0x40
#define CURRENT_SENSOR_ADDR_CAM 0x41

//Max Current WARNINGS in Mili-Amps!
#define CURR_3V3_MAX  250  //250mA
#define CURR_5V_MAX   600  //600mA
#define CURR_CAM_MAX  1000 //1000mA


enum IRIS_ERROR current_error_code(uint8_t currAddr, enum IRIS_ERROR errorType);
enum IRIS_ERROR current_setup(uint8_t currAddr);
enum IRIS_ERROR current_func_validate(uint8_t currAddr);
enum IRIS_ERROR current_monitor_reset_trig(uint8_t currAddr);
int current_monitor_status(uint8_t currAddr, uint8_t *errorCount);
uint16_t read_current(uint8_t currAddr);
uint16_t read_power(uint8_t currAddr);
uint16_t read_bus_voltage(uint8_t currAddr);


#endif