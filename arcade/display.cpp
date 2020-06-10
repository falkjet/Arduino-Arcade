#include "display.h"

void getTextDim(Adafruit_SSD1306 *display, char text[], int *width, int *height) {
  int16_t  x1, y1;
  uint16_t w, h;
  display->getTextBounds(text, 10, 0, &x1, &y1, &w, &h);
  *width = (int)w;
  *height = (int)h;
}
