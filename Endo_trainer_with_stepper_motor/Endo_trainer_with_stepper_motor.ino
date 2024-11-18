#include <LiquidCrystal_I2C.h>

#define dirPin 2
#define stepPin 3
#define stepsPerRevolution 40 // 360 degree in 200 step,   72 degree in 40 step
#define button 4

// Variables for button debouncing
unsigned long lastDebounceTime = 0;  
unsigned long debounceDelay = 50;  // 50ms debounce time
int lastButtonState = LOW;  // previous button state
int buttonState = LOW;  // current button state

int count = 1;

// Define pins for the LEDs
#define redled 12
#define greenled 13

// Add the lcd
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // Declare pins as output:
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(button, INPUT);  // Set the button pin as input
  pinMode(redled, OUTPUT);
  pinMode(greenled, OUTPUT);

  lcd.init();
  lcd.backlight();  // Turn on the LCD backlight
  lcd.print("EndoTrainer Kit");  // Put text on the LCD
  delay(1000);
  lcd.clear();
  lcd.print("Module : 1");
  lcd.setCursor(1,1);
  lcd.print("Start Training");
}

void loop() {
 
  int reading = digitalRead(button);   // Read the state of the button:

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
        count = count +1;
        if(count<7)
        {
          digitalWrite(dirPin, HIGH);  // Set direction to clockwise
          digitalWrite(redled, HIGH);  // Turn on LED 1
          digitalWrite(greenled, LOW);   // Turn off LED 2
          lcd.clear();
          lcd.print("Module changed");
          lcd.setCursor(3,1);
          lcd.print("Waiting....");
          // Spin the stepper motor 1 revolution slowly:
          for (int i = 0; i < stepsPerRevolution; i++) {
            digitalWrite(stepPin, HIGH);
            delayMicroseconds(8000);  // Adjust for desired motor speed
            digitalWrite(stepPin, LOW);
            delayMicroseconds(8000);
          }
        } 
      }
      if (count ==6)
      {
        count = 1;
      }
      else {
        digitalWrite(redled, LOW);  // Turn off LED 1
        digitalWrite(greenled, HIGH); // Turn on LED 2
        digitalWrite(dirPin, LOW);   // Set direction to counterclockwise
        lcd.clear();
        lcd.print("Module No :");
        lcd.setCursor(12,0);
        lcd.print(count);
        lcd.setCursor(1,1);
        lcd.print("Start Training");
      }
    }
  }
  lastButtonState = reading;
}
