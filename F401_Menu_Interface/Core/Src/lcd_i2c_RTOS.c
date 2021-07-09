#include <lcd_i2c_RTOS.h>

/**
 * @brief  Creates lcd_message_t variable containing an instruction to be transmitted
 * @param  message: Message to be transmitted
 * @param  exe_time: Time needed by the display to execute the instruction (ms)
 * @param  mode: Message mode: MODE_COMMAND or MODE_DATA
 * @note   Execution time is converted into ticks depending on the tick frequency used by FreeRTOS
 * @retval lcd_message_t variable
 */
lcd_message_t* lcd_i2c_RTOS_message_new(uint8_t message, uint8_t exe_time, uint8_t mode)
{
	lcd_message_t* lcd_message = malloc(sizeof(*lcd_message));
	lcd_message->message = message;
	lcd_message->exe_time = ceil((float)(exe_time*osKernelGetTickFreq()/1000));
	lcd_message->mode = mode;
	lcd_message->next_msg = NULL;
	lcd_message->prev_msg = NULL;
	return lcd_message;
}

/**
 * @brief  Creates lcd_i2c_RTOS_t variable corresponding to the display on a FreeRTOS environment
 * @param  *hi2c: I2C peripheral used by the mcu
 * @param  addr: I2C address corresponding to the targeted display
 * @param  thread_id: Thread ID of the associated FreeRTOS task
 * @param  n_chars: number of the diaplay columns, or chars (16 or 20)
 * @param  n_lines: number of the diaplay lines (usually 1, 2, or 4)
 * @retval lcd_i2c_RTOS_t variable targeting the desired display
 */
lcd_i2c_RTOS_t* lcd_i2c_RTOS_new(I2C_HandleTypeDef *hi2c, uint8_t addr, osThreadId_t thread_id, uint8_t n_chars, uint8_t n_lines)
{
	lcd_i2c_RTOS_t* lcd_i2c_RTOS = malloc(sizeof(*lcd_i2c_RTOS));
	lcd_i2c_RTOS_Init(lcd_i2c_RTOS, hi2c, addr, thread_id, n_chars, n_lines);
	return lcd_i2c_RTOS;
}

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
LCD_StatusTypeDef lcd_i2c_RTOS_Init(lcd_i2c_RTOS_t* lcd_i2c_RTOS, I2C_HandleTypeDef *hi2c, uint8_t addr, osThreadId_t thread_id, uint8_t n_chars, uint8_t n_lines)
{
	if ((n_chars != 16) && (n_chars != 20)) {
		return LCD_INVALID_PARAM;
	}
	if ((n_lines != 1) && (n_lines != 2) && (n_lines != 2)) {
		return LCD_INVALID_PARAM;
	}

	lcd_i2c_RTOS->hi2c = hi2c;
	lcd_i2c_RTOS->addr = addr;
	lcd_i2c_RTOS->n_chars = n_chars;
	lcd_i2c_RTOS->n_lines = n_lines;
	lcd_i2c_RTOS->display_control = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	lcd_i2c_RTOS->display_mode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	lcd_i2c_RTOS->buffer_first_msg = NULL;
	lcd_i2c_RTOS->buffer_last_msg = NULL;
	lcd_i2c_RTOS->n_messages = 0;
	lcd_i2c_RTOS->thread_id = thread_id;


	if (HAL_I2C_IsDeviceReady(lcd_i2c_RTOS->hi2c, lcd_i2c_RTOS->addr, 1, 20000) != HAL_OK) {
			/* Return false */
			return LCD_I2C_ERROR;
		}
	// Init code
	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, 0x30, 5);
	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, 0x30, 1);
	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, 0x30, 10);
	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, 0x20, 10); // 4bit mode

	// dislay initialisation
	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, 0x28, 1);	// Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, 0x08, 1); 	//Display on/off control --> D=0,C=0, B=0  ---> display off
	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, 0x01, 2);	// clear display
	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, 0x06, 1); 	//Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, 0x0C, 0); 	//Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
	return LCD_OK;
}

/**
 * @brief  Reads the buffer, formats the message and transmits it through I2C protocol
 * @note   While the execution time of a message is 0, transmits another message
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @param  mode: message mode: MODE_COMMAND or MODE_DATA
 * @retval Transmission status :
 * 				0: Buffer is empty or mode is not supported
 * 				1: Transmission was successful
 */
