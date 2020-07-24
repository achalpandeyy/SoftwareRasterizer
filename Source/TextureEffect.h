#ifndef TEXTURE_EFFECT_H

#include "Core/Types.h"
#include "Texture.h"
#include "DefaultVertexShader.h"
#include "DefaultGeometryShader.h"

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

        inline void CopyAttributesFrom(const Vertex& src)
        {
            texture_coordinates = src.texture_coordinates;
        }
    };

    typedef DefaultVertexShader<Vertex> VertexShader;
    typedef DefaultGeometryShader<VertexShader::VertexOut> GeometryShader;

    struct PixelShader
    {
        template <typename Input>
        u32 operator () (const Input& in) const
        {
            assert(texture);
            return texture->GetTexel(in.texture_coordinates.x, in.texture_coordinates.y, TEXTURE_WRAP);
        }

        void BindTexture(const char* path)
        {
            texture = std::make_unique<Texture>();
            texture->texels = (u8*)stbi_load(path, &texture->width, &texture->height, &texture->channel_count, 0);
        }

        std::unique_ptr<Texture> texture = NULL;
    };

    VertexShader vertex_shader;
    GeometryShader geometry_shader;
    PixelShader pixel_shader;
};

inline TextureEffect::Vertex operator + (const TextureEffect::Vertex& v0, const TextureEffect::Vertex& v1)
{
    TextureEffect::Vertex result = { v0.position + v1.position, v0.texture_coordinates + v1.texture_coordinates };
    return result;
}

inline TextureEffect::Vertex operator - (const TextureEffect::Vertex& v0, const TextureEffect::Vertex& v1)
{
    TextureEffect::Vertex result = { v0.position - v1.position, v0.texture_coordinates - v1.texture_coordinates };
    return result;
}

inline TextureEffect::Vertex operator * (const TextureEffect::Vertex& v, f32 s)
{
    TextureEffect::Vertex result = { v.position * s, v.texture_coordinates * s };
    return result;
}

inline TextureEffect::Vertex operator / (const TextureEffect::Vertex& v, f32 s)
{
    TextureEffect::Vertex result = { v.position / s, v.texture_coordinates / s };
    return result;
}

inline TextureEffect::Vertex& operator += (TextureEffect::Vertex& v0, const TextureEffect::Vertex& v1)
{
    v0.position += v1.position;
    v0.texture_coordinates += v1.texture_coordinates;
    return v0;
}

inline TextureEffect::Vertex& operator *= (TextureEffect::Vertex& v, f32 s)
{
    v.position *= s;
    v.texture_coordinates *= s;
    return v;
}

#define TEXTURE_EFFECT_H
#endif
