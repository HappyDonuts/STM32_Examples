#ifndef LCD_I2C_RTOS_H
#define LCD_I2C_RTOS_H

#include "main.h"
#include <stdlib.h>
#include "cmsis_os.h"
#include <math.h>

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

/**
 * @brief  lcd_message_t struct
 */
typedef struct lcd_message_t{
	uint8_t message;
	uint8_t exe_time;
	uint8_t mode;
	lcd_message_t* next_msg;
}lcd_message_t;


/**
 * @brief  lcd_i2c_RTOS_t struct
 */
typedef struct lcd_i2c_RTOS_t{
	uint8_t addr;
	I2C_HandleTypeDef *hi2c;
	uint8_t n_chars;
	uint8_t n_lines;
	uint8_t display_control;
	uint8_t display_mode;
	lcd_message_t* buffer_messages;
	uint8_t buffer_index;
	uint8_t empty;
	osThreadId_t thread_id;
} lcd_i2c_RTOS_t;


/**	--- Initialization functions --- **/


/**
 * @brief  Creates lcd_message_t variable containing an instruction to be transmitted
 * @param  message: Message to be transmitted
 * @param  exe_time: Time needed by the display to execute the instruction (ms)
 * @param  mode: Message mode: MODE_COMMAND or MODE_DATA
 * @note   Execution time is converted into ticks depending on the tick frequency used by FreeRTOS
 * @retval lcd_message_t variable
 */
lcd_message_t* lcd_i2c_RTOS_message_new(uint8_t message, uint8_t exe_time, uint8_t mode);

/**
 * @brief  Creates lcd_i2c_RTOS_t variable corresponding to the display on a FreeRTOS environment
 * @param  *hi2c: I2C peripheral used by the mcu
 * @param  addr: I2C address corresponding to the targeted display
 * @param  thread_id: Thread ID of the associated FreeRTOS task
 * @param  n_chars: number of the diaplay columns, or chars (16 or 20)
 * @param  n_lines: number of the diaplay lines (usually 1, 2, or 4)
 * @retval lcd_i2c_RTOS_t variable targeting the desired display
 */
lcd_i2c_RTOS_t* lcd_i2c_RTOS_new(I2C_HandleTypeDef *hi2c, uint8_t addr, osThreadId_t thread_id, uint8_t n_chars, uint8_t n_lines);



/**	--- Auxiliary functions --- **/


/**
 * @brief  Initializes Liquid Crystal LCD display on a FreeRTOS environment
 * @param  lcd_i2c_RTOS_t variable targeting the desired display
 * @param  *hi2c: I2C peripheral used by the MCU
 * @param  addr: I2C address corresponding to the targeted display
 * @param  thread_id: Thread ID of the associated FreeRTOS task
 * @param  n_chars: number of the display columns, or chars (16 or 20)
 * @param  n_lines: number of the display lines (usually 1, 2, or 4)
 * @retval Initialization status:
 *           - 0: LCD was not detected on I2C port
 *           - 1: LCD initialized OK and ready to use
 */
uint8_t lcd_i2c_RTOS_Init(lcd_i2c_RTOS_t* lcd_i2c_RTOS, I2C_HandleTypeDef *hi2c, uint8_t addr, osThreadId_t thread_id, uint8_t n_chars, uint8_t n_lines);

/**
 * @brief  Reads the buffer, formats the message and transmits it through I2C protocol
 * @note   While the execution time of a message is 0, transmits another message
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @param  mode: message mode: MODE_COMMAND or MODE_DATA
 * @retval Transmission status :
 * 				0: Buffer is empty or mode is not supported
 * 				1: Transmission was successful
 */
uint8_t lcd_i2c_RTOS_Transmit(lcd_i2c_RTOS_t* lcd_i2c_RTOS);

/**
 * @brief  Adds a command to the message buffer
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @param  cmd: command to be added to the buffer
 * @param  exe_time: time needed by the display to  execute the command (ms)
 * @retval Status:
 * 				0: The buffer is full
 * 				1: The command was added succesfully
 */
uint8_t lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS_t* lcd_i2c_RTOS, char cmd, uint8_t exe_time);

