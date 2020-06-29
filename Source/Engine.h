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
    int channels;
    int pitch;
    void* pixels;
};

struct Button
{
    b32 pressed;
    u32 code;
};

struct Vertex
{
    glm::vec3 position;
    glm::vec2 texture_coordinate;
};

struct IndexedTriangleList
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texture_coordinates;
    std::vector<size_t> indices;
};

struct Texture
{
    int width;
    int height;
    int channel_count;

    // NOTE(achal): stb_image returns a unsigned char*.
    u8* texels;

    inline u32 GetTexel(f32 x, f32 y) const
    {
        int texture_x = glm::min((int)(x * width), width - 1);
        int texture_y = glm::min((int)(y * height), height - 1);
        u8* texel = texels + channel_count * ((texture_y * width) + texture_x);

        u8 red = *texel;
        u8 green = *(texel + 1);
        u8 blue = *(texel + 2);

        u32 result = (red << 16 | green << 8 | blue);
        return result;
    }
};

struct Engine
{
    void Initialize();
    void SetupFramebuffer(int width, int height, int bytes_per_pixel, int pitch, void* pixels);
    void UpdateAndRender();

    Framebuffer framebuffer;
    IndexedTriangleList cube;
    Texture texture;
    
    Button buttons[3];
    f32 theta_x = 0.f;
    f32 theta_y = 0.f;
    f32 theta_z = 0.f;
};

#define ENGINE_H
#endif