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

#define WHITE SSD1306_WHITE

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

#define MENU_ANIM_SPEED 4

#define TETRIS_DELAY 500
#define TETRIS_SCALE 4

#define OLED_RESET 4

Adafruit_SSD1306 display(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, OLED_RESET);


int current_game;


void getTextDim(char text[], int *width, int *height) {
  int16_t  x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 10, 0, &x1, &y1, &w, &h);
  *width = (int)w;
  *height = (int)h;
}

void setup() {
  Serial.begin(9600);
  pinMode(leftButtonPin, INPUT);
  pinMode(rightButtonPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for(;;);
  }

  current_game = 0;
  display.clearDisplay();
  display.display();
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
        getTextDim(game_names[selected], &w, &h);

        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(DISPLAY_WIDTH / 2 - w / 2, 12);
        display.print(game_names[selected]);
      }
    }
    
    display.display();
  }
}

int tetris() {
  TetrisBlock current_block = TetrisBlock();
  int ms_left = TETRIS_DELAY;

  bool taken_spaces[16][8];
  for (int x = 0; x < 16; x++) {
    for (int y = 0; y < 8; y++) {
      taken_spaces[x][y] = 0;
    }
  }
  
  bool right_button_state = false;
  bool last_right_button_state = false;
  bool left_button_state = false;
  bool last_left_button_state = false;
  bool rot_lock = false;
  float difficulty = 1;
  int score = 0;
  
  while (true) {
    display.clearDisplay();

    for (int x = 0; x < 16; x++) {
      for (int y = 0; y < 8; y++) {
        if (taken_spaces[x][y]) {
          display.fillRect(x * 8, y * 8, 8, 8, WHITE);
        }
      }
    }

    current_block.render(&display);
    display.setCursor(0, 0);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.print(score);
    display.display();

    right_button_state = digitalRead(rightButtonPin);
    left_button_state = digitalRead(leftButtonPin);
    last_right_button_state = right_button_state;
    last_left_button_state = left_button_state;

    while (ms_left > 0) {
      delay(10);
      ms_left -= 10;

      right_button_state = digitalRead(rightButtonPin);
      left_button_state = digitalRead(leftButtonPin);
  
      if (!rot_lock) {
        if (!right_button_state && last_right_button_state) {
          break;
        }
        
        if (!left_button_state && last_left_button_state) {
          break;
        }
      }
      
      if (!right_button_state && !left_button_state && rot_lock) {
        rot_lock = false;
      }

      if (left_button_state && right_button_state && (!last_left_button_state || !last_right_button_state)) {
        break;
      }

      last_right_button_state = right_button_state;
      last_left_button_state = left_button_state;
    }

    if (ms_left <= 0) {
      current_block.y += 1;

      bool has_collided = false;
      for (int v = 0; v < 4; v++) {
        if (current_block.blocks[v].y + current_block.y > 7) {
          has_collided = true;
          break;
        }

        if (taken_spaces[current_block.blocks[v].x + current_block.x][current_block.blocks[v].y + current_block.y]) {
          has_collided = true;
          break;
        }
      }

      int amount_cleared = 0;
      if (has_collided) {
        current_block.y -= 1;

        for (int i = 0; i < 4; i++) {
          int subblock_x = current_block.blocks[i].x + current_block.x;
          int subblock_y = current_block.blocks[i].y + current_block.y;

          taken_spaces[subblock_x][subblock_y] = true;

          bool all_taken = true;
          for (int x = 0; x < 16; x++) {
            if (!taken_spaces[x][subblock_y]) {
              all_taken = false;
              break;
            }
          }

          if (all_taken) {
            amount_cleared += 1;

            for (int x = 0; x < 16; x++) {
              taken_spaces[x][subblock_y] = 0;
            }

            for (int x = 0; x < 16; x++) {
              for (int y = subblock_y; y > 0; y--) {
                taken_spaces[x][y] = taken_spaces[x][y-1];
              }
            }
          }
        }
  
        current_block = TetrisBlock();
        for (int i = 0; i < 4; i++) {
          int subblock_x = current_block.blocks[i].x + current_block.x;
          int subblock_y = current_block.blocks[i].y + current_block.y;

          if (subblock_x < 0 || subblock_y < 0) {
            continue;
          }

          if (taken_spaces[subblock_x][subblock_y]) {
            return score;
          }
        }
      }

      if (amount_cleared > 0) {
        difficulty += .5;
      }
      score += amount_cleared * amount_cleared;

      for (int j = 0; j < amount_cleared; j++) {
        display.invertDisplay(true);
        delay(250);
        display.invertDisplay(false);
        delay(250);
      }
      
      ms_left = (int)floor(TETRIS_DELAY / difficulty);
    }

    if (left_button_state && right_button_state && (!last_left_button_state || !last_right_button_state)) {
      current_block.rotate();
      rot_lock = true;
      for (int v = 0; v < 4; v++) {
        if (taken_spaces[current_block.blocks[v].x + current_block.x][current_block.blocks[v].y + current_block.y]) {
          current_block.rotate(3);
          rot_lock = false;
          break;
        }
      }
    }

    if (!rot_lock) {
      if (!right_button_state && last_right_button_state) {
        current_block.move(1);

        for (int v = 0; v < 4; v++) {
          if (taken_spaces[current_block.blocks[v].x + current_block.x][current_block.blocks[v].y + current_block.y]) {
            current_block.move(-1);
            break;
          }
          if (current_block.blocks[v].x + current_block.x > 15) {
            current_block.move(-1);
          }
        }
      } 
      if (!left_button_state && last_left_button_state) {
        current_block.move(-1);

        for (int v = 0; v < 4; v++) {
          if (taken_spaces[current_block.blocks[v].x + current_block.x][current_block.blocks[v].y + current_block.y]) {
            current_block.move(1);
            break;
          }

          if (current_block.blocks[v].x + current_block.x < 0) {
            current_block.move(1);
            break;
          }
        }
      }
    }
  }
  
  return -1;
}

void loop() {
  Serial.println("Hello, world");
  current_game = menu(current_game);

  int result;
  switch (current_game) {
    case 0:
      result = snake(&display);
      break;
    case 1:
      result = breakout(&display);
      break;
    case 2:
      result = dino(&display);
      break;
    case 3:
      result = tetris();
      break;
  }

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
      getTextDim("Score: 000", &w, &h);
      display.setCursor(DISPLAY_WIDTH / 2 - w / 2, 20);
      display.print("Score: ");
      display.print(result);
    }
  }

  display.setTextSize(1);
  display.setTextColor(WHITE);
  int w, h;
  getTextDim("Press any key", &w, &h);
  display.setCursor(DISPLAY_WIDTH / 2 - w / 2, 50);
  display.print("Press any key");
  display.display();

  delay(500);
  waitForAnyKey();
}
