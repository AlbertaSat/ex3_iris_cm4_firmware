// sudo apt install libgpiod-dev -  GPIOD C LIBRARY

#include "/usr/include/gpiod.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <gpiod.h>
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gpio.h"
#include "spi_iris.h"

/**
 * @file gpio.c
 * @author Noah Klager
 * @brief GPIO Driver for Theia CM4 
 *        Provides functions to...
 *         - Configure GPIOs for operation
 *            - Bias, Drive, Direction, etc.
 * 
 * @version 0.1
 * @date 2024-11-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/**
 * @brief Modifies specified GPIO's port configuration to requested direction and output value. 
 * This configuration will be permenant even after releasing the 'request' object.
 * 
 * @param chip_path Pointer with File Path to 'chip' object in linux
 * @param offset GPIO pin number offset
 * @param dir Desired GPIO direction (0 = Leave As Is, 1 = Input, 2 = Output)
 * @param outputVal Desired GPIO output value (0 = LOW, 1 = HIGH)
 * @param consumer [Optional] Consumer Name to indicate which process is utilizing GPIO, input NULL if no name is required
 * @return 0 for success, -1 for error
 */
int gpio_config_port(const char *chip_path, int offset, int dir, int outputVal, const char *consumer) {

	struct gpiod_request_config *req_cfg = NULL;
	struct gpiod_line_request *request = NULL;
	struct gpiod_line_settings *settings= NULL;
	struct gpiod_line_config *line_cfg= NULL;
	struct gpiod_chip *chip= NULL;
	int ret;

	settings = gpiod_line_settings_new();
	line_cfg = gpiod_line_config_new();

	if (dir == DIR_IN){
		gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);
	}else if(dir == DIR_OUT){
		gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);
	}

	if (outputVal == VAL_LOW){
		gpiod_line_settings_set_output_value(settings, GPIOD_LINE_VALUE_INACTIVE);
	}else if(outputVal == VAL_HI){
		gpiod_line_settings_set_output_value(settings, GPIOD_LINE_VALUE_ACTIVE);
	}

	chip = gpiod_chip_open(chip_path);
	if (!chip){
		gpiod_line_config_free(line_cfg);
		gpiod_line_settings_free(settings);
		return NULL;
	}

	// Add the settings to the configuration
	ret = gpiod_line_config_add_line_settings(line_cfg, &offset, 1, settings);
	if (ret) {
		gpiod_line_config_free(line_cfg);
		gpiod_chip_close(chip);
		gpiod_line_settings_free(settings);
		return NULL;
	}

	// If 'consumer' label was inputted
	if (consumer) {
		req_cfg = gpiod_request_config_new();
		if (!req_cfg){
			gpiod_request_config_free(req_cfg);
			gpiod_line_config_free(line_cfg);
			gpiod_line_settings_free(settings);
			gpiod_chip_close(chip);
			return NULL;
		}
		gpiod_request_config_set_consumer(req_cfg, consumer);
	}

	// Complete request
	request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);
	gpiod_request_config_free(req_cfg);
	gpiod_line_config_free(line_cfg);
	gpiod_line_settings_free(settings);
	gpiod_chip_close(chip);
	gpiod_line_request_release(request);
	return 0;
}

/**
 * @brief Toggle the CS SPI GPIO, used to initiate a read / write operation
 * 
 * @param cs_request Structure of CS Request Instance
 * @param state Desired state of the CS GPIO either Monitor or Select WR
 * @return Structure of CS Request Instance
 */
struct gpiod_line_request *cs_toggle(struct gpiod_line_request *cs_request, int state){

	struct gpiod_request_config *req_cfg = NULL;
	struct gpiod_line_settings *settings= NULL;
	struct gpiod_line_config *line_cfg= NULL;
	struct gpiod_chip *chip= NULL;
	int ret;
	int offset = SPI_CE_N;

	settings = gpiod_line_settings_new();
	line_cfg = gpiod_line_config_new();

