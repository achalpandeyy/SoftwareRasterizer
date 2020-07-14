#ifndef TEXTURE_EFFECT_H

#include "Core/Types.h"
#include "Texture.h"

#include <stb_image/stb_image.h>
#include <glm/glm.hpp>
#include <memory>

#define TEXTURE_WRAP 1

struct TextureEffect
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec2 texture_coordinates;
    };

    struct PixelShader
    {
        template <typename Input>
        u32 operator () (const Input& in) const
        {
            assert(texture);
            return texture->GetTexel(in.x, in.y, TEXTURE_WRAP);
        }

        void BindTexture(const char* path)
        {
            texture = std::make_unique<Texture>();
            texture->texels = (u8*)stbi_load(path, &texture->width, &texture->height, &texture->channel_count, 0);
        }

        std::unique_ptr<Texture> texture = NULL;
    };

    PixelShader pixel_shader;
};

typedef TextureEffect::Vertex Vertex;

inline Vertex operator + (const Vertex& v0, const Vertex& v1)
{
    Vertex result = { v0.position + v1.position, v0.texture_coordinates + v1.texture_coordinates };
    return result;
}

inline Vertex operator - (const Vertex& v0, const Vertex& v1)
{
    Vertex result = { v0.position - v1.position, v0.texture_coordinates - v1.texture_coordinates };
    return result;
}

inline Vertex operator * (const Vertex& v, f32 s)
{
    Vertex result = { v.position * s, v.texture_coordinates * s };
    return result;
}

inline Vertex operator / (const Vertex& v, f32 s)
{
    Vertex result = { v.position / s, v.texture_coordinates / s };
    return result;
}

inline Vertex& operator += (Vertex& v0, const Vertex& v1)
{
    v0.position += v1.position;
    v0.texture_coordinates += v1.texture_coordinates;
    return v0;
}

#define TEXTURE_EFFECT_H
#endif