/**
 * @brief  Adds a data byte to the message buffer
 * @note   Execution time is considered 0
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @param  data: data byte to be added to the buffer
 * @retval Status:
 * 				0: The buffer is full
 * 				1: The command was added succesfully
 */
uint8_t lcd_i2c_RTOS_send_data (lcd_i2c_RTOS_t* lcd_i2c_RTOS, char data);  // send data to the lcd


/**	--- Interface functions --- **/

void lcd_i2c_RTOS_Handle_Messages(lcd_i2c_RTOS_t* lcd_i2c_RTOS);

/**
 * @brief  Send a string to the the buffer, so it can be displayed on a specific position of the display
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @param  line: line at which the string will be displayed (1, 2, or 4)
 * @param  col: column at which the string will start to be displayed (under 16 or under 20)
 * @retval Write status:
 * 				0: line or column out of the display
 * 				1: string added correctly
 */
uint8_t lcd_i2c_RTOS_Write(lcd_i2c_RTOS_t* lcd_i2c_RTOS, uint8_t line, uint8_t col, char *str);

/**
 * @brief  Turns on the display
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @retval None
 */
void lcd_i2c_RTOS_Display_On(lcd_i2c_RTOS_t* lcd_i2c_RTOS);

/**
 * @brief  Turns off the display
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @retval None
 */
void lcd_i2c_RTOS_Display_Off(lcd_i2c_RTOS_t* lcd_i2c_RTOS);

/**
 * @brief  Turns on the underline cursor on the display
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @retval None
 */
void lcd_i2c_RTOS_Cursor_On(lcd_i2c_RTOS_t* lcd_i2c_RTOS);

/**
 * @brief  Turns off the underline cursor on the display
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @retval None
 */
void lcd_i2c_RTOS_Cursor_Off(lcd_i2c_RTOS_t* lcd_i2c_RTOS);

/**
 * @brief  Turns on the blinking cursor
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @retval None
 */
void lcd_i2c_RTOS_Cursor_Blink_On(lcd_i2c_RTOS_t* lcd_i2c_RTOS);

/**
 * @brief  Turns off the blinking cursor
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @retval None
 */
void lcd_i2c_RTOS_Cursor_Blink_Off(lcd_i2c_RTOS_t* lcd_i2c_RTOS);

/**
 * @brief  Scrolls the display left without changing the RAM
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @retval None
 */
void lcd_i2c_RTOS_Scroll_Left(lcd_i2c_RTOS_t* lcd_i2c_RTOS);

/**
 * @brief  Scrolls the display right without changing the RAM
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @retval None
 */
void lcd_i2c_RTOS_Scroll_Right(lcd_i2c_RTOS_t* lcd_i2c_RTOS);

/**
 * @brief  Sets the entry mode to the right
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @retval None
 */
void lcd_i2c_RTOS_Left_To_Right(lcd_i2c_RTOS_t* lcd_i2c_RTOS);

/**
 * @brief  Sets the entry mode to the left
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @retval None
 */
void lcd_i2c_RTOS_Right_To_Left(lcd_i2c_RTOS_t* lcd_i2c_RTOS);

/**
 * @brief  Justifies the text to the right
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @retval None
 */
void lcd_i2c_RTOS_Autoscroll_On(lcd_i2c_RTOS_t* lcd_i2c_RTOS);

/**
 * @brief  Justifies the text to the left
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @retval None
 */
void lcd_i2c_RTOS_Autoscroll_Off(lcd_i2c_RTOS_t* lcd_i2c_RTOS);

/**
 * @brief  Clears the display removing all characters and resets position of the DDRAM
 * @note   1.52 ms of execution time needed
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @retval None
 */
void lcd_i2c_RTOS_Clear (lcd_i2c_RTOS_t* lcd_i2c_RTOS);

/**
 * @brief  Resets the position of the cursor to its initial state
 * @note   1.52 ms of execution time needed
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @retval None
 */
void lcd_i2c_RTOS_Reset_Poistion(lcd_i2c_RTOS_t* lcd_i2c_RTOS);


#endif
