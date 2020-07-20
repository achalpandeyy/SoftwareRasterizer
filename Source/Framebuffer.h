#ifndef FRAMEBUFFER_H

#include "Core/Types.h"

#include <cassert>
#include <cstring>

struct Framebuffer
{
    inline void PutPixel(int x, int y, u32 color)
    {
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
        u32* pixel = (u32*)pixels + ((size_t)y * (size_t)width) + x;
        *pixel = color;
    }

    inline void Clear()
    {
        memset(pixels, 32, (size_t)width * (size_t)height * (size_t)channel_count);
    }

    int width;
    int height;
    int channel_count;
    void* pixels;
};

#define FRAMEBUFFER_H
#endif
