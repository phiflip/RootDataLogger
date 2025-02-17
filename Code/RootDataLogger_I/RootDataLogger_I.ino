#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
#include "HX711.h"
#include <SoftwareSerial.h>

// Bluetooth HC-05 (RX, TX)
#define BT_RX_PIN 9
#define BT_TX_PIN 4
SoftwareSerial BTSerial(BT_RX_PIN, BT_TX_PIN);

// ====== Loadcell Calibration Values ======
#define calibration_factor_2000kg -1060
#define calibration_factor_200kg -10750

#define LOADCELL_DOUT_PIN 3
#define LOADCELL_SCK_PIN 2
#define LOADCELL_SWITCH_PIN A2  // Loadcell switch is connected to A2

// LED Pins
#define blueLEDpin 5
#define redLEDpin 6
#define greenLEDpin 7
#define echoSwitch 8

// SD Card Configuration
#define LOG_INTERVAL 10     // Time in milliseconds between data entries
#define SYNC_INTERVAL 200  // Time in milliseconds before flushing data to SD card
uint32_t syncTime = 0;      // Stores last sync timestamp

#define ECHO_TO_SERIAL 0  // Set to 1 to enable serial output
#define WAIT_TO_START 0   // Set to 1 to wait for serial input before starting

const int chipSelect = 10;
File logfile;

// RTC (Real-Time Clock)
RTC_PCF8523 RTC;

// HX711 Loadcell Amplifier
HX711 scale;
bool is200kg = true;  // Default to 200kg loadcell

// ========================== ERROR HANDLER ==========================
void error(char *str) {
  Serial.print("Error: ");
  Serial.println(str);
  digitalWrite(redLEDpin, HIGH);  // Turn on red LED to indicate an error
  while (1)
    ;
}

// ========================== SETUP FUNCTION ==========================
void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);

  pinMode(echoSwitch, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);  // Use A2 as a digital GPIO with internal pull-up

  // Initialize HX711
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_gain(64);  // Set the gain for 2mV/V load cells

  // Read switch position and apply the correct calibration
  bool switchState = digitalRead(LOADCELL_SWITCH_PIN);
  if (switchState == LOW) {
    scale.set_scale(calibration_factor_200kg);
    Serial.println("Startup: 200kg loadcell selected.");
    BTSerial.println("Startup: 200kg loadcell selected.");
  } else {
    scale.set_scale(calibration_factor_2000kg);
    Serial.println("Startup: 2000kg loadcell selected.");
    BTSerial.println("Startup: 2000kg loadcell selected.");
  }

  // Let HX711 settle
  delay(1000);

  // Ignore first few readings (to remove initial drift)
  for (int i = 0; i < 5; i++) {
    scale.get_units();
    delay(100);
  }

  // Perform tare once
  Serial.println("Performing tare...");
  scale.tare();
  Serial.println("Tare completed.");



  // Configure LED pins
  pinMode(redLEDpin, OUTPUT);
  pinMode(greenLEDpin, OUTPUT);
  pinMode(blueLEDpin, OUTPUT);

  // Initialize SD card
  Serial.print("Initializing SD card...");
  pinMode(chipSelect, OUTPUT);
  if (!SD.begin(chipSelect)) {
    error("SD card failed, or not present");
  }
  Serial.println("SD card initialized.");

  // Create a new log file
  char filename[] = "ROOT_000.CSV";
  for (uint8_t i = 0; i < 1000; i++) {
    filename[5] = '0' + (i / 100) % 10;
    filename[6] = '0' + (i / 10) % 10;
    filename[7] = '0' + i % 10;
    if (!SD.exists(filename)) {
      logfile = SD.open(filename, FILE_WRITE);
      break;
    }
  }

  if (!logfile) {
    error("Could not create log file");
  }
  Serial.print("Logging to: ");
  Serial.println(filename);

  // Initialize RTC (Real-Time Clock)
  Wire.begin();
  if (!RTC.begin()) {
    logfile.println("RTC failed");
    Serial.println("RTC failed");
  }

  logfile.println("timestamp,cableLength[cm],weight[N]");
}

