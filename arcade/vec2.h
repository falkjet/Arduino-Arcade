#ifndef VEC2_H
#define VEC2_H

class Vec2 {
public:
  int x;
  int y;
  
  Vec2(int x = 0, int y = 0);
  Vec2 operator+(Vec2 const &obj);
  bool operator==(Vec2 const &obj);
  Vec2 rotate90();
  Vec2 rotateNegative90();
};

#endif