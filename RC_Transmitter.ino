// Micro RC Project. A tiny little 2.4GHz and LEGO "Power Functions" / "MECCANO" IR RC transmitter!
// 3.3V, 8MHz Pro Mini
// STM32F103C8T6 ARM version see: https://github.com/TheDIYGuy999/RC_Transmitter_STM32
// 2.4GHz NRF24L01 radio module
// SSD 1306 128 x 63 0.96" OLED
// Custom PCB from OSH Park
// Menu for the following adjustments:
// -Channel reversing
// -Channel travel limitation adjustable in steps of 5%
// -Value changes are stored in EEPROM, individually per vehicle
// Radio transmitter tester included (press "Select" button during power up)
// NRF24L01+PA+LNA SMA radio modules with power amplifier are supported from board version 1.1
// ATARI PONG game :-) Press the "Back" button during power on to start it

const float codeVersion = 2.51; // Software revision

//
// =======================================================================================================
// BUILD OPTIONS (comment out unneeded options)
// =======================================================================================================
//

//#define DEBUG // if not commented out, Serial.print() is active! For debugging only!!
//#define OLED_DEBUG // if not commented out, an additional diagnostics screen is shown during startup

//
// =======================================================================================================
// INCLUDE LIRBARIES & TABS
// =======================================================================================================
//

// Libraries
#include <SPI.h>
#include <RF24.h> // Installed via Sketch > Include Library > Manage Libraries > Type "RF24" (use V1.3.3!)
#include <printf.h>
#include <EEPROMex.h> // https://github.com/thijse/Arduino-EEPROMEx
#include <LegoIr.h> // https://github.com/TheDIYGuy999/LegoIr
#include <statusLED.h> // https://github.com/TheDIYGuy999/statusLED
#include <U8glib.h> // Installed via Sketch > Include Library > Manage Libraries > Type "U8glib"

// Tabs
#include "transmitterConfig.h"
// More tabs (.h files in the sketch directory) see further down

//
// =======================================================================================================
// PIN ASSIGNMENTS & GLOBAL VARIABLES
// =======================================================================================================
//

// Is the radio or IR transmission mode active?
byte transmissionMode = 1; // Radio mode is active by default

// Select trannsmitter operation mode
byte operationMode = 0; // Start in transmitter mode (0 = transmitter mode, 1 = tester mode, 2 = game mode)

// Vehicle address
int vehicleNumber = 1; // Vehicle number one is active by default

// Radio channels (126 channels are supported)
byte chPointer = 0; // Channel 1 (the first entry of the array) is active by default
const byte NRFchannel[] {
  1, 2
};

// the ID number of the used "radio pipe" must match with the selected ID on the transmitter!
// 20 ID's are available @ the moment
const uint64_t pipeOut[] PROGMEM = {
  0xE9E8F0F0B1LL, 0xE9E8F0F0B2LL, 0xE9E8F0F0B3LL, 0xE9E8F0F0B4LL, 0xE9E8F0F0B5LL,
  0xE9E8F0F0B6LL, 0xE9E8F0F0B7LL, 0xE9E8F0F0B8LL, 0xE9E8F0F0B9LL, 0xE9E8F0F0B0LL,
  0xE9E8F0F0C1LL, 0xE9E8F0F0C2LL, 0xE9E8F0F0C3LL, 0xE9E8F0F0C4LL, 0xE9E8F0F0C5LL,
  0xE9E8F0F0C6LL, 0xE9E8F0F0C7LL, 0xE9E8F0F0C8LL, 0xE9E8F0F0C9LL, 0xE9E8F0F0C0LL
};
const int maxVehicleNumber = (sizeof(pipeOut) / (sizeof(uint64_t)));

// Hardware configuration: Set up nRF24L01 radio on hardware SPI bus & pins 7 (CE) & 8 (CSN)
RF24 radio(7, 8);

// The size of this struct should not exceed 32 bytes
struct RcData {
  byte axis1; // Aileron (Steering for car)
  byte axis2; // Elevator
  byte axis3; // Throttle
  byte axis4; // Rudder
  boolean mode1 = false; // Mode1 (toggle speed limitation)
  boolean mode2 = false; // Mode2 (toggle acc. / dec. limitation)
  boolean momentary1 = false; // Momentary push button
  byte pot1; // Potentiometer
};
RcData data;

// This struct defines data, which are embedded inside the ACK payload
struct ackPayload {
  float vcc; // vehicle vcc voltage
  float batteryVoltage; // vehicle battery voltage
  boolean batteryOk; // the vehicle battery voltage is OK!
  byte channel = 1; // the channel number
};
ackPayload payload;

// Did the receiver acknowledge the sent data?
boolean transmissionState;

// LEGO powerfunctions IR
LegoIr pf;
int pfChannel;
const int pfMaxAddress = 3;

// TX voltages
boolean batteryOkTx = false;
#define BATTERY_DETECT_PIN A7 // The 20k & 10k battery detection voltage divider is connected to pin A7
float txVcc;
float txBatt;

//Joystick reversing
boolean joystickReversed[maxVehicleNumber + 1][4] = { // 1 + 10 Vehicle Addresses, 4 Servos
  {false, false, false, false}, // Address 0 used for EEPROM initialisation

  {false, false, false, false}, // Address 1
  {false, false, false, false}, // Address 2
  {false, false, false, false}, // Address 3
  {false, false, false, false}, // Address 4
  {false, false, false, false}, // Address 5
  {false, false, false, false}, // Address 6
  {false, false, false, false}, // Address 7
  {false, false, false, false}, // Address 8
  {false, false, false, false}, // Address 9
  {false, false, false, false}, // Address 10
};

