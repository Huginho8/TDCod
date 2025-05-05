#include "Vec2.h"
#include <cmath>

Vec2::Vec2() : x(0), y(0) {}

Vec2::Vec2(float cx, float cy) : x(cx), y(cy) {}

Vec2::Vec2(const Vec2& from, const Vec2& to)
    : x(to.x - from.x), y(to.y - from.y) {
}

void Vec2::move2D(const Vec2& vec)
{
    x += vec.x;
    y += vec.y;
}

float Vec2::getNorm() const
{
    return std::sqrt(x * x + y * y);
}

float Vec2::getNorm2() const
{
    return x * x + y * y;
}

// Operator overloads

Vec2 operator+(const Vec2& a, const Vec2& b) {
    return Vec2(a.x + b.x, a.y + b.y);
}

Vec2 operator-(const Vec2& a, const Vec2& b) {
    return Vec2(a.x - b.x, a.y - b.y);
}

Vec2 operator-(const Vec2& a) {
    return Vec2(-a.x, -a.y);
}

Vec2 operator*(const Vec2& v, float scalar) {
    return Vec2(v.x * scalar, v.y * scalar);
}

Vec2 operator*(float scalar, const Vec2& v) {
    return v * scalar;
}

Vec2 operator/(const Vec2& v, float scalar) {
    return Vec2(v.x / scalar, v.y / scalar);
}
