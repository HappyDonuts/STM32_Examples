/**
  ******************************************************************************
  * @file    mcp4922.c
  * @author  Javier Morales
  * @brief   C file of mcp4922 SPI DAC module functions.
  *
  * https://github.com/HappyDonuts
  ******************************************************************************
  */

#include "main.h"
#include <stdlib.h>

typedef struct mcp4922_t {
	SPI_HandleTypeDef *hspi;
	GPIO_TypeDef* port_CS;
	uint16_t pin_CS;
	uint16_t v_ref[2];
	uint8_t buffered;
} mcp4922_t;

/**
 * @brief  Creates new mcp4922_t variable corresponding to a mcp4922 module
 * @param  *hspi: SPI peripheral from the mcu
 * @param  port_CS: GPIO port corresponding to CS pin
 * @param  pin_CS: Pin number corresponding to CS pin
 * @param  v_ref_A: Reference voltage channel A (mV)
 * @param  v_ref_B: Reference voltage channel B (mV)
 * @param  buffered: 1: Buffered output	0: Unbuffered output
 * @retval mcp4922_t variable corresponding to the mcp4299 module
 */
mcp4922_t* mcp4922_new(SPI_HandleTypeDef *hspi, GPIO_TypeDef* port_CS, uint16_t pin_CS, uint16_t v_ref_A, uint16_t v_ref_B, uint8_t buffered);

/**
 * @brief  Initialization of mcp4922 module
 * @param  *mcp4922: mcp4922 variable corresponding to the module targeted
 * @param  *hspi: SPI peripheral from the mcu
 * @param  port_CS: GPIO port corresponding to CS pin
 * @param  pin_CS: Pin number corresponding to CS pin
 * @param  v_ref_A: Reference voltage channel A (mV)
 * @param  v_ref_B: Reference voltage channel B (mV)
 * @param  buffered: 1: Buffered output	0: Unbuffered output
 * @retval None
 */
void mcp4922_init(mcp4922_t *mcp4922, SPI_HandleTypeDef *hspi, GPIO_TypeDef* port_CS, uint16_t pin_CS, uint16_t v_ref_A, uint16_t v_ref_B, uint8_t buffered);

/**
 * @brief  Writes a value on the mcp4922 DAC module
 * @param  voltage: Voltage to be output by the DAC (mV)
 * @param  channel: 0: Channel A	1: Channel B
 * @retval None
 */
void mcp4922_write(mcp4922_t *mcp4922, double voltage, uint8_t channel);
