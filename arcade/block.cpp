#include "block.h"

Block::Block(int x, int y, int width, int height, uint8_t health)
: Rect(x, y, width, height) {
    this->health = health;
}

void Block::damage(int amount) {
    if (amount > this->health) {
        this->health = 0;
    } else {
        this->health -= amount;
    }
}