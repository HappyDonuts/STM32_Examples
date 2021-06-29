#ifndef LCD_I2C_RTOS_H
#define LCD_I2C_RTOS_H

#include "main.h"
#include <stdlib.h>

/*
 *  HD44780 datasheet
 */

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En B00000100  // Enable bit
#define Rw B00000010  // Read/Write bit
#define Rs B00000001  // Register select bit

#define BUFFER_SIZE 40

#define MODE_DATA 0
#define MODE_COMMAND 1

typedef struct lcd_message_t{
	uint8_t message;
	uint8_t exe_time;
	uint8_t mode;
}lcd_message_t;

/**
 * @brief  SSD1306 struct
 */
typedef struct lcd_i2c_RTOS_t{
	uint8_t addr;
	I2C_HandleTypeDef *hi2c;
	uint8_t n_chars;
	uint8_t n_lines;
	uint8_t display_control;
	uint8_t display_mode;
	lcd_message_t *buffer_messages[BUFFER_SIZE];
	uint8_t buffer_index;
	uint8_t n_messages;
	uint8_t empty;

} lcd_i2c_RTOS_t;

lcd_message_t* lcd_message_new(uint8_t message, uint8_t exe_time, uint8_t mode);

/**
 * @brief  Creates lcd_i2c_t variable corresponding to the display
 * @param  *hi2c: I2C peripheral used by the mcu
 * @param  addr: I2C address corresponding to the targeted display
 * @retval lcd_i2c_t variable targeting the desired display
 */
lcd_i2c_RTOS_t* lcd_i2c_RTOS_new(I2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t n_chars, uint8_t n_lines);

/**
 * @brief  Initializes Liquid Crystal LCD display
 * @param  *hi2c: I2C peripheral used by the mcu
 * @param  addr: I2C address corresponding to the targeted display
 * @retval Initialization status:
 *           - 0: LCD was not detected on I2C port
 *           - 1: LCD initialized OK and ready to use
 */
uint8_t lcd_i2c_RTOS_Init(lcd_i2c_RTOS_t* lcd_i2c_RTOS, I2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t n_chars, uint8_t n_lines);

uint8_t lcd_i2c_RTOS_Transmit(lcd_i2c_RTOS_t* lcd_i2c_RTOS, uint8_t mode);

uint8_t lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS_t* lcd_i2c_RTOS, char cmd, uint8_t exe_time);

uint8_t lcd_i2c_RTOS_send_data (lcd_i2c_RTOS_t* lcd_i2c_RTOS, char data);  // send data to the lcd

//void lcd_i2c_RTOS_send_string (lcd_i2c_RTOS_t* lcd_i2c_RTOS, char *str);

uint8_t lcd_i2c_RTOS_Write(lcd_i2c_RTOS_t* lcd_i2c_RTOS, uint8_t line, uint8_t col, char *str);

void lcd_i2c_RTOS_Display_On(lcd_i2c_RTOS_t* lcd_i2c_RTOS);

void lcd_i2c_RTOS_Display_Off(lcd_i2c_RTOS_t* lcd_i2c_RTOS);

void lcd_i2c_RTOS_Cursor_On(lcd_i2c_RTOS_t* lcd_i2c_RTOS);

void lcd_i2c_RTOS_Cursor_Off(lcd_i2c_RTOS_t* lcd_i2c_RTOS);

void lcd_i2c_RTOS_Cursor_Blink_On(lcd_i2c_RTOS_t* lcd_i2c_RTOS);

void lcd_i2c_RTOS_Cursor_Blink_Off(lcd_i2c_RTOS_t* lcd_i2c_RTOS);

void lcd_i2c_RTOS_Scroll_Left(lcd_i2c_RTOS_t* lcd_i2c_RTOS);

void lcd_i2c_RTOS_Scroll_Right(lcd_i2c_RTOS_t* lcd_i2c_RTOS);

void lcd_i2c_RTOS_Left_To_Right(lcd_i2c_RTOS_t* lcd_i2c_RTOS);

void lcd_i2c_RTOS_Right_To_Left(lcd_i2c_RTOS_t* lcd_i2c_RTOS);

void lcd_i2c_RTOS_Autoscroll_On(lcd_i2c_RTOS_t* lcd_i2c_RTOS);

void lcd_i2c_RTOS_Autoscroll_Off(lcd_i2c_RTOS_t* lcd_i2c_RTOS);

void lcd_i2c_RTOS_clear (lcd_i2c_RTOS_t* lcd_i2c_RTOS);

void lcd_i2c_RTOS_Reset_Poistion(lcd_i2c_RTOS_t* lcd_i2c_RTOS);


#endif
