#include <Wire.h> // Include the I2C communication library
#include <Adafruit_GFX.h> // Include the Adafruit GFX library for graphics functions
#include <Adafruit_SSD1306.h> // Include the Adafruit SSD1306 library for the OLED display
#include "SoftwareSerial.h" // Include the SoftwareSerial library for serial communication on non-standard pins
#include "DFRobotDFPlayerMini.h" // Include the DFRobotDFPlayerMini library to control the DFPlayer Mini module

// OLED display configuration
#define SCREEN_WIDTH 128 // Define the OLED display width in pixels
#define SCREEN_HEIGHT 64 // Define the OLED display height in pixels
#define OLED_RESET -1 // Define the reset pin for the OLED display (-1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT); // Create an instance of the display object

// Analog pin for sensor input
#define analogPin A0 // Define the analog pin for sensor input

// Software serial for DFPlayer Mini
SoftwareSerial mySoftwareSerial(13, 15); // Define software serial RX on pin 13 and TX on pin 15
DFRobotDFPlayerMini myDFPlayer; // Create an instance of the DFPlayer Mini object

// Timing variables
unsigned long previousMillis = 0; // Store the last time the DFPlayer was checked
const long interval = 3100; // Interval to wait for song playback in milliseconds
bool isPlaying = false; // Boolean to track if the DFPlayer is currently playing

void setup() {
  // Initialize software serial and hardware serial
  mySoftwareSerial.begin(9600); // Begin software serial communication at 9600 baud rate
  Serial.begin(115200); // Begin hardware serial communication at 115200 baud rate
  
  // Initialize DFPlayer Mini
  if (!myDFPlayer.begin(mySoftwareSerial)) { // Check if the DFPlayer Mini initializes correctly
    Serial.println(F("Not initialized:")); // Print error message if initialization fails
    Serial.println(F("1. Check the DFPlayer Mini connections")); // Print instruction to check connections
    Serial.println(F("2. Insert an SD card")); // Print instruction to insert an SD card
    while (true); // Stay in an infinite loop if initialization fails
  }
  
  Serial.println(F("DFPlayer Mini module initialized!")); // Print success message
  myDFPlayer.setTimeOut(500); // Set serial timeout to 500 milliseconds
  myDFPlayer.volume(60); // Set volume to 60 (out of 30-100 range)
  myDFPlayer.EQ(0); // Set equalizer to normal (0)

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Check if the display initializes correctly with I2C address 0x3C
    Serial.println(F("SSD1306 allocation failed")); // Print error message if initialization fails
    while (true); // Stay in an infinite loop if initialization fails
  }
  
  display.clearDisplay(); // Clear the display buffer
  display.setTextSize(2); // Set text size to 2
  display.setTextColor(SSD1306_WHITE); // Set text color to white
  display.setCursor(0, 10); // Set cursor position at (0, 10)
  display.println(F("Initializing...")); // Print "Initializing..." on the display
  display.display(); // Update the display with the above settings
  delay(2000); // Wait for 2 seconds
}

void loop() {
  unsigned long currentMillis = millis(); // Get the current time in milliseconds

  // Read the input on analog pin 0
  int sensorValue = analogRead(A0); // Read the analog sensor value from pin A0

  // Update OLED display based on sensor value
  display.clearDisplay(); // Clear the display buffer
  display.setTextSize(2); // Set text size to 2
  
  if (sensorValue > 500) { // Check if sensor value is greater than 500
    // If sensor value is greater than 500, display "Cough"
    int16_t x1, y1; // Variables to store text bounds
    uint16_t w, h; // Variables to store text width and height
    display.getTextBounds("Cough", 0, 0, &x1, &y1, &w, &h); // Get the bounds of the text "Cough"
    display.setCursor((SCREEN_WIDTH - w) / 2, (SCREEN_HEIGHT - h) / 2); // Center the text on the display
    display.println(F("Cough")); // Print "Cough" on the display
    
    if (!isPlaying) { // Check if the DFPlayer is not already playing
      // Play sound if not already playing
      myDFPlayer.play(1); // Play the first track on the DFPlayer
      isPlaying = true; // Set isPlaying to true
      previousMillis = currentMillis; // Update the previousMillis with the current time
    }
  } else { // If sensor value is less than or equal to 500
    // If sensor value is less than or equal to 500, display "No cough"
    int16_t x1, y1; // Variables to store text bounds
    uint16_t w, h; // Variables to store text width and height
    display.getTextBounds("No cough", 0, 0, &x1, &y1, &w, &h); // Get the bounds of the text "No cough"
    display.setCursor((SCREEN_WIDTH - w) / 2, (SCREEN_HEIGHT - h) / 2); // Center the text on the display
    display.println(F("No cough")); // Print "No cough" on the display
    isPlaying = false; // Reset isPlaying to false if no cough is detected
  }

  // Reset the playing status after the song has finished playing
  if (isPlaying && currentMillis - previousMillis >= interval) { // Check if the song playback interval has passed
    isPlaying = false; // Reset isPlaying to false
  }

  display.display(); // Update the display with the above settings
  delay(100); // Short delay to avoid rapid updates
}
