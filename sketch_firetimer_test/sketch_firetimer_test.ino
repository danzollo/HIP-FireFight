

#include <Adafruit_DotStar.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <stdio.h>

#define TIMEMS_SCREEN_UPDATE  250
#define TIMEMS_TO_EXTINGUISH  1000
#define NUMFIRES              4
#define TIMEMS_ROUND          (3*60000)

// OLED defines
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

// DotStar LEDs
// Number of LEDs in strip
#define COLOR       0x0f0f0f //0x7f7f7f
// Here's how to control the LEDs from any two pins:
#define DATAPIN     4
#define CLOCKPIN    5

uint32_t fireColors[NUMFIRES] = {0x0f0000,0x000f00,0x00000f,0x0f0f00};
int sensorPins[NUMFIRES] = {6,7,8,9,};
int startPin = 10;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// Hardware SPI is a little faster, but must be wired to specific pins
// (Arduino Uno = pin 11 for data, 13 for clock, other boards are different).
//Adafruit_DotStar strip(NUMFIRES, DATAPIN, CLOCKPIN, DOTSTAR_RBG);
Adafruit_DotStar strip(NUMFIRES, DOTSTAR_RBG);

void setup() {
  // Initialize LEDs
  strip.begin();
  strip.show();

  // Initialize OLED
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // Clear the buffer
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.setTextWrap(false);
  PrintStatusScreen(0);

  // Initialize Start Button
  pinMode(startPin, INPUT_PULLUP);
  // Initialize Hall Effect sensors
  for (int sensor = 0; sensor < NUMFIRES; sensor++)
  {
    pinMode(sensorPins[sensor], INPUT_PULLUP);
  }
}

uint32_t extinguishTimes[NUMFIRES] = {0,0,0,0,};
uint8_t firePattern = 0;
  
void loop() {
  // Check Hall Effect Sensors
  if (CheckFires(&firePattern))
  {
    PrintStatusScreen(firePattern);
    // Turn LEDs on/off
    SetLEDs(firePattern); 
  }
   
  delay(10);
}

// Check the fires
bool CheckFires(uint8_t *firePattern)
{
  bool patternUpdated = false;
  // Check Hall Effect Sensors
  for (int fireIdx = 0; fireIdx < NUMFIRES; fireIdx++)
  {
    if ((*firePattern & (1<<fireIdx)) && (digitalRead(sensorPins[fireIdx]) == 0))
      {
          // Fire is extinguished
            *firePattern &= ~(1<<fireIdx);
            patternUpdated = true;
      }
      else if (!(*firePattern & (1<<fireIdx)) && (digitalRead(sensorPins[fireIdx]) > 0))
      {
          // Fire is extinguished
            *firePattern |= (1<<fireIdx);
            patternUpdated = true;
      }
  }
  return patternUpdated;
}

void PrintStatusScreen(int fires) {
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("FIRES:  ");
  display.println(fires);

  for (int fireIdx = 0; fireIdx < NUMFIRES; fireIdx++)
  {
    if (fires & (1<<(fireIdx)))
    {
      display.print(fireIdx);
    }
    else
    {
      display.print(' ');
    }
  }
  display.display();
}

// Turn on LEDs that haven't been extinguished
void SetLEDs(uint8_t firePattern)
{
  bool pixValue;
  for (int fireIdx = 0; fireIdx < NUMFIRES; fireIdx++)
  {
    pixValue = (firePattern>>fireIdx) & 0x01;
    if (pixValue)
    {
      // Turn the pixel on
      strip.setPixelColor(fireIdx, fireColors[fireIdx]);
    }
    else
    {
      // Turn the pixel off
      strip.setPixelColor(fireIdx, 0);
    }
    // Refresh strip
    strip.show();
  }
}