// ========================== LOADCELL SWITCHING FUNCTION ==========================
// Stores the last known switch state to detect changes
bool lastSwitchState = HIGH;  // Assume 2000kg is the initial state

void updateLoadcell() {
  bool switchState = digitalRead(LOADCELL_SWITCH_PIN);  // Read switch state

  if (switchState != lastSwitchState) {  // Only update if the switch position changed
    scale.set_gain(64);                  // Set the gain for 2mV/V output cells

    if (switchState == LOW) {  // Switch pressed → Activate 200kg loadcell
      scale.set_scale(calibration_factor_200kg);
      scale.tare();
      Serial.println("Switched to 200kg loadcell.");
      BTSerial.println("Switched to 200kg loadcell.");
    } else {  // Switch open → Activate 2000kg loadcell
      scale.set_scale(calibration_factor_2000kg);

      scale.tare();
      Serial.println("Switched to 2000kg loadcell.");
      BTSerial.println("Switched to 2000kg loadcell.");
    }
    lastSwitchState = switchState;  // Update the last known state
    delay(500);                     // Short delay to prevent switch bouncing
  }
}



// ========================== MAIN LOOP ==========================
void loop() {
  int echoSwitchState = digitalRead(echoSwitch);
  delay(2);

  // Wait for the next LOG_INTERVAL
  delay((LOG_INTERVAL - 1) - (millis() % LOG_INTERVAL));

  uint32_t m = millis();
  DateTime now = RTC.now();
  uint16_t milliseconds = m % 1000;

  // Generate timestamp with milliseconds
  char timestamp[24];
  sprintf(timestamp, "%02d-%02d-%02d.%03d",
          now.hour(), now.minute(), now.second(), milliseconds);

  // Check and update loadcell calibration
  updateLoadcell();

  // Read cable sensor voltage
  int cableSensorReading = analogRead(A0);
  float voltage = cableSensorReading * 4.92 / 1024.0;
  float cableLength = (voltage * (41.85));

  // Read weight from HX711 and convert to Newtons
  float weightN = scale.get_units() * 9.81;

  // ========================== SD CARD LOGGING ==========================
  if (echoSwitchState == 1) {
    digitalWrite(greenLEDpin, HIGH);
    digitalWrite(blueLEDpin, LOW);

    logfile.print(timestamp);
    logfile.print(", ");
    logfile.print(cableLength);
    logfile.print(", ");
    logfile.println(weightN, 3);

    logfile.flush();
    delay(1);
  }

  // ========================== BLUETOOTH OUTPUT ==========================
  if (echoSwitchState == 0) {
    int batteryVoltageSensorReading = analogRead(A1);
    float batteryVoltage = batteryVoltageSensorReading / 61.5;

    digitalWrite(greenLEDpin, LOW);
    digitalWrite(blueLEDpin, HIGH);

    BTSerial.print(timestamp);
    BTSerial.print(", ");
    BTSerial.print(cableLength);
    BTSerial.print("cm, ");
    BTSerial.print(weightN, 3);
    BTSerial.print("N, ");
    BTSerial.print(batteryVoltage);
    BTSerial.println("V");

    Serial.print(timestamp);
    Serial.print(", ");
    Serial.print(cableLength);
    Serial.print(" cm, ");
    Serial.print(weightN, 3);
    Serial.print(" N, ");
    Serial.print(batteryVoltage);
    Serial.println(" V");

    delay(500);
  }

  digitalWrite(greenLEDpin, LOW);
  digitalWrite(blueLEDpin, LOW);

  // ========================== SD CARD SYNC ==========================
  if ((millis() - syncTime) >= SYNC_INTERVAL) {
    syncTime = millis();
    digitalWrite(redLEDpin, HIGH);
    logfile.flush();
    digitalWrite(redLEDpin, LOW);
  }

  // ========================== BLUETOOTH RECEIVING ==========================
  if (BTSerial.available()) {
    String incoming = BTSerial.readStringUntil('\n');
    Serial.print("BT received: ");
    Serial.println(incoming);
  }
}
