#ifndef RECT_H
#define RECT_H

#include <Arduino.h>

class Rect {
public:
  int x;
  int y;
  int width;
  int height;
  Rect(int x, int y, int width, int height);
  bool intersects(const Rect& rect);
};


#endif /* RECT_H */
