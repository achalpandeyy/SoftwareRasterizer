#ifndef Z_BUFFER_H

#include "Core/Types.h"

#include <cassert>
#include <climits>

struct ZBuffer
{
    u32 width;
    u32 height;
    f32* z_values;

    inline void Clear()
    {
        for (u32 y = 0; y < height; ++y)
        {
            for (u32 x = 0; x < width; ++x)
                z_values[y * width + x] = std::numeric_limits<f32>::infinity();
        }
    }

    inline b32 TestAndSet(int x, int y, f32 z)
    {
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
        if (z < z_values[y * width + x])
        {
            z_values[y * width + x] = z;
            return true;
        }
        return false;
    }
};


#define Z_BUFFER_H
#endif
