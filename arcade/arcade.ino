
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

#define OLED_RESET     4 
Adafruit_SSD1306 display(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, OLED_RESET);


class Vec2 {
public:
  int x;
  int y;
  Vec2(int x = 0, int y = 0) {
    this->x = x;
    this-> y = y;
  }

  Vec2 operator+(Vec2 const &obj) {
    Vec2 res(this->x + obj.x, this->y + obj.y);
    return res;
  }
};

class Block
{
public:
  int x;
  int y;
  int width;
  int height;
  uint8_t health;
  Block(int x, int y, int width, int height, uint8_t health = 1) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->health = health;
  }
  void damage(int amount) {
    if (amount > this->health) {
      this->health = 0;
    } else {
      this->health -= amount;
    }
  }
  void damage() {
    this->damage(1);
  }

  // void draw
};


void setup() {
  Serial.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("display.begin failed!"));
    for(;;); // Don't proceed, loop forever
  }
  
  display.clearDisplay();
  display.display();

  int result = breakOut();
}

void loop() {
}

int breakOut() {
  display.clearDisplay();
  display.display();

  Block blocks[] = {
    Block(0, 0, 16, 8, 1), Block(16, 0, 16, 8, 1), Block(32, 0, 16, 8, 1), Block(16*3, 0, 16, 8, 1), Block(64, 0, 16, 8, 1), Block(16*5, 0, 16, 8, 1), Block(16*6, 0, 16, 8, 1), Block(16*7, 0, 16, 8, 1),
    Block(0, 8, 16, 8, 1), Block(16, 8, 16, 8, 1), Block(32, 8, 16, 8, 1), Block(16*3, 8, 16, 8, 1), Block(64, 8, 16, 8, 1), Block(16*5, 8, 16, 8, 1), Block(16*6, 8, 16, 8, 1), Block(16*7, 8, 16, 8, 1)
  };
  
  // // draw blocks
  // for (Block block: blocks) {
  //   display.drawRect(block.x+1, block.y+1 , block.width-2, block.height-2, SSD1306_WHITE);
  // }

  Vec2 ballPos = Vec2(63, 50);
  Vec2 ballVel = Vec2(1, 2);
  
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

    ballPos = ballPos + ballVel;

    if (ballPos.y > 60) {
      ballVel.y = -abs(ballVel.y);
      Serial.println("UP");
    }
    
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

    for (const Block& block: blocks) {
      // Serial.print(block.health); Serial.println(0);
      if (block.health != 0) {
        // Serial.print(block.health); Serial.println(0);
        if (ballPos.y < block.y + block.height + 2 && ballPos.x < block.x + block.width + 3 && ballPos.x > block.x - 3) {
          ballVel.y = abs(ballVel.y);
          block.damage(1);
          // Serial.println("UP");
        }
        // Serial.println("BLOCK");
      }
    }

    Serial.print("("); Serial.print(ballPos.x); Serial.print(", "); Serial.print(ballPos.y); Serial.print("), ");
    Serial.print("("); Serial.print(ballVel.x); Serial.print(", "); Serial.print(ballVel.y); Serial.println(")");
    display.display();
  }


  display.display();
}