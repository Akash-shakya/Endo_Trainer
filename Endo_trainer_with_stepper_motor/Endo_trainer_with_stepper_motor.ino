// Define pin connections & motor's steps per revolution
const int dirPin = 2;
const int stepPin = 3;
const int buttonPin = 4; // Define the push button pin
const int anticlockwiseButtonPin = 5;
const int stepsPerRevolution = 40;

// Variable to store the button state
int buttonState = 0;
int lastButtonState = 0;

int anticlockwiseButtonState = 0;
int lastAnticlockwiseButtonState = 0;

bool motorRunning = false;

// Define pins for the LEDs
#define redled 12
#define greenled 13

int count = 1;

void setup()
{
  // Declare pins as Outputs and Inputs
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP); // Use internal pull-up resistor
  pinMode(anticlockwiseButtonPin, INPUT_PULLUP);
  pinMode(redled, OUTPUT);
  pinMode(greenled, OUTPUT);
  // Initially, motor is stopped
  digitalWrite(dirPin, LOW);
  Serial.begin(9600);
}

void loop()
{
  // Read the push button state
  buttonState = digitalRead(buttonPin);

  // Check if button was pressed (button state changes from HIGH to LOW)
  if (buttonState == LOW && lastButtonState == HIGH) {
    // Toggle motor running state when button is pressed
    motorRunning = !motorRunning;
    digitalWrite(dirPin, HIGH);
    count = count +1;
    delay(200); // Simple debounce
  }
  
  // Read the anticlockwise push button state
  anticlockwiseButtonState = digitalRead(anticlockwiseButtonPin);

  // Check if anticlockwise button was pressed (button state changes from HIGH to LOW)
  if (anticlockwiseButtonState == LOW && lastAnticlockwiseButtonState == HIGH) {
    // Set motor direction to counterclockwise
    motorRunning = !motorRunning;
    digitalWrite(dirPin, LOW);  // Set direction to anticlockwise
    count = count -1;
    delay(200); // Simple debounce
  }
  
  // Update the last button state
  lastButtonState = buttonState;
  lastAnticlockwiseButtonState = anticlockwiseButtonState;

  if(count<6)
  {
  // If motorRunning is true, run the motor
      if (motorRunning) {
        // Set motor direction clockwise
        
        digitalWrite(redled, HIGH);  
        digitalWrite(greenled, LOW);
        Serial.println("Module changed");
        // Spin motor slowly for one revolution
        for (int x = 0; x < stepsPerRevolution; x++) {
          digitalWrite(stepPin, HIGH);
          delayMicroseconds(10000);  // Adjust speed here
          digitalWrite(stepPin, LOW);
          delayMicroseconds(10000);
        }
    
        // Once motor completes a full revolution, stop
        motorRunning = false;
      }
      else{
          Serial.println(count);
          Serial.println("Start Training");
          digitalWrite(redled, LOW);  
          digitalWrite(greenled, HIGH);
        }
        
  }
  else{
    count = 1;
    }
}
