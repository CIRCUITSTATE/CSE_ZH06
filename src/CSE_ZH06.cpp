
//======================================================================================//
/*
  Filename: CSE_ZH06.cpp
  Description: Main source file for the CSE_ZH06 Arduino library.
  Framework: Arduino, PlatformIO
  Author: Vishnu Mohanan (@vishnumaiea, @vizmohanan)
  Maintainer: CIRCUITSTATE Electronics (@circuitstate)
  Version: 0.0.1
  License: MIT
  Source: https://github.com/CIRCUITSTATE/CSE_ZH06
  Last Modified: +05:30 20:40:19 PM 23-12-2024, Monday
 */
//======================================================================================//

#include "CSE_ZH06.h"
#include <CSE_MillisTimer.h>

//======================================================================================//

#if SOFTWARE_SERIAL_REQUIRED
  /**
   * @brief Accepts a software serial port to be used.
   * 
   * @param swSerial 
   * @return CSE_ZH06:: 
   */
  CSE_ZH06:: CSE_ZH06 (SoftwareSerial& swSerial): _serial (&swSerial) {
  }
#else
  /**
   * @brief Accepts a hardware serial port to be used.
   * 
   * @param hwSerial 
   * @return CSE_ZH06:: 
   */
  CSE_ZH06:: CSE_ZH06 (HardwareSerial& hwSerial): _serial (&hwSerial) {
  }
#endif

//======================================================================================//
/**
 * @brief Does nothing for now.
 * 
 * @return CSE_ZH06::~ 
 */
CSE_ZH06::~ CSE_ZH06 () {
}

//======================================================================================//
/**
 * @brief Does nothing for now. The user must initialize the serial port they want to use
 * before interacting with the sensor.
 * 
 */
void CSE_ZH06:: begin() {
}

//======================================================================================//
/**
 * @brief Sends some data to the sensor. The sensor's response is saved to the `rxData`
 * array. Receiving does not work as expected.
 * 
 * @param txData The array of bytes to send.
 * @param txLength The length of the data to send. Should be less than or equal to `txData` length.
 * @param rxData The array for saving the bytes to received.
 * @param rxLength The length of the data to receive. Should be less than or equal to `rxData` length.
 */
void CSE_ZH06:: sendData (uint8_t* txData, uint8_t txLength, uint8_t* rxData, uint8_t rxLength) {
  for (int i = 0; i < txLength; i++) {  // Write the data to the sensor
    _serial->write (txData [i]);
  }

  //TODO: Add a user defined delay here, so that the sensor will have enough time to respond.

  if (rxLength > 0) {
    _serial->readBytes (rxData, rxLength);  // Read the reply from the sensor
  }

  return;
}

//======================================================================================//
/**
 * @brief Sends some data to the sensor. This expects a one way transfer and does not
 * read back what the sensor sent.
 * 
 * @param txData An array of bytes to send.
 * @param txLength The length of the data to send. Should be less than or equal to `txData` length.
 */
void CSE_ZH06:: sendData (uint8_t* txData, uint8_t txLength) {
  sendData (txData, txLength, rxData, 0);
  return;
}

//======================================================================================//
/**
 * @brief Reads the PM data from the sensor. It first sends the request for the PM data
 * and reads back the response. The response contains the PM data for PM1.0, PM2.5 and PM10.
 * These values are then stored to the pm1, pm25 and pm10 variables and the user can directly
 * read them. Additionally, the user can also pass a pointer to an array to store the data.
 * The array should be of type `uint16_t` and should have a length of 3.
 * 
 * @param data An array of three `uint16_t` to store the PM data. Optional. Defaults to `NULL`.
 * @return true Operation successful.
 * @return false Operation failed.
 */
