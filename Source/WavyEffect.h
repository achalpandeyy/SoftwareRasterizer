#ifndef WAVY_EFFECT_H

#include "Core/Types.h"
#include "DefaultVertexShader.h"
#include "Texture.h"

#include <glm/glm.hpp>
#include <cassert>
#include <memory>

#define TEXTURE_WRAP 1

struct WavyEffect
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

    struct VertexShader
    {
        typedef Vertex VertexOut;

        VertexOut operator () (const Vertex& v)
        {
            VertexOut result;
            result.position = glm::vec3(model * glm::vec4(v.position, 1.f));
            result.position.y += amplitude * glm::sin(time * scroll_frequency + result.position.x * wave_frequency);
            result.texture_coordinates = v.texture_coordinates;
            return result;
        }

        f32 time = 0.f;
        f32 wave_frequency = 10.f;
        f32 scroll_frequency = 5.f;
        f32 amplitude = 0.05f;
        glm::mat4 model;
    };

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
    PixelShader pixel_shader;
};

inline WavyEffect::Vertex operator + (const WavyEffect::Vertex& v0, const WavyEffect::Vertex& v1)
{
    WavyEffect::Vertex result = { v0.position + v1.position, v0.texture_coordinates + v1.texture_coordinates };
    return result;
}

inline WavyEffect::Vertex operator - (const WavyEffect::Vertex& v0, const WavyEffect::Vertex& v1)
{
    WavyEffect::Vertex result = { v0.position - v1.position, v0.texture_coordinates - v1.texture_coordinates };
    return result;
}

inline WavyEffect::Vertex operator * (const WavyEffect::Vertex& v, f32 s)
{
    WavyEffect::Vertex result = { v.position * s, v.texture_coordinates * s };
    return result;
}

inline WavyEffect::Vertex operator / (const WavyEffect::Vertex& v, f32 s)
{
    WavyEffect::Vertex result = { v.position / s, v.texture_coordinates / s };
    return result;
}

inline WavyEffect::Vertex& operator += (WavyEffect::Vertex& v0, const WavyEffect::Vertex& v1)
{
    v0.position += v1.position;
    v0.texture_coordinates += v1.texture_coordinates;
    return v0;
}

inline WavyEffect::Vertex& operator *= (WavyEffect::Vertex& v, f32 s)
{
    v.position *= s;
    v.texture_coordinates *= s;
    return v;
}

#define WAVY_EFFECT_H
#endif
