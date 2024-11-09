// Define the pin numbers for the potentiometers
const int potPins[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11}; // Adjust pins as needed
const int numPots = sizeof(potPins) / sizeof(potPins[0]); // Number of potentiometers

void setup() {
    // Initialize serial communication at 9600 bits per second
    Serial.begin(9600);
    // Print the table header
    Serial.println("");
    Serial.println("PO1\tPO2\tPO3\tPO4\tPO5\tPO6\tPO7\tPO8\tPO9\tPO10\tPO11\tPO12");
    Serial.println("---------------------------------------------------------------------------------------------");
    
}

void loop() {
    // Create an array to hold the mapped angles
    int potAngles[numPots];

    // Read the value from each potentiometer and map it to degrees (0-300)
    for (int i = 0; i < numPots; i++) {
        int potValue = analogRead(potPins[i]); // Read the potentiometer value
        potAngles[i] = map(potValue, 0, 1023, 0, 300); // Map the value to 0-300
        
    }
    
    // Print the values in table format
    for (int i = 0; i < numPots; i++) {
        Serial.print(potAngles[i]);
        Serial.print(","); // Tab space for alignment
    }
    Serial.println(); // Move to the next line after printing all angles

    // Wait for a short time before the next reading
    delay(10);
}
