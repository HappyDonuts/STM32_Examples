/**
 *  @filename   :   epdif.c
 *  @brief      :   Implements EPD interface functions
 *                  Users have to implement all the functions in epdif.cpp
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     July 7 2017
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "epdif.h"
#include <stdlib.h>

/* User: include hardware files */
#include "main.h"

/* User: modify pin structure */
epd_pin_t* epd_pin_new(GPIO_TypeDef* port, uint8_t pin)
{
	epd_pin_t* epd_pin = malloc(sizeof(*epd_pin));
	epd_pin_init(epd_pin, port, pin);
	return epd_pin;
}

/* User: modify pin structure */
void epd_pin_init(epd_pin_t* epd_pin, GPIO_TypeDef* port, uint8_t pin)
{
	epd_pin->port = port;
	epd_pin->pin = pin;
}

/* User: modify SPI parameter */
epd_handle_t* epd_handle_new(SPI_HandleTypeDef* hspi, epd_pin_t* epd_pin_cs, epd_pin_t* epd_pin_rst, epd_pin_t* epd_pin_dc, epd_pin_t* epd_pin_busy, uint16_t width, uint16_t height)
{
	epd_handle_t* epd_handle = malloc(sizeof(*epd_handle));
	epd_handle_init(epd_handle, hspi, epd_pin_cs, epd_pin_rst, epd_pin_dc, epd_pin_busy, width, height);
	return epd_handle;
}

/* User: modify SPI parameter */
int epd_handle_init(epd_handle_t* epd, SPI_HandleTypeDef* hspi, epd_pin_t* epd_pin_cs, epd_pin_t* epd_pin_rst, epd_pin_t* epd_pin_dc, epd_pin_t* epd_pin_busy, uint16_t width, uint16_t height)
{
	epd->hspi = hspi;
	epd->epd_pin_cs = epd_pin_cs;
	epd->epd_pin_rst = epd_pin_rst;
	epd->epd_pin_dc = epd_pin_dc;
	epd->epd_pin_busy = epd_pin_busy;
	epd->width = width;
	epd->height = height;
	return 0;
}

/* User: modify digital write function */
void EpdDigitalWriteCallback(epd_pin_t* epd_pin, uint8_t value)
{
	if (value == HIGH) {
		HAL_GPIO_WritePin(epd_pin->port, epd_pin->pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(epd_pin->port, epd_pin->pin, GPIO_PIN_RESET);
	}
}

/* User: modify digital read function */
int EpdDigitalReadCallback(epd_pin_t* epd_pin)
{
	if (HAL_GPIO_ReadPin(epd_pin->port, epd_pin->pin) == GPIO_PIN_SET) {
		return HIGH;
	} else {
		return LOW;
	}
}

/* User: modify delay function */
void EpdDelayMsCallback(uint16_t delaytime) {
	HAL_Delay(delaytime);
}

/* User: modify SPI transfer and digital write functions */
void EpdSpiTransferCallback(epd_handle_t* epd_handle, uint8_t data)
{
	HAL_GPIO_WritePin(epd_handle->epd_pin_cs->port, epd_handle->epd_pin_cs->pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(epd_handle->hspi, &data, 1, 1000);
	HAL_GPIO_WritePin(epd_handle->epd_pin_cs->port, epd_handle->epd_pin_cs->pin, GPIO_PIN_SET);
}



