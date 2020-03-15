/*
 * breathing_LED.c
 *
 *  Created on: Mar 12, 2020
 *      Author: Javi
 */

/* INCLUDES -------------------------------- */
#include "breathing_LED.h"
/* PERIPHERALS ----------------------------- */
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

TIM_HandleTypeDef* tim_led = &htim2;
TIM_HandleTypeDef* tim_mod = &htim3;
/* DEFINES  -------------------------------- */

/* TYPEDEF --------------------------------- */

/* VARIABLES ------------------------------- */
uint16_t duty_cycle = 0;
uint8_t size;

const uint16_t sinwave[] = {32768,34825,36874,38908,40916,42893,44830,46719,
		48553,50325,52028,53654,55198,56654,58015,59277,
		60434,61482,62416,63234,63931,64506,64955,65277,
		65470,65535,65470,65277,64955,64506,63931,63234,
		62416,61482,60434,59277,58015,56654,55198,53654,
		52028,50325,48553,46719,44830,42893,40916,38908,
		36874,34825,32768,30710,28661,26627,24619,22642,
		20705,18816,16982,15210,13507,11881,10337,8881,
		7520,6258,5101,4053,3119,2301,1604,1029,
		580,258,65,0,65,258,580,1029,
		1604,2301,3119,4053,5101,6258,7520,8881,
		10337,11881,13507,15210,16982,18816,20705,22642,
		24619,26627,28661,30710,32768};

/* FUNCTION PROTOTYPES --------------------- */


/**
  * @brief  The application entry point.
  * @retval none
  */
void main_s(void){
	/* MAIN CODE */
	size = 99;
	HAL_TIM_Base_Start_IT(tim_led);
	HAL_TIM_Base_Start_IT(tim_mod);
	
	while(1){
		/* WHILE CODE */
		
	}
}

/**
  * @brief  Period elapsed callback.
  * @retval none
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	/* TIM CALLBACK CODE */
	if (htim == tim_mod){
		static uint8_t index = 0;
		duty_cycle = sinwave[index]*100/65535;
		if (index == size){
			index = 0;
		} else {
			index++;
		}
	}
	
	if (htim == tim_led){
		static uint16_t counter = 0;
		if (counter < duty_cycle){
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 0);
		} else {
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 1);
		}
		if (counter == size){
			counter = 0;
		} else {
			counter++;
		}
	}
}

/**
  * @brief  EXTI line detection callbacks.
  * @retval none
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	/* GPIO CALLBACK CODE */
	
}
