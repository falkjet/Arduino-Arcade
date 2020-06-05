#ifndef BLOCK_H
#define BLOCK_H

#include "rect.h"

class Block : public Rect {
public:
  uint8_t health;
  Block(int x, int y, int width, int height, uint8_t health = 1);
  void damage(int amount=1);
};




#endif /* BLOCK_H */
