#include "renderer.hpp"

Renderer::Renderer(int width, int height) : m_HeightCanvas(height), m_WidthCanvas(width), m_RasterPerfOption(RASTERIZERPERFORMANCECLASS::SIMD256)
{
    m_FrameBuffer = new uint32_t[width*height];
    m_DepthBuffer = new float[width*height];
    m_BackFaceCulling = true;
}

inline void Renderer::RasterizeSIMD128(int px, int py, const Edge &e0, const Edge &e1, const Edge &e2, float invArea, Triangle3D tri, uint32_t color)
{
    // X = [px, px+1, px+2, px+3]
    __m128i X = _mm_set_epi32(px+3, px+2, px+1, px);
    __m128i Y = _mm_set1_epi32(py);

    __m128i a0 = _mm_set1_epi32(e0.a);
    __m128i b0 = _mm_set1_epi32(e0.b);
    __m128i c0 = _mm_set1_epi32(e0.c);

    __m128i a1 = _mm_set1_epi32(e1.a);
    __m128i b1 = _mm_set1_epi32(e1.b);
    __m128i c1 = _mm_set1_epi32(e1.c);

    __m128i a2 = _mm_set1_epi32(e2.a);
    __m128i b2 = _mm_set1_epi32(e2.b);
    __m128i c2 = _mm_set1_epi32(e2.c);

    // e0 = a0*X + b0*Y + c0
    __m128i E0 = _mm_add_epi32(
                    _mm_add_epi32(
                        _mm_mullo_epi32(a0, X),
                        _mm_mullo_epi32(b0, Y)),
                    c0);

    __m128i E1 = _mm_add_epi32(
                    _mm_add_epi32(
                        _mm_mullo_epi32(a1, X),
                        _mm_mullo_epi32(b1, Y)),
                    c1);

    __m128i E2 = _mm_add_epi32(
                    _mm_add_epi32(
                        _mm_mullo_epi32(a2, X),
                        _mm_mullo_epi32(b2, Y)),
                    c2);

    // mask = (E0 >= 0) & (E1 >= 0) & (E2 >= 0)
    __m128i zero = _mm_setzero_si128();
    __m128i m0 = _mm_cmpgt_epi32(E0, _mm_set1_epi32(-1));
    __m128i m1 = _mm_cmpgt_epi32(E1, _mm_set1_epi32(-1));
    __m128i m2 = _mm_cmpgt_epi32(E2, _mm_set1_epi32(-1));

    __m128i mask = _mm_and_si128(_mm_and_si128(m0, m1), m2);

    __m128 E0f = _mm_cvtepi32_ps(E0); // convert 4 ints → 4 floats
    __m128 E1f = _mm_cvtepi32_ps(E1);
    __m128 E2f = _mm_cvtepi32_ps(E2);


    __m128 vInvArea = _mm_set1_ps(invArea); // all 4 floats = invArea
    __m128 alpha = _mm_mul_ps(E1f, vInvArea);
    __m128 beta = _mm_mul_ps(E2f, vInvArea);
    __m128 gamma = _mm_mul_ps(E0f, vInvArea);
    __m128 z0 = _mm_set1_ps(tri.v0.z);
    __m128 z1 = _mm_set1_ps(tri.v1.z);
    __m128 z2 = _mm_set1_ps(tri.v2.z);
    __m128 z = _mm_add_ps(
               _mm_add_ps(_mm_mul_ps(alpha, z0),
                          _mm_mul_ps(beta, z1)),
               _mm_mul_ps(gamma, z2));


    // Now write only the pixels that are inside
    alignas(16) uint32_t m[4];
    _mm_store_si128((__m128i*)m, mask);


    for (int i = 0; i < 4; i++)
    {
        if (m[i] == 0xFFFFFFFF)
            PutPixelZ(px + i, py, z[i], (E0f[i] <= 25 || E1f[i] <= 25 || E2f[i] <= 25) ? BLACK: PURPLE);
    }
}

