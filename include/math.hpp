#ifndef MATH_HPP
#define MATH_HPP
#include <math.h>




constexpr float PI = 3.14159265358979323846f;


struct Vec3 {
    float x, y, z;
     Vec3 operator+(const Vec3& other) const {
        return {x + other.x, y + other.y, z + other.z};
    }

    // Subtraction
    Vec3 operator-(const Vec3& other) const {
        return {x - other.x, y - other.y, z - other.z};
    }

    // Optional: scalar multiplication
    Vec3 operator*(float s) const {
        return {x * s, y * s, z * s};
    }

    Vec3& operator+=(const Vec3& other) {
        x += other.x; y += other.y; z += other.z;
        return *this;
    }

    Vec3& operator-=(const Vec3& other) {
        x -= other.x; y -= other.y; z -= other.z;
        return *this;
    }
};

struct Vec2 {
    float x, y;
        Vec2 operator+(const Vec2& other) const {
        return {x + other.x, y + other.y};
    }

    Vec2 operator-(const Vec2& other) const {
        return {x - other.x, y - other.y};
    }

    Vec2& operator+=(const Vec2& other) {
        x += other.x; y += other.y;
        return *this;
    }

    Vec2& operator-=(const Vec2& other) {
        x -= other.x; y -= other.y;
        return *this;
    }
};

struct Vec2Int {
    int x, y;
};



struct Triangle3D { 
    Vec3 v0, v1, v2; 
    Vec3 Normal;
};

Vec3 RotateX(const Vec3 &v, float angle);
Vec3 RotateY(const Vec3 &v, float angle);
Vec3 RotateZ(const Vec3 &v, float angle);


void PrintVec3(const Vec3& v);
void PrintVec2Int(const Vec2Int& v);

Vec2Int ProjectPerspective(const Vec3 &p, float f, int width, int height);
Vec3 NormalizeVec3(const Vec3& v);
Vec3 CrossProduct(const Vec3& a, const Vec3& b);
float DotProduct(const Vec3& a, const Vec3& b);
inline float Magnitude(const Vec3& v);


extern Vec3 cubeVertices[8];
extern Triangle3D cubeTriangles[12];

#include "renderer.hpp"

#endif