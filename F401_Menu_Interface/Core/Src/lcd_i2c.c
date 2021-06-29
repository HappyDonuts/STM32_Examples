
/** Put this in the src folder **/

#include <lcd_i2c.h>


/**
 * @brief  Creates lcd_i2c_t variable corresponding to the display
 * @param  *hi2c: I2C peripheral used by the mcu
 * @param  addr: I2C address corresponding to the targeted display
 * @retval lcd_i2c_t variable targeting the desired display
 */
lcd_i2c_t* lcd_i2c_new(I2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t n_chars, uint8_t n_lines)
{
	lcd_i2c_t* lcd_i2c = malloc(sizeof(*lcd_i2c));
	lcd_i2c_Init(lcd_i2c, hi2c, addr, n_chars, n_lines);
	return lcd_i2c;
}

/**
 * @brief  Initializes Liquid Crystal LCD display
 * @param  *hi2c: I2C peripheral used by the mcu
 * @param  addr: I2C address corresponding to the targeted display
 * @retval Initialization status:
 *           - 0: LCD was not detected on I2C port
 *           - 1: LCD initialized OK and ready to use
 */
uint8_t lcd_i2c_Init(lcd_i2c_t* lcd_i2c, I2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t n_chars, uint8_t n_lines)
{
	lcd_i2c->hi2c = hi2c;
	lcd_i2c->addr = addr;
	lcd_i2c->n_chars = n_chars;
	lcd_i2c->n_lines = n_lines;
	lcd_i2c->display_control = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	lcd_i2c->display_mode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

	if (HAL_I2C_IsDeviceReady(lcd_i2c->hi2c, lcd_i2c->addr, 1, 20000) != HAL_OK) {
			/* Return false */
			return 0;
		}
	// Init code
	HAL_Delay(50);  // wait for >40ms
	lcd_send_cmd (lcd_i2c, 0x30);
	HAL_Delay(5);  // wait for >4.1ms
	lcd_send_cmd (lcd_i2c, 0x30);
	HAL_Delay(1);  // wait for >100us
	lcd_send_cmd (lcd_i2c, 0x30);
	HAL_Delay(10);
	lcd_send_cmd (lcd_i2c, 0x20);  // 4bit mode
	HAL_Delay(10);

	// dislay initialisation
	lcd_send_cmd (lcd_i2c, 0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	HAL_Delay(1);
	lcd_send_cmd (lcd_i2c, 0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
	HAL_Delay(1);
	lcd_send_cmd (lcd_i2c, 0x01);  // clear display
	HAL_Delay(1);
	HAL_Delay(1);
	lcd_send_cmd (lcd_i2c, 0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	HAL_Delay(1);
	lcd_send_cmd (lcd_i2c, 0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
	return 1;
}

void lcd_send_cmd (lcd_i2c_t* lcd_i2c, char cmd)
{
  char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd&0xf0);
	data_l = ((cmd<<4)&0xf0);
	data_t[0] = data_u|0x0C;  //en=1, rs=0
	data_t[1] = data_u|0x08;  //en=0, rs=0
	data_t[2] = data_l|0x0C;  //en=1, rs=0
	data_t[3] = data_l|0x08;  //en=0, rs=0
	HAL_I2C_Master_Transmit (lcd_i2c->hi2c, lcd_i2c->addr, (uint8_t *) data_t, 4, 100);
}

void lcd_send_data (lcd_i2c_t* lcd_i2c, char data)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_u|0x0D;  //en=1, rs=1
	data_t[1] = data_u|0x09;  //en=0, rs=1
	data_t[2] = data_l|0x0D;  //en=1, rs=1
	data_t[3] = data_l|0x09;  //en=0, rs=1
	HAL_I2C_Master_Transmit (lcd_i2c->hi2c, lcd_i2c->addr,(uint8_t *) data_t, 4, 100);
}


void lcd_send_string (lcd_i2c_t* lcd_i2c, char *str)
{
	while (*str) lcd_send_data (lcd_i2c, *str++);
}

uint8_t lcd_i2c_Write(lcd_i2c_t* lcd_i2c, uint8_t line, uint8_t col, char *str)
{
	static uint8_t start_line_0 = 0x00;
	static uint8_t start_line_1 = 0x40;
	static uint8_t start_line_2, start_line_3;
	uint8_t position;

	if (lcd_i2c->n_chars == 20) {
		start_line_2 = 0x14;
		start_line_3 = 0x54;
	} else if (lcd_i2c->n_chars == 16) {
		start_line_2 = 0x10;
		start_line_3 = 0x50;
	}
	if (col >= lcd_i2c->n_chars) {
		return 0;
	} else {
		if ((line == 0) && (lcd_i2c->n_lines > 0)) {
			position = start_line_0 + col;
		} else if ((line == 1) && (lcd_i2c->n_lines > 1)) {
			position = start_line_1 + col;
		} else if ((line == 2) && (lcd_i2c->n_lines > 2)) {
			position = start_line_2 + col;
		} else if ((line == 3) && (lcd_i2c->n_lines > 3)) {
			position = start_line_3 + col;
		} else {
			return 0;
		}
	}
	lcd_send_cmd (lcd_i2c, LCD_SETDDRAMADDR|position);
	lcd_send_string(lcd_i2c, str);
	return 1;
}

/**
 * @brief  Turns on the display
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_Display_On(lcd_i2c_t* lcd_i2c)
{
	lcd_i2c->display_control |= LCD_DISPLAYON;
	lcd_send_cmd (lcd_i2c, LCD_DISPLAYCONTROL | lcd_i2c->display_control);
}

/**
 * @brief  Turns off the display
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_Display_Off(lcd_i2c_t* lcd_i2c)
{
	lcd_i2c->display_control &= ~LCD_DISPLAYON;
	lcd_send_cmd (lcd_i2c, LCD_DISPLAYCONTROL | lcd_i2c->display_control);
}

/**
 * @brief  Turns the underline cursor on the display
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_Cursor_On(lcd_i2c_t* lcd_i2c)
{
	lcd_i2c->display_control |= LCD_CURSORON;
	lcd_send_cmd (lcd_i2c, LCD_DISPLAYCONTROL | lcd_i2c->display_control);
}

/**
 * @brief  Turns the underline cursor off the display
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_Cursor_Off(lcd_i2c_t* lcd_i2c)
{
	lcd_i2c->display_control &= ~LCD_CURSORON;
	lcd_send_cmd (lcd_i2c, LCD_DISPLAYCONTROL | lcd_i2c->display_control);
}

/**
 * @brief  Turns on the blinking cursor
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_Cursor_Blink_On(lcd_i2c_t* lcd_i2c)
{
	lcd_i2c->display_control |= LCD_BLINKON;
	lcd_send_cmd (lcd_i2c, LCD_DISPLAYCONTROL | lcd_i2c->display_control);
}

/**
 * @brief  Turns off the blinking cursor
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_Cursor_Blink_Off(lcd_i2c_t* lcd_i2c)
{
	lcd_i2c->display_control &= ~LCD_BLINKON;
	lcd_send_cmd (lcd_i2c, LCD_DISPLAYCONTROL | lcd_i2c->display_control);
}

/**
 * @brief  Scrolls the display left without changing the RAM
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_Scroll_Left(lcd_i2c_t* lcd_i2c)
{
	lcd_send_cmd (lcd_i2c, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

/**
 * @brief  Scrolls the display right without changing the RAM
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_Scroll_Right(lcd_i2c_t* lcd_i2c)
{
	lcd_send_cmd (lcd_i2c, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

/**
 * @brief  Sets the entry mode to the right
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_Left_To_Right(lcd_i2c_t* lcd_i2c)
{
	lcd_i2c->display_mode |= LCD_ENTRYLEFT;
	lcd_send_cmd(lcd_i2c, LCD_ENTRYMODESET | lcd_i2c->display_mode);
}

/**
 * @brief  Sets the entry mode to the left
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_Right_To_Left(lcd_i2c_t* lcd_i2c)
{
	lcd_i2c->display_mode &= ~LCD_ENTRYLEFT;
	lcd_send_cmd(lcd_i2c, LCD_ENTRYMODESET | lcd_i2c->display_mode);
}

/**
 * @brief  Justifies the text to the right
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_Autoscroll_On(lcd_i2c_t* lcd_i2c)
{
	lcd_i2c->display_mode |= LCD_ENTRYSHIFTINCREMENT;
	lcd_send_cmd(lcd_i2c, LCD_ENTRYMODESET | lcd_i2c->display_mode);
}

/**
 * @brief  Justifies the text to the left
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_Autoscroll_Off(lcd_i2c_t* lcd_i2c)
{
	lcd_i2c->display_mode &= ~LCD_ENTRYSHIFTINCREMENT;
	lcd_send_cmd(lcd_i2c, LCD_ENTRYMODESET | lcd_i2c->display_mode);
}

/**
 * @brief  Clears the display removing all characters and resets position of the DDRAM
 * @note   Long?
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_clear (lcd_i2c_t* lcd_i2c)
{
//	lcd_send_cmd (lcd_i2c, 0x00);
	lcd_send_cmd(lcd_i2c, LCD_CLEARDISPLAY);
//	for (int i=0; i<100; i++)
//	{
//		lcd_send_data (lcd_i2c, ' ');
//	}
}

/**
 * @brief  Resets the position of the cursor to its initial state
 * @note   Long?
 * @param  lcd_i2c: lcd_i2c_t variable corresponding to the display
 * @retval None
 */
void lcd_i2c_Reset_Poistion(lcd_i2c_t* lcd_i2c)
{
	lcd_send_cmd(lcd_i2c, LCD_RETURNHOME);
}