inline void Renderer::RasterizeSIMD256(int px, int py, const Edge &e0, const Edge &e1, const Edge &e2,
                                       float invArea, Triangle3D tri, uint32_t color)
{
  //  printf("here\n");
    // X = [px, px+1, ..., px+7]
    __m256i X = _mm256_set_epi32(px+7, px+6, px+5, px+4, px+3, px+2, px+1, px);
    __m256i Y = _mm256_set1_epi32(py);

    // replicate edge coefficients
    __m256i a0 = _mm256_set1_epi32(e0.a);
    __m256i b0 = _mm256_set1_epi32(e0.b);
    __m256i c0 = _mm256_set1_epi32(e0.c);

    __m256i a1 = _mm256_set1_epi32(e1.a);
    __m256i b1 = _mm256_set1_epi32(e1.b);
    __m256i c1 = _mm256_set1_epi32(e1.c);

    __m256i a2 = _mm256_set1_epi32(e2.a);
    __m256i b2 = _mm256_set1_epi32(e2.b);
    __m256i c2 = _mm256_set1_epi32(e2.c);

    // compute edge function: E = a*X + b*Y + c
    __m256i E0 = _mm256_add_epi32(_mm256_add_epi32(_mm256_mullo_epi32(a0, X),
                                                   _mm256_mullo_epi32(b0, Y)),
                                  c0);
    __m256i E1 = _mm256_add_epi32(_mm256_add_epi32(_mm256_mullo_epi32(a1, X),
                                                   _mm256_mullo_epi32(b1, Y)),
                                  c1);
    __m256i E2 = _mm256_add_epi32(_mm256_add_epi32(_mm256_mullo_epi32(a2, X),
                                                   _mm256_mullo_epi32(b2, Y)),
                                  c2);

    // mask = (E0 >= 0) & (E1 >= 0) & (E2 >= 0)
    __m256i m0 = _mm256_cmpgt_epi32(E0, _mm256_set1_epi32(-1));
    __m256i m1 = _mm256_cmpgt_epi32(E1, _mm256_set1_epi32(-1));
    __m256i m2 = _mm256_cmpgt_epi32(E2, _mm256_set1_epi32(-1));
    __m256i mask = _mm256_and_si256(_mm256_and_si256(m0, m1), m2);

    // convert edges to float for barycentric interpolation
    __m256 E0f = _mm256_cvtepi32_ps(E0);
    __m256 E1f = _mm256_cvtepi32_ps(E1);
    __m256 E2f = _mm256_cvtepi32_ps(E2);

    __m256 vInvArea = _mm256_set1_ps(invArea);
    __m256 alpha = _mm256_mul_ps(E1f, vInvArea);
    __m256 beta  = _mm256_mul_ps(E2f, vInvArea);
    __m256 gamma = _mm256_mul_ps(E0f, vInvArea);

    __m256 z0 = _mm256_set1_ps(tri.v0.z);
    __m256 z1 = _mm256_set1_ps(tri.v1.z);
    __m256 z2 = _mm256_set1_ps(tri.v2.z);

    __m256 z = _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(alpha, z0),
                                           _mm256_mul_ps(beta, z1)),
                             _mm256_mul_ps(gamma, z2));

    // store mask to memory for scalar test
    alignas(32) uint32_t m[8];
    _mm256_store_si256((__m256i*)m, mask);

    alignas(32) float zValues[8];
    _mm256_store_ps(zValues, z);
    for (int i = 0; i < 8; i++)
    {
        if (m[i] == 0xFFFFFFFF)
            PutPixelZ(px + i, py, z[i], (E0f[i] <= 25 || E1f[i] <= 25 || E2f[i] <= 25) ? BLACK: PURPLE);
    }
}

