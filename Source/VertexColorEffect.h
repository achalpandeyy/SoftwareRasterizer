#ifndef VERTEX_COLOR_EFFECT_H

#include "Core/Types.h"

#include <glm/glm.hpp>

struct VertexColorEffect
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 color;

        // Copies all attributes except the position attribute.
        inline void CopyAttributesFrom(const Vertex& src)
        {
            color = src.color;
        }
    };

    struct PixelShader
    {
        template <typename Input>
        u32 operator () (const Input& in)
        {
            u8 red = (u8)(in.color.r * 255);
            u8 green = (u8)(in.color.g * 255);
            u8 blue = (u8)(in.color.b * 255);
            u32 result = (red << 16 | green << 8 | blue);

            return result;
        }
    };

    PixelShader pixel_shader;
};

typedef VertexColorEffect::Vertex Vertex;

inline Vertex operator + (const Vertex& v0, const Vertex& v1)
{
    Vertex result = { v0.position + v1.position, v0.color + v1.color };
    return result;
}

inline Vertex operator - (const Vertex& v0, const Vertex& v1)
{
    Vertex result = { v0.position - v1.position, v0.color - v1.color };
    return result;
}

inline Vertex operator * (const Vertex& v, f32 s)
{
    Vertex result = { v.position * s, v.color * s };
    return result;
}

inline Vertex operator / (const Vertex& v, f32 s)
{
    Vertex result = { v.position / s, v.color / s };
    return result;
}

inline Vertex& operator += (Vertex& v0, const Vertex& v1)
{
    v0.position += v1.position;
    v0.color += v1.color;
    return v0;
}

#define VERTEX_COLOR_EFFECT_H
#endif
