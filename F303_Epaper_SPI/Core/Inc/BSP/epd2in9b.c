/**
 *  @filename   :   epd2in9b.c
 *  @brief      :   Implements for Dual-color e-paper library
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     July 13 2017
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

#include "epd2in9b.h"

int EPD_Init(epd_handle_t* epd_handle) {

	/* EPD hardware init start */
	EPD_Reset(epd_handle);
	EPD_SendCommand(epd_handle, BOOSTER_SOFT_START);
	EPD_SendData(epd_handle, 0x17);
	EPD_SendData(epd_handle, 0x17);
	EPD_SendData(epd_handle, 0x17);
	EPD_SendCommand(epd_handle, POWER_ON);
	EPD_WaitUntilIdle(epd_handle);
	EPD_SendCommand(epd_handle, PANEL_SETTING);
	EPD_SendData(epd_handle, 0x8F);
	EPD_SendCommand(epd_handle, VCOM_AND_DATA_INTERVAL_SETTING);
	EPD_SendData(epd_handle, 0x77);
	EPD_SendCommand(epd_handle, TCON_RESOLUTION);
	EPD_SendData(epd_handle, 0x80);
	EPD_SendData(epd_handle, 0x01);
	EPD_SendData(epd_handle, 0x28);
	EPD_SendCommand(epd_handle, VCM_DC_SETTING_REGISTER);
	EPD_SendData(epd_handle, 0X0A);
	/* EPD hardware init end */
	return 0;
}

/**
 *  @brief: this calls the corresponding function from epdif.h
 *          usually there is no need to change this function
 */
void EPD_DigitalWrite(epd_pin_t* epd_pin, uint8_t value)
{
	EpdDigitalWriteCallback(epd_pin, value);
}

/**
 *  @brief: this calls the corresponding function from epdif.h
 *          usually there is no need to change this function
 */
int EPD_DigitalRead(epd_pin_t* epd_pin)
{
	return EpdDigitalReadCallback(epd_pin);
}

/**
 *  @brief: this calls the corresponding function from epdif.h
 *          usually there is no need to change this function
 */
void EPD_DelayMs(uint16_t delaytime)
{  // 1ms
	EpdDelayMsCallback(delaytime);
}

/**
 *  @brief: basic function for sending commands
 */
void EPD_SendCommand(epd_handle_t* epd_handle, uint8_t command) {
	EPD_DigitalWrite(epd_handle->epd_pin_dc, LOW);
	EpdSpiTransferCallback(epd_handle, command);
}

/**
 *  @brief: basic function for sending data
 */
void EPD_SendData(epd_handle_t* epd_handle, uint8_t data) {
	EPD_DigitalWrite(epd_handle->epd_pin_dc, HIGH);
	EpdSpiTransferCallback(epd_handle, data);
}

/**
 *  @brief: Wait until the busy_pin goes HIGH
 */
void EPD_WaitUntilIdle(epd_handle_t* epd_handle) {
	while(EPD_DigitalRead(epd_handle->epd_pin_busy) == 0) {      //0: busy, 1: idle
	EPD_DelayMs(100);
	}
}

/**
 *  @brief: module reset.
 *          often used to awaken the module in deep sleep,
 *          see EPD::Sleep();
 */
void EPD_Reset(epd_handle_t* epd_handle) {
	EPD_DigitalWrite(epd_handle->epd_pin_rst, LOW);                //module reset
	EPD_DelayMs(200);
	EPD_DigitalWrite(epd_handle->epd_pin_rst, HIGH);
	EPD_DelayMs(200);
}

void EPD_DisplayFrame(epd_handle_t* epd_handle, const uint8_t* frame_buffer_black, const uint8_t* frame_buffer_red)
{
	if (frame_buffer_black != NULL) {
		EPD_SendCommand(epd_handle, DATA_START_TRANSMISSION_1);
		EPD_DelayMs(2);
		for (int i = 0; i < epd_handle->width * epd_handle->height / 8; i++) {
			EPD_SendData(epd_handle, frame_buffer_black[i]);
		}
		EPD_DelayMs(2);
	}
	if (frame_buffer_red != NULL) {
		EPD_SendCommand(epd_handle, DATA_START_TRANSMISSION_2);
		EPD_DelayMs(2);
		for (int i = 0; i < epd_handle->width * epd_handle->height / 8; i++) {
			EPD_SendData(epd_handle, frame_buffer_red[i]);
		}
		EPD_DelayMs(2);
	}
	EPD_SendCommand(epd_handle, DISPLAY_REFRESH);
	EPD_WaitUntilIdle(epd_handle);
}

/* After this command is transmitted, the chip would enter the deep-sleep mode to save power. 
   The deep sleep mode would return to standby by hardware reset. The only one parameter is a 
   check code, the command would be executed if check code = 0xA5. 
   You can use EPD_Reset() to awaken and EPD_Init() to initialize */
void EPD_Sleep(epd_handle_t* epd_handle) {
	EPD_SendCommand(epd_handle, VCM_DC_SETTING_REGISTER);         //to solve Vcom drop
	EPD_SendData(epd_handle, 0x00);
	EPD_SendCommand(epd_handle, POWER_SETTING);         //power setting
	EPD_SendData(epd_handle, 0x02);        //gate switch to external
	EPD_SendData(epd_handle, 0x00);
	EPD_SendData(epd_handle, 0x00);
	EPD_SendData(epd_handle, 0x00);
	EPD_WaitUntilIdle(epd_handle);
	EPD_SendCommand(epd_handle, POWER_OFF);         //power off
}

/* END OF FILE */

