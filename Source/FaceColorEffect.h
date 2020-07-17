#ifndef FACE_COLOR_EFFECT_H

#include "Core/Types.h"

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

    struct PixelShader
    {
        template <typename Input>
        u32 operator () (const Input& in)
        {
            return in.color;
        }
    };

    PixelShader pixel_shader;
};

typedef FaceColorEffect::Vertex Vertex;

inline Vertex operator + (const Vertex& v0, const Vertex& v1)
{
    Vertex result = { v0.position + v1.position, v0.color };
    return result;
}

inline Vertex operator - (const Vertex& v0, const Vertex& v1)
{
    Vertex result = { v0.position - v1.position, v0.color };
    return result;
}

inline Vertex operator * (const Vertex& v, f32 s)
{
    Vertex result = { v.position * s, v.color };
    return result;
}

inline Vertex operator / (const Vertex& v, f32 s)
{
    Vertex result = { v.position / s, v.color };
    return result;
}

inline Vertex& operator += (Vertex& v0, const Vertex& v1)
{
    v0.position += v1.position;
    return v0;
}

#define FACE_COLOR_EFFECT_H
#endif
