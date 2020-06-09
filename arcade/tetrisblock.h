#ifndef TETRISBLOCK_H
#define TETRISBLOCK_H

#include <Adafruit_SSD1306.h>
#include "vec2.h"

class TetrisBlock {
public:
  int x, y;
  int length;
  Vec2 blocks[4];

  TetrisBlock();
  void render(Adafruit_SSD1306 *display, int override_x = 0, int override_y = 0, int size=8, bool center=false);
  void move(int x);

  void rotate(int t = 1, bool force = false);
};


#endif /* TETRISBLOCK_H */
