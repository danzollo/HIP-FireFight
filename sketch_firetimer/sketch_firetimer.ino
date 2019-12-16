

#include <Adafruit_DotStar.h>
#include <SPI.h>

#define TIMEMS_SCREEN_UPDATE  250
#define TIMEMS_TO_EXTINGUISH  1000
// Number of LEDs in strip
#define NUMPIXELS   4
#define COLOR       0x7f7f7f
// Here's how to control the LEDs from any two pins:
#define DATAPIN     4
#define CLOCKPIN    5
Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);
// Hardware SPI is a little faster, but must be wired to specific pins
// (Arduino Uno = pin 11 for data, 13 for clock, other boards are different).
//Adafruit_DotStar strip(NUMPIXELS, DOTSTAR_BRG);

uint8_t firePattern = (1<<NUMPIXELS)-1;
int sensorPins[NUMPIXELS] = {6,7,8,9,};
int startPin = 10;

void setup() {
  // Initialize LEDs
  strip.begin(); 
  // Turn all LEDs off ASAP
  strip.show();

  //:TODO: Initialize OLED
  // Initialize Start Button
  pinMode(startPin, INPUT_PULLUP);
  // Initialize Hall Effect sensors
  for (int sensor = 0; sensor < NUMPIXELS; sensor++)
  {
    pinMode(senosrPins[sensor], INPUT_PULLUP);
  }
}

void loop() {
  uint32_t score = 0;
  uint32_t extinguishTimes[NUMPIXELS] = {0,0,0,0,};
  uint32_t startTime = 0;
  uint32_t lastUpdate = 0;
  uint8_t firePattern = 0;

  // Holding pattern between runs
  while (firePattern == 0)
  {
    // All the fires are out
    //:TODO: Update screens    
    // Wait for re-start button.
    delay(20);
    if (digitalRead(startPin) == 0)
    {
      score = 0;
      for (int sensor = 0; sensor < NUMPIXELS; sensor++)
      {
        extinguishTimes[sensor] = 0;
      }
      startTime = 0;
      lastUpdate = 0;
      firePattern = (1<<NUMPIXELS)-1;
      
      // Turn LEDs on/off
      SetLEDs(firePattern);
      // Set Start Time 
      startTime = millis();
    }
  }
 
  if ((millis() - lastUpdate) > TIMEMS_SCREEN_UPDATE)
  {
    // Calculate Score
    score = CalculateScore(startTime, extinguishTimes);   
    //:TODO: Update screens
  }
  // Check Hall Effect Sensors
  if (CheckFires(&firePattern, extinguishTimes))
  {
    // Turn LEDs on/off
    SetLEDs(firePattern); 
  }

  lastUpdate = millis();
}

// Calculate new score
uint32_t CalculateScore(uint32_t startTime, uint32_t extinguishTimes[])
{
  uint32_t score = 0;
  
  for (int pixel = 0; pixel < NUMPIXELS; pixels++)
  {
    if (extinguishTimes[sensor] > 0)
    {
      score += extinguishTimes[sensor] - startTime;
    }
    else
    {
      score += millis() - startTime;
    }
  }
}

// Check the fires
bool CheckFires(uint8_t &firePattern, uint32_t extinguishTimes[])
{
  bool patternUpdated = false;
  // Check Hall Effect Sensors
  for (int sensor = 0; sensor < NUMPIXELS; sensor++)
  {
    if ((extinguishTimes[sensor] == 0)
      && (digitalRead(senosrPins[sensor]) == 0))
    {
      // Begin extinguishing the fire.
      extinguishTimes[sensor] = millis();
    }
    else if ((millis() - extinguishTimes[sensor]) >= TIMEMS_TO_EXTINGUISH)
    {
      // Fire is extinguished
      firePattern &= ~(1<<sensor);
      patternUpdated = true;
    }
    else if (digitalRead(senosrPins[sensor]) != 0)
    {
      // Reset the fire.
      extinguishTimes[sensor] = 0;
    }
  }
  return patternUpdated;
}

// Turn on LEDs that haven't been extinguished
void SetLEDs(uint8_t firePattern)
{
  bool pixValue;
  for (int pixel = 0; pixel < NUMPIXELS; pixels++)
  {
    pixValue = (firePattern>>pixel) && 0x01;
    if (pixValue)
    {
      // Turn the pixel on
      strip.setPixelColor(pixel, COLOR);
    }
    else
    {
      // Turn the pixel off
      strip.setPixelColor(pixel, 0);
    }
    // Refresh strip
    strip.show();
  }
}
