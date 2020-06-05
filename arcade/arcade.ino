
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "vec2.h"
#include "block.h"

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

#define SNAKE_MAX_LENGTH 16
#define SNAKE_INIT_LENGTH 4
#define SNAKE_GAME_SPEED 20

#define OLED_RESET     4 
Adafruit_SSD1306 display(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, OLED_RESET);

const int leftButtonPin = 2;
const int rightButtonPin = 3; 


const int padY = 60;
const int padWidth = 20;

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
  Serial.println("Serial started");
  pinMode(leftButtonPin, INPUT);
  pinMode(rightButtonPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("display.begin failed!"));
    for(;;); // Don't proceed, loop forever
  }

  current_game = 2;
  
  display.clearDisplay();
  display.display();
}

int waitForAnyKey() {
  while ((digitalRead(leftButtonPin) == HIGH) || (digitalRead(rightButtonPin) == HIGH))
  {
    delay(10);
  }

  while (!((digitalRead(leftButtonPin) == HIGH) || (digitalRead(rightButtonPin) == HIGH)))
  {
    delay(10);
  }

  int pressedButton = (int)(digitalRead(leftButtonPin) == HIGH);

  while ((digitalRead(leftButtonPin) == HIGH) || (digitalRead(rightButtonPin) == HIGH))
  {
    delay(10);
  }
  return pressedButton;
}

int menu(String game_names[], int selected = 0) {
  int n_games = 3;
  
  int offset = 0;
  int anim_speed = 3;

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

    if (right_button_state && !last_right_button_state) {
      selected += 1;
      offset += 24;
      if (selected >= n_games) {
        selected = 0;
        offset = -n_games * 24;
        anim_speed = 3 * n_games;
      }
    }

    if (left_button_state && !last_left_button_state) {
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
      anim_speed = 3;
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

      display.drawRect(rect_x, rect_y, rect_size, rect_size, SSD1306_WHITE);

      if (i == selected) {
        display.fillRect(rect_x + 3, rect_y + 3, rect_size-6, rect_size-6, SSD1306_WHITE);

        int16_t  x1, y1;
        uint16_t w, h;
         
        display.getTextBounds(game_names[selected], 10, 0, &x1, &y1, &w, &h);

        display.setTextSize(2); // Draw 2X-scale text
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0); // DISPLAY_WIDTH / 2 - w / 2, 0);
        display.print(game_names[selected]);
      }
    }
    
    display.display();
  }
}

int dino() {
  const Vec2 dinoSize(22, 24);
  float player_pos = 0;
  float player_vel = 0;
  int obstacle_pos = 128;
  int score = 0;
  
  const float jump_height = 3.0;
  const float gravity = 0.2;
  const float obstacle_speed = 2;
  float speed = 1;

  const Vec2 player_size(22, 24);
  const Vec2 obstacle_size(10, 10);

  while(true) {
    player_pos += player_vel * speed;
    if (player_pos < 0) {
      player_pos = 0;
      player_vel = 0;
    } else {
       player_vel  -= gravity * speed;
    }
    obstacle_pos -= obstacle_speed*speed;

    if (player_pos == 0 && digitalRead(rightButtonPin)) {
      player_vel = jump_height;
    }

    if (obstacle_pos + obstacle_size.x < 0) {
      score++;
      obstacle_pos = 128;
    }
    
    Block player_rect = Block(20, 50-player_size.y - player_pos, player_size.x, player_size.y);
    Block obstacle_rect = Block(obstacle_pos, 50 - obstacle_size.y , obstacle_size.x, obstacle_size.y);

    if (player_rect.intersects(obstacle_rect)) {
      return score;
    }

    display.clearDisplay();
    display.drawRect(obstacle_rect.x, obstacle_rect.y, obstacle_rect.width, obstacle_rect.height, SSD1306_WHITE);
    display.drawRect(player_rect.x, player_rect.y, player_rect.width, player_rect.height, SSD1306_WHITE);
    display.setCursor(90, 10);
    display.print(score);
    display.display();

  }

  return -1;
}

