#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <iostream>
#include <cstdint>
#include <cassert>
#include <algorithm>
#include <immintrin.h>
#include "math.hpp"
#include "camera.hpp"

#define ARGB(a, r, g, b) ((uint32_t)( ((a) & 0xFF) << 24 | ((r) & 0xFF) << 16 | ((g) & 0xFF) << 8 | ((b) & 0xFF) ))
#define BLACK (ARGB(0xff, 0, 0, 0))
#define WHITE (ARGB(0xff, 0xff, 0xff, 0xff))
#define PURPLE (ARGB(0xff, 0xff, 0x00, 0xff))

class Camera;
#define EDGE_FUNCTION(xA,yA,xB,yB,px,py) \
    ((yA - yB) * (px) + (xB - xA) * (py) + (xA * yB - xB * yA))

/*
    Can maybe make this representation smaller?
*/
struct Triangle
{
    int x1; int y1; int z1;
    int x2; int y2; int z2;
    int x3; int y3; int z3;
};



// Signed area of triangle in 2D screen space (XY plane)
inline float TriangleSignedArea(const Triangle3D& tri)
{
    return (tri.v1.x - tri.v0.x) * (tri.v2.y - tri.v0.y)
         - (tri.v2.x - tri.v0.x) * (tri.v1.y - tri.v0.y);
}


struct Edge {
    int a; // coefficient for x
    int b; // coefficient for y
    int c; // constant term
};

Edge MakeEdge(int x0, int y0, int x1, int y1);

enum class RASTERIZERPERFORMANCECLASS
{
    NORMAL,
    SIMD128,
    SIMD256,
    SIMD512,
};


/*
    Lets try to keep this entirely drawing functionality??

    maybe that is the best way???
*/

class Renderer
{
public:
    Renderer(int width = 800, int height = 600);
    ~Renderer();


    inline uint32_t* GetFrameBuffer() const {return m_FrameBuffer;} 
    

    
    inline void RasterizeSIMD128(int px, int py, const Edge &e0, const Edge &e1, const Edge &e2, \
        float invArea, Triangle3D tri, uint32_t color);
    inline void RasterizeSIMD256(int px, int py, const Edge &e0, const Edge &e1, const Edge &e2, \
        float invArea, Triangle3D tri, uint32_t color);

    void SetRenderClass(RASTERIZERPERFORMANCECLASS perf) {m_RasterPerfOption = perf;}
    //void RasterTriangle2D(Triangle& tri);
    void RasterTriangle3D(Triangle3D& tri, Camera* camera);

    inline void PutPixelZ(int x, int y, float z, uint32_t color);
    void ToggleBackFaceCulling() {m_BackFaceCulling = !m_BackFaceCulling;}
    int GetWidth() const {return m_WidthCanvas;}
    int GetHeight() const {return m_HeightCanvas;}
    void PutPixel(int x, int y, uint32_t color);
    void DrawLine(int x1, int y1, int x2, int y2, uint32_t color);
    void DrawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
    void ClearFrameBufferSIMD(uint32_t color);
    void ClearFrameBuffer(uint32_t color);
    void ClearDepthBuffer();
    int RasterCount;
private:
    bool m_BackFaceCulling;
    int m_HeightCanvas;
    int m_WidthCanvas;
    uint32_t*  m_FrameBuffer;
    float* m_DepthBuffer;


    RASTERIZERPERFORMANCECLASS m_RasterPerfOption;


};















#endif