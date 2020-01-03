# HIP-FireFight
Arduino sketch for the HIP Fire Fight robot course.

## Wire List
* A4 -> OLED SDA
* A5 -> OLED SCL
* 6 -> Fire 1
* 7 -> Fire 2
* 8 -> Fire 3
* 9 -> Fire 4
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
