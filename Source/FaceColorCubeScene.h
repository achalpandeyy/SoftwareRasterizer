#ifndef FACE_COLOR_CUBE_SCENE_H

#include "Scene.h"
#include "IndexedTriangleList.h"
#include "Pipeline.h"
#include "FaceColorEffect.h"

struct FaceColorCubeScene : public Scene
{
    typedef Pipeline<FaceColorEffect> Pipeline;
    typedef Pipeline::Vertex Vertex;

    inline u32 PackColor(const glm::vec3& color)
    {
        u8 red = (u8)(color.r * 255);
        u8 green = (u8)(color.g * 255);
        u8 blue = (u8)(color.b * 255);
        u32 result = (red << 16 | green << 8 | blue);

        return result;
    }

    FaceColorCubeScene()
    {
        // Vertex Positions.
        it_list.vertices.resize(24);

        f32 half_side_length = 0.5f;

        // Face 0
        it_list.vertices[0].position = { -half_side_length, -half_side_length, -half_side_length };
        it_list.vertices[0].color = PackColor(glm::vec3(1.f, 0.f, 0.f));

        it_list.vertices[1].position = { -half_side_length, half_side_length, -half_side_length };
        it_list.vertices[1].color = PackColor(glm::vec3(1.f, 0.f, 0.f));

        it_list.vertices[2].position = { -half_side_length, half_side_length, half_side_length };
        it_list.vertices[2].color = PackColor(glm::vec3(1.f, 0.f, 0.f));

        it_list.vertices[3].position = { -half_side_length, -half_side_length, half_side_length };
        it_list.vertices[3].color = PackColor(glm::vec3(1.f, 0.f, 0.f));

        // Face 1
        it_list.vertices[4].position = { -half_side_length, half_side_length, half_side_length };
        it_list.vertices[4].color = PackColor(glm::vec3(0.f, 1.f, 0.f));

        it_list.vertices[5].position = { half_side_length, half_side_length, half_side_length };
        it_list.vertices[5].color = PackColor(glm::vec3(0.f, 1.f, 0.f));

        it_list.vertices[6].position = { half_side_length, -half_side_length, half_side_length };
        it_list.vertices[6].color = PackColor(glm::vec3(0.f, 1.f, 0.f));

        it_list.vertices[7].position = { -half_side_length, -half_side_length, half_side_length };
        it_list.vertices[7].color = PackColor(glm::vec3(0.f, 1.f, 0.f));

        // Face 2
        it_list.vertices[8].position = { half_side_length, half_side_length, half_side_length };
        it_list.vertices[8].color = PackColor(glm::vec3(0.f, 0.f, 1.f));

        it_list.vertices[9].position = { half_side_length, half_side_length, -half_side_length };
        it_list.vertices[9].color = PackColor(glm::vec3(0.f, 0.f, 1.f));

        it_list.vertices[10].position = { half_side_length, -half_side_length, -half_side_length };
        it_list.vertices[10].color = PackColor(glm::vec3(0.f, 0.f, 1.f));

        it_list.vertices[11].position = { half_side_length, -half_side_length, half_side_length };
        it_list.vertices[11].color = PackColor(glm::vec3(0.f, 0.f, 1.f));

        // Face 3
        it_list.vertices[12].position = { half_side_length, half_side_length, -half_side_length };
        it_list.vertices[12].color = PackColor(glm::vec3(1.f, 1.f, 0.f));

        it_list.vertices[13].position = { -half_side_length, half_side_length, -half_side_length };
        it_list.vertices[13].color = PackColor(glm::vec3(1.f, 1.f, 0.f));

        it_list.vertices[14].position = { -half_side_length, -half_side_length, -half_side_length };
        it_list.vertices[14].color = PackColor(glm::vec3(1.f, 1.f, 0.f));

        it_list.vertices[15].position = { half_side_length, -half_side_length, -half_side_length };
        it_list.vertices[15].color = PackColor(glm::vec3(1.f, 1.f, 0.f));

        // Face 4
        it_list.vertices[16].position = { half_side_length, half_side_length, half_side_length };
        it_list.vertices[16].color = PackColor(glm::vec3(0.f, 1.f, 1.f));

        it_list.vertices[17].position = { half_side_length, half_side_length, -half_side_length };
        it_list.vertices[17].color = PackColor(glm::vec3(0.f, 1.f, 1.f));

        it_list.vertices[18].position = { -half_side_length, half_side_length, -half_side_length };
        it_list.vertices[18].color = PackColor(glm::vec3(0.f, 1.f, 1.f));

        it_list.vertices[19].position = { -half_side_length, half_side_length, half_side_length };
        it_list.vertices[19].color = PackColor(glm::vec3(0.f, 1.f, 1.f));

        // Face 5
        it_list.vertices[20].position = { half_side_length, -half_side_length, half_side_length };
        it_list.vertices[20].color = PackColor(glm::vec3(1.f, 0.f, 1.f));

        it_list.vertices[21].position = { -half_side_length, -half_side_length, half_side_length };
        it_list.vertices[21].color = PackColor(glm::vec3(1.f, 0.f, 1.f));

        it_list.vertices[22].position = { -half_side_length, -half_side_length, -half_side_length };
        it_list.vertices[22].color = PackColor(glm::vec3(1.f, 0.f, 1.f));

        it_list.vertices[23].position = { half_side_length, -half_side_length, -half_side_length };
        it_list.vertices[23].color = PackColor(glm::vec3(1.f, 0.f, 1.f));

        // Indices.
        // Assemble your cube in such a way that all the face normals face outside -- this constraint 
        // gives you the winding of the triangles.
        it_list.indices.resize(36);

        // Face 0
        it_list.indices[0] = 0; it_list.indices[1] = 3; it_list.indices[2] = 2;
        it_list.indices[3] = 2; it_list.indices[4] = 1; it_list.indices[5] = 0;

        // Face 1
        it_list.indices[6] = 6; it_list.indices[7] = 5; it_list.indices[8] = 4;
        it_list.indices[9] = 4; it_list.indices[10] = 7; it_list.indices[11] = 6;

        // Face 2
        it_list.indices[12] = 9; it_list.indices[13] = 8; it_list.indices[14] = 11;
        it_list.indices[15] = 11; it_list.indices[16] = 10; it_list.indices[17] = 9;

        // Face 3
        it_list.indices[18] = 13; it_list.indices[19] = 12; it_list.indices[20] = 15;
        it_list.indices[21] = 15; it_list.indices[22] = 14; it_list.indices[23] = 13;

        // Face 4
        it_list.indices[24] = 16; it_list.indices[25] = 17; it_list.indices[26] = 18;
        it_list.indices[27] = 18; it_list.indices[28] = 19; it_list.indices[29] = 16;

        // Face 5
        it_list.indices[30] = 22; it_list.indices[31] = 23; it_list.indices[32] = 20;
        it_list.indices[33] = 20; it_list.indices[34] = 21; it_list.indices[35] = 22;

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

#define FACE_COLOR_CUBE_SCENE_H
#endif