//Joystick percent negative
byte joystickPercentNegative[maxVehicleNumber + 1][4] = { // 1 + 10 Vehicle Addresses, 4 Servos
  {100, 100, 100, 100}, // Address 0 not used

  {100, 100, 100, 100}, // Address 1
  {100, 100, 100, 100}, // Address 2
  {100, 100, 100, 100}, // Address 3
  {100, 100, 100, 100}, // Address 4
  {100, 100, 100, 100}, // Address 5
  {100, 100, 100, 100}, // Address 6
  {100, 100, 100, 100}, // Address 7
  {100, 100, 100, 100}, // Address 8
  {100, 100, 100, 100}, // Address 9
  {100, 100, 100, 100}, // Address 10
};

//Joystick percent positive
byte joystickPercentPositive[maxVehicleNumber + 1][4] = { // 1 + 10 Vehicle Addresses, 4 Channels
  {100, 100, 100, 100}, // Address 0 not used

  {100, 100, 100, 100}, // Address 1
  {100, 100, 100, 100}, // Address 2
  {100, 100, 100, 100}, // Address 3
  {100, 100, 100, 100}, // Address 4
  {100, 100, 100, 100}, // Address 5
  {100, 100, 100, 100}, // Address 6
  {100, 100, 100, 100}, // Address 7
  {100, 100, 100, 100}, // Address 8
  {100, 100, 100, 100}, // Address 9
  {100, 100, 100, 100}, // Address 10
};

// Joysticks
#define JOYSTICK_1 A1
#define JOYSTICK_2 A0
#define JOYSTICK_3 A3
#define JOYSTICK_4 A2

// Joystick push buttons
#define JOYSTICK_BUTTON_LEFT 4
#define JOYSTICK_BUTTON_RIGHT 2

byte leftJoystickButtonState;
byte rightJoystickButtonState;

// Buttons
#define BUTTON_LEFT 1 // - or channel select
#define BUTTON_RIGHT 10 // + or transmission mode select
#define BUTTON_SEL 0 // select button for menu
#define BUTTON_BACK 9 // back button for menu

byte leftButtonState = 7; // init states with 7 (see macro below)!
byte rightButtonState = 7;
byte selButtonState = 7;
byte backButtonState = 7;

// macro for detection of rising edge and debouncing
/*the state argument (which must be a variable) records the current and the last 3 reads
  by shifting one bit to the left at each read and bitwise anding with 15 (=0b1111).
  If the value is 7(=0b0111) we have one raising edge followed by 3 consecutive 1's.
  That would qualify as a debounced raising edge*/
#define DRE(signal, state) (state=(state<<1)|(signal&1)&15)==7

// Status LED objects (false = logic not inverted)
#ifdef ledInversed // inversed logic
statusLED greenLED(true); // green: ON = transmitter ON, flashing = Communication with vehicle OK
statusLED redLED(true); // red: ON = battery empty
#endif
#ifndef ledInversed // not inversed logic
statusLED greenLED(false); // green: ON = transmitter ON, flashing = Communication with vehicle OK
statusLED redLED(false); // red: ON = battery empty
#endif

// OLED display. Select the one you have! Otherwise sthe sreen could be slightly offset sideways!
//U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_FAST);  // I2C / TWI  FAST instead of NONE = 400kHz I2C!
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_FAST);  // I2C / TWI  FAST instead of NONE = 400kHz I2C!
int activeScreen = 0; // the currently displayed screen number (0 = splash screen)
boolean displayLocked = true;
byte menuRow = 0; // Menu active cursor line

// EEPROM (max. total size is 512 bytes)
// Always get the adresses first and in the same order
// Blocks of 21 x 4 bytes = 84 bytes each!
int addressReverse = EEPROM.getAddress(sizeof(byte) * 84);
int addressNegative = EEPROM.getAddress(sizeof(byte) * 84);
int addressPositive = EEPROM.getAddress(sizeof(byte) * 84);

//
// =======================================================================================================
// INCLUDE TABS (header files in sketch directory)
// =======================================================================================================
//

// Tabs (header files in sketch directory)
#include "readVCC.h"
//#include "transmitterConfig.h"
#include "MeccanoIr.h" // https://github.com/TheDIYGuy999/MeccanoIr
#include "pong.h" // A little pong game :-)
#include "pgmRead64.h" // Read 64 bit blocks from PROGMEM

//
// =======================================================================================================
// RADIO SETUP
// =======================================================================================================
//

void setupRadio() {
  radio.begin();
  radio.setChannel(NRFchannel[chPointer]);

  radio.powerUp();

  // Set Power Amplifier (PA) level to one of four levels: RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH and RF24_PA_MAX
  if (boardVersion < 1.1 ) radio.setPALevel(RF24_PA_MIN); // No independent NRF24L01 3.3V PSU, so only "MIN" transmission level allowed
  else radio.setPALevel(RF24_PA_MAX); // Independent NRF24L01 3.3V PSU, so "FULL" transmission level allowed

  radio.setDataRate(RF24_250KBPS);
  //radio.setAutoAck(pipeOut[vehicleNumber - 1], true); // Ensure autoACK is enabled
  radio.setAutoAck(pgm_read_64(&pipeOut, vehicleNumber - 1), true); // Ensure autoACK is enabled
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.setRetries(5, 5);                  // 5x250us delay (blocking!!), max. 5 retries
  //radio.setCRCLength(RF24_CRC_8);          // Use 8-bit CRC for performance*/

#ifdef DEBUG
  radio.printDetails();
  delay(1800);
#endif

  // All axes to neutral position
  data.axis1 = 50;
  data.axis2 = 50;
  data.axis3 = 50;
  data.axis4 = 50;

  // Transmitter
  if (operationMode == 0) {
    //radio.openWritingPipe(pipeOut[vehicleNumber - 1]); // Vehicle Number 1 = Array number 0, so -1!
    radio.openWritingPipe(pgm_read_64(&pipeOut, vehicleNumber - 1)); // Vehicle Number 1 = Array number 0, so -1!
    radio.write(&data, sizeof(RcData));
  }

  // Receiver (radio tester mode)
  if (operationMode == 1) {
    //radio.openReadingPipe(1, pipeOut[vehicleNumber - 1]);
    radio.openReadingPipe(1, pgm_read_64(&pipeOut, vehicleNumber - 1));
    radio.startListening();
  }
}

