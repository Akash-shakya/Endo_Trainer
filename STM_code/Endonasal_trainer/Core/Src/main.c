
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "main.h"
#include "lcd.h"
/* Private includes ----------------------------------------------------------*/
#define LED_GPIO_PORT GPIOA
#define RED_LED_PIN GPIO_PIN_4
#define GREEN_LED_PIN GPIO_PIN_10

#define BUTTON_GPIO_PORT GPIOA
#define button_CW GPIO_PIN_0
#define button_ACW GPIO_PIN_1


#define MOTOR_GPIO_PORT GPIOC
#define STEP_PIN GPIO_PIN_1
#define DIR_PIN GPIO_PIN_0

int count = 1;

// Stepper Motor Control Variables
const int STEPS_PER_REVOLUTION = 320;
int stepDelayMicroseconds = 1000;

int motorRunning = 0;

// Variables for button state and debounce logic
int buttonState = 0;          // Current button state
int lastButtonState = 0;      // Previous button state

int anticlockwiseButtonState = 0;
int lastAnticlockwiseButtonState = 0;
/* Private function prototypes -----------------------------------------------*/

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void delay_ms(uint32_t ms);
char countstr[50];
int main(void)
{

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

	/* USER CODE BEGIN 2 */
	Lcd_PortType ports[] = { GPIOC, GPIOB, GPIOA, GPIOA };  // Lcd_PinType pins[] = {D4_Pin, D5_Pin, D6_Pin, D7_Pin};
	Lcd_PinType pins[] = {GPIO_PIN_7, GPIO_PIN_6, GPIO_PIN_7, GPIO_PIN_6};

	Lcd_HandleTypeDef lcd;

	// Lcd_create(ports, pins, RS_GPIO_Port, RS_Pin, EN_GPIO_Port, EN_Pin, LCD_4_BIT_MODE);
	lcd = Lcd_create(ports, pins, GPIOB, GPIO_PIN_5, GPIOB, GPIO_PIN_4, LCD_4_BIT_MODE);

	Lcd_cursor(&lcd, 0,1);
	Lcd_string(&lcd, "EndoTrainer Kit");
	HAL_Delay(2000);
	Lcd_clear(&lcd);
  while (1)
  {
	  buttonState = HAL_GPIO_ReadPin(BUTTON_GPIO_PORT, button_CW);

	  // Check if button was pressed (LOW)
	  if (buttonState == GPIO_PIN_RESET && lastButtonState == GPIO_PIN_SET) {
		  motorRunning = 1;
		  HAL_GPIO_WritePin(GPIOC, DIR_PIN, GPIO_PIN_SET);  // Clockwise
		  count++;
		  HAL_Delay(200);  // Debounce delay
	  }

	  anticlockwiseButtonState = HAL_GPIO_ReadPin(BUTTON_GPIO_PORT, button_ACW);
	  if (anticlockwiseButtonState == GPIO_PIN_RESET && lastAnticlockwiseButtonState == GPIO_PIN_SET) {
		  motorRunning = 1;
		  HAL_GPIO_WritePin(GPIOC, DIR_PIN, GPIO_PIN_RESET);  // Counterclockwise
		  count--;
		  HAL_Delay(200);  // Debounce delay
	  }
      // Update the last button states
      lastButtonState = buttonState;
      lastAnticlockwiseButtonState = anticlockwiseButtonState;

      if (count < 6 && count > 0) {
		  // If motor is running, move the stepper motor
		  if (motorRunning==1) {
				  // Turn on red LED
				  HAL_GPIO_WritePin(LED_GPIO_PORT, RED_LED_PIN, GPIO_PIN_SET);
				  HAL_GPIO_WritePin(LED_GPIO_PORT, GREEN_LED_PIN, GPIO_PIN_RESET);
				  Lcd_clear(&lcd);
				  Lcd_string(&lcd, "Module changed");

				  // Rotate the motor for one revolution
				  for (int i = 0; i < STEPS_PER_REVOLUTION; i++) {
					  HAL_GPIO_WritePin(GPIOC, STEP_PIN, GPIO_PIN_SET);  // Step signal
					  HAL_Delay(stepDelayMicroseconds / 1000);  // Delay in milliseconds (convert microseconds to ms)
					  HAL_GPIO_WritePin(GPIOC, STEP_PIN, GPIO_PIN_RESET); // Step signal off
					  HAL_Delay(stepDelayMicroseconds / 1000);  // Delay in milliseconds (convert microseconds to ms)
				  }
				  motorRunning = 0;  // Stop the motor after one revolution
				  Lcd_clear(&lcd);
		  }
		  else
			  {
				  // Show training details on LCD
				  HAL_GPIO_WritePin(LED_GPIO_PORT, RED_LED_PIN, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(LED_GPIO_PORT, GREEN_LED_PIN, GPIO_PIN_SET);
				  Lcd_string(&lcd, "Module No :");
				  Lcd_cursor(&lcd, 0,13);
				  itoa(count, countstr, 10);
				  Lcd_string(&lcd,countstr);
				  Lcd_cursor(&lcd, 1,0);
				  Lcd_string(&lcd, "Start Training  ");
				  }
      }
	  else {
		  if (count > 5) {
			  count = 1;
		  }
		  else {
			  count = 5;
			  }
	    }
      }
}

void delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

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
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, DIR_Pin|STEP_Pin |D4_Pin, GPIO_PIN_RESET);

  /*Configure LED  GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_PORT, RED_LED_PIN|GREEN_LED_PIN, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, D7_Pin|D6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, EN_Pin|RS_Pin|D5_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : DIR_Pin STEP_Pin
                           D4_Pin */
  GPIO_InitStruct.Pin = DIR_Pin|STEP_Pin|D4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : button_cw_Pin button_acw_Pin */
  GPIO_InitStruct.Pin = button_cw_Pin|button_acw_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BUTTON_GPIO_PORT, &GPIO_InitStruct);

  /*Configure GPIO pins : RED_LEDPIN AND GREEN lED PIN */
  GPIO_InitStruct.Pin = RED_LED_PIN|GREEN_LED_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);


  /*Configure GPIO pins : D7_Pin D6_Pin */
  GPIO_InitStruct.Pin = D7_Pin|D6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : EN_Pin RS_Pin D5_Pin */
  GPIO_InitStruct.Pin = EN_Pin|RS_Pin|D5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
