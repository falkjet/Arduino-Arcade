#include "breakout.h"

int breakout(Adafruit_SSD1306 *display) {
  display->clearDisplay();
  display->display();
  Block blocks[] = {
    Block(0, 0, 16, 8, 1), Block(16, 0, 16, 8, 1), Block(32, 0, 16, 8, 1), Block(16*3, 0, 16, 8, 1), Block(64, 0, 16, 8, 1), Block(16*5, 0, 16, 8, 1), Block(16*6, 0, 16, 8, 1), Block(16*7, 0, 16, 8, 1),
    Block(0, 8, 16, 8, 1), Block(16, 8, 16, 8, 1), Block(32, 8, 16, 8, 1), Block(16*3, 8, 16, 8, 1), Block(64, 8, 16, 8, 1), Block(16*5, 8, 16, 8, 1), Block(16*6, 8, 16, 8, 1), Block(16*7, 8, 16, 8, 1)
  };
  int padPos = 63;
  Vec2 ballPos = Vec2(63, 50);
  Vec2 ballVel = Vec2(1, -2);
  
  while (true) { //game loop
    display->clearDisplay();
    int aliveBlocks = 0;
    for (Block block: blocks) {
      if (block.health != 0){
        display->drawRect(block.x+1, block.y+1 , block.width-2, block.height-2, WHITE);
      }
    }

    display->drawCircle(ballPos.x, ballPos.y, 2, WHITE);
    display->drawLine(padPos, padY, padPos + padWidth, padY, WHITE);
    ballPos = ballPos + ballVel;
    
    if (ballPos.y < 4) {
      ballVel.y = abs(ballVel.y);
    }
    if (ballPos.x > 126) {
      ballVel.x = -abs(ballVel.x);
    }
    if (ballPos.x < 4) {
      ballVel.x = abs(ballVel.x);
    }

    int numBlocksAlive = 0;
    for (const Block& block: blocks) {
      if (block.health != 0) {
        numBlocksAlive++;
        int inside_bottom = block.y + block.height + 2 - ballPos.y;
        int inside_top = block.height - inside_bottom + 4;
        int inside_right = block.x + block.width + 2 - ballPos.x;
        int inside_left = block.width - inside_right + 4;
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

    display->display();
  }
  display->display();
}