#include "util.hpp"
void Renderer::RasterTriangle3D(Triangle3D &tri, Camera* camera)
{
    //ScopeTimer scope("RasterTriangle3D");

    Vec3 triCenter = (tri.v0 + tri.v1 + tri.v2) * (1.0f / 3.0f);
    Vec3 toTri = NormalizeVec3(triCenter);  // since camera at origin in view space

    if (m_BackFaceCulling && DotProduct(toTri, tri.Normal) <= 0.0f)
        return;
    /*
        Lets make this a function --> bool ClipNearZ(tri)
    */
    float nearZ = 0.1f;   // pick a reasonable near plane
    float minZ = std::min(tri.v0.z, std::min(tri.v1.z, tri.v2.z));
    if (minZ < nearZ)
        return;





    /*
        Bounding box
    */
    Vec2Int p0 = ProjectPerspective(tri.v0, 500.0f, m_WidthCanvas, m_HeightCanvas);
    Vec2Int p1 = ProjectPerspective(tri.v1, 500.0f, m_WidthCanvas, m_HeightCanvas);
    Vec2Int p2 = ProjectPerspective(tri.v2, 500.0f, m_WidthCanvas, m_HeightCanvas);

    int minX = std::min(p0.x, std::min(p1.x, p2.x));
    int maxX = std::max(p0.x, std::max(p1.x, p2.x));
    int minY = std::min(p0.y, std::min(p1.y, p2.y));
    int maxY = std::max(p0.y, std::max(p1.y, p2.y));


    // Lets make this a function --> bool IsOffScreen(p0, p1, p2)
    if (maxX < 0 || minX >= m_WidthCanvas ||
        maxY < 0 || minY >= m_HeightCanvas)
    {
        return;
    }

    // --- Critical fix ---
    minX = std::max(minX, 0);
    maxX = std::min(maxX, m_WidthCanvas  - 1);
    minY = std::max(minY, 0);
    maxY = std::min(maxY, m_HeightCanvas - 1);

    RasterCount++;
    //printf("%d\n", RasterCount);
    float area = (p1.x - p0.x)*(p2.y - p0.y) - (p2.x - p0.x)*(p1.y - p0.y);
    float invArea = 1.0f / area;
    int sign = (area > 0) ? 1 : -1;

    Edge e0 = MakeEdge(p1.x, p1.y, p2.x, p2.y);
    Edge e1 = MakeEdge(p2.x, p2.y, p0.x, p0.y);
    Edge e2 = MakeEdge(p0.x, p0.y, p1.x, p1.y);

    e0.a *= sign; e0.b *= sign; e0.c *= sign;
    e1.a *= sign; e1.b *= sign; e1.c *= sign;
    e2.a *= sign; e2.b *= sign; e2.c *= sign;




   // 4. Rasterize with SIMD in chunks of 4 pixels
    int endX = minX + ((maxX - minX + 1) & ~3); // round down to multiple of 4
    for (int py = minY; py <= maxY; py++)
    {

        switch (m_RasterPerfOption)
        {
            case RASTERIZERPERFORMANCECLASS::NORMAL:  {

                    for (int px = minX; px <= maxX; px++)
                    {
                        int e0v = e0.a*px + e0.b*py + e0.c;
                        int e1v = e1.a*px + e1.b*py + e1.c;
                        int e2v = e2.a*px + e2.b*py + e2.c;

                                                // Barycentric coordinates
                        float alpha = e1v * invArea; // opposite v0
                        float beta  = e2v * invArea; // opposite v1
                        float gamma = e0v * invArea; // opposite v2

                        // Interpolate depth
                        float z = alpha * tri.v0.z + beta * tri.v1.z + gamma * tri.v2.z;

                       // printf("here %d, %d, %d\n", e0v, e1v, e2v);
                        if (e0v >= 0 && e1v >= 0 && e2v >= 0)
                            PutPixelZ(px, py, z, (e0v <= 25 || e1v <= 25 || e2v <= 25) ? BLACK: PURPLE);
                    }

                    break;
            }
            case RASTERIZERPERFORMANCECLASS::SIMD128: {
                    for (int px = minX; px < endX; px += 4)
                        RasterizeSIMD128(px, py, e0, e1, e2, invArea, tri, PURPLE);

                    break;
            }
            case RASTERIZERPERFORMANCECLASS::SIMD256: {
                    for (int px = minX; px < endX; px += 8)
                        RasterizeSIMD256(px, py, e0, e1, e2, invArea, tri, PURPLE);
                    break;
            }
            default:
                assert(false);
        }

    }
   // printf("here\n");
}