//
// =======================================================================================================
// LEGO POWERFUNCTIONS SETUP
// =======================================================================================================
//

void setupPowerfunctions() {
  pfChannel = vehicleNumber - 1;  // channel 0 - 3 is labelled as 1 - 4 on the LEGO devices!

  if (pfChannel > pfMaxAddress) pfChannel = pfMaxAddress;

  pf.begin(3, pfChannel);  // Pin 3, channel 0 - 3
}

//
// =======================================================================================================
// MAIN ARDUINO SETUP (1x during startup)
// =======================================================================================================
//

void setup() {

#ifdef DEBUG
  Serial.begin(115200);
  printf_begin();
  delay(3000);
#endif

  // LED setup
  greenLED.begin(6); // Green LED on pin 5
  redLED.begin(5); // Red LED on pin 6

  // Pinmodes (all other pinmodes are handled inside libraries)
  pinMode(JOYSTICK_BUTTON_LEFT, INPUT_PULLUP);
  pinMode(JOYSTICK_BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_SEL, INPUT_PULLUP);
  pinMode(BUTTON_BACK, INPUT_PULLUP);

  // EEPROM setup
  EEPROM.readBlock(addressReverse, joystickReversed); // restore all arrays from the EEPROM
  EEPROM.readBlock(addressNegative, joystickPercentNegative);
  EEPROM.readBlock(addressPositive, joystickPercentPositive);

  if (joystickReversed[0][0] || (!digitalRead(BUTTON_BACK) && !digitalRead(BUTTON_SEL))) { // 255 is EEPROM default after a program download,
    // this indicates that we have to initialise the EEPROM with our default values! Or manually triggered with
    // both "SEL" & "BACK" buttons pressed during switching on!
    memset(joystickReversed, 0, sizeof(joystickReversed)); // init arrays first
    memset(joystickPercentNegative, 100, sizeof(joystickPercentNegative));
    memset(joystickPercentPositive, 100, sizeof(joystickPercentPositive));
    EEPROM.updateBlock(addressReverse, joystickReversed); // then write defaults to EEPROM
    EEPROM.updateBlock(addressNegative, joystickPercentNegative);
    EEPROM.updateBlock(addressPositive, joystickPercentPositive);
  }

  // Switch to radio tester mode, if "Select" button is pressed
  if (digitalRead(BUTTON_BACK) && !digitalRead(BUTTON_SEL)) {
    operationMode = 1;
  }

  // Switch to game mode, if "Back" button is pressed
  if (!digitalRead(BUTTON_BACK) && digitalRead(BUTTON_SEL)) {
    operationMode = 2;
  }

  // Joystick setup
  JoystickOffset(); // Compute all joystick center points
  readJoysticks(); // Then do the first jocstick read

  // Radio setup
  setupRadio();

  // LEGO Powerfunctions setup
  setupPowerfunctions();

  // Display setup
  u8g.setFontRefHeightExtendedText();
  u8g.setDefaultForegroundColor();
  u8g.setFontPosTop();
  u8g.setFont(u8g_font_6x10);

  // Splash screen
  checkBattery();
  activeScreen = 0; // 0 = splash screen active
  drawDisplay();
#ifdef OLED_DEBUG
  activeScreen = 100; // switch to the diagnostics screen
  delay(1500);
  drawDisplay();
#endif
  activeScreen = 1; // switch to the main screen
  delay(1500);
}

//
// =======================================================================================================
// BUTTONS
// =======================================================================================================
//

// Sub function for channel travel adjustment and limitation --------------------------------------
void travelAdjust(boolean upDn) {
  byte inc = 5;
  if (upDn) inc = 5; // Direction +
  else inc = -5; // -

  if ( (menuRow & 0x01) == 0) { // even (2nd column)
    joystickPercentPositive[vehicleNumber][(menuRow - 6) / 2 ] += inc; // row 6 - 12 = 0 - 3
  }
  else { // odd (1st column)
    joystickPercentNegative[vehicleNumber][(menuRow - 5) / 2 ] += inc;  // row 5 - 11 = 0 - 3
  }
  joystickPercentPositive[vehicleNumber][(menuRow - 6) / 2 ] = constrain(joystickPercentPositive[vehicleNumber][(menuRow - 6) / 2 ], 20, 100);
  joystickPercentNegative[vehicleNumber][(menuRow - 5) / 2 ] = constrain(joystickPercentNegative[vehicleNumber][(menuRow - 5) / 2 ], 20, 100);
}

