
//======================================================================================//
/*
  Filename: CSE_ZH06.h
  Description: Main header file for the CSE_ZH06 Arduino library.
  Framework: Arduino, PlatformIO
  Author: Vishnu Mohanan (@vishnumaiea, @vizmohanan)
  Maintainer: CIRCUITSTATE Electronics (@circuitstate)
  Version: 0.0.1
  License: MIT
  Source: https://github.com/CIRCUITSTATE/CSE_ZH06
  Last Modified: +05:30 00:05:27 AM 24-12-2024, Tuesday
 */
//======================================================================================//

#include <Arduino.h>
#include <CSE_MillisTimer.h>

//======================================================================================//

// Include guards
#ifndef _CSE_ZH06_H_INCLUDED
#define _CSE_ZH06_H_INCLUDED

// Define default values
#define _HAVE_HWSERIAL1

// Check for specific architectures and boards
#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO) || defined(ESP8266)
  // Arduino Uno and Nano have only one hardware serial port
  #define _HAVE_HWSERIAL1
#elif defined(ARDUINO_AVR_MEGA2560)
  // Arduino Mega has four hardware serial ports
  #define _HAVE_HWSERIAL1
  #define _HAVE_HWSERIAL2
  #define _HAVE_HWSERIAL3
  #define _HAVE_HWSERIAL4
#elif defined(ARDUINO_ARCH_ESP32)
  // ESP32 typically has three hardware serial ports
  #define _HAVE_HWSERIAL1
  #define _HAVE_HWSERIAL2
  #define _HAVE_HWSERIAL3
#else
  // Else assume that the board has at least two hardware serial ports
  #define _HAVE_HWSERIAL1
  #define _HAVE_HWSERIAL2
#endif

// Define a flag to include SoftwareSerial if no additional UARTs are available
#if !defined(_HAVE_HWSERIAL2) && !defined(_HAVE_HWSERIAL3) && !defined(_HAVE_HWSERIAL4)
  #define SOFTWARE_SERIAL_REQUIRED 1
#endif

// The SoftwareSerial is loaded automatically when required.
#ifdef SOFTWARE_SERIAL_REQUIRED
  #include <SoftwareSerial.h>
#endif

//======================================================================================//

class CSE_ZH06 {
  public:
    uint8_t rxData [32] = {0};
    uint8_t txData [9] = {0};

    uint16_t pm1 = 0; // PM 1.0
    uint16_t pm25 = 0; // PM 2.5
    uint16_t pm10 = 0; // PM 10.0

    // Adapt the constructor to the type of serial port being used.
    #if SOFTWARE_SERIAL_REQUIRED
      CSE_ZH06 (SoftwareSerial& swSerial);  // Constructor with SoftwareSerial
    #else
      CSE_ZH06 (HardwareSerial& hwSerial);  // Constructor with HardwareSerial
    #endif

    ~CSE_ZH06();

    void begin(); // Does nothing for now.

    bool getPmData (uint16_t* data = NULL); // Get the PM data
    bool sleep (bool toSleep = true); // Puts the sensor to sleep
    bool wake();  // Wakes the sensor
  
  private:
    // Create the suitable type of serial port.
    #if SOFTWARE_SERIAL_REQUIRED
      SoftwareSerial* _serial;  // Software serial port
    #else
      HardwareSerial* _serial; // Hardware serial port
    #endif

    void sendData (uint8_t* txData, uint8_t txLength);
    void sendData (uint8_t* txData, uint8_t txLength, uint8_t* rxData, uint8_t rxLength);
};

//======================================================================================//

#endif
