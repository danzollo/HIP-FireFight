# HIP-FireFight
Arduino sketch for the HIP Fire Fight robot course.

## Parts List
* (1) 128x64 OLED I2C display
* (1) Momentary Push Button
* (4) A3144 Hall Effect Sensor
* (4) DotStar 5050 LEDs (RGB or White) ([adafruit](https://www.adafruit.com/product/2343))
* (4) 5050 LED Breakout PCB ([adafruit](https://www.adafruit.com/product/1762))
* (10'+) CAT5e. 2 possible configurations:
   * 1 cable: Vcc, Gnd, SDA In, SCK In, Fire1, Fire2, Fire3, Fire4
   * 2 cable: Vcc, Gnd, SDA In, SCK In, SDA Out, SCK Out, Fire1, Fire2 (Chain 2 cables by tying S* Out to S* In) 

## Wire List
* A4 -> OLED SDA
* A5 -> OLED SCL
* 6 -> Fire 1 (Hall Effect Out 1)
* 7 -> Fire 2 (Hall Effect Out 2)
* 8 -> Fire 3 (Hall Effect Out 3)
* 9 -> Fire 4 (Hall Effect Out 4)
* 11 -> Fire Chain SData In
* 13 -> Fire Chain SCLK In
* 5V -> (OLED Vcc, Fire Chain Vcc)
* Gnd -> (OLED Gnd, Fire Chain Gnd)

## Use
1. Power on/Reset
1. After the splash screen, it will display “Press Start”.
1. Ready the robot at the starting position. 
1. Start the robot and press the start button at the same time.
1. The fires will light.  The robot needs to extinguish them by holding a magnet in close proximity to the fire for 1 second.
1. The program will keep time and score for the duration of the round.
1. When the round ends, the screen will display “GAME OVER” and the final score. The round ends when either:
   1. Time runs out.
   1. All of the fires are extinguished.