// Main buttons function --------------------------------------------------------------------------
void readButtons() {

  // Every 10 ms
  static unsigned long lastTrigger;
  if (millis() - lastTrigger >= 10) {
    lastTrigger = millis();

    // Left joystick button (Mode 1)
    if (DRE(digitalRead(JOYSTICK_BUTTON_LEFT), leftJoystickButtonState) && (transmissionMode == 1)) {
      data.mode1 = !data.mode1;
      drawDisplay();
    }

    // Right joystick button (Mode 2)
    if (DRE(digitalRead(JOYSTICK_BUTTON_RIGHT), rightJoystickButtonState) && (transmissionMode == 1)) {
      data.mode2 = !data.mode2;
      drawDisplay();
    }

    if (activeScreen <= 10) { // if menu is not displayed ----------

      // Left button: Channel selection +
      if (DRE(digitalRead(BUTTON_LEFT), leftButtonState) && (transmissionMode < 3)) {
        vehicleNumber ++;
        if (vehicleNumber > maxVehicleNumber) vehicleNumber = 1;
        setupRadio(); // Re-initialize the radio with the new pipe address
        setupPowerfunctions(); // Re-initialize the LEGO IR transmitter with the new channel address
        drawDisplay();
      }

      // Right button: Change transmission mode. Radio <> IR
      if (infrared) { // only, if transmitter has IR option
        if (DRE(digitalRead(BUTTON_RIGHT), rightButtonState)) {
          if (transmissionMode < 3) transmissionMode ++;
          else {
            transmissionMode = 1;
            setupRadio(); // Re-initialize radio, if we switch back to radio mode!
          }
          drawDisplay();
        }
      }
      else { // only, if transmitter has no IR option
        // Right button: Channel selection -
        if (DRE(digitalRead(BUTTON_RIGHT), rightButtonState) && (transmissionMode < 3)) {
          vehicleNumber --;
          if (vehicleNumber < 1) vehicleNumber = maxVehicleNumber;
          setupRadio(); // Re-initialize the radio with the new pipe address
          setupPowerfunctions(); // Re-initialize the LEGO IR transmitter with the new channel address
          drawDisplay();
        }
      }
    }
    else { // if menu is displayed -----------
      // Right button: Value -
      if (DRE(digitalRead(BUTTON_RIGHT), rightButtonState)) {
        if (activeScreen == 11) {
          joystickReversed[vehicleNumber][menuRow - 1] = false;
        }
        if (activeScreen == 12) {
          travelAdjust(false); // -
        }
        drawDisplay();
      }

      // Left button: Value +
      if (DRE(digitalRead(BUTTON_LEFT), leftButtonState)) {
        if (activeScreen == 11) {
          joystickReversed[vehicleNumber][menuRow - 1] = true;
        }
        if (activeScreen == 12) {
          travelAdjust(true); // +
        }
        drawDisplay();
      }
    }

    // Menu buttons:

    // Select button: opens the menu and scrolls through menu entries
    if (DRE(digitalRead(BUTTON_SEL), selButtonState) && (transmissionMode == 1)) {
      activeScreen = 11; // 11 = Menu screen 1
      menuRow ++;
      if (menuRow > 4) activeScreen = 12; // 12 = Menu screen 2
      if (menuRow > 12) {
        activeScreen = 11; // Back to menu 1, entry 1
        menuRow = 1;
      }
      drawDisplay();
    }

    // Back / Momentary button:
    if (activeScreen <= 10) { // Momentary button, if menu is NOT displayed
      if (!digitalRead(BUTTON_BACK)) data.momentary1 = true;
      else data.momentary1 = false;
    }
    else { // Goes back to the main screen & saves the changed entries in the EEPROM
      if (DRE(digitalRead(BUTTON_BACK), backButtonState)) {
        activeScreen = 1; // 1 = Main screen
        menuRow = 0;
        drawDisplay();
        EEPROM.updateBlock(addressReverse, joystickReversed); // update changed values in EEPROM
        EEPROM.updateBlock(addressNegative, joystickPercentNegative);
        EEPROM.updateBlock(addressPositive, joystickPercentPositive);
      }
    }
  }
}

//
// =======================================================================================================
// JOYSTICKS
// =======================================================================================================
//

int offset[4]; // the auto calibration offset of each joystick

// Auto-zero subfunction (called during setup, if a pot and no 3 position switch is connected) ----
void JoystickOffset() {
#ifndef CH1Switch
  offset[0] = 512 - analogRead(JOYSTICK_1);
#endif

#ifndef CH2Switch
  offset[1] = 512 - analogRead(JOYSTICK_2);
#endif

#ifndef CH3Switch
  offset[2] = 512 - analogRead(JOYSTICK_3);
#endif

#ifndef CH4Switch
  offset[3] = 512 - analogRead(JOYSTICK_4);
#endif
}

// Mapping and reversing subfunction ----
byte mapJoystick(byte input, byte arrayNo) {
  int reading[4];
  reading[arrayNo] = analogRead(input) + offset[arrayNo]; // read joysticks and add the offset
  reading[arrayNo] = constrain(reading[arrayNo], (1023 - range[arrayNo]), range[arrayNo]); // then limit the result before we do more calculations below

#ifndef CONFIG_4_CH // In most "car style" transmitters, less than one half of the throttle potentiometer range is used for the reverse. So we have to enhance this range!
  if (reading[2] < (range[2] / 2) ) {
    reading[2] = constrain(reading[2], reverseEndpoint, (range[2] / 2)); // limit reverse range, which will be mapped later on
    reading[2] = map(reading[2], reverseEndpoint, (range[2] / 2), 0, (range[2] / 2)); // reverse range mapping (adjust reverse endpoint in transmitterConfig.h)
  }
#endif

  if (transmissionMode == 1 && operationMode != 2 ) { // Radio mode and not game mode
    if (joystickReversed[vehicleNumber][arrayNo]) { // reversed
      return map(reading[arrayNo], (1023 - range[arrayNo]), range[arrayNo], (joystickPercentPositive[vehicleNumber][arrayNo] / 2 + 50), (50 - joystickPercentNegative[vehicleNumber][arrayNo] / 2));
    }
    else { // not reversed
      return map(reading[arrayNo], (1023 - range[arrayNo]), range[arrayNo], (50 - joystickPercentNegative[vehicleNumber][arrayNo] / 2), (joystickPercentPositive[vehicleNumber][arrayNo] / 2 + 50));
    }
  }
  else { // IR mode
    return map(reading[arrayNo], (1023 - range[arrayNo]), range[arrayNo], 0, 100);
  }
}

