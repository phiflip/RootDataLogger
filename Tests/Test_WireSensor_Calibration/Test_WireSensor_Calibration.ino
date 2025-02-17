// Simple test_wire_sensor.ino - Basic Calibration for Wire Sensor
// Reads sensor voltage and calculates displacement conversion factor

#include <Arduino.h>

const int wireSensorPin = A0;  // Analog input pin for the Wire Sensor
//Logger I = 4.92 V, Logger-II = 4.98 V 
const int adcResolution = 1023; // 10-bit ADC resolution
const float referenceVoltage = 4.92; // Reference voltage of the microcontroller 

void setup() {
    Serial.begin(9600);
    Serial.println("Wire Sensor Calibration - Move the sensor and observe voltage changes.");
}

void loop() {
    float voltage = readWireSensor();
    Serial.print("Sensor Voltage: ");
    Serial.print(voltage, 3);
    Serial.println(" V");
    delay(500);
}

float readWireSensor() {
    int rawValue = analogRead(wireSensorPin);
    return (rawValue * referenceVoltage) / adcResolution;
}