	// Select configuration settings depend on what state to put CS GPIO in
	if (state == CS_MONITOR){
		gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);
		gpiod_line_settings_set_output_value(settings, GPIOD_LINE_VALUE_INACTIVE);
		gpiod_line_settings_set_bias(settings, GPIOD_LINE_BIAS_PULL_UP);
		gpiod_line_settings_set_edge_detection(settings, GPIOD_LINE_EDGE_FALLING);

	}else if (state == CS_RW){
		gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);
		gpiod_line_settings_set_output_value(settings, GPIOD_LINE_VALUE_INACTIVE);
		gpiod_line_settings_set_drive(settings, GPIOD_LINE_DRIVE_OPEN_DRAIN);
		gpiod_line_settings_set_bias(settings, GPIOD_LINE_BIAS_PULL_UP);

	}

	ret = gpiod_line_config_add_line_settings(line_cfg, &offset, 1, settings);
	if (ret) {
		gpiod_line_config_free(line_cfg);
		gpiod_line_settings_free(settings);
		return NULL;
	}

	// Complete request
	gpiod_line_request_reconfigure_lines(cs_request, line_cfg);
	gpiod_line_config_free(line_cfg);
	gpiod_line_settings_free(settings);
	gpiod_chip_close(chip);
	return cs_request;

}


/**
 * @brief Configure a Group of GPIOs using a provided array of settings
 * 
 * @param chip_path Pointer to character array containing path to chip being configured
 * @param numOffsets Number of GPIO's being configured
 * @param offset Pointer to array containing GPIO index numbers being configured
 * @param dir Pointer to array containing Direction State of each GPIO in the group
 * @param outputVal Pointer to array containing Output State of each GPIO in the group
 * @param drive Pointer to array containing Drive State of each GPIO in the group
 * @param bias Pointer to array containing Bias State of each GPIO in the group
 * @param consumer [Optional] Consumer Name to indicate which process is utilizing GPIO, input NULL if no name is required
 * @return Structure of GPIO Request Instance, NULL if an error occured
 */
struct gpiod_line_request *gpio_config_group(const char *chip_path, int numOffsets, int *offset, int *dir, int *outputVal, int *drive, int *bias, const char *consumer) {

	struct gpiod_request_config *req_cfg = NULL;
	struct gpiod_line_request *request = NULL;
	struct gpiod_line_settings *settings= NULL;
	struct gpiod_line_config *line_cfg= NULL;
	struct gpiod_chip *chip= NULL;
	int ret;

	settings = gpiod_line_settings_new();
	line_cfg = gpiod_line_config_new();

	chip = gpiod_chip_open(chip_path);
	if (!chip){
		gpiod_line_config_free(line_cfg);
		gpiod_line_settings_free(settings);
		return NULL;
	}

	for (int index = 0; index < numOffsets; index++){

		if (dir[index] == DIR_IN){
			gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);
		}else if(dir[index] == DIR_OUT){
			gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);
		}

		if (outputVal[index] == VAL_LOW){
			gpiod_line_settings_set_output_value(settings, GPIOD_LINE_VALUE_INACTIVE);
		}else if(outputVal[index] == VAL_HI){
			gpiod_line_settings_set_output_value(settings, GPIOD_LINE_VALUE_ACTIVE);
		}

		if (drive[index] == DRV_PP){
			gpiod_line_settings_set_drive(settings, GPIOD_LINE_DRIVE_PUSH_PULL);
		}else if(drive[index] == DRV_OD){
			gpiod_line_settings_set_drive(settings, GPIOD_LINE_DRIVE_OPEN_DRAIN);
		}else if(drive[index] == DRV_OS){
			gpiod_line_settings_set_drive(settings, GPIOD_LINE_DRIVE_OPEN_SOURCE);
		}

		if (bias[index] == BIAS_DIS){
			gpiod_line_settings_set_bias(settings, GPIOD_LINE_BIAS_DISABLED);
		}else if(bias[index] == BIAS_PU){
			gpiod_line_settings_set_bias(settings, GPIOD_LINE_BIAS_PULL_UP);
		}else if(bias[index] == BIAS_PD){
			gpiod_line_settings_set_bias(settings, GPIOD_LINE_BIAS_PULL_DOWN);
		}

		ret = gpiod_line_config_add_line_settings(line_cfg, offset + index, 1, settings);
		if (ret) {
			gpiod_line_config_free(line_cfg);
			gpiod_chip_close(chip);
			gpiod_line_settings_free(settings);
			return NULL;
		}
	}

	// If 'consumer' label was inputted
	if (consumer) {
		req_cfg = gpiod_request_config_new();
		if (!req_cfg){
			gpiod_request_config_free(req_cfg);
			gpiod_line_config_free(line_cfg);
			gpiod_line_settings_free(settings);
			gpiod_chip_close(chip);
			return NULL;
		}
		gpiod_request_config_set_consumer(req_cfg, consumer);
	}

	// Complete request
	request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);
	gpiod_request_config_free(req_cfg);
	gpiod_line_config_free(line_cfg);
	gpiod_line_settings_free(settings);
	gpiod_chip_close(chip);
	return request;
}


