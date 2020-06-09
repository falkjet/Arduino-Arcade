#include "tetrisblock.h"

TetrisBlock::TetrisBlock() {
  x = 7;
  y = 0;
    
  switch ((int)floor(random(7))) {
    case 0: // I-block
    this->length = 4;
    this->blocks[0] = Vec2(-1, 0);
    this->blocks[1] = Vec2(0, 0);
    this->blocks[2] = Vec2(1, 0);
    this->blocks[3] = Vec2(2, 0);
    break;
    case 1: // J-block
    this->length = 3;
    this->blocks[0] = Vec2(-1, -1);
    this->blocks[1] = Vec2(-1, 0);
    this->blocks[2] = Vec2(0, 0);
    this->blocks[3] = Vec2(1, 0);
    break;
    case 2: // L-block
    this->length = 3;
    this->blocks[0] = Vec2(-1, 0);
    this->blocks[1] = Vec2(0, 0);
    this->blocks[2] = Vec2(1, 0);
    this->blocks[3] = Vec2(1, -1);
    break;
    case 3: // O-block
    this->length = 2;
    this->blocks[0] = Vec2(0, 0);
    this->blocks[1] = Vec2(1, 0);
    this->blocks[2] = Vec2(0, 1);
    this->blocks[3] = Vec2(1, 1);
    break;
    case 4: // S-block
    this->length = 3;
    this->blocks[0] = Vec2(-1, 0);
    this->blocks[1] = Vec2(0, 0);
    this->blocks[2] = Vec2(0, -1);
    this->blocks[3] = Vec2(1, -1);
    break;
    case 5: // T-block
    this->length = 3;
    this->blocks[0] = Vec2(-1, 0);
    this->blocks[1] = Vec2(0, 0);
    this->blocks[2] = Vec2(0, -1);
    this->blocks[3] = Vec2(1, 0);
    break;
    case 6: // Z-block
    this->length = 3;
    this->blocks[0] = Vec2(-1, -1);
    this->blocks[1] = Vec2(0, -1);
    this->blocks[2] = Vec2(0, 0);
    this->blocks[3] = Vec2(1, 0);
    break;
  }
  this->rotate((int)floor(random(4)));
}

void TetrisBlock::render(Adafruit_SSD1306 *display){
  for (int i = 0; i < 4; i++) {
    display->drawRect((this->x + this->blocks[i].x) * 8, (this->y + this->blocks[i].y) * 8, 8, 8, WHITE);
  }
}

void TetrisBlock::move(int x) {
  this->x += x;
}

void TetrisBlock::rotate(int t, bool force) {
  for (int v = 0; v < t; v++) {
    for (int i = 0; i < 4; i++) {
      this->blocks[i] = this->blocks[i].rotate90();
    }
  }
}