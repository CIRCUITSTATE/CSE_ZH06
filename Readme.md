
![CSE_ZH06](https://socialify.git.ci/CIRCUITSTATE/CSE_CircularBuffer/image?description=1&font=KoHo&forks=1&issues=1&logo=https%3A%2F%2Fwww.circuitstate.com%2Fwp-content%2Fuploads%2F2024%2F05%2FCIRCUITSTATE-R-Emblem-20052024-2.svg&name=1&pattern=Circuit%20Board&pulls=1&stargazers=1&theme=Auto)

# CSE_ZH06

**CSE_ZH06** is an Arduino library from [*CIRCUITSTATE Electronics*](https://www.circuitstate.com/). It helps you read **Particulate Matter (PM)** density values from the **Winsen ZH06** laser dust sensor module. The sensor supports reading **PM1.0**, **PM2.5** and **PM10** densities. The values are returned in **micrograms per cubic meter (ug/m3)**. The sensor uses UART for communication and you can use hardware serial or software serial ports with it.

## Installation

This library is available from the official **Arduino Library Manager**. Open the Arduino IDE, search for "CSE_ZH06" and install the latest version of the library.

Additionally, you can download the latest release package from the GitHub repository and install it manually. To do so, open the Arduino IDE, go to `Sketch > Include Library > Add .ZIP Library…` and select the downloaded file.

Another method is to clone the GitHub repository directly into your `libraries` folder. The development branch will have the latest features, bug fixes and other changes. To do so, navigate to your `libraries` folder (usually located at `Documents/Arduino/libraries` on Windows and `~/Documents/Arduino/libraries` on macOS) and execute the following command:

```
git clone https://github.com/CIRCUITSTATE/CSE_ZH06.git
```

The library can also be installed via **PlatformIO**. All officially listed Arduino listed libraries are automatically fetched by PlatformIO. Use the `lib_deps` search option to install the library.

## Dependencies

This library uses the [**CSE_MillisTimer**](https://github.com/CIRCUITSTATE/CSE_MillisTimer) library for timing requirements. Make sure to install it.

## Example

The following example shows how to read the PM values from the sensor and print them. You can find more examples in the [examples folder](/examples/).

```cpp
#include <Arduino.h>
#include <CSE_ZH06.h>

// Define the serial port pins here.
#define  PIN_SERIAL_TX1  16
#define  PIN_SERIAL_RX1  17

CSE_ZH06 pmSensor (Serial1);  // Create a CSE_ZH06 object to access the sensor

// Setup runs once.
void setup() {
  Serial.begin (115200);  // Initialize the debug serial port
  Serial1.begin (9600, SERIAL_8N1, PIN_SERIAL_RX1, PIN_SERIAL_TX1); // Initialize the sensor serial port

  pmSensor.begin(); // Initialize the sensor
}

// Inifinite loop.
void loop() {
  if (pmSensor.getPmData()) {
    Serial.println ("Data received. Printing data..");
    Serial.print ("PM1.0: ");
    Serial.print (pmSensor.pm1);
    Serial.println (" ug/m3");

    Serial.print ("PM2.5: ");
    Serial.print (pmSensor.pm25);
    Serial.println (" ug/m3");

    Serial.print ("PM10: ");
    Serial.print (pmSensor.pm10);
    Serial.println (" ug/m3");
    Serial.println();
  }
  
  else {
    Serial.println ("Data not received. Retrying..");
  }

  delay (1000);
}
```

## API Reference

Please see the [API.md](/docs/API.md) file for the API reference.

## References

- 
