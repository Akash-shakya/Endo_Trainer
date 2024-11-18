#include <LiquidCrystal_I2C.h>

#define dirPin 2
#define stepPin 3
#define stepsPerRevolution 72
#define button 4

// Variables for button debouncing
unsigned long lastDebounceTime = 0;  
unsigned long debounceDelay = 50;  // 50ms debounce time
int lastButtonState = LOW;  // previous button state
int buttonState = LOW;  // current button state

// Define pins for the LEDs (if needed)
#define ledPin1 12
#define ledPin2 13

// Add the lcd
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // Declare pins as output:
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(button, INPUT);  // Set the button pin as input
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);

  lcd.init();
  lcd.backlight();  // Turn on the LCD backlight
  lcd.print("EndoTrainer Kit");  // Put text on the LCD
}

void loop() {
  // Read the state of the button:
  int reading = digitalRead(button);

  // Check if the button state has changed:
  if (reading != lastButtonState) {
    lastDebounceTime = millis();  // reset the debounce timer
  }

  // Only change the buttonState if the debounce delay has passed:
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      // If the button is pressed, move the motor:
      if (buttonState == HIGH) {
        // Set the spinning direction clockwise:
        digitalWrite(dirPin, HIGH);  // Set direction to clockwise
        digitalWrite(ledPin1, HIGH);  // Turn on LED 1
        digitalWrite(ledPin2, LOW);   // Turn off LED 2
        lcd.clear();
        lcd.print("Waiting...");
        // Spin the stepper motor 1 revolution slowly:
        for (int i = 0; i < stepsPerRevolution; i++) {
          digitalWrite(stepPin, HIGH);
          delayMicroseconds(8000);  // Adjust for desired motor speed
          digitalWrite(stepPin, LOW);
          delayMicroseconds(8000);
        }
      } 
      // If the button is released, reverse direction:
      else {
        digitalWrite(ledPin1, LOW);  // Turn off LED 1
        digitalWrite(ledPin2, HIGH); // Turn on LED 2
        digitalWrite(dirPin, LOW);   // Set direction to counterclockwise
        lcd.clear();
        lcd.print("Module Change");
        lcd.setCursor(2,1);
        lcd.print("Start Trainig");
      }
    }
  }

  // Save the current button state for next loop iteration
  lastButtonState = reading;
}
