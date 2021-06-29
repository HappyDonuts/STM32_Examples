
/** Put this in the src folder **/

#include <lcd_i2c_RTOS.h>

lcd_message_t* lcd_i2c_RTOS_message_new(uint8_t message, uint8_t exe_time, uint8_t mode)
{
	lcd_message_t* lcd_message = malloc(sizeof(*lcd_message));
	lcd_message->message = message;
	lcd_message->exe_time = exe_time;
	lcd_message->mode = mode;
	return lcd_message;
}

/**
 * @brief  Creates lcd_i2c_t variable corresponding to the display
 * @param  *hi2c: I2C peripheral used by the mcu
 * @param  addr: I2C address corresponding to the targeted display
 * @retval lcd_i2c_t variable targeting the desired display
 */
lcd_i2c_RTOS_t* lcd_i2c_RTOS_new(I2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t n_chars, uint8_t n_lines)
{
	lcd_i2c_RTOS_t* lcd_i2c_RTOS = malloc(sizeof(*lcd_i2c_RTOS));
	lcd_i2c_RTOS_Init(lcd_i2c_RTOS, hi2c, addr, n_chars, n_lines);
	return lcd_i2c_RTOS;
}

/**
 * @brief  Initializes Liquid Crystal LCD display
 * @param  *hi2c: I2C peripheral used by the mcu
 * @param  addr: I2C address corresponding to the targeted display
 * @retval Initialization status:
 *           - 0: LCD was not detected on I2C port
 *           - 1: LCD initialized OK and ready to use
 */
uint8_t lcd_i2c_RTOS_Init(lcd_i2c_RTOS_t* lcd_i2c_RTOS, I2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t n_chars, uint8_t n_lines)
{
	lcd_i2c_RTOS->hi2c = hi2c;
	lcd_i2c_RTOS->addr = addr;
	lcd_i2c_RTOS->n_chars = n_chars;
	lcd_i2c_RTOS->n_lines = n_lines;
	lcd_i2c_RTOS->display_control = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	lcd_i2c_RTOS->display_mode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	for (int i=0; i<BUFFER_SIZE; i++){
		lcd_i2c_RTOS->buffer_messages[i] = lcd_i2c_RTOS_message_new(0, 0, 0);
//		lcd_i2c_RTOS->buffer_messages[i]->exe_time = 0;
	}
	lcd_i2c_RTOS->buffer_index = 0;
	lcd_i2c_RTOS->empty = 1;


	if (HAL_I2C_IsDeviceReady(lcd_i2c_RTOS->hi2c, lcd_i2c_RTOS->addr, 1, 20000) != HAL_OK) {
			/* Return false */
			return 0;
		}
	// Init code
	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, 0x30, 5);
	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, 0x30, 1);
	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, 0x30, 10);
	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, 0x20, 10);  // 4bit mode

	// dislay initialisation
	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, 0x28, 1); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, 0x08, 1); //Display on/off control --> D=0,C=0, B=0  ---> display off
	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, 0x01, 2);  // clear display
	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, 0x06, 1); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, 0x0C, 0); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
	return 1;
}

