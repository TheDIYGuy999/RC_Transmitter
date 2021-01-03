#ifndef transmitterConfig_h
#define transmitterConfig_h

#include "Arduino.h"

#define CONFIG_WLTOYS_2 // <- Select the correct transmitter configuration here before uploading!

//
// =======================================================================================================
// TRANSMITTEER SPECIFIC CONFIGURATIONS
// =======================================================================================================
//

// Configuration for the standard "Micro RC" transmitter with 4 channels and IR support----------------------
#ifdef CONFIG_MICRO_RC
// Battery type
const float cutoffVoltage = 4.4; // 4 x Eneloop cell
const float diodeDrop = 0.72;

// Channels, we have
#define CH1
#define CH2 
#define CH3
#define CH4

// Infrared
boolean infrared = true;

// Board type
const float boardVersion = 1.1; // Board revision (MUST MATCH WITH YOUR BOARD REVISION!!)

// Joystick calibration
int range[4] = {1023, 1023, 1023, 1023}; // 1023, if entire pot wiper range is used (CH1, 2, 3, 4)
int reverseEndpoint = 0; // the point, where the throttle joystick hits its reverse end stop

#endif

// Configuration for a 2 channel transmitter with steering wheel. No IR support----------------------
#ifdef CONFIG_2_CH
// Battery type
const float cutoffVoltage = 4.4; // 4 x Eneloop cell
const float diodeDrop = 0.3;

// Channels, we have
#define CH1
#define CH3

// Infrared
boolean infrared = false;

// Board type
const float boardVersion = 1.1; // Board revision (MUST MATCH WITH YOUR BOARD REVISION!!)

// Joystick calibration
int range[4] = {820, 820, 820, 820}; // 1023, if entire pot wiper range is used (CH1, 2, 3, 4)
int reverseEndpoint = 273; // the point, where the throttle joystick hits its reverse end stop

#endif

// Configuration for a 2+1 channel transmitter with steering wheel. No IR support----------------------
#ifdef CONFIG_3_CH
// Battery type
const float cutoffVoltage = 4.4; // 4 x Eneloop cell
const float diodeDrop = 0.3;

// Channels, we have
#define CH1
#define CH2 // switch with one resistor to gnd and one to vcc (for 3 speed gearbox) 
#define CH3

// 3 position switches, we have (= no auto calibtation for these channels during startup)
#define CH2Switch

// Infrared
boolean infrared = false;

// Board type
const float boardVersion = 1.1; // Board revision (MUST MATCH WITH YOUR BOARD REVISION!!)

// Joystick calibration
int range[4] = {820, 820, 820, 820}; // 1023, if entire pot wiper range is used (CH1, 2, 3, 4)
int reverseEndpoint = 273; // the point, where the throttle joystick hits its reverse end stop

#endif

// Configuration for a 2+1 channel transmitter with steering wheel. Based on a WLtoys transmitter. No IR support----------------------
#ifdef CONFIG_WLTOYS
// Battery type
const float cutoffVoltage = 4.4; // 4 x Eneloop cell
const float diodeDrop = 0.0; // No protection diode in this transmitter

// General settings
#define ledInversed // LED common is wired to VCC, so we have to inverse the logic!

// Channels, we have
#define CH1 // Steering
#define CH2 // switch with one resistor to gnd and one to vcc (for 3 speed gearbox) 
#define CH3 // Throttle

// 3 position switches, we have (= no auto calibtation for these channels during startup)
#define CH2Switch

// Infrared
boolean infrared = false;

// Board type
const float boardVersion = 1.1; // Board revision (MUST MATCH WITH YOUR BOARD REVISION!!)

// Joystick calibration
int range[4] = {900, 900, 900, 900}; // 1023, if entire pot wiper range is used (CH1, 2, 3, 4)
int reverseEndpoint = 170; // the point, where the throttle joystick hits its reverse end stop

#endif

// Configuration for a 2+2 channel transmitter with steering wheel. Based on a WLtoys transmitter. No IR support----------------------
#ifdef CONFIG_WLTOYS_2
// Battery type
const float cutoffVoltage = 4.4; // 4 x Eneloop cell
const float diodeDrop = 0.0; // No protection diode in this transmitter

// General settings
#define ledInversed // LED common is wired to VCC, so we have to inverse the logic!

// Channels, we have
#define CH1 // Steering
#define CH2 // switch with one resistor to gnd and one to vcc (for 3 speed gearbox) 
#define CH3 // Throttle
#define CH4 // 3 x push buttons: 1 pressed = 0V, 2 pressed = 0.76V, 3 pressed = 3.3V, no button pressed = 1.65V

// 3 position switches, we have (= no auto calibtation for these channels during startup)
#define CH2Switch
#define CH4Switch

// Infrared
boolean infrared = false;

// Board type
const float boardVersion = 1.1; // Board revision (MUST MATCH WITH YOUR BOARD REVISION!!)

// Joystick calibration
int range[4] = {890, 900, 900, 900}; // 1023, if entire pot wiper range is used (CH1, 2, 3, 4)
int reverseEndpoint = 170; // the point, where the throttle joystick hits its reverse end stop

#endif

// Configuration for a 2+2 channel transmitter with steering wheel. Based on a small WLtoys transmitter. No IR support----------------------
#ifdef CONFIG_WLTOYS_MINI
// Battery type
const float cutoffVoltage = 4.4; // 4 x Eneloop cell
const float diodeDrop = 0.0; // No protection diode in this transmitter

// General settings
//#define ledInversed // LED common is wired to VCC, so we have to inverse the logic!

// Channels, we have
#define CH1 // Steering
#define CH2 // switch with one resistor to gnd and one to vcc (for 3 speed gearbox) 
#define CH3 // Throttle
#define CH4 // 3 x push buttons: 1 pressed = 0V, 2 pressed = 0.76V, 3 pressed = 3.3V, no button pressed = 1.65V

// 3 position switches, we have (= no auto calibtation for these channels during startup)
#define CH2Switch
#define CH4Switch

// Infrared
boolean infrared = false;

// Board type
const float boardVersion = 1.1; // Board revision (MUST MATCH WITH YOUR BOARD REVISION!!)

// Joystick calibration
int range[4] = {900, 900, 800, 900}; // 1023, if entire pot wiper range is used (CH1, 2, 3, 4)
int reverseEndpoint = 170; // the point, where the throttle joystick hits its reverse end stop

#endif



#endif