bool CSE_ZH06:: getPmData (uint16_t* data) {
  uint8_t aqiCommand [9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};

  // Send the PM data request command.
  sendData (aqiCommand, 9);

  uint8_t rxDataByte = 0;
  bool readCompleted = false;
  uint8_t byteCount = 0;

  CSE_MillisTimer timer (3000); // Create a timer to wait for the reply
  timer.start();  // Start the timer before starting to read the data

  while ((!readCompleted) && (!timer.isElapsed())) {  // Read the data
    if (_serial->available() > 0) {
      rxDataByte = _serial->read();

      if ((rxDataByte == 0xFF) && (byteCount == 0)) { // Read the Start Byte 1
        rxData [0] = rxDataByte;
        byteCount++;
      }

      else if ((rxDataByte == 0x86) && (byteCount == 1)) { // Read the command byte
        rxData [1] = rxDataByte;
        byteCount++;
      }

      else if ((byteCount > 1) && (byteCount < 9)) {  // Read the remaining data bytes
        rxData [byteCount] = rxDataByte;
        byteCount++;
      }

      if (byteCount >= 9) {
        readCompleted = true;
      }
    }
  }

  // If the byte count at this point is not above 9,
  // we can assume that the operation was timed out.
  if (byteCount < 9) {
    Serial.println ("Data not received. Timed out.");
    return false;
  }

  // Serial.println ("Data received. Printing data..");

  // for (int i = 0; i < 9; i++) {
  //   Serial.print (rxData [i], HEX);
  //   Serial.print (" ");
  // }

  // Serial.println();

  // Combine bytes 2 and 3 to get the PM1.0 value
  pm1 = float (uint16_t (rxData [6] << 8) | uint16_t (rxData [7]));
  // Combine bytes 4 and 5 to get the PM2.5 value
  pm25 = float (uint16_t (rxData [2] << 8) | uint16_t (rxData [3]));
  // Combine bytes 6 and 7 to get the PM10 value
  pm10 = float (uint16_t (rxData [4] << 8) | uint16_t (rxData [5]));

  // Save the data to the user provided array
  if (data != NULL) {
    data [0] = pm1;
    data [1] = pm25;
    data [2] = pm10;
  }

  return true;
}

//======================================================================================//
/**
 * @brief Puts the sensor into sleep mode as specified in the datasheet. This function
 * can actually put the sensor to sleep mode as well as wake it up from the sleep mode.
 * Use the `toSleep` parameter to specify whether to put the sensor to sleep or wake it up.
 * 
 * @param toSleep Whether to sleep or wake up the sensor.
 * @return true Operation successful.
 * @return false Operation failed.
 */
bool CSE_ZH06:: sleep (bool toSleep) {
  // Default command for sleeping.
  uint8_t sleepCommand [9] = {0xFF, 0x01, 0xA7, 0x01, 0x00, 0x00, 0x00, 0x00, 0x57};

  if (!toSleep) { // Wake up the sensor
    sleepCommand [3] = 0x00;  // Wake command
    sleepCommand [8] = 0x58;
  }

  // Send the PM data request command.
  sendData (sleepCommand, 9);

  uint8_t rxDataByte = 0;
  bool readCompleted = false;
  uint8_t byteCount = 0;

  CSE_MillisTimer timer (3000); // Create a timer to wait for the reply
  timer.start();  // Start the timer before starting to read the data

  while ((!readCompleted) && (!timer.isElapsed())) {  // Read the data
    if (_serial->available() > 0) {
      rxDataByte = _serial->read();

      if ((rxDataByte == 0xFF) && (byteCount == 0)) { // Read the Start Byte 1
        rxData [0] = rxDataByte;
        byteCount++;
      }

      else if ((rxDataByte == 0xA7) && (byteCount == 1)) { // Read the command byte
        rxData [1] = rxDataByte;
        byteCount++;
      }

      else if ((byteCount > 1) && (byteCount < 9)) {  // Read the remaining data bytes
        rxData [byteCount] = rxDataByte;
        byteCount++;
      }

      if (byteCount >= 9) {
        readCompleted = true;
      }
    }
  }

  // If the byte count at this point is not above 9,
  // we can assume that the operation was timed out.
  if (byteCount < 9) {
    // Serial.println ("Data not received. Timed out.");
    return false;
  }

  if ((rxData [0] == 0xFF) && (rxData [1] == 0xA7)) {
    if (rxData [2] == 0x01) {
      // if (toSleep) Serial.println ("Sensor is sleeping..");
      // else Serial.println ("Sensor is awake..");
      return true;  // Successful operation
    }
    else {
      return false; // Operation failed
    }
  }

  return false;
}

//======================================================================================//
/**
 * @brief Wakes up the sensor from the sleep mode.
 * 
 * @return true Operation successful.
 * @return false Operation failed.
 */
bool CSE_ZH06:: wake() {
  return sleep (false);
}

//======================================================================================//
