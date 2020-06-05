#include "vec2.h"

Vec2::Vec2(int x, int y) {
    this->x = x;
    this->y = y;
}

Vec2 Vec2::operator+(Vec2 const &obj) {
    Vec2 res(this->x + obj.x, this->y + obj.y);
    return res;
}

bool Vec2::operator==(Vec2 const &obj) {
    return (obj.x == this->x) && (obj.y == this->y);
}
Vec2 Vec2::rotate90() {
    return Vec2(-(this->y), this->x);
}

Vec2 Vec2::rotateNegative90() {
    return Vec2(this->y, -(this->x));
}