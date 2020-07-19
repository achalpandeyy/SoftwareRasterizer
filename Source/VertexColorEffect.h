#ifndef VERTEX_COLOR_EFFECT_H

#include "Core/Types.h"
#include "DefaultVertexShader.h"

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

    typedef DefaultVertexShader<Vertex> VertexShader;

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

    VertexShader vertex_shader;
    PixelShader pixel_shader;
};

inline VertexColorEffect::Vertex operator + (const VertexColorEffect::Vertex& v0, const VertexColorEffect::Vertex& v1)
{
    VertexColorEffect::Vertex result = { v0.position + v1.position, v0.color + v1.color };
    return result;
}

inline VertexColorEffect::Vertex operator - (const VertexColorEffect::Vertex& v0, const VertexColorEffect::Vertex& v1)
{
    VertexColorEffect::Vertex result = { v0.position - v1.position, v0.color - v1.color };
    return result;
}

inline VertexColorEffect::Vertex operator * (const VertexColorEffect::Vertex& v, f32 s)
{
    VertexColorEffect::Vertex result = { v.position * s, v.color * s };
    return result;
}

inline VertexColorEffect::Vertex operator / (const VertexColorEffect::Vertex& v, f32 s)
{
    VertexColorEffect::Vertex result = { v.position / s, v.color / s };
    return result;
}

inline VertexColorEffect::Vertex& operator += (VertexColorEffect::Vertex& v0, const VertexColorEffect::Vertex& v1)
{
    v0.position += v1.position;
    v0.color += v1.color;
    return v0;
}

inline VertexColorEffect::Vertex& operator *= (VertexColorEffect::Vertex& v, f32 s)
{
    v.position *= s;
    v.color *= s;
    return v;
}

#define VERTEX_COLOR_EFFECT_H
#endif