inline void Renderer::PutPixelZ(int x, int y, float z, uint32_t color)
{
    static __int128_t count = 0;
    if (!((x >= 0) && (x < m_WidthCanvas) && (y >= 0) && (y < m_HeightCanvas)))
        return;
    int index = y * m_WidthCanvas + x;
    if (z > m_DepthBuffer[index] && z < 0) // closer than old Depth, and still in front of camera
    {
        //printf("(%d, %d) %.3f > %.3f\n", x, y, z, m_DepthBuffer[index]);
        m_DepthBuffer[index] = z; // closer
        m_FrameBuffer[index] = color;
    }
}

void Renderer::PutPixel(int x, int y, uint32_t color)
{
#ifdef BUILD_DEBUG
    assert(x >= 0);
    assert(x < m_WidthCanvas);
    assert(y >= 0);
    assert(y < m_HeightCanvas);
#endif
    if (!((x >= 0) && (x < m_WidthCanvas) && (y >= 0) && (y < m_HeightCanvas)))
        return;


    m_FrameBuffer[y*m_WidthCanvas + x] = color;
}

void Renderer::DrawLine(int x1, int y1, int x2, int y2, uint32_t color)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;

    int err = dx - dy;
    while (true) {
        PutPixel(x1, y1, color);

        // Stop when we reach the end point
        if (x1 == x2 && y1 == y2) break;

        int e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/rasterization-stage.html

        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void Renderer::DrawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
    DrawLine(x0, y0, x1, y1, color);
    DrawLine(x0, y0, x2, y2, color);
    DrawLine(x1, y1, x2, y2, color);
}

void Renderer::ClearFrameBuffer(uint32_t color) 
{
    for (int i = 0; i < m_WidthCanvas*m_HeightCanvas; i++) 
        m_FrameBuffer[i] = color; 
}

void Renderer::ClearDepthBuffer()
{
    int size = m_WidthCanvas * m_HeightCanvas;
    int i = 0;

    // AVX: set 8 copies of far depth (float)
    __m256 vdepth = _mm256_set1_ps(-5000.0f);  // <- notice _ps for floats

    // Vectorized loop: store 8 floats at a time
    for (; i <= size - 8; i += 8)
    {
        _mm256_storeu_ps(m_DepthBuffer + i, vdepth);  // <- store floats
    }

    // Scalar fallback
    for (; i < size; i++)
    {
        m_DepthBuffer[i] = -5000.0f;
    }
}

void Renderer::ClearFrameBufferSIMD(uint32_t color)
{
    int size = m_WidthCanvas * m_HeightCanvas;
    int i = 0;

    // Set 256-bit vector with 8 copies of color
    __m256i vcolor = _mm256_set1_epi32(color);

    // Clear in chunks of 8 pixels
    for (; i <= size - 8; i += 8)
    {
        _mm256_storeu_si256((__m256i*)(m_FrameBuffer + i), vcolor);
    }

    // Scalar fallback for remaining pixels
    for (; i < size; i++)
    {
        m_FrameBuffer[i] = color;
    }
}

Edge MakeEdge(int x0, int y0, int x1, int y1)
{
    Edge e;
    e.a = y0 - y1;        // (yA - yB)
    e.b = x1 - x0;        // (xB - xA)
    e.c = x0 * y1 - x1 * y0; // constant term
    return e;
}