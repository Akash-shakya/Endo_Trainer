#include "stm32f4xx_hal.h"
#include "lcd.h"

#define redled GPIO_PIN_0
#define greenled GPIO_PIN_1
#define button_Pin GPIO_PIN_11
#define BUTTON_GPIO_PORT GPIOB
#define LED_GPIO_PORT GPIOA

#define STEP_PIN GPIO_PIN_4
#define DIR_PIN GPIO_PIN_12
#define MOTOR_GPIO_PORT GPIOA

// Stepper Motor Control Variables
const int stepsPerRevolution = 40;
int stepDelayMicroseconds = 8000;
int count = 1;

// Variables for button state and debounce logic
int buttonState = 0;          // Current button state
int lastButtonState = 0;      // Previous button state
unsigned long lastDebounceTime = 0;  // Time of the last button state change
unsigned long debounceDelay = 50;    // Debounce time in milliseconds

void SystemClock_Config(void);
static void MX_GPIO_Init(void);

int main(void) {
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  // Lcd_PortType ports[] = { D4_GPIO_Port, D5_GPIO_Port, D6_GPIO_Port, D7_GPIO_Port };
  Lcd_PortType ports[] = { GPIOC, GPIOB, GPIOA, GPIOA };
  // Lcd_PinType pins[] = {D4_Pin, D5_Pin, D6_Pin, D7_Pin};
  Lcd_PinType pins[] = {GPIO_PIN_7, GPIO_PIN_6, GPIO_PIN_7, GPIO_PIN_6};

  Lcd_HandleTypeDef lcd;
  // Lcd_create(ports, pins, RS_GPIO_Port, RS_Pin, EN_GPIO_Port, EN_Pin, LCD_4_BIT_MODE);
  lcd = Lcd_create(ports, pins, GPIOB, GPIO_PIN_5, GPIOB, GPIO_PIN_4, LCD_4_BIT_MODE);
  Lcd_string(&lcd, " EndoTrainer Kit");
  HAL_Delay(1000);
  Lcd_clear(&lcd);
  Lcd_string(&lcd, "Module No : 1");
  Lcd_cursor(&lcd, 1,0);
  Lcd_string(&lcd, "Start Training");
  while (1)
   {
       // Read the state of the push button (input)
    int reading = HAL_GPIO_ReadPin(BUTTON_GPIO_PORT, button_Pin);

    // Check if the button state has changed (HIGH to LOW or LOW to HIGH)
    if (reading != lastButtonState) {
      lastDebounceTime = HAL_GetTick();  // Reset the debounce timer
    }

    // If the time since the last state change is greater than debounce delay, update the button state
    if ((HAL_GetTick() - lastDebounceTime) > debounceDelay) {
      if (reading != buttonState) {
        buttonState = reading;

        // Only toggle the LED if the new button state is LOW (button is pressed)
        if (buttonState == GPIO_PIN_RESET)
        {
          count = count +1;
          if(count<7)
          {
        	Lcd_clear(&lcd);
        	Lcd_cursor(&lcd, 1,0);
        	Lcd_string(&lcd, "Module changed");
        	Lcd_cursor(&lcd, 1,3);
        	Lcd_string(&lcd, "Waiting...");

            HAL_GPIO_WritePin(GPIOA, DIR_PIN, GPIO_PIN_SET);
            HAL_GPIO_WritePin(LED_GPIO_PORT, redled, GPIO_PIN_SET);
            HAL_GPIO_WritePin(LED_GPIO_PORT, greenled, GPIO_PIN_RESET);

            for (int i = 0; i < stepsPerRevolution; i++) {
              // Pulse the step pin HIGH to make the motor step
              HAL_GPIO_WritePin(MOTOR_GPIO_PORT, STEP_PIN, GPIO_PIN_SET);
              HAL_Delay(stepDelayMicroseconds / 1000);  // Delay in milliseconds (convert microseconds to ms)
              // Pulse the step pin LOW to complete the step
              HAL_GPIO_WritePin(MOTOR_GPIO_PORT, STEP_PIN, GPIO_PIN_RESET);
              HAL_Delay(stepDelayMicroseconds / 1000);  // Delay in milliseconds (convert microseconds to ms)
            }
          }

        }
        if (count ==6)
        {
          count = 1;
          HAL_GPIO_WritePin(LED_GPIO_PORT, redled, GPIO_PIN_RESET);
          HAL_GPIO_WritePin(LED_GPIO_PORT, greenled, GPIO_PIN_SET);
        }
        else{
			HAL_GPIO_WritePin(GPIOA, DIR_PIN, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LED_GPIO_PORT, redled, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LED_GPIO_PORT, greenled, GPIO_PIN_SET);
			Lcd_clear(&lcd);
			Lcd_string(&lcd, "Module No :");
			Lcd_cursor(&lcd, 0,12);
			Lcd_string(&lcd, count);
			Lcd_cursor(&lcd, 1,1);
			Lcd_string(&lcd, "Start Training");
         }

      }
    }
    lastButtonState = reading;
  }
}

void SystemClock_Config(void) {
  // System Clock Configuration code here
}

static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pins : redled_Pin greenled_Pin */
  GPIO_InitStruct.Pin = redled | greenled;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);

  /*Configure GPIO pin : button_Pin */
  GPIO_InitStruct.Pin = button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BUTTON_GPIO_PORT, &GPIO_InitStruct);

  /*Configure GPIO pins : steppin_Pin dirpin_Pin */
  GPIO_InitStruct.Pin = STEP_PIN|DIR_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MOTOR_GPIO_PORT, &GPIO_InitStruct);

  /*Configure GPIO pins : PB4 PB5 PB6 */
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Configure GPIO pin : PC7 */
     GPIO_InitStruct.Pin = GPIO_PIN_7;
     GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
     GPIO_InitStruct.Pull = GPIO_NOPULL;
     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
     HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

     /*Configure GPIO pins : PA6 PA7 */
       GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
       GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
       GPIO_InitStruct.Pull = GPIO_NOPULL;
       GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
       HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}