LCD_StatusTypeDef lcd_i2c_RTOS_Transmit(lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
	uint8_t data_u, data_l;
	uint8_t data_t[4];
	lcd_message_t* msg;
	uint8_t keep_tx_msg = 1;

	while(keep_tx_msg) {
		if (lcd_i2c_RTOS->n_messages == 0) {
			return LCD_BUFFER_EMPTY;
		} else {
			msg = lcd_i2c_RTOS->buffer_last_msg;
			data_u = (msg->message&0xf0);
			data_l = ((msg->message<<4)&0xf0);

			if (msg->mode == MODE_COMMAND) {
				data_t[0] = data_u|0x0C;  //en=1, rs=0
				data_t[1] = data_u|0x08;  //en=0, rs=0
				data_t[2] = data_l|0x0C;  //en=1, rs=0
				data_t[3] = data_l|0x08;  //en=0, rs=0
			} else if (msg->mode == MODE_DATA) {
				data_t[0] = data_u|0x0D;  //en=1, rs=1
				data_t[1] = data_u|0x09;  //en=0, rs=1
				data_t[2] = data_l|0x0D;  //en=1, rs=1
				data_t[3] = data_l|0x09;  //en=0, rs=1
			} else {
				return LCD_INVALID_PARAM;
			}

			if (HAL_I2C_IsDeviceReady(lcd_i2c_RTOS->hi2c, lcd_i2c_RTOS->addr, 1, 10) != HAL_OK) {
				return LCD_I2C_ERROR;
			} else {
				HAL_I2C_Master_Transmit (lcd_i2c_RTOS->hi2c, lcd_i2c_RTOS->addr, (uint8_t *) data_t, 4, 100);
			}

			if (msg->exe_time != 0) {
				keep_tx_msg = 0;
			}
			lcd_i2c_RTOS->buffer_last_msg = msg->prev_msg;
			free(msg);
			lcd_i2c_RTOS->n_messages--;
			if (lcd_i2c_RTOS->n_messages == 0) {
				lcd_i2c_RTOS->buffer_first_msg = NULL;
				lcd_i2c_RTOS->buffer_last_msg = NULL;
			}
		}
	}
	return LCD_OK;
}

/**
 * @brief  Push a command to the start of the buffer
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @param  cmd: command to be added to the buffer
 * @param  exe_time: time needed by the display to  execute the command (ms)
 * @retval Status:
 * 				0: The buffer is full
 * 				1: The command was added succesfully
 */
LCD_StatusTypeDef lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS_t* lcd_i2c_RTOS, char cmd, uint8_t exe_time)
{
	if (lcd_i2c_RTOS->n_messages >= BUFFER_SIZE) {
		return LCD_BUFFER_FULL;
	} else {
		lcd_message_t* msg = lcd_i2c_RTOS_message_new(cmd, exe_time, MODE_COMMAND);
		msg->next_msg = lcd_i2c_RTOS->buffer_first_msg;
		if (lcd_i2c_RTOS->buffer_first_msg != NULL) {
			lcd_i2c_RTOS->buffer_first_msg->prev_msg = msg;
		} else {
			lcd_i2c_RTOS->buffer_last_msg = msg;
		}
		lcd_i2c_RTOS->buffer_first_msg = msg;
		lcd_i2c_RTOS->n_messages++;
		if (osThreadGetState(lcd_i2c_RTOS->thread_id) == osThreadBlocked) {
			osThreadResume(lcd_i2c_RTOS->thread_id);
		}
		return LCD_OK;
	}
}

/**
 * @brief  Adds a data byte to the message buffer
 * @note   Execution time is considered 0
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @param  data: data byte to be added to the buffer
 * @retval Status:
 * 				0: The buffer is full
 * 				1: The command was added succesfully
 */
LCD_StatusTypeDef lcd_i2c_RTOS_send_data (lcd_i2c_RTOS_t* lcd_i2c_RTOS, char data)
{
	if (lcd_i2c_RTOS->n_messages >= BUFFER_SIZE) {
		return LCD_BUFFER_FULL;
	} else {
		lcd_message_t* msg = lcd_i2c_RTOS_message_new(data, 0, MODE_DATA);
		msg->next_msg = lcd_i2c_RTOS->buffer_first_msg;
		msg->prev_msg = NULL;
		if (lcd_i2c_RTOS->buffer_first_msg != NULL) {
			lcd_i2c_RTOS->buffer_first_msg->prev_msg = msg;
		} else {
			lcd_i2c_RTOS->buffer_last_msg = msg;
		}
		lcd_i2c_RTOS->buffer_first_msg = msg;
		lcd_i2c_RTOS->n_messages++;
		if (osThreadGetState(lcd_i2c_RTOS->thread_id) == osThreadBlocked) {
			osThreadResume(lcd_i2c_RTOS->thread_id);
		}
		return LCD_OK;
	}
}

