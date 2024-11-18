import serial
import time
import os  # For handling directory creation

# Set the port and baud rate
port = 'COM8'  # Replace with your Arduino's port
baud_rate = 230400

# Prompt for user input and create the file name
filename = input("Enter the user name: ")
filename = f"{filename}.txt"

# Define the data directory
data_folder = 'Endo_Data'

# Check if the 'data' folder exists, create it if it doesn't
if not os.path.exists(data_folder):
    os.makedirs(data_folder)

# Create the full file path by joining the folder and filename. 
output_file = os.path.join(data_folder, filename)

# Open the serial connection
ser = serial.Serial(port, baud_rate)
time.sleep(2)  # Wait for the Arduino to initialize

# Open the file in write mode
with open(output_file, 'w') as f:
    print("Start collecting data... Press Ctrl+C to stop.")
    while True:
        try:
            # Read data from the serial port
            data = ser.readline().decode('utf-8').strip()  # Decode the byte string to a normal string
            
            # Print the data to the console
            print(data)
            
            # Save the data to the file
            f.write(data + '\n')
            
        except KeyboardInterrupt:
            print("\nData collection stopped.")
            break
        except Exception as e:
            print(f"Error: {e}")
            break

# Close the serial connection
ser.close()