void loop() {
  String game_names[] = {"Snake", "Breakout", "Dino"};

  current_game = menu(game_names, current_game);

  int result;
  switch (current_game) {
    case 0:
      result = snake();
      break;
    case 1:
      result = breakout();
      break;
    case 2:
      result = dino();
      break;
  }

  Serial.print("RESULT: ");
  Serial.println(result);

  display.clearDisplay();
  
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);

  int score;

  if (result == -1) {
    display.println(F(" You won!"));
  } else {
    display.println(F("You lost!"));

    if (result == 0) {
      score = -1;
    } else {
      score = result;
    }
  }

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  int w, h;
  getTextDim("Press any key", &w, &h);
  
  display.setCursor(DISPLAY_WIDTH / 2 - w / 2, 50);
  display.print("Press any key");

  if (score != -1) {
    getTextDim("Score: 000", &w, &h);
    display.setCursor(DISPLAY_WIDTH / 2 - w / 2, 20);
    display.print("Score: ");
    display.print(score);
  }

  display.display();

  delay(500);

  waitForAnyKey();
}

int snake() {
  Vec2 food = Vec2(6, 4);

  Vec2 snake[SNAKE_MAX_LENGTH];

  Vec2 direction = Vec2(1, 0);

  for (int i = 0; i < SNAKE_MAX_LENGTH; i++) {
    snake[i] = Vec2(-1, -1);
  }

  for (int i = 0; i < SNAKE_INIT_LENGTH; i++) {
    snake[i] = Vec2(8, 4);
  }

  bool lastFrameLeftButtonPressed = false;
  bool lastFrameRightButtonPressed = false;

  bool leftButtonPressed = false;
  bool rightButtonPressed = false;

  int food_eaten = 0;

  while (true) {
    for (int n = 0; n < SNAKE_GAME_SPEED; n++) {
      leftButtonPressed = digitalRead(leftButtonPin) == HIGH;
      rightButtonPressed = digitalRead(rightButtonPin) == HIGH;
  
      if (rightButtonPressed && !lastFrameRightButtonPressed) {
        direction = direction.rotate90();
      }
      
      if (leftButtonPressed && !lastFrameLeftButtonPressed) {
        direction = direction.rotateNegative90();
      }
  
      lastFrameLeftButtonPressed = leftButtonPressed;
      lastFrameRightButtonPressed = rightButtonPressed;
      delay(10);
    }

    for (int i = SNAKE_MAX_LENGTH-1; i > 0; i--) {
      if (snake[i].x == -1) {
        continue;
      }
      snake[i].x = snake[i - 1].x;
      snake[i].y = snake[i - 1].y;
    }
        
    snake[0].x += direction.x;
    snake[0].y += direction.y;

    snake[0].x = (snake[0].x + 16) % 16;
    snake[0].y = (snake[0].y + 8) % 8;

    for (int v = 0; v < SNAKE_MAX_LENGTH; v++) {
      if (snake[v] == food) {
        food_eaten += 1;
        food.x = random(0, 15);
        food.y = random(0, 7);
  
        for (int i = 0; i < SNAKE_MAX_LENGTH; i++) {
          if (snake[i].x == -1) {
            snake[i].x = snake[i - 1].x;
            snake[i].y = snake[i - 1].y;
            break;
          }
        }
      }
    }

    for (int i = 1; i < SNAKE_MAX_LENGTH; i++) {
      if (snake[i] == snake[0]) {
        return food_eaten;
      }
    }

    display.clearDisplay();

    display.fillRect(food.x * 8 + 2, food.y * 8 + 2, 4, 4, SSD1306_WHITE);
    display.drawRect(food.x * 8, food.y * 8, 8, 8, SSD1306_WHITE);

    for (int i = 0; i < SNAKE_MAX_LENGTH; i++) {
      display.drawRect(snake[i].x * 8, snake[i].y * 8, 8, 8, SSD1306_WHITE);
    }

    display.display();
  }
}