// Main Joystick function ----
void readJoysticks() {

  // save previous joystick positions
  byte previousAxis1 = data.axis1;
  byte previousAxis2 = data.axis2;
  byte previousAxis3 = data.axis3;
  byte previousAxis4 = data.axis4;

  // Read current joystick positions, then scale and reverse output signals, if necessary (only for the channels we have)
#ifdef CH1
  data.axis1 = mapJoystick(JOYSTICK_1, 0); // Aileron (Steering for car)
#endif

#ifdef CH2
  data.axis2 = mapJoystick(JOYSTICK_2, 1); // Elevator
#endif

#ifdef CH3
  data.axis3 = mapJoystick(JOYSTICK_3, 2); // Throttle
#endif

#ifdef CH4
  data.axis4 = mapJoystick(JOYSTICK_4, 3); // Rudder
#endif

  // in case of an overflow, set axis to zero (prevent it from overflowing < 0)
  if (data.axis1 > 150) data.axis1 = 0;
  if (data.axis2 > 150) data.axis2 = 0;
  if (data.axis3 > 150) data.axis3 = 0;
  if (data.axis4 > 150) data.axis4 = 0;

  // Only allow display refresh, if no value has changed!
  if (previousAxis1 != data.axis1 ||
      previousAxis2 != data.axis2 ||
      previousAxis3 != data.axis3 ||
      previousAxis4 != data.axis4) {
    displayLocked = true;
  }
  else {
    displayLocked = false;
  }
}

//
// =======================================================================================================
// POTENTIOMETER
// =======================================================================================================
//

void readPotentiometer() {
  data.pot1 = map(analogRead(A6), 0, 1023, 0, 100);
  data.pot1 = constrain(data.pot1, 0, 100);
}

//
// =======================================================================================================
// TRANSMIT LEGO POWERFUNCTIONS IR SIGNAL
// =======================================================================================================
//

void transmitLegoIr() {
  static byte speedOld[2];
  static byte speed[2];
  static byte pwm[2];
  static unsigned long previousMillis;

  unsigned long currentMillis = millis();

  // Flash green LED
  greenLED.flash(30, 2000, 0, 0);

  // store joystick positions into an array-----
  speed[0] = data.axis3;
  speed[1] = data.axis2;

  // compute pwm value for "red" and "blue" motor, if speed has changed more than +/- 3, or every 0.6s
  // NOTE: one IR pulse at least every 1.2 s is required in order to prevent the vehivle from stopping
  // due to a signal timeout!
  for (int i = 0; i <= 1; i++) {
    if ((speedOld[i] - 3) > speed[i] || (speedOld[i] + 3) < speed[i] || currentMillis - previousMillis >= 600) {
      speedOld[i] = speed[i];
      previousMillis = currentMillis;
      if (speed[i] >= 0 && speed[i] < 6) pwm[i] = PWM_REV7;
      else if (speed[i] >= 6 && speed[i] < 12) pwm[i] = PWM_REV6;
      else if (speed[i] >= 12 && speed[i] < 18) pwm[i] = PWM_REV5;
      else if (speed[i] >= 18 && speed[i] < 24) pwm[i] = PWM_REV4;
      else if (speed[i] >= 24 && speed[i] < 30) pwm[i] = PWM_REV3;
      else if (speed[i] >= 30 && speed[i] < 36) pwm[i] = PWM_REV2;
      else if (speed[i] >= 36 && speed[i] < 42) pwm[i] = PWM_REV1;
      else if (speed[i] >= 42 && speed[i] < 58) pwm[i] = PWM_BRK;
      else if (speed[i] >= 58 && speed[i] < 64) pwm[i] = PWM_FWD1;
      else if (speed[i] >= 64 && speed[i] < 70) pwm[i] = PWM_FWD2;
      else if (speed[i] >= 70 && speed[i] < 76) pwm[i] = PWM_FWD3;
      else if (speed[i] >= 76 && speed[i] < 82) pwm[i] = PWM_FWD4;
      else if (speed[i] >= 82 && speed[i] < 88) pwm[i] = PWM_FWD5;
      else if (speed[i] >= 88 && speed[i] < 94) pwm[i] = PWM_FWD6;
      else if (speed[i] >= 94) pwm[i] = PWM_FWD7;

      // then transmit IR data
      pf.combo_pwm(pwm[1], pwm[0]); // red and blue in one IR package
    }
  }
}

//
// =======================================================================================================
// TRANSMIT MECCANO / ERECTOR IR SIGNAL
// =======================================================================================================
//

void transmitMeccanoIr() {

  static boolean A;
  static boolean B;
  static boolean C;
  static boolean D;

  // Flash green LED
  greenLED.flash(30, 1000, 0, 0);

  // Channel A ----
  if (data.axis1 > 90) { // A +
    buildIrSignal(1);
    A = true;
  }
  if (data.axis1 < 10) { // A -
    buildIrSignal(2), A = true;
    A = true;
  }
  if (data.axis1 < 90 && data.axis1 > 10 && A) { // A OFF
    buildIrSignal(3);
    A = false;
  }

  // Channel B ----
  if (data.axis2 > 90) { // B +
    buildIrSignal(4);
    B = true;
  }
  if (data.axis2 < 10) { // B -
    buildIrSignal(5), A = true;
    B = true;
  }
  if (data.axis2 < 90 && data.axis2 > 10 && B) { // B OFF
    buildIrSignal(6);
    B = false;
  }

  // Channel C ----
  if (data.axis3 > 90) { // C +
    buildIrSignal(7);
    C = true;
  }
  if (data.axis3 < 10) { // C -
    buildIrSignal(8), A = true;
    C = true;
  }
  if (data.axis3 < 90 && data.axis3 > 10 && C) { // C OFF
    buildIrSignal(9);
    C = false;
  }

  // Channel D ----
  if (data.axis4 > 90) { // D +
    buildIrSignal(10);
    D = true;
  }
  if (data.axis4 < 10) { // D -
    buildIrSignal(11), A = true;
    D = true;
  }
  if (data.axis4 < 90 && data.axis4 > 10 && D) { // D OFF
    buildIrSignal(12);
    D = false;
  }
}

