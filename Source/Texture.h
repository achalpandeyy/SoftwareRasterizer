#ifndef TEXTURE_H

#include "Core/Types.h"

#include <glm/glm.hpp>

struct Texture
{
    int width;
    int height;
    int channel_count;

    // NOTE(achal): stb_image returns a unsigned char*.
    u8* texels;

    inline u32 GetTexel(f32 x, f32 y, b32 wrap) const
    {
        int texture_x = 0;
        int texture_y = 0;
        if (!wrap)
        {
            texture_x = glm::min((int)(x * width), width - 1);
            texture_y = glm::min((int)(y * height), height - 1);
        }
        else
        {
            texture_x = (int)std::fmod(x * width, width - 1);
            texture_y = (int)std::fmod(y * height, height - 1);
        }

        u8* texel = texels + (size_t)channel_count * (((size_t)texture_y * (size_t)width) + (size_t)texture_x);

        u8 red = *texel;
        u8 green = *(texel + 1);
        u8 blue = *(texel + 2);

        u32 result = (red << 16 | green << 8 | blue);
        return result;
    }
};

#define TEXTURE_H
#endif
