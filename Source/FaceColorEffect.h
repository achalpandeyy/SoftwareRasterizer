#ifndef FACE_COLOR_EFFECT_H

#include "Core/Types.h"
#include "DefaultVertexShader.h"

#include <glm/glm.hpp>

struct FaceColorEffect
{
    struct Vertex
    {
        glm::vec3 position;
        u32 color;

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
            return in.color;
        }
    };

    VertexShader vertex_shader;
    PixelShader pixel_shader;
};

inline FaceColorEffect::Vertex operator + (const FaceColorEffect::Vertex& v0, const FaceColorEffect::Vertex& v1)
{
    FaceColorEffect::Vertex result = { v0.position + v1.position, v0.color };
    return result;
}

inline FaceColorEffect::Vertex operator - (const FaceColorEffect::Vertex& v0, const FaceColorEffect::Vertex& v1)
{
    FaceColorEffect::Vertex result = { v0.position - v1.position, v0.color };
    return result;
}

inline FaceColorEffect::Vertex operator * (const FaceColorEffect::Vertex& v, f32 s)
{
    FaceColorEffect::Vertex result = { v.position * s, v.color };
    return result;
}

inline FaceColorEffect::Vertex operator / (const FaceColorEffect::Vertex& v, f32 s)
{
    FaceColorEffect::Vertex result = { v.position / s, v.color };
    return result;
}

inline FaceColorEffect::Vertex& operator += (FaceColorEffect::Vertex& v0, const FaceColorEffect::Vertex& v1)
{
    v0.position += v1.position;
    return v0;
}

inline FaceColorEffect::Vertex& operator *= (FaceColorEffect::Vertex& v, f32 s)
{
    v.position *= s;
    return v;
}

#define FACE_COLOR_EFFECT_H
#endif
