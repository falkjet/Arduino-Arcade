#include  "tetris.h"
#include  "tetrisblock.h"
#include  "buttons.h"

int tetris(Adafruit_SSD1306 *display) {
  TetrisBlock current_block = TetrisBlock();
  TetrisBlock next_block = TetrisBlock();
  
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

  float difficulty = 0.5;
  
  int score = 0;
  
  while (true) {
    display->clearDisplay();

    for (int x = 0; x < 16; x++) {
      for (int y = 0; y < 8; y++) {
        if (taken_spaces[x][y]) {
          display->fillRect(x * 8, y * 8, 8, 8, WHITE);
        }
      }
    }

    current_block.render(display);
    display->setCursor(0, 0);
    display->setTextColor(WHITE);
    display->setTextSize(1);

    int w, h;
    getTextDim(display, score, &w, &h);
       
    display->setCursor(DISPLAY_WIDTH - TETRIS_BOUNDS * 4 - w / 2, 12);
    display->print(score);
    display->drawRect(0, 0, TETRIS_BOUNDS * 8, DISPLAY_HEIGHT, WHITE);
    display->drawRect(DISPLAY_WIDTH - (TETRIS_BOUNDS * 8), 0, TETRIS_BOUNDS * 8, DISPLAY_HEIGHT, WHITE);

    next_block.render(display, 14, 12, 4, true);

    display->display();

    right_button_state = digitalRead(rightButtonPin);
    left_button_state = digitalRead(leftButtonPin);
    last_right_button_state = right_button_state;
    last_left_button_state = left_button_state;

    while (ms_left > 0) {
      delay(5);
      ms_left -= 5;

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

        if (current_block.blocks[v].x + current_block.x >= 0 && current_block.blocks[v].y + current_block.y >= 0) {
          if (taken_spaces[current_block.blocks[v].x + current_block.x][current_block.blocks[v].y + current_block.y]) {
            has_collided = true;
            break;
          }
        }
      }

      int amount_cleared = 0;
      if (has_collided) {
        current_block.y -= 1;

        for (int i = 0; i < 4; i++) {
          int subblock_x = current_block.blocks[i].x + current_block.x;
          int subblock_y = current_block.blocks[i].y + current_block.y;

          taken_spaces[subblock_x][subblock_y] = true;
        }

        for (int i = 0; i < 4; i++) {
          int subblock_y = current_block.blocks[i].y + current_block.y;
          
          bool all_taken = true;
          for (int x = TETRIS_BOUNDS; x < 16 - TETRIS_BOUNDS; x++) {
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
  
        current_block = next_block;
        next_block = TetrisBlock();
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
        difficulty += .2;
      }
      score += amount_cleared * amount_cleared;

      for (int j = 0; j < amount_cleared; j++) {
        display->invertDisplay(true);
        delay(250);
        display->invertDisplay(false);
        delay(250);
      }
      
      ms_left = (int)floor(TETRIS_DELAY / difficulty);
    }

    if (left_button_state && right_button_state && (!last_left_button_state || !last_right_button_state)) {
      current_block.rotate();
      rot_lock = true;
      for (int v = 0; v < 4; v++) {
        bool subblock_taken = taken_spaces[current_block.blocks[v].x + current_block.x][current_block.blocks[v].y + current_block.y];
        bool subblock_out_of_bounds = (current_block.blocks[v].x + current_block.x < TETRIS_BOUNDS) || (current_block.blocks[v].x + current_block.x >= 16 - TETRIS_BOUNDS);
        
        if (subblock_taken || subblock_out_of_bounds) {
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
          if (current_block.blocks[v].x + current_block.x >= 16 - TETRIS_BOUNDS) {
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

          if (current_block.blocks[v].x + current_block.x < TETRIS_BOUNDS) {
            current_block.move(1);
            break;
          }
        }
      }
    }
  }
  
  return -1;
}
