

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
#define COLOR       0x7f7f7f
// Here's how to control the LEDs from any two pins:
#define DATAPIN     4
#define CLOCKPIN    5

int sensorPins[NUMFIRES] = {6,7,8,9,};
int startPin = 10;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_DotStar strip(NUMFIRES, DATAPIN, CLOCKPIN, DOTSTAR_BRG);
// Hardware SPI is a little faster, but must be wired to specific pins
// (Arduino Uno = pin 11 for data, 13 for clock, other boards are different).
//Adafruit_DotStar strip(NUMFIRES, DOTSTAR_BRG);

void setup() {
  // Initialize LEDs
  strip.begin();
  strip.show();

  //:TODO: Initialize OLED
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // Clear the buffer
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.setTextWrap(false);

  // Initialize Start Button
  pinMode(startPin, INPUT_PULLUP);
  // Initialize Hall Effect sensors
  for (int sensor = 0; sensor < NUMFIRES; sensor++)
  {
    pinMode(sensorPins[sensor], INPUT_PULLUP);
  }
}

uint32_t finalTime = 0;
uint32_t fireCount = 0;
uint32_t score = 0;
uint32_t extinguishTimes[NUMFIRES] = {0,0,0,0,};
uint32_t startTime = 0;
uint32_t lastUpdate = 0;
int32_t countdown;
uint8_t firePattern = 0;
uint8_t tmpPattern = 0;
  
void loop() {
  // Holding pattern between runs
  while (firePattern == 0)
  {
    while (digitalRead(startPin) == LOW);
    
    // All the fires are out
    PrintStatusScreen(NUMFIRES, TIMEMS_ROUND, -1);
    // Wait for re-start button.
    delay(50);
    if (digitalRead(startPin) == LOW)
    {
      score = 0;
      for (int fireIdx = 0; fireIdx < NUMFIRES; fireIdx++)
      {
        extinguishTimes[fireIdx] = 0;
      }
      finalTime = 0;
      startTime = 0;
      lastUpdate = 0;
      firePattern = (1<<NUMFIRES)-1;
      
      // Turn LEDs on/off
      SetLEDs(firePattern);
      // Set Start Time 
      startTime = millis();
    }
  }
  
  // Check Hall Effect Sensors
  if (CheckFires(&firePattern, extinguishTimes))
  {
    // Turn LEDs on/off
    SetLEDs(firePattern); 
    if (firePattern == 0)
    {
      finalTime = millis();
      if (finalTime > (startTime + TIMEMS_ROUND))
      {
        finalTime = startTime + TIMEMS_ROUND;
      }
    }
  }
   
  if ((millis() - lastUpdate) > TIMEMS_SCREEN_UPDATE)
  {
    // Calculate Score
    score = CalculateScore(millis(), startTime, extinguishTimes);
    fireCount = 0;
    tmpPattern = firePattern;
    while (tmpPattern > 0)
    {
      if (tmpPattern & 0x1)
      {
        fireCount++;
      }
      tmpPattern >>= 1;
    }
    countdown = TIMEMS_ROUND - (millis() - startTime);
    PrintStatusScreen(fireCount, countdown, score);
    lastUpdate = millis();
  }

  if ((firePattern == 0) || (countdown <= 0))
  {
    bool toggle = false;

    if (finalTime == 0)
    {
        finalTime = startTime + TIMEMS_ROUND;
    }
    
    // Update stats
    fireCount = 0;
    tmpPattern = firePattern;
    while (tmpPattern > 0)
    {
      if (tmpPattern & 0x1)
      {
        fireCount++;
      }
      tmpPattern >>= 1;
    }
    score = CalculateScore(finalTime, startTime, extinguishTimes);
    countdown = TIMEMS_ROUND - (finalTime - startTime);
    
    // Display final score
    while (digitalRead(startPin) != LOW)
    {
      if (toggle)
      {
        PrintStatusScreen(fireCount, countdown, score);
      }
      else
      {
        display.clearDisplay();
        display.setCursor(0,0);
        display.println(" ");
        display.println("   GAME   ");
        display.println("   OVER   ");
        display.print  ("    ");
        display.println(score);
        display.display();
      }
      delay(20);
      
      if ((millis() - lastUpdate) > 1500)
      {
          lastUpdate = millis();
          toggle = !toggle;
      }
    }
    firePattern = 0;
  }
}

// Calculate new score
uint32_t CalculateScore(uint32_t currentTime, uint32_t startTime, uint32_t extinguishTimes[])
{
  uint32_t score = 0;
  
  for (int fireIdx = 0; fireIdx < NUMFIRES; fireIdx++)
  {
    if (extinguishTimes[fireIdx] > 0)
    {
      score += extinguishTimes[fireIdx] - startTime;
    }
    else
    {
      score += currentTime - startTime;
    }
  }
  return score;
}

// Check the fires
bool CheckFires(uint8_t *firePattern, uint32_t extinguishTimes[])
{
  bool patternUpdated = false;
  // Check Hall Effect Sensors
  for (int fireIdx = 0; fireIdx < NUMFIRES; fireIdx++)
  {
    if (*firePattern & (1<<fireIdx))
    {
      if (digitalRead(sensorPins[fireIdx]) == 0)
      {
        if (extinguishTimes[fireIdx] == 0)
        {
          // Begin extinguishing the fire.
          extinguishTimes[fireIdx] = millis();
        }
        else if ((millis() - extinguishTimes[fireIdx]) >= TIMEMS_TO_EXTINGUISH)
        {
          // Fire is extinguished
            *firePattern &= ~(1<<fireIdx);
            patternUpdated = true;
        }
      }
      else
      {
        // Reset the fire.
        extinguishTimes[fireIdx] = 0;
      }
    }
  }
  return patternUpdated;
}

void PrintStatusScreen(int fires, int32_t timeMS, int32_t score) {
  char timeStr[5];
  int32_t min = 0;
  int32_t sec = (timeMS+500)/1000;

  while (sec >= 60) {
    min++;
    sec -= 60;
  }
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("FIRES:   ");
  display.println(fires);
  
  display.print("TIME: ");
  sprintf(timeStr, "%ld:%02ld", min, sec);
  display.println(timeStr);

  if (score < 0)
  {
    display.println("PRESS");
    display.println("     START");
  }
  else
  {
    display.println("SCORE:");
    display.print("    ");
    display.println(score);
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
      strip.setPixelColor(fireIdx, COLOR);
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
