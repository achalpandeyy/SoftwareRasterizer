#ifndef CUBE_VERTEX_POSITION_COLOR_SCENE_H

#include "Scene.h"
#include "Pipeline.h"
#include "IndexedTriangleList.h"
#include "VertexPositionColorEffect.h"

#include <glm/glm.hpp>

struct CubeVertexPositionColorScene : public Scene
{
    typedef Pipeline<VertexPositionColorEffect> Pipeline;
    typedef Pipeline::Vertex Vertex;

    CubeVertexPositionColorScene()
    {
        it_list.vertices.resize(8);
        f32 half_side_length = 0.5f;

        it_list.vertices[0].position = { -half_side_length, -half_side_length, -half_side_length };
        it_list.vertices[1].position = { half_side_length, -half_side_length, -half_side_length };
        it_list.vertices[2].position = { -half_side_length, half_side_length, -half_side_length };
        it_list.vertices[3].position = { half_side_length, half_side_length, -half_side_length };
        it_list.vertices[4].position = { -half_side_length, -half_side_length, half_side_length };
        it_list.vertices[5].position = { half_side_length, -half_side_length, half_side_length };
        it_list.vertices[6].position = { -half_side_length, half_side_length, half_side_length };
        it_list.vertices[7].position = { half_side_length, half_side_length, half_side_length };

        it_list.indices.resize(36);

        size_t i = 0;
        it_list.indices[i++] = 0; it_list.indices[i++] = 2; it_list.indices[i++] = 1;
        it_list.indices[i++] = 2, it_list.indices[i++] = 3; it_list.indices[i++] = 1;
        
        it_list.indices[i++] = 1; it_list.indices[i++] = 3; it_list.indices[i++] = 5;
        it_list.indices[i++] = 3; it_list.indices[i++] = 7; it_list.indices[i++] = 5;

        it_list.indices[i++] = 2; it_list.indices[i++] = 6; it_list.indices[i++] = 3;
        it_list.indices[i++] = 3; it_list.indices[i++] = 6; it_list.indices[i++] = 7;

        it_list.indices[i++] = 4; it_list.indices[i++] = 5; it_list.indices[i++] = 7;
        it_list.indices[i++] = 4; it_list.indices[i++] = 7; it_list.indices[i++] = 6;

        it_list.indices[i++] = 0; it_list.indices[i++] = 4; it_list.indices[i++] = 2;
        it_list.indices[i++] = 2; it_list.indices[i++] = 4; it_list.indices[i++] = 6;
        
        it_list.indices[i++] = 0; it_list.indices[i++] = 1; it_list.indices[i++] = 4;
        it_list.indices[i++] = 1; it_list.indices[i++] = 5; it_list.indices[i++] = 4;
    }

    void Draw() override
    {
        pipeline.Draw(it_list);
    }

    void SetFramebuffer(Framebuffer* framebuffer) override
    {
        pipeline.framebuffer = framebuffer;
    }

    void SetZBuffer(ZBuffer* z_buffer) override
    {
        pipeline.z_buffer = z_buffer;
    }

    void SetModel(const glm::mat4& model) override
    {
        pipeline.effect.vertex_shader.model = model;
    }

    IndexedTriangleList<Vertex> it_list;
    Pipeline pipeline;
};

#define CUBE_VERTEX_POSITION_COLOR_SCENE_H
#endif