/**
 * @brief Configure a GPIO as an input with detection settings
 * 
 * @param chip_path Pointer to character array containing path to chip being configured
 * @param offset Index of GPIO being configured as an input
 * @param edgeDetect Type of edge detection being used on the input GPIO
 * @param consumer [Optional] Consumer Name to indicate which process is utilizing GPIO, input NULL if no name is required
 * @return Structure of GPIO Request Instance, NULL if an error occured
 */
struct gpiod_line_request *gpio_config_input_detect(const char *chip_path, int offset, int edgeDetect, const char *consumer) {

	struct gpiod_request_config *req_cfg = NULL;
	struct gpiod_line_request *request = NULL;
	struct gpiod_line_settings *settings= NULL;
	struct gpiod_line_config *line_cfg= NULL;
	struct gpiod_chip *chip= NULL;
	int ret;

	settings = gpiod_line_settings_new();
	line_cfg = gpiod_line_config_new();

	gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);
	gpiod_line_settings_set_output_value(settings, GPIOD_LINE_VALUE_INACTIVE);
	gpiod_line_settings_set_bias(settings, GPIOD_LINE_BIAS_PULL_UP);

	if (edgeDetect == EDGE_RIS){
		gpiod_line_settings_set_edge_detection(settings, GPIOD_LINE_EDGE_RISING);
	}else if(edgeDetect == EDGE_FALL){
		gpiod_line_settings_set_edge_detection(settings, GPIOD_LINE_EDGE_FALLING);
	}else if(edgeDetect == EDGE_BOTH){
		gpiod_line_settings_set_edge_detection(settings, GPIOD_LINE_EDGE_BOTH);
	}

	chip = gpiod_chip_open(chip_path);
	if (!chip){
		gpiod_line_config_free(line_cfg);
		gpiod_line_settings_free(settings);
		return NULL;
	}

	// Add the settings to the configuration
	ret = gpiod_line_config_add_line_settings(line_cfg, &offset, 1, settings);
	if (ret) {
		gpiod_line_config_free(line_cfg);
		gpiod_chip_close(chip);
		gpiod_line_settings_free(settings);
		return NULL;
	}

	// If 'consumer' label was inputted
	if (consumer) {
		req_cfg = gpiod_request_config_new();
		if (!req_cfg){
			gpiod_request_config_free(req_cfg);
			gpiod_line_config_free(line_cfg);
			gpiod_line_settings_free(settings);
			gpiod_chip_close(chip);
			return NULL;
		}
		gpiod_request_config_set_consumer(req_cfg, consumer);
	}

	// Complete request
	request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);

	gpiod_request_config_free(req_cfg);
	gpiod_line_config_free(line_cfg);
	gpiod_line_settings_free(settings);
	gpiod_chip_close(chip);
	return request;
}