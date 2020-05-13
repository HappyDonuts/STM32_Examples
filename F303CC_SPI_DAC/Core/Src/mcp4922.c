/**
  ******************************************************************************
  * @file    mcp4725.c
  * @author  Javier Morales
  * @brief   C file of mcp4725 I2C DAC module functions.
  *
  * https://github.com/HappyDonuts
  ******************************************************************************
  */

#include "mcp4922.h"
#include "math.h"

/**
 * @brief  Creates new mcp4922_t variable corresponding to a mcp4922 module
 * @param  *hspi: SPI peripheral from the mcu
 * @param  addr: I2C address used by the module
 * @retval mcp_t variable corresponding to the mcp module
 */
mcp4922_t* mcp4922_new(SPI_HandleTypeDef *hspi, GPIO_TypeDef* port_CS, uint16_t pin_CS, uint16_t v_ref_A, uint16_t v_ref_B, uint8_t buffered){
	mcp4922_t* mcp4922 = malloc(sizeof(*mcp4922));
	mcp4922_init(mcp4922, hspi, port_CS, pin_CS, v_ref_A, v_ref_B, buffered);
	return mcp4922;
}

/**
 * @brief  Initialization of mcp4725 module
 * @param  *mcp: mcp variable corresponding to the module targeted
 * @param  *hi2c: I2C peripheral from the mcu
 * @param  addr: I2C address used by the module
 * @retval None
 */
void mcp4922_init(mcp4922_t *mcp4922, SPI_HandleTypeDef *hspi, GPIO_TypeDef* port_CS, uint16_t pin_CS, uint16_t v_ref_A, uint16_t v_ref_B, uint8_t buffered){
	mcp4922->hspi = hspi;
	mcp4922->port_CS = port_CS;
	mcp4922->pin_CS = pin_CS;
	mcp4922->v_ref[0] = v_ref_A;
	mcp4922->v_ref[1] = v_ref_B;
	mcp4922->buffered = buffered;
}

/**
 * @brief  Writes a value on the mcp4725 DAC module
 * @param  value: From 0 to 4095 (12 bits) value to be written on the DAC
 * @param  eeprom: If 1, saves the value on eeprom (persistence after reset)
 * @retval None
 */
void mcp4922_write(mcp4922_t *mcp4922, double voltage, uint8_t channel){
	uint16_t data;
	if (voltage > mcp4922->v_ref[channel]){
		data = 4095;
	} else if (voltage < 0){
		data = 0;
	} else {
		double dac_value = round(voltage*4095/mcp4922->v_ref[channel]);
		data = dac_value;
	}

	uint8_t command[2];
	command[0] = 0b00110000 | (channel<<7) | ((mcp4922->buffered)<<6) | (data>>8);
	command[1] = (uint8_t)data;

	HAL_GPIO_WritePin(mcp4922->port_CS, mcp4922->pin_CS, GPIO_PIN_RESET);
	HAL_SPI_Transmit(mcp4922->hspi, command, 2, 10);
	HAL_GPIO_WritePin(mcp4922->port_CS, mcp4922->pin_CS, GPIO_PIN_SET);
}
