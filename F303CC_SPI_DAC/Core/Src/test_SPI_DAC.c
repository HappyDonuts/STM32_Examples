/*
 * test_SPI_DAC.c
 *
 *  Created on: May 11, 2020
 *      Author: Javi
 */

/* INCLUDES -------------------------------- */
#include "test_SPI_DAC.h"
#include "mcp4922.h"
/* PERIPHERALS ----------------------------- */
extern SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef* spi = &hspi1;

GPIO_TypeDef* PORT_CS = GPIOB;
uint16_t PIN_CS = GPIO_PIN_0;
/* DEFINES  -------------------------------- */

/* TYPEDEF --------------------------------- */

/* VARIABLES ------------------------------- */
mcp4922_t* dac_1;
/* FUNCTION PROTOTYPES --------------------- */
void setDAC(uint16_t data, uint8_t channel);

/**
  * @brief  The application entry point.
  * @retval none
  */
void main_s(void){
	/* MAIN CODE */
	dac_1 = mcp4922_new(spi, PORT_CS, PIN_CS, 3300, 3300, 1);
//	mcp4922_write(dac_1, 1500, 0);

	while(1){
		/* WHILE CODE */
		mcp4922_write(dac_1, 1500, 0);
		HAL_Delay(1);
		mcp4922_write(dac_1, 0, 0);
		HAL_Delay(1);
	}
}

