#ifndef TETRIS_H
#define TETRIS_H

#include "display.h"

#define TETRIS_DELAY 500
#define TETRIS_SCALE 4
#define TETRIS_BOUNDS 3

int tetris(Adafruit_SSD1306 *display, bool easter_egg=false);

#endif /* TETRIS_H */
