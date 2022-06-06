/**
 *  @filename   :   epdif.h
 *  @brief      :   Header file of epdif.c providing EPD interface functions
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

#ifndef EPDIF_H
#define EPDIF_H

#include "main.h"


// Pin level definition
#define LOW             0
#define HIGH            1


/* User: modify pin structure */
typedef struct {
  GPIO_TypeDef* port;
  int pin;
} epd_pin_t;

/* User: modify SPI parameter */
typedef struct epd_handle_t {
	SPI_HandleTypeDef* hspi;
	epd_pin_t* epd_pin_cs;
	epd_pin_t* epd_pin_rst;
	epd_pin_t* epd_pin_dc;
	epd_pin_t* epd_pin_busy;
	int width;
	int height;
//	uint8_t* frame_buffer_black;
//	uint8_t* frame_buffer_red;
} epd_handle_t;

epd_pin_t* epd_pin_new(GPIO_TypeDef* port, uint8_t pin);
void epd_pin_init(epd_pin_t* epd_pin, GPIO_TypeDef* port, uint8_t pin);
epd_handle_t* epd_handle_new(SPI_HandleTypeDef* hspi, epd_pin_t* epd_pin_cs, epd_pin_t* epd_pin_rst, epd_pin_t* epd_pin_dc, epd_pin_t* epd_pin_busy, uint16_t width, uint16_t height);
int epd_handle_init(epd_handle_t* epd, SPI_HandleTypeDef* hspi, epd_pin_t* epd_pin_cs, epd_pin_t* epd_pin_rst, epd_pin_t* epd_pin_dc, epd_pin_t* epd_pin_busy, uint16_t width, uint16_t height);
void EpdDigitalWriteCallback(epd_pin_t* epd_pin, uint8_t value);
int EpdDigitalReadCallback(epd_pin_t* epd_pin);
void EpdDelayMsCallback(uint16_t delaytime);
void EpdSpiTransferCallback(epd_handle_t* epd_handle, uint8_t data);

#endif /* EPDIF_H */
