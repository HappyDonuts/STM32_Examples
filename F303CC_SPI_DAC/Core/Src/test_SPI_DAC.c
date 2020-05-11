/*
 * test_SPI_DAC.c
 *
 *  Created on: May 11, 2020
 *      Author: Javi
 */

/* INCLUDES -------------------------------- */
#include "test_SPI_DAC.h"
/* PERIPHERALS ----------------------------- */
extern SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef* spi = &hspi1;

GPIO_TypeDef* PORT_CS = GPIOB;
uint16_t PIN_CS = 0;
/* DEFINES  -------------------------------- */

/* TYPEDEF --------------------------------- */

/* VARIABLES ------------------------------- */

/* FUNCTION PROTOTYPES --------------------- */


/**
  * @brief  The application entry point.
  * @retval none
  */
void main_s(void){
	/* MAIN CODE */
	

	while(1){
		/* WHILE CODE */
		
	}
}

/**
  * @brief  EXTI line detection callbacks.
  * @retval none
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	/* GPIO CALLBACK CODE */
	
}