uint8_t lcd_i2c_RTOS_Transmit(lcd_i2c_RTOS_t* lcd_i2c_RTOS, uint8_t mode)
{
	if (lcd_i2c_RTOS->empty == 0) {
		return 0;
	} else {
		uint8_t data_u, data_l;
		uint8_t data_t[4];
		lcd_message_t* msg = lcd_i2c_RTOS->buffer_messages[lcd_i2c_RTOS->buffer_index];
		data_u = (msg->message&0xf0);
		data_l = ((msg->message<<4)&0xf0);

		if (mode == MODE_COMMAND) {
			data_t[0] = data_u|0x0C;  //en=1, rs=0
			data_t[1] = data_u|0x08;  //en=0, rs=0
			data_t[2] = data_l|0x0C;  //en=1, rs=0
			data_t[3] = data_l|0x08;  //en=0, rs=0
		} else if (mode == MODE_DATA) {
			data_t[0] = data_u|0x0D;  //en=1, rs=1
			data_t[1] = data_u|0x09;  //en=0, rs=1
			data_t[2] = data_l|0x0D;  //en=1, rs=1
			data_t[3] = data_l|0x09;  //en=0, rs=1
		} else {
			return 0;
		}
		HAL_I2C_Master_Transmit (lcd_i2c_RTOS->hi2c, lcd_i2c_RTOS->addr, (uint8_t *) data_t, 4, 100);

		if (lcd_i2c_RTOS->buffer_index == 0) {
			lcd_i2c_RTOS->empty = 0;
		} else {
			lcd_i2c_RTOS->buffer_index--;
		}
		return 1;
	}
}

uint8_t lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS_t* lcd_i2c_RTOS, char cmd, uint8_t exe_time)
{
	if (lcd_i2c_RTOS->buffer_index >= BUFFER_SIZE-1) {
		return 1;
	} else {
		lcd_i2c_RTOS->buffer_messages[lcd_i2c_RTOS->buffer_index] = lcd_message_new(cmd, exe_time, MODE_COMMAND);
		lcd_i2c_RTOS->buffer_index++;
		return 0;
	}
}

uint8_t lcd_i2c_RTOS_send_data (lcd_i2c_RTOS_t* lcd_i2c_RTOS, char data)
{
	if (lcd_i2c_RTOS->buffer_index >= BUFFER_SIZE-1) {
		return 1;
	} else {
		lcd_i2c_RTOS->buffer_messages[lcd_i2c_RTOS->buffer_index] = lcd_message_new(data, 0, MODE_DATA);
		lcd_i2c_RTOS->buffer_index++;
		return 0;
	}
}

//void lcd_i2c_RTOS_send_string (lcd_i2c_RTOS_t* lcd_i2c_RTOS, char *str)
//{
//	while (*str) lcd_i2c_RTOS_send_data (lcd_i2c_RTOS, *str++);
//}

uint8_t lcd_i2c_RTOS_Write(lcd_i2c_RTOS_t* lcd_i2c_RTOS, uint8_t line, uint8_t col, char *str)
{
	static uint8_t start_line_0 = 0x00;
	static uint8_t start_line_1 = 0x40;
	static uint8_t start_line_2, start_line_3;
	uint8_t position;

	if (lcd_i2c_RTOS->n_chars == 20) {
		start_line_2 = 0x14;
		start_line_3 = 0x54;
	} else if (lcd_i2c_RTOS->n_chars == 16) {
		start_line_2 = 0x10;
		start_line_3 = 0x50;
	}
	if (col >= lcd_i2c_RTOS->n_chars) {
		return 0;
	} else {
		if ((line == 0) && (lcd_i2c_RTOS->n_lines > 0)) {
			position = start_line_0 + col;
		} else if ((line == 1) && (lcd_i2c_RTOS->n_lines > 1)) {
			position = start_line_1 + col;
		} else if ((line == 2) && (lcd_i2c_RTOS->n_lines > 2)) {
			position = start_line_2 + col;
		} else if ((line == 3) && (lcd_i2c_RTOS->n_lines > 3)) {
			position = start_line_3 + col;
		} else {
			return 0;
		}
	}
	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, LCD_SETDDRAMADDR|position, 1);
	while (*str) lcd_i2c_RTOS_send_data (lcd_i2c_RTOS, *str++);
//	lcd_i2c_RTOS_send_string(lcd_i2c_RTOS, str);
	return 1;
}