int breakout() {
  display.clearDisplay();
  display.display();

  Block blocks[] = {
    Block(0, 0, 16, 8, 1), Block(16, 0, 16, 8, 1), Block(32, 0, 16, 8, 1), Block(16*3, 0, 16, 8, 1), Block(64, 0, 16, 8, 1), Block(16*5, 0, 16, 8, 1), Block(16*6, 0, 16, 8, 1), Block(16*7, 0, 16, 8, 1),
    Block(0, 8, 16, 8, 1), Block(16, 8, 16, 8, 1), Block(32, 8, 16, 8, 1), Block(16*3, 8, 16, 8, 1), Block(64, 8, 16, 8, 1), Block(16*5, 8, 16, 8, 1), Block(16*6, 8, 16, 8, 1), Block(16*7, 8, 16, 8, 1)
  };
  

  int padPos = 63;
  // // draw blocks
  // for (Block block: blocks) {
  //   display.drawRect(block.x+1, block.y+1 , block.width-2, block.height-2, SSD1306_WHITE);
  // }

  Vec2 ballPos = Vec2(63, 50);
  Vec2 ballVel = Vec2(1, -2);
  
  while (true) //game loop
  {
    display.clearDisplay();
    int aliveBlocks = 0;
    for (Block block: blocks) {
      if (block.health != 0){
        display.drawRect(block.x+1, block.y+1 , block.width-2, block.height-2, SSD1306_WHITE);
      }
    }

    display.drawCircle(ballPos.x, ballPos.y, 2, SSD1306_WHITE);

    display.drawLine(padPos, padY, padPos + padWidth, padY, SSD1306_WHITE);

    ballPos = ballPos + ballVel;

    // if (ballPos.y > 60) {
    //   ballVel.y = -abs(ballVel.y);
    //   Serial.println("UP");
    // }
    
    if (ballPos.y < 4) {
      ballVel.y = abs(ballVel.y);
      Serial.println("DOWN");
    }
    if (ballPos.x > 126) {
      ballVel.x = -abs(ballVel.x);
      Serial.println("UP");
    }
    
    if (ballPos.x < 4) {
      ballVel.x = abs(ballVel.x);
      Serial.println("DOWN");
    }

    //ballPos.x += ballVel.x; ballPos.y += ballVel.y;

    int numBlocksAlive = 0;

    for (const Block& block: blocks) {
      // Serial.print(block.health); Serial.println(0);
      if (block.health != 0) {
        numBlocksAlive++;
        // Serial.print(block.health); Serial.println(0);
        int inside_bottom = block.y + block.height + 2 - ballPos.y;
        int inside_top = block.height - inside_bottom + 4;
        int inside_right = block.x + block.width + 2 - ballPos.x;
        int inside_left = block.width - inside_right + 4;
        // int inside_left = 
        if (inside_bottom > 0 && inside_right > 0 && inside_left > 0 && inside_top > 0) {
          block.damage(1);
          if (min(inside_top, inside_top) > min(inside_left, inside_right)){
            if (inside_top < inside_top) {
              ballVel.y = -abs(ballVel.y);
            } else {
              ballVel.y = abs(ballVel.y);
            }
            
          } else {
            if (inside_left < inside_right) {
              ballVel.x = -abs(ballVel.x);
            } else {
              ballVel.x = abs(ballVel.x);
            }
          }
        }
        // Serial.println("BLOCK");
      }
    }

    if (numBlocksAlive == 0) {
      Serial.println("you won");
      return -1;
    }


    if (ballPos.y + 4 > padY) {
      if (ballPos.x > padPos - 2 && ballPos.x < 2+ padPos + padWidth) {
        ballVel.y = -abs(ballVel.y);
        ballVel.x = -(padPos + padWidth/2 -ballPos.x)/4;
      }
    }

    if (ballPos.y > padY +4) {
      Serial.println("you lost");
      return (sizeof(blocks) / sizeof(blocks[0])) - numBlocksAlive;
    }

    int buttonState = digitalRead(leftButtonPin);

    digitalWrite(LED_BUILTIN, buttonState == HIGH);

    if (buttonState == HIGH) {
      padPos -= 3;
    }

    buttonState = digitalRead(rightButtonPin);

    digitalWrite(LED_BUILTIN, buttonState == HIGH);

    if (buttonState == HIGH) {
      padPos += 3;
    }


    // Serial.print("("); Serial.print(ballPos.x); Serial.print(", "); Serial.print(ballPos.y); Serial.print("), ");
    // Serial.print("("); Serial.print(ballVel.x); Serial.print(", "); Serial.print(ballVel.y); Serial.println(")");
    display.display();
  }


  display.display();
}