void lcd_i2c_RTOS_Handle_Messages(lcd_i2c_RTOS_t* lcd_i2c_RTOS) {
	uint32_t now;
	uint8_t lcd_busy;
	static uint32_t tick_cooldown;

	if (lcd_i2c_RTOS->n_messages == 0) {
		osThreadSuspend(lcd_i2c_RTOS->thread_id);
	} else {
		now = osKernelGetTickCount();
		if (now > tick_cooldown) {
//			lcd_busy = get_busy_state
			lcd_busy = 0;
			if (lcd_busy == 0) {
				tick_cooldown = now + lcd_i2c_RTOS->buffer_last_msg->exe_time;
				lcd_i2c_RTOS_Transmit(lcd_i2c_RTOS);
			}
		}
	}
}

/**
 * @brief  Send a string to the the buffer, so it can be displayed on a specific
 * 		   position of the display.
 * @param  lcd_i2c_RTOS: pointer to lcd_i2c_RTOS structure containing the info
 * 		   for the specified display.
 * @param  line: line at which the string will be displayed (1, 2, or 4).
 * @param  col: column at which the string will start to be displayed
 * 		   (under 16 or under 20).
 * @param  clear: clears the selected line before displaying the string.
 * 			 @arg CLEAR_ON: to clear the line
 * 			 @arg CLEAR_OFF: to not clear the line
 * @retval LCD Status
 */
LCD_StatusTypeDef lcd_i2c_RTOS_Write(lcd_i2c_RTOS_t* lcd_i2c_RTOS, uint8_t line, uint8_t col, char *str, uint8_t clear)
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
		return LCD_INVALID_PARAM;
	} else {
		if ((line == 0) && (lcd_i2c_RTOS->n_lines > 0)) {
			position = start_line_0;
		} else if ((line == 1) && (lcd_i2c_RTOS->n_lines > 1)) {
			position = start_line_1;
		} else if ((line == 2) && (lcd_i2c_RTOS->n_lines > 2)) {
			position = start_line_2;
		} else if ((line == 3) && (lcd_i2c_RTOS->n_lines > 3)) {
			position = start_line_3;
		} else {
			return LCD_INVALID_PARAM;
		}
	}
	if (clear) {
		lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, LCD_SETDDRAMADDR|position, 1);
		for (int i=0; i<lcd_i2c_RTOS->n_chars; i++) {
			LCD_StatusTypeDef ret = lcd_i2c_RTOS_send_data (lcd_i2c_RTOS, ' ');
			if (ret != LCD_OK) {
				return ret;
			}
		}
	}

	lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, LCD_SETDDRAMADDR|(position + col), 1);
	while (*str) {
		LCD_StatusTypeDef ret = lcd_i2c_RTOS_send_data (lcd_i2c_RTOS, *str++);
		if (ret != LCD_OK) {
			return ret;
		}
	}
	return LCD_OK;
}

/**
 * @brief  Turns on the display
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @retval None
 */
LCD_StatusTypeDef lcd_i2c_RTOS_Display_On(lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
	lcd_i2c_RTOS->display_control |= LCD_DISPLAYON;
	return lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, LCD_DISPLAYCONTROL | lcd_i2c_RTOS->display_control, 0);
}

/**
 * @brief  Turns off the display
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @retval None
 */
LCD_StatusTypeDef lcd_i2c_RTOS_Display_Off(lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
	lcd_i2c_RTOS->display_control &= ~LCD_DISPLAYON;
	return lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, LCD_DISPLAYCONTROL | lcd_i2c_RTOS->display_control, 0);
}

/**
 * @brief  Turns on the underline cursor on the display
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @retval None
 */
LCD_StatusTypeDef lcd_i2c_RTOS_Cursor_On(lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
	lcd_i2c_RTOS->display_control |= LCD_CURSORON;
	return lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, LCD_DISPLAYCONTROL | lcd_i2c_RTOS->display_control, 0);
}