/**
 * @brief  Turns on the display
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_RTOS_Display_On(lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
	lcd_i2c_RTOS->display_control |= LCD_DISPLAYON;
	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, LCD_DISPLAYCONTROL | lcd_i2c_RTOS->display_control, 0);
}

/**
 * @brief  Turns off the display
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_RTOS_Display_Off(lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
	lcd_i2c_RTOS->display_control &= ~LCD_DISPLAYON;
	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, LCD_DISPLAYCONTROL | lcd_i2c_RTOS->display_control, 0);
}

/**
 * @brief  Turns the underline cursor on the display
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_RTOS_Cursor_On(lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
	lcd_i2c_RTOS->display_control |= LCD_CURSORON;
	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, LCD_DISPLAYCONTROL | lcd_i2c_RTOS->display_control, 0);
}

/**
 * @brief  Turns the underline cursor off the display
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_RTOS_Cursor_Off(lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
	lcd_i2c_RTOS->display_control &= ~LCD_CURSORON;
	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, LCD_DISPLAYCONTROL | lcd_i2c_RTOS->display_control, 0);
}

/**
 * @brief  Turns on the blinking cursor
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_RTOS_Cursor_Blink_On(lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
	lcd_i2c_RTOS->display_control |= LCD_BLINKON;
	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, LCD_DISPLAYCONTROL | lcd_i2c_RTOS->display_control, 0);
}

/**
 * @brief  Turns off the blinking cursor
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_RTOS_Cursor_Blink_Off(lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
	lcd_i2c_RTOS->display_control &= ~LCD_BLINKON;
	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, LCD_DISPLAYCONTROL | lcd_i2c_RTOS->display_control, 0);
}

/**
 * @brief  Scrolls the display left without changing the RAM
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_RTOS_Scroll_Left(lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT, 0);
}

/**
 * @brief  Scrolls the display right without changing the RAM
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_RTOS_Scroll_Right(lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT, 0);
}

/**
 * @brief  Sets the entry mode to the right
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_RTOS_Left_To_Right(lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
	lcd_i2c_RTOS->display_mode |= LCD_ENTRYLEFT;
	lcd_i2c_RTOS_send_cmd(lcd_i2c_RTOS, LCD_ENTRYMODESET | lcd_i2c_RTOS->display_mode, 0);
}

/**
 * @brief  Sets the entry mode to the left
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_RTOS_Right_To_Left(lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
	lcd_i2c_RTOS->display_mode &= ~LCD_ENTRYLEFT;
	lcd_i2c_RTOS_send_cmd(lcd_i2c_RTOS, LCD_ENTRYMODESET | lcd_i2c_RTOS->display_mode, 0);
}

/**
 * @brief  Justifies the text to the right
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_RTOS_Autoscroll_On(lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
	lcd_i2c_RTOS->display_mode |= LCD_ENTRYSHIFTINCREMENT;
	lcd_i2c_RTOS_send_cmd(lcd_i2c_RTOS, LCD_ENTRYMODESET | lcd_i2c_RTOS->display_mode, 0);
}

/**
 * @brief  Justifies the text to the left
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_RTOS_Autoscroll_Off(lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
	lcd_i2c_RTOS->display_mode &= ~LCD_ENTRYSHIFTINCREMENT;
	lcd_i2c_RTOS_send_cmd(lcd_i2c_RTOS, LCD_ENTRYMODESET | lcd_i2c_RTOS->display_mode, 0);
}

/**
 * @brief  Clears the display removing all characters and resets position of the DDRAM
 * @note   Long?
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_RTOS_clear (lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
//	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, 0x00);
	lcd_i2c_RTOS_send_cmd(lcd_i2c_RTOS, LCD_CLEARDISPLAY, 2);
//	for (int i=0; i<100; i++)
//	{
//		lcd_i2c_RTOS_send_data (lcd_i2c, ' ');
//	}
}

/**
 * @brief  Resets the position of the cursor to its initial state
 * @note   Long?
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_RTOS_Reset_Poistion(lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
	lcd_i2c_RTOS_send_cmd(lcd_i2c_RTOS, LCD_RETURNHOME, 2);
}


