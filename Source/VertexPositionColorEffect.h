#ifndef VERTEX_POSITION_COLOR_EFFECT_H

#include "Core/Types.h"
#include "DefaultGeometryShader.h"

#include <glm/glm.hpp>

struct VertexPositionColorEffect
{
    struct Vertex
    {
        glm::vec3 position;

        inline void CopyAttributesFrom(const Vertex& src) {}
    };

    struct VertexShader
    {
        struct VertexOut
        {
            glm::vec3 position;
            glm::vec3 color;
        };

        VertexOut operator () (const Vertex& v)
        {
            VertexOut result;
            result.position = glm::vec3(model * glm::vec4(v.position, 1.f));
            result.color = glm::abs(result.position);
            return result;
        }

        glm::mat4 model;
    };

    typedef DefaultGeometryShader<VertexShader::VertexOut> GeometryShader;

    struct PixelShader
    {
        template <typename Input>
        u32 operator () (const Input& in) const
        {
            u8 red = (u8)(in.color.r * 255);
            u8 green = (u8)(in.color.g * 255);
            u8 blue = (u8)(in.color.b * 255);
            u32 result = (red << 16 | green << 8 | blue);

            return result;
        }
    };

    VertexShader vertex_shader;
    GeometryShader geometry_shader;
    PixelShader pixel_shader;
};

inline VertexPositionColorEffect::Vertex operator + (const VertexPositionColorEffect::Vertex& v0, const VertexPositionColorEffect::Vertex& v1)
{
    VertexPositionColorEffect::Vertex result = { v0.position + v1.position };
    return result;
}

inline VertexPositionColorEffect::Vertex operator - (const VertexPositionColorEffect::Vertex& v0, const VertexPositionColorEffect::Vertex& v1)
{
    VertexPositionColorEffect::Vertex result = { v0.position - v1.position };
    return result;
}

inline VertexPositionColorEffect::Vertex operator * (const VertexPositionColorEffect::Vertex& v, f32 s)
{
    VertexPositionColorEffect::Vertex result = { v.position * s };
    return result;
}

inline VertexPositionColorEffect::Vertex operator / (const VertexPositionColorEffect::Vertex& v, f32 s)
{
    VertexPositionColorEffect::Vertex result = { v.position / s };
    return result;
}

inline VertexPositionColorEffect::Vertex& operator += (VertexPositionColorEffect::Vertex& v0, const VertexPositionColorEffect::Vertex& v1)
{
    v0.position += v1.position;
    return v0;
}

inline VertexPositionColorEffect::Vertex& operator *= (VertexPositionColorEffect::Vertex& v, f32 s)
{
    v.position *= s;
    return v;
}

typedef VertexPositionColorEffect::VertexShader::VertexOut VertexOut;

inline VertexOut operator + (const VertexOut& v0, const VertexOut& v1)
{
    VertexOut result = { v0.position + v1.position, v0.color + v1.color };
    return result;
}

inline VertexOut operator - (const VertexOut& v0, const VertexOut& v1)
{
    VertexOut result = { v0.position - v1.position, v0.color - v1.color };
    return result;
}

inline VertexOut operator * (const VertexOut& v, f32 s)
{
    VertexOut result = { v.position * s, v.color * s };
    return result;
}

inline VertexOut operator / (const VertexOut& v, f32 s)
{
    VertexOut result = { v.position / s, v.color / s };
    return result;
}

inline VertexOut& operator += (VertexOut& v0, const VertexOut& v1)
{
    v0.position += v1.position;
    v0.color += v1.color;
    return v0;
}

inline VertexOut& operator *= (VertexOut& v, f32 s)
{
    v.position *= s;
    v.color *= s;
    return v;
}

#define VERTEX_POSITION_COLOR_EFFECT_H
#endif
