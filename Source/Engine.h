#ifndef ENGINE_H

#include "glm/glm/glm.hpp"

#include <cmath>
#include <vector>

#include <stdint.h>
typedef uint32_t b32;

typedef uint8_t u8;
typedef uint32_t u32;

typedef float f32;

#define PI32 3.14159265f

struct Framebuffer
{
    inline u32* GetPixelPointer(int x, int y)
    {
        return ((u32*)pixels + y * width + x);
    }

    int width;
    int height;
    int bytes_per_pixel;
    int pitch;
    void* pixels;
};

struct Button
{
    b32 pressed;
    u32 code;
};

struct IndexedTriangleList
{
    std::vector<glm::vec3> vertices;
    std::vector<size_t> indices;
};

struct Engine
{
    void Initialize();
    void SetupFramebuffer(int width, int height, int bytes_per_pixel, int pitch, void* pixels);
    void UpdateAndRender();

    Framebuffer framebuffer;
    IndexedTriangleList cube;
    
    Button buttons[3];
    f32 theta_x = 0.f;
    f32 theta_y = 0.f;
    f32 theta_z = 0.f;
};

#define ENGINE_H
#endif