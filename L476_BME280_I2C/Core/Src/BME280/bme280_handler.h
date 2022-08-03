/*
 * bme280_handler.h
 *
 *  Created on: Aug 11, 2021
 *      Author: Javier.MORALES
 */

#ifndef SRC_BME280_BME280_HANDLER_H_
#define SRC_BME280_BME280_HANDLER_H_

#include <stdlib.h>
#include <string.h>
#include "bme280.h"
#include "main.h"


typedef struct bme280_handle_t{
	float temperature;
	float humidity;
	float pressure;
	uint8_t sensor_error;
} bme280_handle_t;


bme280_handle_t* bme280_handle_new(void);

void bme280_handle_init(bme280_handle_t* bme280_handle);

int8_t bme280h_user_i2c_read(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len);

void bme280h_user_delay_ms(uint32_t period);

int8_t bme280h_user_i2c_write(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len);

void bme280h_get_data(bme280_handle_t* bme280_handle);

#endif /* SRC_BME280_BME280_HANDLER_H_ */
