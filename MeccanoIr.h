/*
  MeccanoIr.h - Not a "real" library for MECCANO / ERECTOR IR devices - just put it in the same folder as your sketch.
  Created by TheDIYGuy999, Oct 2016
  Released into the public domain.
*/

#ifndef MeccanoIr_h
#define MeccanoIr_h

#include "Arduino.h"

//
// =======================================================================================================
// IR CODE DEFINITION
// =======================================================================================================
//

// Meccano binary code: // means "1", no marking means "0"

// CHANNEL A *************************************************

// A + IR code---------------------------
const byte IrSignalAplus[] PROGMEM = {
  // ON, OFF (in 10's of microseconds)
  180, 180,
  30, 60, //
  30, 30,
  30, 60, //
  30, 30,
  30, 30,
  30, 30,
  30, 60, //
  30, 30,
  30, 60, //
  30, 30,
  30, 30,
  30, 30,
  120, 240
};

// A - IR code---------------------------
const byte IrSignalAminus[] PROGMEM = {
  // ON, OFF (in 10's of microseconds)
  180, 180,
  30, 30,
  30, 60, //
  30, 60, //
  30, 30,
  30, 30,
  30, 30,
  30, 30,
  30, 60, //
  30, 60, //
  30, 30,
  30, 30,
  30, 30,
  120, 240
};

// A OFF IR code---------------------------
const byte IrSignalAoff[] PROGMEM = {
  // ON, OFF (in 10's of microseconds)
  180, 180,
  30, 30,
  30, 30,
  30, 60, //
  30, 30,
  30, 30,
  30, 30,
  30, 30,
  30, 30,
  30, 60, //
  30, 30,
  30, 30,
  30, 30,
  120, 240
};

// CHANNEL B *************************************************

// B + IR code---------------------------
const byte IrSignalBplus[] PROGMEM = {
  // ON, OFF (in 10's of microseconds)
  180, 180,
  30, 60, //
  30, 30,
  30, 30,
  30, 60, //
  30, 30,
  30, 30,
  30, 60, //
  30, 30,
  30, 30,
  30, 60, //
  30, 30,
  30, 30,
  120, 240
};

// B - IR code---------------------------
const byte IrSignalBminus[] PROGMEM = {
  // ON, OFF (in 10's of microseconds)
  180, 180,
  30, 30,
  30, 60, //
  30, 30,
  30, 60, //
  30, 30,
  30, 30,
  30, 30,
  30, 60, //
  30, 30,
  30, 60, //
  30, 30,
  30, 30,
  120, 240
};

// B OFF IR code---------------------------
const byte IrSignalBoff[] PROGMEM = {
  // ON, OFF (in 10's of microseconds)
  180, 180,
  30, 30,
  30, 30,
  30, 30,
  30, 60, //
  30, 30,
  30, 30,
  30, 30,
  30, 30,
  30, 30,
  30, 60, //
  30, 30,
  30, 30,
  120, 240
};

// CHANNEL C *************************************************

// C + IR code---------------------------
const byte IrSignalCplus[] PROGMEM = {
  // ON, OFF (in 10's of microseconds)
  180, 180,
  30, 60, //
  30, 30,
  30, 30,
  30, 30,
  30, 60, //
  30, 30,
  30, 60, //
  30, 30,
  30, 30,
  30, 30,
  30, 60, //
  30, 30,
  120, 240
};

// C - IR code---------------------------
const byte IrSignalCminus[] PROGMEM = {
  // ON, OFF (in 10's of microseconds)
  180, 180,
  30, 30,
  30, 60, //
  30, 30,
  30, 30,
  30, 60, //
  30, 30,
  30, 30,
  30, 60, //
  30, 30,
  30, 30,
  30, 60, //
  30, 30,
  120, 240
};

