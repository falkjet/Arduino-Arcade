#ifndef DISPLAY_H
#define DISPLAY_H

#define WHITE SSD1306_WHITE
#define BLACK SSD1306_BLACK

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

#include <Adafruit_SSD1306.h>

void getTextDim(Adafruit_SSD1306 *display, char text[], int *width, int *height);

#endif
