/*
  A simple "1972 Atari Pong" game. Handy, if you crashed your RC car ;-)
  This code is based on: https://github.com/eholk/Arduino-Pong/blob/master/pong.ino
  Modified to use it with the u8glib and my "Micro RC" transmitter by TheDIYGuy999
*/

#ifndef pong_h
#define pong_h

#include "Arduino.h"

const int PADDLE_RATE = 17; // 17
const int BALL_RATE = 8; // 8
const int FRAME_RATE = 70; // Refresh display every 70ms = 14.2Hz (less slows the ball update down on an 8MHz MPU...)
const int PADDLE_HEIGHT = 14; // 24

const int half_paddle = PADDLE_HEIGHT / 2;

int ball_x = 64, ball_y = 32; // 64, 32
int ball_dir_x = -1, ball_dir_y = 1; // 1, 1

int new_x;
int new_y;

unsigned long ball_update;
unsigned long paddle_update;

const int CPU_X = 12;
int cpu_y = 16;

const int PLAYER_X = 115;
int player_y = 16;

int game_over_difference = 10; // The game is over after this point difference is reached!

int cpu_points = 0;
int player_points = 0;

boolean cpu_won = false;
boolean player_won = false;

//
// =======================================================================================================
// DISPLAY UPDATE SUBFUNCTION
// =======================================================================================================
//

void displayUpdate() {

  static unsigned long lastDisplay;
  if (millis() - lastDisplay >= FRAME_RATE) {
    lastDisplay = millis();

    // clear screen ----
    u8g.firstPage();
    do {
      //u8g.drawPixel(new_x, new_y);
      u8g.drawCircle(new_x, new_y, 1); // Ball
      u8g.drawVLine(CPU_X, cpu_y, PADDLE_HEIGHT); // CPU paddle
      u8g.drawVLine(PLAYER_X, player_y, PADDLE_HEIGHT); // Player paddle
      u8g.drawVLine(64, 0, 3); // Vertical dashed line segments
      u8g.drawVLine(64, 5, 3);
      u8g.drawVLine(64, 10, 3);
      u8g.drawVLine(64, 15, 3);
      u8g.drawVLine(64, 20, 3);
      u8g.drawVLine(64, 25, 3);
      u8g.drawVLine(64, 30, 3);
      u8g.drawVLine(64, 35, 3);
      u8g.drawVLine(64, 40, 3);
      u8g.drawVLine(64, 45, 3);
      u8g.drawVLine(64, 50, 3);
      u8g.drawVLine(64, 55, 3);
      u8g.drawVLine(64, 60, 3);

      u8g.setPrintPos(40, 10);  // CPU points counter
      u8g.print(cpu_points);

      u8g.setPrintPos(80, 10); // Player points counter
      u8g.print(player_points);

      // Game over window
      if (cpu_won || player_won) {
        u8g.setColorIndex(0);
        u8g.drawBox(22, 12, 84, 45); // Clear area behind window
        u8g.setColorIndex(1);
        u8g.drawFrame(22, 12, 84, 45); // Draw window frame

        u8g.setPrintPos(36, 28);
        u8g.print("GAME OVER"); // Game over
        u8g.setPrintPos(31, 48);
        u8g.print("Press BACK!"); // Bress button "Back" to restart
      }
      if (cpu_won ) {
        u8g.setPrintPos(38, 38);
        u8g.print("YOU LOST"); // You lost
      }
      if (player_won ) {
        u8g.setPrintPos(40, 38);
        u8g.print("YOU WON"); // You won
      }

      // show display queue ----
    } while ( u8g.nextPage() );
  }
}

//
// =======================================================================================================
// PONG GAME
// =======================================================================================================
//

void pong() {
  unsigned long time = millis();

  static boolean center;

  // Restart game ----------------------------------------------------------
  if (digitalRead(BUTTON_BACK) == LOW) {
    cpu_won = false;
    player_won = false;
    cpu_points = 0;
    player_points = 0;
  }

  // Ball update -----------------------------------------------------------
  static unsigned long lastBall;
  if (millis() - lastBall >= BALL_RATE && !cpu_won && !player_won) {
    lastBall = millis();

    new_x = ball_x + ball_dir_x;
    new_y = ball_y + ball_dir_y;

    // Counter
    if (new_x > 54 && new_x < 74) center = true;
    if (center && new_x < CPU_X) player_points++, center = false; // Count CPU points
    if (center && new_x > PLAYER_X) cpu_points++, center = false; // Count Player points

    if (cpu_points - player_points >= game_over_difference) cpu_won = true; // Game over, you lost
    if (player_points - cpu_points >= game_over_difference) player_won = true; // Game over, you won

    // Check if we hit the vertical walls
    if (new_x == 2 || new_x == 124) {
      ball_dir_x = -ball_dir_x;
      new_x += ball_dir_x + ball_dir_x;
    }

    // Check if we hit the horizontal walls
    if (new_y == 2 || new_y == 62) {
      ball_dir_y = -ball_dir_y;
      new_y += ball_dir_y + ball_dir_y;
    }

    // Check if we hit the CPU paddle
    if (new_x == CPU_X
        && new_y >= cpu_y
        && new_y <= cpu_y + PADDLE_HEIGHT)
    {
      ball_dir_x = -ball_dir_x;
      new_x += ball_dir_x + ball_dir_x;
    }

    // Check if we hit the player paddle
    if (new_x == PLAYER_X
        && new_y >= player_y
        && new_y <= player_y + PADDLE_HEIGHT)
    {
      ball_dir_x = -ball_dir_x;
      new_x += ball_dir_x + ball_dir_x;
    }

    ball_x = new_x;
    ball_y = new_y;
  }

  // Paddle update -----------------------------------------------------------
  static unsigned long lastPaddle;
  if (millis() - lastPaddle >= PADDLE_RATE && !cpu_won && !player_won) {
    lastPaddle = millis();

    // CPU paddle control----
    if (cpu_y + half_paddle > ball_y) {
      cpu_y -= 1;
    }
    if (cpu_y + half_paddle < ball_y) {
      cpu_y += 1;
    }
    if (cpu_y < 1) cpu_y = 1;
    if (cpu_y + PADDLE_HEIGHT > 63) cpu_y = 63 - PADDLE_HEIGHT;

    // Player paddle control----
#ifdef CH2 // If we have a channel 2 (4 channel joystick transmitter)
    player_y = map(data.axis2, 100, 0, 0, (63 - PADDLE_HEIGHT));
#else // Else (2 channel car style transmitter)
    player_y = map(data.axis1, 100, 0, 0, (63 - PADDLE_HEIGHT));
#endif
  }

  // Display update ------------------------------------------------------------
  displayUpdate();
}

#endif
