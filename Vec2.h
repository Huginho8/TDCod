#ifndef VEC2_H
#define VEC2_H

class Vec2 {
public:
    float x, y;

    // Constructors
    Vec2();
    Vec2(float x, float y);
    Vec2(const Vec2& from, const Vec2& to);

    // Member functions
    void move2D(const Vec2& vec);
    float getNorm() const;
    float getNorm2() const;
};

// Operator overloads
Vec2 operator+(const Vec2& a, const Vec2& b);
Vec2 operator-(const Vec2& a, const Vec2& b);
Vec2 operator-(const Vec2& a);
Vec2 operator*(const Vec2& v, float scalar);
Vec2 operator*(float scalar, const Vec2& v);
Vec2 operator/(const Vec2& v, float scalar);

#endif // VEC2_H