//
// =======================================================================================================
// TRANSMIT RADIO DATA
// =======================================================================================================
//

void transmitRadio() {

  static boolean previousTransmissionState;
  static float previousRxVcc;
  static float previousRxVbatt;
  static boolean previousBattState;
  static unsigned long previousSuccessfulTransmission;

  if (transmissionMode == 1) { // If radio mode is active: ----

    // Send radio data and check if transmission was successful
    if (radio.write(&data, sizeof(struct RcData)) ) {
      if (radio.isAckPayloadAvailable()) {
        radio.read(&payload, sizeof(struct ackPayload)); // read the payload, if available
        previousSuccessfulTransmission = millis();
      }
    }

    // Switch channel for next transmission
    chPointer ++;
    if (chPointer >= sizeof((*NRFchannel) / sizeof(byte))) chPointer = 0;
    radio.setChannel(NRFchannel[chPointer]);

    // if the transmission was not confirmed (from the receiver) after > 1s...
    if (millis() - previousSuccessfulTransmission > 1000) {
      greenLED.on();
      transmissionState = false;
      memset(&payload, 0, sizeof(payload)); // clear the payload array, if transmission error
#ifdef DEBUG
      Serial.println("Data transmission error, check receiver!");
#endif
    }
    else {
      greenLED.flash(30, 100, 0, 0); //30, 100
      transmissionState = true;
#ifdef DEBUG
      Serial.println("Data successfully transmitted");
#endif
    }

    if (!displayLocked) { // Only allow display refresh, if not locked ----
      // refresh transmission state on the display, if changed
      if (transmissionState != previousTransmissionState) {
        previousTransmissionState = transmissionState;
        drawDisplay();
      }

      // refresh Rx Vcc on the display, if changed more than +/- 0.05V
      if (payload.vcc - 0.05 >= previousRxVcc || payload.vcc + 0.05 <= previousRxVcc) {
        previousRxVcc = payload.vcc;
        drawDisplay();
      }

      // refresh Rx V Batt on the display, if changed more than +/- 0.3V
      if (payload.batteryVoltage - 0.3 >= previousRxVbatt || payload.batteryVoltage + 0.3 <= previousRxVbatt) {
        previousRxVbatt = payload.batteryVoltage;
        drawDisplay();
      }

      // refresh battery state on the display, if changed
      if (payload.batteryOk != previousBattState) {
        previousBattState = payload.batteryOk;
        drawDisplay();
      }
    }

#ifdef DEBUG
    Serial.print(data.axis1);
    Serial.print("\t");
    Serial.print(data.axis2);
    Serial.print("\t");
    Serial.print(data.axis3);
    Serial.print("\t");
    Serial.print(data.axis4);
    Serial.print("\t");
    Serial.println(F_CPU / 1000000, DEC);
#endif
  }
  else { // else infrared mode is active: ----
    radio.powerDown();
  }
}

//
// =======================================================================================================
// READ RADIO DATA (for radio tester mode)
// =======================================================================================================
//

void readRadio() {

  static unsigned long lastRecvTime = 0;
  byte pipeNo;

  payload.batteryVoltage = txBatt; // store the battery voltage for sending
  payload.vcc = txVcc; // store the vcc voltage for sending
  payload.batteryOk = batteryOkTx; // store the battery state for sending

  if (radio.available(&pipeNo)) {
    radio.writeAckPayload(pipeNo, &payload, sizeof(struct ackPayload) );  // prepare the ACK payload
    radio.read(&data, sizeof(struct RcData)); // read the radia data and send out the ACK payload
    lastRecvTime = millis();
#ifdef DEBUG
    Serial.print(data.axis1);
    Serial.print("\t");
    Serial.print(data.axis2);
    Serial.print("\t");
    Serial.print(data.axis3);
    Serial.print("\t");
    Serial.print(data.axis4);
    Serial.println("\t");
#endif
  }

  // Switch channel
  if (millis() - lastRecvTime > 500) {
    chPointer ++;
    if (chPointer >= sizeof((*NRFchannel) / sizeof(byte))) chPointer = 0;
    radio.setChannel(NRFchannel[chPointer]);
    payload.channel = NRFchannel[chPointer];
  }

  if (millis() - lastRecvTime > 1000) { // set all analog values to their middle position, if no RC signal is received during 1s!
    data.axis1 = 50; // Aileron (Steering for car)
    data.axis2 = 50; // Elevator
    data.axis3 = 50; // Throttle
    data.axis4 = 50; // Rudder
    payload.batteryOk = true; // Clear low battery alert (allows to re-enable the vehicle, if you switch off the transmitter)
#ifdef DEBUG
    Serial.println("No Radio Available - Check Transmitter!");
#endif
  }

  if (millis() - lastRecvTime > 2000) {
    setupRadio(); // re-initialize radio
    lastRecvTime = millis();
  }
}

//
// =======================================================================================================
// LED
// =======================================================================================================
//

