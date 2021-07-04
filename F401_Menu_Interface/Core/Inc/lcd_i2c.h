#ifndef LCD_I2C_H
#define LCD_I2C_H

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


/**
 * @brief  lcd_i2c_RTOS_t struct
 */
typedef struct lcd_i2c_t{
	uint8_t addr;
	I2C_HandleTypeDef *hi2c;
	uint8_t n_chars;
	uint8_t n_lines;
	uint8_t display_control;
	uint8_t display_mode;
} lcd_i2c_t;


/**	--- Initialization functions --- **/


/**
 * @brief  Creates lcd_i2c variable corresponding to the display
 * @note   Execution time is 81 ms (lcd_i2c_Init function)
 * @param  *hi2c: I2C peripheral used by the mcu
 * @param  addr: I2C address corresponding to the targeted display
 * @param  n_chars: number of the diaplay columns, or chars (16 or 20)
 * @param  n_lines: number of the diaplay lines (usually 1, 2, or 4)
 * @retval lcd_i2c variable targeting the desired display
 */
lcd_i2c_t* lcd_i2c_new(I2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t n_chars, uint8_t n_lines);


/**	--- Auxiliary functions --- **/


/**
 * @brief  Initializes Liquid Crystal LCD display
 * @note   Execution time is 81 ms
 * @param  lcd_i2c variable targeting the desired display
 * @param  *hi2c: I2C peripheral used by the MCU
 * @param  addr: I2C address corresponding to the targeted display
 * @param  n_chars: number of the display columns, or chars (16 or 20)
 * @param  n_lines: number of the display lines (usually 1, 2, or 4)
 * @retval Initialization status:
 *           - 0: LCD was not detected on I2C port
 *           - 1: LCD initialized OK and ready to use
 */
uint8_t lcd_i2c_Init(lcd_i2c_t* lcd_i2c, I2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t n_chars, uint8_t n_lines);

/**
 * @brief  Transmits a command through I2C protocol
 * @note   Execution time is considered 0
 * @param  lcd_i2c: variable targeting the desired display
 * @param  cmd: data byte to be added to the buffer
 * @retval None
 */
void lcd_send_cmd (lcd_i2c_t* lcd_i2c, char cmd);  // send command to the lcd

/**
 * @brief  Transmits a data byte through I2C protocol
 * @note   Execution time is considered 0
 * @param  lcd_i2c: variable targeting the desired display
 * @param  data: data byte to be added to the buffer
 * @retval None
 */
void lcd_i2c_send_data (lcd_i2c_t* lcd_i2c, char data);  // send data to the lcd


/**	--- Interface functions --- **/


/**
 * @brief  Send a string to the display, so it can be displayed on a specific position of the display
 * @param  lcd_i2c: variable targeting the desired display
 * @param  line: line at which the string will be displayed (1, 2, or 4)
 * @param  col: column at which the string will start to be displayed (under 16 or under 20)
 * @retval Write status:
 * 				0: line or column out of the display
 * 				1: string sent correctly
 */
uint8_t lcd_i2c_Write(lcd_i2c_t* lcd_i2c, uint8_t line, uint8_t col, char *str);

/**
 * @brief  Turns on the display
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_Display_On(lcd_i2c_t* lcd_i2c);

/**
 * @brief  Turns off the display
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_Display_Off(lcd_i2c_t* lcd_i2c);

/**
 * @brief  Turns the underline cursor on the display
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_Cursor_On(lcd_i2c_t* lcd_i2c);

/**
 * @brief  Turns the underline cursor off the display
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_Cursor_Off(lcd_i2c_t* lcd_i2c);

/**
 * @brief  Turns on the blinking cursor
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_Cursor_Blink_On(lcd_i2c_t* lcd_i2c);

/**
 * @brief  Turns off the blinking cursor
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_Cursor_Blink_Off(lcd_i2c_t* lcd_i2c);

/**
 * @brief  Scrolls the display left without changing the RAM
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_Scroll_Left(lcd_i2c_t* lcd_i2c);

/**
 * @brief  Scrolls the display right without changing the RAM
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_Scroll_Right(lcd_i2c_t* lcd_i2c);

/**
 * @brief  Sets the entry mode to the right
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_Left_To_Right(lcd_i2c_t* lcd_i2c);

/**
 * @brief  Sets the entry mode to the left
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_Right_To_Left(lcd_i2c_t* lcd_i2c);

/**
 * @brief  Justifies the text to the right
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_Autoscroll_On(lcd_i2c_t* lcd_i2c);

/**
 * @brief  Justifies the text to the left
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_Autoscroll_Off(lcd_i2c_t* lcd_i2c);

/**
 * @brief  Clears the display removing all characters and resets position of the DDRAM
 * @note   Needs 1.52 ms of execution time
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_Clear (lcd_i2c_t* lcd_i2c);

/**
 * @brief  Resets the position of the cursor to its initial state
 * @note   Needs 1.52 ms of execution time
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_Reset_Poistion(lcd_i2c_t* lcd_i2c);


#endif
