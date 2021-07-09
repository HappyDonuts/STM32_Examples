/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ssd1306_basic.h"
#include "lcd_i2c.h"
#include "lcd_i2c_RTOS.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim2;

/* Definitions for encoderPolling */
osThreadId_t encoderPollingHandle;
const osThreadAttr_t encoderPolling_attributes = {
  .name = "encoderPolling",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for oledDisplayUpda */
osThreadId_t oledDisplayUpdaHandle;
const osThreadAttr_t oledDisplayUpda_attributes = {
  .name = "oledDisplayUpda",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for lcdSendMessages */
osThreadId_t lcdSendMessagesHandle;
const osThreadAttr_t lcdSendMessages_attributes = {
  .name = "lcdSendMessages",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal1,
};
/* Definitions for lcdDisplayUpdat */
osThreadId_t lcdDisplayUpdatHandle;
const osThreadAttr_t lcdDisplayUpdat_attributes = {
  .name = "lcdDisplayUpdat",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for mutex_position */
osMutexId_t mutex_positionHandle;
const osMutexAttr_t mutex_position_attributes = {
  .name = "mutex_position"
};
/* Definitions for mutex_cursor */
osMutexId_t mutex_cursorHandle;
const osMutexAttr_t mutex_cursor_attributes = {
  .name = "mutex_cursor"
};
/* USER CODE BEGIN PV */
	int16_t position = 0;
	uint8_t cursor_enable = 0;
	uint8_t button_flag = 0;
	uint32_t lastTick = 0;
	uint32_t now;

	ssd1306_t* ssd1306_1;
	lcd_i2c_t* lcd_i2c_1;
	lcd_i2c_RTOS_t* lcd_i2c_RTOS_1;

	uint32_t tick_begin;
	uint32_t tick_end;
	uint32_t time_ms;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_I2C1_Init(void);
void StartEncoderPolling(void *argument);
void StartOledDisplayUpdate(void *argument);
void StartlcdSendMessages(void *argument);
void StartLcdDisplayUpdate(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  ssd1306_1 = ssd1306_new(&hi2c1, 0x3C<<1); // 0x79
//  lcd_i2c_1 = lcd_i2c_new(&hi2c1, 0x27<<1, 16, 2);
  lcd_i2c_RTOS_1 = lcd_i2c_RTOS_new(&hi2c1, 0x27<<1, lcdSendMessagesHandle, 16, 2);

  SSD1306_GotoXY(ssd1306_1, 2, 0);
  SSD1306_Puts(ssd1306_1, "  Menu 1", &Font_11x18, 1);
  SSD1306_GotoXY(ssd1306_1, 2, 18);
  SSD1306_Puts(ssd1306_1, "  Menu 2", &Font_11x18, 1);
  SSD1306_GotoXY(ssd1306_1, 2, 36);
  SSD1306_Puts(ssd1306_1, "  Menu 3", &Font_11x18, 1);
  SSD1306_GotoXY(ssd1306_1, 2, 53);
  SSD1306_Puts(ssd1306_1, "  Menu 4", &Font_7x10, 1);
  SSD1306_GotoXY(ssd1306_1, SSD1306_WIDTH/2, 53);
  SSD1306_Puts(ssd1306_1, "  Menu 5", &Font_7x10, 1);
  SSD1306_UpdateScreen(ssd1306_1);


//  lcd_i2c_Write(lcd_i2c_1, 0, 0, "Hello World!");
//  lcd_i2c_Write(lcd_i2c_1, 1, 0, "LCD 16x2 DEMO");
//  lcd_i2c_Write(lcd_i2c_1, 2, 0, "Blah");
//  lcd_i2c_Write(lcd_i2c_1, 3, 0, "Blah again");

//  char* text = "Hello!";
//  lcd_i2c_RTOS_Write(lcd_i2c_RTOS_1, 0, 0, text, 1);
//  lcd_i2c_RTOS_Write(lcd_i2c_RTOS_1, 1, 0, "Using FreeRTOS", 1);
//  lcd_i2c_RTOS_Write(lcd_i2c_RTOS_1, 2, 0, "Blah");
//  lcd_i2c_RTOS_Write(lcd_i2c_RTOS_1, 3, 0, "Blah again");

  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();
  /* Create the mutex(es) */
  /* creation of mutex_position */
  mutex_positionHandle = osMutexNew(&mutex_position_attributes);

  /* creation of mutex_cursor */
  mutex_cursorHandle = osMutexNew(&mutex_cursor_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of encoderPolling */
  encoderPollingHandle = osThreadNew(StartEncoderPolling, NULL, &encoderPolling_attributes);

  /* creation of oledDisplayUpda */
  oledDisplayUpdaHandle = osThreadNew(StartOledDisplayUpdate, NULL, &oledDisplayUpda_attributes);

  /* creation of lcdSendMessages */
  lcdSendMessagesHandle = osThreadNew(StartlcdSendMessages, NULL, &lcdSendMessages_attributes);

  /* creation of lcdDisplayUpdat */
  lcdDisplayUpdatHandle = osThreadNew(StartLcdDisplayUpdate, NULL, &lcdDisplayUpdat_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

}

/* USER CODE BEGIN 4 */
//void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
//
//}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	now = HAL_GetTick();
	if (now > lastTick){
		button_flag = 1;
		lastTick = now + 200;
	}
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartEncoderPolling */
/**
  * @brief  Function implementing the encoderPolling thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartEncoderPolling */
void StartEncoderPolling(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
	uint32_t tick = osKernelGetTickCount();
  for(;;)
  {
	osMutexAcquire(mutex_positionHandle, 0);
	position = ((int16_t)__HAL_TIM_GET_COUNTER(&htim2))/4;
	if (position < 0){
		__HAL_TIM_SET_COUNTER(&htim2, 0);
		position = 0;
	}
	if (position > 4){
		__HAL_TIM_SET_COUNTER(&htim2, 4*4);
		position = 4;
	}
	osMutexRelease(mutex_positionHandle);

    if (button_flag){
    	osMutexAcquire(mutex_cursorHandle, 0);
        cursor_enable = !cursor_enable;
        osMutexRelease(mutex_cursorHandle);
        __HAL_TIM_SET_COUNTER(&htim2, 0);
        button_flag = 0;
    }

	tick += 17;
	osDelayUntil(tick);
  }
  osThreadTerminate(NULL);
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartOledDisplayUpdate */
/**
* @brief Function implementing the oledDisplayUpda thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartOledDisplayUpdate */
void StartOledDisplayUpdate(void *argument)
{
  /* USER CODE BEGIN StartOledDisplayUpdate */
  /* Infinite loop */
    uint32_t tick = osKernelGetTickCount();
  for(;;)
  {
	  osMutexAcquire(mutex_cursorHandle, 0);
	  uint16_t cursor_l = cursor_enable;
	  osMutexRelease(mutex_cursorHandle);

      if (cursor_l){
          osMutexAcquire(mutex_positionHandle, 0);
          int16_t position_l = position;
          osMutexRelease(mutex_positionHandle);

          tick_begin = osKernelGetTickCount();
          SSD1306_SetCursor(ssd1306_1, position_l);
          tick_end = osKernelGetTickCount();
          time_ms = tick_end - tick_begin;
      } else {
          SSD1306_ResetCursor(ssd1306_1);
      }
      SSD1306_UpdateScreen(ssd1306_1);

      tick += 100;
      osDelayUntil(tick);
  }
  osThreadTerminate(NULL);

  /* USER CODE END StartOledDisplayUpdate */
}

/* USER CODE BEGIN Header_StartlcdSendMessages */
/**
* @brief Function implementing the lcdSendMessages thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartlcdSendMessages */
void StartlcdSendMessages(void *argument)
{
  /* USER CODE BEGIN StartlcdSendMessages */
  /* Infinite loop */
	uint32_t tick = osKernelGetTickCount();
  for(;;)
  {
	  lcd_i2c_RTOS_Handle_Messages(lcd_i2c_RTOS_1);
	  tick += 1;
	  osDelayUntil(tick);
  }
  osThreadTerminate(NULL);
  /* USER CODE END StartlcdSendMessages */
}

/* USER CODE BEGIN Header_StartLcdDisplayUpdate */
/**
* @brief Function implementing the lcdDisplayUpdat thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartLcdDisplayUpdate */
void StartLcdDisplayUpdate(void *argument)
{
  /* USER CODE BEGIN StartLcdDisplayUpdate */
  /* Infinite loop */
	uint32_t tick = osKernelGetTickCount();
	lcd_i2c_RTOS_Write(lcd_i2c_RTOS_1, 0, 0, "Cursor position:", CLEAR_ON);
  for(;;)
  {
	  osMutexAcquire(mutex_positionHandle, 0);
	  uint16_t position_l = position;
	  osMutexRelease(mutex_positionHandle);

	  char position_str[5];
	  sprintf(position_str, "%d", position_l);
	  lcd_i2c_RTOS_Write(lcd_i2c_RTOS_1, 1, 0, position_str, CLEAR_OFF);
	  tick += 200;
	  osDelayUntil(tick);
  }
  /* USER CODE END StartLcdDisplayUpdate */
}

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM11 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM11) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
