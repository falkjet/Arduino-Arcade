#include "rect.h"

Rect::Rect(int x, int y, int width, int height) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
}


bool Rect::intersects(const Rect& rect){
    if (this->x >= rect.x +  rect.width || rect.x >= this->x + this->width) 
        return false; 
  
    if (this->y >= rect.y + rect.height || rect.y >= this->y + this->height)
        return false; 
  
    return true; 
}

