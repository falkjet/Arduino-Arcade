#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "vec2.h"
#include "block.h"
#include "tetrisblock.h"
#include "breakout.h"
#include "buttons.h"
#include "dino.h"
#include "snake.h"
#include "tetris.h"
#include "display.h"

#define WHITE SSD1306_WHITE

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

#define MENU_ANIM_SPEED 4

#define OLED_RESET 4

Adafruit_SSD1306 display(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, OLED_RESET);

int menu_round = 0;
bool easter_egg = false;
int current_game;

void setup() {
  // Serial.begin(9600);
  
  pinMode(leftButtonPin, INPUT);
  pinMode(rightButtonPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for(;;);
  }

  current_game = 0;
  display.clearDisplay();
  display.setTextColor(WHITE);

  display.setTextSize(2);

  int w, h;
  
  getTextDim(&display, "Arduino", &w, &h);
  display.setCursor(DISPLAY_WIDTH / 2 - w / 2, 0);
  display.print("Arduino");
  getTextDim(&display, "Arcade", &w, &h);
  display.setCursor(DISPLAY_WIDTH / 2 - w / 2, 20);
  display.print("Arcade");
  
  display.display();

  display.setTextSize(1);
  getTextDim(&display, "Press any key", &w, &h);
  display.setCursor(DISPLAY_WIDTH / 2 - w / 2, DISPLAY_HEIGHT - h - 5);
  display.print("Press any key");

  delay(2000);
  display.display();

  int seed = 0;
  while (!(digitalRead(rightButtonPin) || digitalRead(leftButtonPin))) {
    seed++;
    delay(1);
  }
  while ((digitalRead(rightButtonPin) || digitalRead(leftButtonPin))) {
    seed++;
    delay(1);
  }
  randomSeed(seed % 100000);
}

int menu(int start_selected = 0) {
  char * game_names[] = {"Snake", "Breakout", "Dino", "Tetris"};
  
  int n_games = 4;
  int selected = start_selected;
  int offset = 0;
  int anim_speed = MENU_ANIM_SPEED;

  bool right_button_state = false;
  bool last_right_button_state = false;
  bool left_button_state = false;
  bool last_left_button_state = false;

  int frame_ix = 0;
  while (true) {
    frame_ix += 1;
    display.clearDisplay();

    right_button_state = digitalRead(rightButtonPin);
    left_button_state = digitalRead(leftButtonPin);

    if (!right_button_state && last_right_button_state) {
      selected += 1;
      offset += 24;
      if (selected >= n_games) {
        selected = 0;
        offset = -n_games * 24;
        anim_speed = MENU_ANIM_SPEED * n_games;
        menu_round += 1;
        if (menu_round >= 3) {
          menu_round = 0;
          easter_egg = !easter_egg;
          display.invertDisplay(true);
          display.display();
          delay(250);
          display.invertDisplay(false);
          display.display();
          offset = 0;
        }
      }
    }
    if (!left_button_state && last_left_button_state) {
      selected -= 1;
      offset -= 24;
      if (selected < 0) {
        selected = n_games - 1;
        offset = n_games * 24;
        anim_speed = MENU_ANIM_SPEED * n_games;
      }
    }
    if (left_button_state && right_button_state && (!last_left_button_state || !last_right_button_state)) {
      while (digitalRead(rightButtonPin) || digitalRead(leftButtonPin)) {
        delay(1);
      }
      return selected;
    }

    if (offset > 0) {
      offset -= anim_speed;
    }
    if (offset < 0) {
      offset += anim_speed;
    }
    if (abs(offset) < abs(anim_speed)) {
      offset = 0;
    }
    if (offset == 0) {
      anim_speed = MENU_ANIM_SPEED;
    }

    last_left_button_state = left_button_state;
    last_right_button_state = right_button_state;

    for (int i = 0; i < n_games; i++) {
      int game_off = i - selected;
      int rect_size = 12;
      int y_offset = 2;
      
      if (i == selected) {
        rect_size = 16;
        y_offset = 0;
      }

      int rect_x = DISPLAY_WIDTH / 2 - rect_size / 2 + game_off * 24 + offset;
      int rect_y = DISPLAY_HEIGHT - 20 + y_offset;

      display.drawRect(rect_x, rect_y, rect_size, rect_size, WHITE);

      if (i == selected) {
        display.fillRect(rect_x + 3, rect_y + 3, rect_size-6, rect_size-6, WHITE);

        int w, h;
        getTextDim(&display, game_names[selected], &w, &h);

        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(DISPLAY_WIDTH / 2 - w / 2, 12);
        display.print(game_names[selected]);
      }
    }
    
    display.display();
  }
}

void game_end_screen(int result) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 0);

  if (result == -1) {
    display.println(F(" You won!"));
  } else {
    display.println(F("You lost!"));
    if (result != 0) {
      int w, h;
      display.setTextSize(1);
      getTextDim(&display, "Score: 000", &w, &h);
      display.setCursor(DISPLAY_WIDTH / 2 - w / 2, 20);
      display.print("Score: ");
      display.print(result);
    }
  }

  display.setTextSize(1);
  display.setTextColor(WHITE);
  int w, h;
  getTextDim(&display, "Press any key", &w, &h);
  display.setCursor(DISPLAY_WIDTH / 2 - w / 2, 50);
  display.print("Press any key");
  display.display();

  delay(500);
  waitForAnyKey();
}

void loop() {
  current_game = menu(current_game);

  int result;
  switch (current_game) {
    case 0:
      result = snake(&display, easter_egg);
      break;
    case 1:
      result = breakout(&display, easter_egg);
      break;
    case 2:
      result = dino(&display, easter_egg);
      break;
    case 3:
      result = tetris(&display, easter_egg);
      break;
  }

  game_end_screen(result);
}
