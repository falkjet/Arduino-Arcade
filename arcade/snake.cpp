#include "snake.h"
#include "vec2.h"
#include "buttons.h"

int snake(Adafruit_SSD1306 *display, bool easter_egg) {
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
      if (easter_egg) {
        break;
      }
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
            if (i == SNAKE_MAX_LENGTH - 1) {
              return -1;
            }

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

    display->clearDisplay();

    display->fillRect(food.x * 8 + 2, food.y * 8 + 2, 4, 4, WHITE);
    display->drawRect(food.x * 8, food.y * 8, 8, 8, WHITE);

    for (int i = 0; i < SNAKE_MAX_LENGTH; i++) {
      display->drawRect(snake[i].x * 8, snake[i].y * 8, 8, 8, WHITE);
    }

    display->display();
  }
}