void led() {

  // Red LED (ON = battery empty, number of pulses are indicating the vehicle number)
  if (batteryOkTx && (payload.batteryOk || transmissionMode > 1 || !transmissionState) ) {
    if (transmissionMode == 1) redLED.flash(140, 150, 500, vehicleNumber); // ON, OFF, PAUSE, PULSES
    if (transmissionMode == 2) redLED.flash(140, 150, 500, pfChannel + 1); // ON, OFF, PAUSE, PULSES
    if (transmissionMode == 3) redLED.off();

  } else {
    redLED.on(); // Always ON = battery low voltage (Rx or Tx)
  }
}

//
// =======================================================================================================
// CHECK TX BATTERY VOLTAGE
// =======================================================================================================
//

void checkBattery() {

  // Every 500 ms
  static unsigned long lastTrigger;
  if (millis() - lastTrigger >= 500) {
    lastTrigger = millis();

#if F_CPU == 16000000 // 16MHz / 5V
    txBatt = (analogRead(BATTERY_DETECT_PIN) / 68.2) + diodeDrop; // 1023steps / 15V = 68.2 + diode drop!
#else // 8MHz / 3.3V
    txBatt = (analogRead(BATTERY_DETECT_PIN) / 103.33) + diodeDrop; // 1023steps / 9.9V = 103.33 + diode drop!
#endif

    txVcc = readVcc() / 1000.0 ;

    if (txBatt >= cutoffVoltage) {
      batteryOkTx = true;
#ifdef DEBUG
      Serial.print(txBatt);
      Serial.println(" Tx battery OK");
#endif
    } else {
      batteryOkTx = false;
#ifdef DEBUG
      Serial.print(txBatt);
      Serial.println(" Tx battery empty!");
#endif
    }
  }
}

//
// =======================================================================================================
// DRAW DISPLAY
// =======================================================================================================
//

