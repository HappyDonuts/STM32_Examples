/*
 * bme280_handler.c
 *
 *  Created on: Aug 11, 2021
 *      Author: Javier.MORALES
 */

#include "bme280_handler.h"

extern I2C_HandleTypeDef hi2c1;

struct bme280_dev dev;
struct bme280_data comp_data;
int8_t rslt;

/**
 * @brief  Creates kim_handler_t variable
 * @param  *hrtc: RTC peripheral
 * @param  *huart: UART peripheral used for logging (can be NULL)
 * @retval kim_handler_t pointer to kim_handler structure
 */
bme280_handle_t* bme280_handle_new(void)
{
	bme280_handle_t* bme280_handle = malloc(sizeof(*bme280_handle));
	bme280_handle_init(bme280_handle);
	return bme280_handle;
}

/**
 * @brief  Initializes kim handler struct
 * @param  *kim_handler: pointer to kim_handler structure
 * @param  *hrtc: RTC peripheral
 * @param  *huart: UART peripheral used for logging (can be NULL)
 * @retval none
 */
void bme280_handle_init(bme280_handle_t* bme280_handle)
{
	bme280_handle->sensor_error = 0;
	dev.dev_id = BME280_I2C_ADDR_PRIM;
	dev.intf = BME280_I2C_INTF;
	dev.read = bme280h_user_i2c_read;
	dev.write = bme280h_user_i2c_write;
	dev.delay_ms = bme280h_user_delay_ms;

	rslt = bme280_init(&dev);

	/* BME280 설정 */
	dev.settings.osr_h = BME280_OVERSAMPLING_1X;
	dev.settings.osr_p = BME280_OVERSAMPLING_16X;
	dev.settings.osr_t = BME280_OVERSAMPLING_2X;
	dev.settings.filter = BME280_FILTER_COEFF_16;
	rslt = bme280_set_sensor_settings(BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL, &dev);
}

int8_t bme280h_user_i2c_read(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
  if(HAL_I2C_Master_Transmit(&hi2c1, (id << 1), &reg_addr, 1, 10) != HAL_OK) return -1;
  if(HAL_I2C_Master_Receive(&hi2c1, (id << 1) | 0x01, data, len, 10) != HAL_OK) return -1;

  return 0;
}

void bme280h_user_delay_ms(uint32_t period)
{
  HAL_Delay(period);
}

int8_t bme280h_user_i2c_write(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
  int8_t *buf;
  buf = malloc(len +1);
  buf[0] = reg_addr;
  memcpy(buf +1, data, len);

  if(HAL_I2C_Master_Transmit(&hi2c1, (id << 1), (uint8_t*)buf, len + 1, HAL_MAX_DELAY) != HAL_OK) return -1;

  free(buf);
  return 0;
}

void bme280h_get_data(bme280_handle_t* bme280_handle)
{
	rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, &dev);
	dev.delay_ms(40);

	rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, &dev);
	if(rslt == BME280_OK)
	{
		bme280_handle->temperature = comp_data.temperature / 100.0;      /* °C  */
		bme280_handle->humidity = comp_data.humidity / 1024.0;           /* %   */
		bme280_handle->pressure = comp_data.pressure / 10000.0;          /* hPa */
	} else {
		bme280_handle->sensor_error = 1;
	}

}