/**
 * @brief  Turns off the underline cursor on the display
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @retval None
 */
LCD_StatusTypeDef lcd_i2c_RTOS_Cursor_Off(lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
	lcd_i2c_RTOS->display_control &= ~LCD_CURSORON;
	return lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, LCD_DISPLAYCONTROL | lcd_i2c_RTOS->display_control, 0);
}

/**
 * @brief  Turns on the blinking cursor
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @retval None
 */
LCD_StatusTypeDef lcd_i2c_RTOS_Cursor_Blink_On(lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
	lcd_i2c_RTOS->display_control |= LCD_BLINKON;
	return lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, LCD_DISPLAYCONTROL | lcd_i2c_RTOS->display_control, 0);
}

/**
 * @brief  Turns off the blinking cursor
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @retval None
 */
LCD_StatusTypeDef lcd_i2c_RTOS_Cursor_Blink_Off(lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
	lcd_i2c_RTOS->display_control &= ~LCD_BLINKON;
	return lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, LCD_DISPLAYCONTROL | lcd_i2c_RTOS->display_control, 0);
}

/**
 * @brief  Scrolls the display left without changing the RAM
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @retval None
 */
LCD_StatusTypeDef lcd_i2c_RTOS_Scroll_Left(lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
	return lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT, 0);
}

/**
 * @brief  Scrolls the display right without changing the RAM
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @retval None
 */
LCD_StatusTypeDef lcd_i2c_RTOS_Scroll_Right(lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
	return lcd_i2c_RTOS_send_cmd (lcd_i2c_RTOS, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT, 0);
}

/**
 * @brief  Sets the entry mode to the right
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @retval None
 */
LCD_StatusTypeDef lcd_i2c_RTOS_Left_To_Right(lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
	lcd_i2c_RTOS->display_mode |= LCD_ENTRYLEFT;
	return lcd_i2c_RTOS_send_cmd(lcd_i2c_RTOS, LCD_ENTRYMODESET | lcd_i2c_RTOS->display_mode, 0);
}

/**
 * @brief  Sets the entry mode to the left
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @retval None
 */
LCD_StatusTypeDef lcd_i2c_RTOS_Right_To_Left(lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
	lcd_i2c_RTOS->display_mode &= ~LCD_ENTRYLEFT;
	return lcd_i2c_RTOS_send_cmd(lcd_i2c_RTOS, LCD_ENTRYMODESET | lcd_i2c_RTOS->display_mode, 0);
}

/**
 * @brief  Justifies the text to the right
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @retval None
 */
LCD_StatusTypeDef lcd_i2c_RTOS_Autoscroll_On(lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
	lcd_i2c_RTOS->display_mode |= LCD_ENTRYSHIFTINCREMENT;
	return lcd_i2c_RTOS_send_cmd(lcd_i2c_RTOS, LCD_ENTRYMODESET | lcd_i2c_RTOS->display_mode, 0);
}

/**
 * @brief  Justifies the text to the left
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @retval None
 */
LCD_StatusTypeDef lcd_i2c_RTOS_Autoscroll_Off(lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
	lcd_i2c_RTOS->display_mode &= ~LCD_ENTRYSHIFTINCREMENT;
	return lcd_i2c_RTOS_send_cmd(lcd_i2c_RTOS, LCD_ENTRYMODESET | lcd_i2c_RTOS->display_mode, 0);
}

/**
 * @brief  Clears the display removing all characters and resets position of the DDRAM
 * @note   1.52 ms of execution time needed
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @retval None
 */
LCD_StatusTypeDef lcd_i2c_RTOS_Clear (lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
	return lcd_i2c_RTOS_send_cmd(lcd_i2c_RTOS, LCD_CLEARDISPLAY, 2);
}

/**
 * @brief  Resets the position of the cursor to its initial state
 * @note   1.52 ms of execution time needed
 * @param  lcd_i2c_RTOS: variable targeting the desired display
 * @retval None
 */
LCD_StatusTypeDef lcd_i2c_RTOS_Reset_Poistion(lcd_i2c_RTOS_t* lcd_i2c_RTOS)
{
	return lcd_i2c_RTOS_send_cmd(lcd_i2c_RTOS, LCD_RETURNHOME, 2);
}


