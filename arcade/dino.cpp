#include "dino.h"
#include "buttons.h"
#include "block.h"
#include "vec2.h"

int dino(Adafruit_SSD1306 *display, bool easter_egg) {
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

  if (easter_egg) {
    speed = 3;
  }

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

    display->clearDisplay();
    display->drawRect(obstacle_rect.x, obstacle_rect.y, obstacle_rect.width, obstacle_rect.height, WHITE);
    display->drawRect(player_rect.x, player_rect.y, player_rect.width, player_rect.height, WHITE);
    display->setCursor(90, 10);
    display->print(score);
    display->display();

  }

  return -1;
}
