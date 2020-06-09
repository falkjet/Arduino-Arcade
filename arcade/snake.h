#ifndef SNAKE_H
#define SNAKE_H

#include <Adafruit_SSD1306.h>


#define SNAKE_MAX_LENGTH 40
#define SNAKE_INIT_LENGTH 3
#define SNAKE_GAME_SPEED 20

int snake(Adafruit_SSD1306 *display);

#endif /* SNAKE_H */
