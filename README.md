#This is an Arduino 2.4GHz radio and IR transmitter
## Features:
- IR transmitter for "LEGO" Powerfunctions and trains as well as for "MECCANO" motors
- 2.4GHz radio transmitter for RC cars, helicopters etc.
- 4 analog axes, 2 mode switches (joystick buttons)
- 4 push buttons for transmission mode and vehicle selection as well as menu navigation
- 5 seletcable vehicle addresses, so you can use it for more than one vehicle
- 3.3V, 8MHz Pro Mini (a Pro Micro is not supported anymore due to additional functionality)
- 2.4GHz NRF24L01 radio module
- Support for 0.96" I2C OLED
- Configuration menu for: Channel direction reversing, servo travel adjustment (independent for each vehicle)
- Configuration values are stored in EEPROM
- NRF24L01+PA+LNA SMA radio modules with power amplifier are supported from board version 1.1
- very compact
- Eagle PCB included

New in V 1.3:
- Infrared codes for "MECCANO" IR controlled motors. Separate library required: https://github.com/TheDIYGuy999/MeccanoIr
- Multichannel transmission

New in V 1.4:
- 10 vehicle addresses instead of 5
- The "BACK" button can now be used (if the menu screen is not displayed on the OLED) as a momentary button, for example for a horn on the vehicle. It is sent as "data.momentary1"
- The "A6" potentiometer connector is now integrated in the code and is sent to the vehicle as "data.pot1"
- SimpleTimer library replaced with my own code, which uses significantly less memory
- EEPROM reset function added: press "Select" and "Back" during power on

New in V 1.41:
- Transmitter battery voltage now monitored correctly on 16MHz / 5V systems

New in V 1.42:
- Vehicle battery state change OLED bug fixed

New in V 1.43:
- Radio module sleep mode active while IR mode is in use

New in V 1.5:
- MECCANO IR support enhanced. Requires MECCANO library update to V 1.1

New in V 1.6:
- Buttons locked, if not required in selected transmission mode. This ensures, that no variables can be changed "in background"
- Automatic joystick zero point calibration after transmitter was switched on. This ensures, that you always have precise zero points, even if your potentiometers are not exacltly linear.



## Usage

See pictures in this repo

(c) 2016 TheDIYGuy999