// C OFF IR code---------------------------
const byte IrSignalCoff[] PROGMEM = {
  // ON, OFF (in 10's of microseconds)
  180, 180,
  30, 30,
  30, 30,
  30, 30,
  30, 30,
  30, 60, //
  30, 30,
  30, 30,
  30, 30,
  30, 30,
  30, 30,
  30, 60, //
  30, 30,
  120, 240
};

// CHANNEL D *************************************************

// D + IR code---------------------------
const byte IrSignalDplus[] PROGMEM = {
  // ON, OFF (in 10's of microseconds)
  180, 180,
  30, 60, //
  30, 30,
  30, 30,
  30, 30,
  30, 30,
  30, 60, //
  30, 60, //
  30, 30,
  30, 30,
  30, 30,
  30, 30,
  30, 60, //
  120, 240
};

// D - IR code---------------------------
const byte IrSignalDminus[] PROGMEM = {
  // ON, OFF (in 10's of microseconds)
  180, 180,
  30, 30,
  30, 60, //
  30, 30,
  30, 30,
  30, 30,
  30, 60, //
  30, 30,
  30, 60, //
  30, 30,
  30, 30,
  30, 30,
  30, 60, //
  120, 240
};

// D OFF IR code---------------------------
const byte IrSignalDoff[] PROGMEM = {
  // ON, OFF (in 10's of microseconds)
  180, 180,
  30, 30,
  30, 30,
  30, 30,
  30, 30,
  30, 30,
  30, 60, //
  30, 30,
  30, 30,
  30, 30,
  30, 30,
  30, 30,
  30, 60, //
  120, 240
};

//
// =======================================================================================================
// IR SIGNAL GENERATION
// =======================================================================================================
//

void buildIrSignal(byte channel) {

  const byte s = 28; // The length of the IR code arrays
  byte IrSignal[s];

  // 1. Read the array from PROGMEM ******
  for (int i = 0; i < s; i++) {
    if (channel == 1) IrSignal[i] = pgm_read_byte(&IrSignalAplus[i]); // Channel A+
    if (channel == 2) IrSignal[i] = pgm_read_byte(&IrSignalAminus[i]); // Channel A-
    if (channel == 3) IrSignal[i] = pgm_read_byte(&IrSignalAoff[i]); // Channel A OFF

    if (channel == 4) IrSignal[i] = pgm_read_byte(&IrSignalBplus[i]); // Channel B+
    if (channel == 5) IrSignal[i] = pgm_read_byte(&IrSignalBminus[i]); // Channel B-
    if (channel == 6) IrSignal[i] = pgm_read_byte(&IrSignalBoff[i]); // Channel B OFF

    if (channel == 7) IrSignal[i] = pgm_read_byte(&IrSignalCplus[i]); // Channel C+
    if (channel == 8) IrSignal[i] = pgm_read_byte(&IrSignalCminus[i]); // Channel C-
    if (channel == 9) IrSignal[i] = pgm_read_byte(&IrSignalCoff[i]); // Channel C OFF

    if (channel == 10) IrSignal[i] = pgm_read_byte(&IrSignalDplus[i]); // Channel D+
    if (channel == 11) IrSignal[i] = pgm_read_byte(&IrSignalDminus[i]); // Channel D-
    if (channel == 12) IrSignal[i] = pgm_read_byte(&IrSignalDoff[i]); // Channel D OFF
  }

  // 2. Generate the IR pulses ******
  for (int i = 0; i < (s - 1); i = i + 2) {
    // HIGH (38KHz modulated) ----
    long microsecs = IrSignal[i] * 10;
    while (microsecs > 0) {

      digitalWrite(3, HIGH);
#if F_CPU == 16000000 // 16MHz CPU
      delayMicroseconds(10);
#endif
      digitalWrite(3, LOW);
#if F_CPU == 16000000 // 16MHz CPU
      delayMicroseconds(10);
#endif
      microsecs -= 26; // 38 kHz is about 13 microseconds high and 13 microseconds low
    }

    // LOW ----
    delayMicroseconds(IrSignal[i + 1] * 10);

  } // End of for loop
  delay(30);
}

#endif
