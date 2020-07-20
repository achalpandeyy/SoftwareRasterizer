#ifndef WAVY_PLANE_SCENE_H

#include "Core/Types.h"
#include "Scene.h"
#include "IndexedTriangleList.h"
#include "Pipeline.h"
#include "TextureEffect.h"
#include "WavyEffect.h"
#include "Framebuffer.h"
#include "ZBuffer.h"

struct WavyPlaneScene : public Scene
{
    typedef Pipeline<WavyEffect> Pipeline;
    typedef Pipeline::Vertex Vertex;

    WavyPlaneScene()
    {
        size_t divs_x = 20;
        size_t divs_y = 20;

        size_t vertex_count_x = divs_x + 1;
        size_t vertex_count_y = divs_y + 1;

        size_t total_vertex_count = vertex_count_x * vertex_count_y;
        it_list.vertices.resize(total_vertex_count);

        f32 half_side_length = 0.5f;
        f32 dx = (2.f * half_side_length) / (f32)divs_x;
        f32 dy = (2.f * half_side_length) / (f32)divs_y;

        glm::vec3 top_left_pos(-half_side_length, half_side_length, 0.f);
        glm::vec2 top_left_tex(0.f, 0.f);

        for (size_t yi = 0; yi < vertex_count_y; ++yi)
        {
            for (size_t xi = 0; xi < vertex_count_x; ++xi)
            {
                it_list.vertices[yi * vertex_count_x + xi].position = { top_left_pos.x + xi * dx, top_left_pos.y - (yi * dy), 0.f };
                it_list.vertices[yi * vertex_count_x + xi].texture_coordinates = { top_left_tex.x + (xi * dx), top_left_tex.y + (yi * dy)};
            }
        }

        it_list.indices.resize(divs_x * divs_y * 2 * 3);
        size_t i = 0;
        for (size_t yi = 0; yi < divs_y; ++yi)
        {
            for (size_t xi = 0; xi < divs_x; ++xi)
            {
                assert(i < (divs_x * divs_y * 2 * 3));
                size_t top_left_idx = xi + yi * vertex_count_x;
                it_list.indices[i++] = top_left_idx;
                it_list.indices[i++] = top_left_idx + vertex_count_x;
                it_list.indices[i++] = top_left_idx + vertex_count_x + 1;

                it_list.indices[i++] = top_left_idx + vertex_count_x + 1;
                it_list.indices[i++] = top_left_idx + 1;
                it_list.indices[i++] = top_left_idx;
            }
        }

#if 0
        it_list.vertices.resize(4);

        it_list.vertices[0].position = { -half_side_length, -half_side_length, 0.f };
        it_list.vertices[0].texture_coordinates = { 0.f, 0.f };

        it_list.vertices[1].position = { half_side_length, -half_side_length, 0.f };
        it_list.vertices[1].texture_coordinates = { 1.f, 0.f };

        it_list.vertices[2].position = { half_side_length, half_side_length, 0.f };
        it_list.vertices[2].texture_coordinates = { 1.f, 1.f };

        it_list.vertices[3].position = { -half_side_length, half_side_length, 0.f };
        it_list.vertices[3].texture_coordinates = { 0.f, 1.f };

        it_list.indices.resize(6);
        it_list.indices[0] = 0; it_list.indices[1] = 1; it_list.indices[2] = 3;
        it_list.indices[3] = 1; it_list.indices[4] = 2; it_list.indices[5] = 3;
#endif

        pipeline.effect.pixel_shader.BindTexture("../Resources/karasuno.png");
    }

    void Draw() override
    {
        pipeline.effect.vertex_shader.time = time;
        pipeline.Draw(it_list);
    }

    void SetFramebuffer(Framebuffer* fb) override
    {
        pipeline.framebuffer = fb;
    }

    void SetZBuffer(ZBuffer* zb) override
    {
        pipeline.z_buffer = zb;
    }

    void SetModel(const glm::mat4& model) override
    {
        pipeline.effect.vertex_shader.model = model;
    }

    void SetTime(f32 t) override
    {
        time = t;
    }

    IndexedTriangleList<Vertex> it_list;
    Pipeline pipeline;
    f32 time;
};

#define WAVY_PLANE_SCENE_H
#endif
