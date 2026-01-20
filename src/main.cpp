#include <SDL2/SDL.h>
#include <iostream>
#include <cstdint>
#include <chrono>


#include "sdl.hpp" // manager class
#include "renderer.hpp"
#include "math.hpp"



const int WIDTH = 800;
const int HEIGHT = 600;



double GetFPS()
{
    static auto last = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> dt = now - last;
    last = now;

    return 1.0 / dt.count();
}


int main(int argc, char** argv) {



    SDLManager* sdl = new SDLManager(WIDTH, HEIGHT);
    Renderer* renderer = new Renderer(WIDTH, HEIGHT);
    Camera* camera  = new Camera();
    sdl->AddCameraHook(camera);
   

    // 2. Allocate our own software framebuffer
    uint32_t* framebuffer = new uint32_t[WIDTH * HEIGHT];

    bool running = true;
    SDL_Event event;
    
    double tick = 0.0f;
    double frame_threshold = 1.0f/60.0f;
    bool done = false;
    float angle = 0;

    float angleX = 0.0f;
    float angleY = 0.0f;
    float angleZ = 0.0f;
    const float rotationSpeed = 0.0001f; // radians per frame

    Vec3 cubeCenter = {0.0f, 0.0f, 0.0f}; // 10 units in front

    while (running) {
    static auto last = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> dt = now - last;
    last = now;
    tick += dt.count();

    sdl->PollEvent();
    if (tick < frame_threshold)
        continue;
    tick = 0.0f;

    // Clear depth first
    renderer->ClearDepthBuffer();
    renderer->ClearFrameBufferSIMD(WHITE);
    camera->UpdateCamera();
    double fps = 1.0 / dt.count();

    for (int i = 0; i < 12; i++) {
        Triangle3D triCopy = cubeTriangles[i];

        // Translate to origin
        triCopy.v0 = triCopy.v0 - cubeCenter;
        triCopy.v1 = triCopy.v1 - cubeCenter;
        triCopy.v2 = triCopy.v2 - cubeCenter;

        // Rotate around origin
        triCopy.v0 = RotateX(RotateY(RotateZ(triCopy.v0, angleZ), angleY), angleX);
        triCopy.v1 = RotateX(RotateY(RotateZ(triCopy.v1, angleZ), angleY), angleX);
        triCopy.v2 = RotateX(RotateY(RotateZ(triCopy.v2, angleZ), angleY), angleX);


        camera->GetLocation();
        camera->GetView();
        // Translate cube in front of camera
        Vec3 worldLoc = {0.0f, 0.0f, -10.0f};
        triCopy.v0 = triCopy.v0 + worldLoc;
        triCopy.v1 = triCopy.v1 + worldLoc;
        triCopy.v2 = triCopy.v2 + worldLoc;



        Triangle3D cameraSpaceTri = Triangle_WorldToCamera(triCopy, camera);

        renderer->RasterTriangle3D(cameraSpaceTri);
    }

    // Increment rotation angles
    angleX += 0.01f;
    angleY += 0.01f;
    angleZ += 0.01f;

    // Update SDL
    sdl->SimpleUpdateTexture(renderer->GetFrameBuffer());
    sdl->RenderTexture();
    
}




    return 0;
}
