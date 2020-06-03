
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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

class Vec2 {
public:
  int x;
  int y;
  Vec2(int x = 0, int y = 0) {
    this->x = x;
    this->y = y;
  }

  Vec2 operator+(Vec2 const &obj) {
    Vec2 res(this->x + obj.x, this->y + obj.y);
    return res;
  }

  bool operator==(Vec2 const &obj) {
    return (obj.x == this->x) && (obj.y == this->y);
  }

  Vec2 rotate90() {
    return Vec2(-(this->y), this->x);
  }
  Vec2 rotateNegative90() {
    return Vec2(this->y, -(this->x));
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
  Serial.println("Serial started");
  pinMode(leftButtonPin, INPUT);
  pinMode(rightButtonPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("display.begin failed!"));
    for(;;); // Don't proceed, loop forever
  }
  
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

void loop() {
  int result;
  if (waitForAnyKey() == 0) {
    result = breakOut();
  } else {
    result = snake();
  }

  display.clearDisplay();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);

  switch (result)
  {
  case 0:
    display.println(F("You lost!"));
    break;
  case 1:
    display.println(F("You won!"));
  default:
  
    break;
  }
  display.display();

  delay(500);
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

    if (snake[0] == food) {
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

    for (int i = 1; i < SNAKE_MAX_LENGTH; i++) {
      if (snake[i] == snake[0]) {
        return 0;
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

int breakOut() {
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
      return 1;
    }


    if (ballPos.y + 4 > padY) {
      if (ballPos.x > padPos - 2 && ballPos.x < 2+ padPos + padWidth) {
        ballVel.y = -abs(ballVel.y);
        ballVel.x = -(padPos + padWidth/2 -ballPos.x)/4;
      }
    }

    if (ballPos.y > padY +4) {
      Serial.println("you lost");
      return 0;
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
