/**
  ******************************************************************************
  * @file    mcp4922.h
  * @author  Javier Morales
  * @brief   Header file of mcp4922 SPI DAC module.
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

mcp4922_t* mcp4922_new(SPI_HandleTypeDef *hspi, GPIO_TypeDef* port_CS, uint16_t pin_CS, uint16_t v_ref_A, uint16_t v_ref_B, uint8_t buffered);

void mcp4922_init(mcp4922_t *mcp4922, SPI_HandleTypeDef *hspi, GPIO_TypeDef* port_CS, uint16_t pin_CS, uint16_t v_ref_A, uint16_t v_ref_B, uint8_t buffered);

void mcp4922_write(mcp4922_t *mcp4922, double voltage, uint8_t channel);
