** Arduino I2C Dongle **
A USB dongle firmware, turning an Arduino UNO into an I2C bridge.

Syntax is influenced by that of i2ctools for Linux embedded systems.


*** Building ***
Build with platformio, download here: http://www.platformio.org
Platformio also flashes the firmware. Just plug in your UNO and type "platformio" in your shell and everything should work.

***Usage***
Connect to the virtual com port of your UNO with a terminal program (like minicom on Linux). 115200 baud.

Typing "help" should display this: 

Usage: 
 Reading
 read <i2c device address> <register address> <length>

 Writing
 write <i2c device address> <register address> <data byte 1> <data byte 2> ...

 Toggle echo (default is "on")
 echo (toggles echo)

 Display this help
 help

 All numbers can be in dec, oct (prefix 0) or hex (prefix 0x) format.

 i2c device address is in 7 bit format

 Commands can be shorted to just the first letter.

 Example writing:
> w 0x50 0 10 1 2 5
 Writes 4 bytes to address 0 of an eeprom memory (0x50 is standard address for 24cxx memories)
 Output:
        OK

Example reading:
> r 0x50 0 10
 Reads 10 bytes from address 0 of that same eeprom.
Output:
   OK(hex) a,1,2,5,1,0,0,2,0,0,
