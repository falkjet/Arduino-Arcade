#ifndef BREAKOUT_H
#define BREAKOUT_H

#include <Adafruit_SSD1306.h>
#include "buttons.h"
#include "block.h"
#include "vec2.h"

#define WHITE SSD1306_WHITE


const int padY = 60;
const int padWidth = 20;


int breakout(Adafruit_SSD1306 *display, bool easter_egg=false);

#endif /* BREAKOUT_H */
