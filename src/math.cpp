#include "math.hpp"


float s = 1.0f; // half-size
Vec3 cubeVertices[8] = {
    {-s, -s, -s}, // 0: left-bottom-back
    { s, -s, -s}, // 1: right-bottom-back
    { s,  s, -s}, // 2: right-top-back
    {-s,  s, -s}, // 3: left-top-back
    {-s, -s,  s}, // 4: left-bottom-front
    { s, -s,  s}, // 5: right-bottom-front
    { s,  s,  s}, // 6: right-top-front
    {-s,  s,  s}, // 7: left-top-front
};


Triangle3D cubeTriangles[12] = {
    // Back face
    {cubeVertices[0], cubeVertices[1], cubeVertices[2]},
    {cubeVertices[0], cubeVertices[2], cubeVertices[3]},

    // Front face
    {cubeVertices[4], cubeVertices[6], cubeVertices[5]},
    {cubeVertices[4], cubeVertices[7], cubeVertices[6]},

    // Left face
    {cubeVertices[0], cubeVertices[3], cubeVertices[7]},
    {cubeVertices[0], cubeVertices[7], cubeVertices[4]},

    // Right face
    {cubeVertices[1], cubeVertices[5], cubeVertices[6]},
    {cubeVertices[1], cubeVertices[6], cubeVertices[2]},

    // Top face
    {cubeVertices[3], cubeVertices[2], cubeVertices[6]},
    {cubeVertices[3], cubeVertices[6], cubeVertices[7]},

    // Bottom face
    {cubeVertices[0], cubeVertices[4], cubeVertices[5]},
    {cubeVertices[0], cubeVertices[5], cubeVertices[1]},
};








Vec2Int ProjectPerspective(const Vec3 &p, float f, int width, int height)
{  
    Vec2Int screen;
    float cx = width / 2.0f;
    float cy = height / 2.0f;

    screen.x = (p.x * f / -p.z) + cx;
    screen.y = cy - (p.y * f / -p.z);


    return screen;
}

Vec3 RotateX(const Vec3 &v, float angle)
{
    float c = cosf(angle);
    float s = sinf(angle);
    return {v.x, v.y * c - v.z * s, v.y * s + v.z * c};
}

Vec3 RotateY(const Vec3 &v, float angle)
{
    float c = cosf(angle);
    float s = sinf(angle);
    return {v.x * c + v.z * s, v.y, -v.x * s + v.z * c};
}

Vec3 RotateZ(const Vec3 &v, float angle)
{
    float c = cosf(angle);
    float s = sinf(angle);
    return {v.x * c - v.y * s, v.x * s + v.y * c, v.z};
}

void PrintVec3(const Vec3 &v)
{
    printf("<%.3f, %.3f, %.3f>\n", v.x, v.y, v.z);
}

void PrintVec2Int(const Vec2Int &v)
{
     printf("<%d, %d>\n", v.x, v.y);
}
