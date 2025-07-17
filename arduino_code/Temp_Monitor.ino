// This example reads temperature from a MAX6675 thermocouple and logs the data
// to a file on an SD card every 30 seconds with a timestamp from a DS3231 RTC.

#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <RTClib.h>
#include <max6675.h>

// --- Pin Definitions ---

// Thermocouple pins (using software SPI)
int thermoDO = 4;
int thermoCS = 5;
int thermoCLK = 6;

// SD Card Chip Select pin (using hardware SPI)
// Note: On an Arduino Uno, the hardware SPI pins are 11 (MOSI), 12 (MISO), and 13 (SCK).
// These are used automatically. You only need to define the CS pin.
const int chipSelect = 10;

// --- Object Initialization ---

// Initialize the RTC object
RTC_DS3231 rtc;

// Initialize the MAX6675 library for the thermocouple
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

// File object for the log file on the SD card
File dataFile;

void setup() {
  // Start serial communication for debugging purposes
  Serial.begin(9600);
  
  // --- RTC Setup ---
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1);
  }

  // The following block will run if the RTC has lost power.
  // It sets the RTC to the date & time this sketch was compiled.
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When the sketch is compiled, the C++ preprocessor saves the date and time as strings.
    // The F() macro stores them in flash memory to save RAM.
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  // --- Thermocouple Setup ---
  Serial.println("MAX6675 & SD Card Datalogger with RTC");
  delay(500); // wait for MAX chip to stabilize

  // --- SD Card Setup ---
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1);
  }
  Serial.println("card initialized.");

  // Create/Open the file and write a header.
  dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.println("Date,Time,Temp (C),Temp (F)");
    dataFile.close();
    Serial.println("Log file header written.");
  } else {
    Serial.println("Error opening datalog.txt");
  }
}

void loop() {
  // Get the current time from the RTC
  DateTime now = rtc.now();

  // Read the temperature from the thermocouple
  float tempC = thermocouple.readCelsius();
  float tempF = thermocouple.readFahrenheit();

  // Open the log file on the SD card to append data
  dataFile = SD.open("datalog.txt", FILE_WRITE);

  if (dataFile) {
    // --- Print data to the Serial Monitor for real-time viewing ---
    Serial.print("Logging: ");
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.print(", ");
    Serial.print(tempC);
    Serial.print(" C, ");
    Serial.print(tempF);
    Serial.println(" F");

    // --- Write the data to the SD card file in CSV format ---
    dataFile.print(now.year(), DEC);
    dataFile.print('/');
    dataFile.print(now.month(), DEC);
    dataFile.print('/');
    dataFile.print(now.day(), DEC);
    dataFile.print(",");
    dataFile.print(now.hour(), DEC);
    dataFile.print(':');
    dataFile.print(now.minute(), DEC);
    dataFile.print(':');
    dataFile.print(now.second(), DEC);
    dataFile.print(",");
    dataFile.print(tempC);
    dataFile.print(",");
    dataFile.println(tempF);

    // Close the file to save the data
    dataFile.close();
    
  } else {
    Serial.println("Error opening datalog.txt");
  }
  
  // Wait for 30 seconds before the next reading
  delay(30000);
}
