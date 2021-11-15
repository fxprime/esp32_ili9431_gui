# ESP23  + ILI9341 (MODULEMORE.com) wiring

## Wiring
 - MISO 19
 - MOSI 23
 - SCK 18
 - D/C 21
 - CS 22
 - RST 17
 - GND GND
 - VCC 3.3
 - LED 3.3


## TOUCH Wiring
 - MISO 19
 - MOSI 23
 - SCK 18
 - CS 16




## Setting pin to User_Setup.h

```cpp

#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   22  // Chip select control pin
#define TFT_DC   21  // Data Command control pin
#define TFT_RST  17  // Reset pin (could connect to RST pin)
//#define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST

// For ESP32 Dev board (only tested with GC9A01 display)
// The hardware SPI can be mapped to any pins

//#define TFT_MOSI 15 // In some display driver board, it might be written as "SDA" and so on.
//#define TFT_SCLK 14
//#define TFT_CS   5  // Chip select control pin
//#define TFT_DC   27  // Data Command control pin
//#define TFT_RST  33  // Reset pin (could connect to Arduino RESET pin)
//#define TFT_BL   22  // LED back-light

#define TOUCH_CS 16     // Chip select pin (T_CS) of touch screen
```


## Extensions/Touch.cpp

```cpp
//Change Line 141 to
  if (getTouchRawZ() <= threshold || getTouchRawZ() >4000) return false;

```

//This is for noisy touch output (always 4095 report)