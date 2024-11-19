#include "stm32c0xx_hal.h"

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

}