void drawDisplay() {

  u8g.firstPage();  // clear screen
  do {
    switch (activeScreen) {
      case 0: // Screen # 0 splash screen-----------------------------------

        if (operationMode == 0) u8g.drawStr(3, 10, "Micro RC Transmitter");
        if (operationMode == 1) u8g.drawStr(3, 10, "Micro RC Tester");
        if (operationMode == 2) u8g.drawStr(3, 10, "Micro PONG");

        // Dividing Line
        u8g.drawLine(0, 13, 128, 13);

        // Software version
        u8g.setPrintPos(3, 30);
        u8g.print("SW: ");
        u8g.print(codeVersion);

        // Hardware version
        u8g.print(" HW: ");
        u8g.print(boardVersion);

        u8g.setPrintPos(3, 43);
        u8g.print("created by:");
        u8g.setPrintPos(3, 55);
        u8g.print("TheDIYGuy999");

        break;

      case 100: // Screen # 100 diagnosis screen-----------------------------------

        u8g.drawStr(3, 10, "Joystick readings:");

        // Joysticks:
        u8g.setPrintPos(3, 30);
        u8g.print("Axis 1: ");
        u8g.print(data.axis1);
        u8g.setPrintPos(3, 40);
        u8g.print("Axis 2: ");
        u8g.print(data.axis2);
        u8g.setPrintPos(3, 50);
        u8g.print("Axis 3: ");
        u8g.print(data.axis3);
        u8g.setPrintPos(3, 60);
        u8g.print("Axis 4: ");
        u8g.print(data.axis4);

        break;

      case 1: // Screen # 1 main screen-------------------------------------

        // Tester mode ==================
        if (operationMode == 1) {
          // screen dividing lines ----
          u8g.drawLine(0, 12, 128, 12);

          // Tx: data ----
          u8g.setPrintPos(0, 10);
          u8g.print("CH: ");
          u8g.print(vehicleNumber);
          u8g.setPrintPos(50, 10);
          u8g.print("Bat: ");
          u8g.print(txBatt);
          u8g.print("V");

          drawTarget(0, 14, 50, 50, data.axis4, data.axis3); // left joystick
          drawTarget(74, 14, 50, 50, data.axis1, data.axis2); // right joystick
          drawTarget(55, 14, 14, 50, 14, data.pot1); // potentiometer
        }

        // Transmitter mode ================
        if (operationMode == 0) {
          // screen dividing lines ----
          u8g.drawLine(0, 13, 128, 13);
          u8g.drawLine(64, 0, 64, 64);

          // Tx: data ----
          u8g.setPrintPos(0, 10);
          if (transmissionMode > 1) {
            u8g.print("Tx: IR   ");
            if (transmissionMode < 3) u8g.print(pfChannel + 1);

            u8g.setPrintPos(68, 10);
            if (transmissionMode == 2) u8g.print("LEGO");
            if (transmissionMode == 3) u8g.print("MECCANO");
          }
          else {
            u8g.print("Tx: 2.4G");
            u8g.setPrintPos(52, 10);
            u8g.print(vehicleNumber);
          }

          u8g.setPrintPos(3, 25);
          u8g.print("Vcc: ");
          u8g.print(txVcc);

          u8g.setPrintPos(3, 35);
          u8g.print("Bat: ");
          u8g.print(txBatt);

          // Rx: data. Only display the following content, if in radio mode ----
          if (transmissionMode == 1) {
            u8g.setPrintPos(68, 10);
            if (transmissionState) {
              u8g.print("Rx: OK");
            }
            else {
              u8g.print("Rx: ??");
            }

            u8g.setPrintPos(3, 45);
            u8g.print("Mode 1: ");
            u8g.print(data.mode1);

            u8g.setPrintPos(3, 55);
            u8g.print("Mode 2: ");
            u8g.print(data.mode2);

            if (transmissionState) {
              u8g.setPrintPos(68, 25);
              u8g.print("Vcc: ");
              u8g.print(payload.vcc);

              u8g.setPrintPos(68, 35);
              u8g.print("Bat: ");
              u8g.print(payload.batteryVoltage);

              u8g.setPrintPos(68, 45);
              if (payload.batteryOk) {
                u8g.print("Bat. OK ");
              }
              else {
                u8g.print("Low Bat. ");
              }
              u8g.setPrintPos(68, 55);
              u8g.print("CH: ");
              u8g.print(payload.channel);
            }
          }
        }

        // Game mode ================
        // called directly inside the loop() function to increase speed!

        break;

      case 11: // Screen # 11 Menu 1 (channel reversing)-----------------------------------

        u8g.setPrintPos(0, 10);
        u8g.print("Channel Reverse (");
        u8g.print(vehicleNumber);
        u8g.print(")");

        // Dividing Line
        u8g.drawLine(0, 13, 128, 13);

        // Cursor
        if (menuRow == 1) u8g.setPrintPos(0, 25);
        if (menuRow == 2) u8g.setPrintPos(0, 35);
        if (menuRow == 3) u8g.setPrintPos(0, 45);
        if (menuRow == 4) u8g.setPrintPos(0, 55);
        u8g.print(">");

        // Servos
        u8g.setPrintPos(10, 25);
        u8g.print("CH. 1 (R -): ");
        u8g.print(joystickReversed[vehicleNumber][0]); // 0 = Channel 1 etc.

        u8g.setPrintPos(10, 35);
        u8g.print("CH. 2 (R |): ");
        u8g.print(joystickReversed[vehicleNumber][1]);

        u8g.setPrintPos(10, 45);
        u8g.print("CH. 3 (L |): ");
        u8g.print(joystickReversed[vehicleNumber][2]);

        u8g.setPrintPos(10, 55);
        u8g.print("CH. 4 (L -): ");
        u8g.print(joystickReversed[vehicleNumber][3]);

        break;

      case 12: // Screen # 12 Menu 2 (channel travel limitation)-----------------------------------

        u8g.setPrintPos(0, 10);
        u8g.print("Channel % - & + (");
        u8g.print(vehicleNumber);
        u8g.print(")");

        // Dividing Line
        u8g.drawLine(0, 13, 128, 13);

        // Cursor
        if (menuRow == 5) u8g.setPrintPos(45, 25);
        if (menuRow == 6) u8g.setPrintPos(90, 25);
        if (menuRow == 7) u8g.setPrintPos(45, 35);
        if (menuRow == 8) u8g.setPrintPos(90, 35);
        if (menuRow == 9) u8g.setPrintPos(45, 45);
        if (menuRow == 10) u8g.setPrintPos(90, 45);
        if (menuRow == 11) u8g.setPrintPos(45, 55);
        if (menuRow == 12) u8g.setPrintPos(90, 55);
        u8g.print(">");

        // Servo travel percentage
        u8g.setPrintPos(0, 25);
        u8g.print("CH. 1:   ");
        u8g.print(joystickPercentNegative[vehicleNumber][0]); // 0 = Channel 1 etc.
        u8g.setPrintPos(100, 25);
        u8g.print(joystickPercentPositive[vehicleNumber][0]);

        u8g.setPrintPos(0, 35);
        u8g.print("CH. 2:   ");
        u8g.print(joystickPercentNegative[vehicleNumber][1]);
        u8g.setPrintPos(100, 35);
        u8g.print(joystickPercentPositive[vehicleNumber][1]);

        u8g.setPrintPos(0, 45);
        u8g.print("CH. 3:   ");
        u8g.print(joystickPercentNegative[vehicleNumber][2]);
        u8g.setPrintPos(100, 45);
        u8g.print(joystickPercentPositive[vehicleNumber][2]);

        u8g.setPrintPos(0, 55);
        u8g.print("CH. 4:   ");
        u8g.print(joystickPercentNegative[vehicleNumber][3]);
        u8g.setPrintPos(100, 55);
        u8g.print(joystickPercentPositive[vehicleNumber][3]);

        break;
    }
  } while ( u8g.nextPage() ); // show display queue
}

// Draw target subfunction for radio tester mode ----
void drawTarget(int x, int y, int w, int h, int posX, int posY) {
  u8g.drawFrame(x, y, w, h);
  u8g.drawDisc((x + w / 2) - (w / 2) + (posX / 2), (y + h / 2) + (h / 2) - (posY / 2), 5, 5);
}

//
// =======================================================================================================
// MAIN LOOP
// =======================================================================================================
//

void loop() {

  // only read analog inputs in transmitter (0) or game mode (2)
  if (operationMode == 0 || operationMode == 2) {

    // Read joysticks
    readJoysticks();

    // Read Potentiometer
    readPotentiometer();
  }

  // Transmit data via infrared or 2.4GHz radio
  if (operationMode == 1) readRadio(); // 2.4 GHz radio tester
  if (operationMode == 0) {
    transmitRadio(); // 2.4 GHz radio
    if (transmissionMode == 2) transmitLegoIr(); // LEGO Infrared
    if (transmissionMode == 3) transmitMeccanoIr(); // MECCANO Infrared
  }

  // Refresh display every 200 ms in tester mode (otherwise only, if value has changed)
  static unsigned long lastDisplay;
  if (operationMode == 1 && millis() - lastDisplay >= 200) {
    lastDisplay = millis();
    drawDisplay();
  }

  // Atari Pong game :-)
  if (operationMode == 2) pong();

  // If not in game mode:
  else {
    led(); // LED control
    checkBattery(); // Check battery
    readButtons();
  }
}
