#ifndef CUBE_SCENE_H

#include "Scene.h"
#include "IndexedTriangleList.h"
#include "Pipeline.h"
#include "TextureEffect.h"

struct CubeScene : public Scene
{
    typedef Pipeline<TextureEffect> Pipeline;
    typedef Pipeline::Vertex Vertex;

    CubeScene()
    {
        // Vertex Positions.
        it_list.vertices.resize(14);

        f32 half_side_length = 0.5f;

        it_list.vertices[0].position = { -half_side_length, -half_side_length, -half_side_length };
        it_list.vertices[1].position = { half_side_length, -half_side_length, -half_side_length };
        it_list.vertices[2].position = { half_side_length, -half_side_length, half_side_length };
        it_list.vertices[3].position = { -half_side_length, -half_side_length, half_side_length };

        it_list.vertices[4].position = { -half_side_length, half_side_length, half_side_length };
        it_list.vertices[5].position = { -half_side_length, half_side_length, -half_side_length };
        it_list.vertices[6].position = { half_side_length, half_side_length, -half_side_length };
        it_list.vertices[7].position = { half_side_length, half_side_length, half_side_length };

        it_list.vertices[8].position = { -half_side_length, half_side_length, -half_side_length };   // 5"
        it_list.vertices[9].position = { half_side_length, half_side_length, -half_side_length };    // 6'
        it_list.vertices[10].position = { -half_side_length, half_side_length, -half_side_length };  // 5'
        it_list.vertices[11].position = { -half_side_length, -half_side_length, -half_side_length }; // 0'
        it_list.vertices[12].position = { -half_side_length, -half_side_length, -half_side_length }; // 0"
        it_list.vertices[13].position = { half_side_length, -half_side_length, -half_side_length };  // 1'

        // Texture Coordinates (for dice_skin.png)
        it_list.vertices[0].texture_coordinates = { 2.f, 0.f };
        it_list.vertices[1].texture_coordinates = { 2.f, 3.f };
        it_list.vertices[2].texture_coordinates = { 2.f, 2.f };
        it_list.vertices[3].texture_coordinates = { 2.f, 1.f };

        it_list.vertices[4].texture_coordinates = { 1.f, 1.f };
        it_list.vertices[5].texture_coordinates = { 1.f, 0.f };
        it_list.vertices[6].texture_coordinates = { 1.f, 3.f };
        it_list.vertices[7].texture_coordinates = { 1.f, 2.f };

        it_list.vertices[8].texture_coordinates = { 0.f, 1.f };  // 5"
        it_list.vertices[9].texture_coordinates = { 0.f, 2.f };  // 6'
        it_list.vertices[10].texture_coordinates = { 1.f, 4.f }; // 5'
        it_list.vertices[11].texture_coordinates = { 2.f, 4.f }; // 0'
        it_list.vertices[12].texture_coordinates = { 3.f, 1.f }; // 0"
        it_list.vertices[13].texture_coordinates = { 3.f, 2.f }; // 1'

        // Indices.
        // Assemble your cube in such a way that all the face normals face outside -- this constraint 
        // gives you the winding of the triangles.
        it_list.indices.resize(36);

        it_list.indices[0] = 3; it_list.indices[1] = 5; it_list.indices[2] = 0;
        it_list.indices[3] = 3; it_list.indices[4] = 4; it_list.indices[5] = 5;

        it_list.indices[6] = 2; it_list.indices[7] = 4; it_list.indices[8] = 3;
        it_list.indices[9] = 2; it_list.indices[10] = 7; it_list.indices[11] = 4;

        it_list.indices[12] = 1; it_list.indices[13] = 7; it_list.indices[14] = 2;
        it_list.indices[15] = 1; it_list.indices[16] = 6; it_list.indices[17] = 7;

        it_list.indices[18] = 1; it_list.indices[19] = 10; it_list.indices[20] = 6;
        it_list.indices[21] = 1; it_list.indices[22] = 11; it_list.indices[23] = 10;

        it_list.indices[24] = 7; it_list.indices[25] = 8; it_list.indices[26] = 4;
        it_list.indices[27] = 7; it_list.indices[28] = 9; it_list.indices[29] = 8;

        it_list.indices[30] = 3; it_list.indices[31] = 12; it_list.indices[32] = 2;
        it_list.indices[33] = 12; it_list.indices[34] = 13; it_list.indices[35] = 2;

        pipeline.effect.pixel_shader.BindTexture("../Resources/sauron.png");
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

#define CUBE_SCENE_H
#endif
