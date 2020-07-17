#ifndef COLOR_CUBE_SCENE_H

#include "IndexedTriangleList.h"
#include "Pipeline.h"
#include "VertexColorEffect.h"

struct ColorCubeScene
{
    typedef Pipeline<VertexColorEffect> Pipeline;
    typedef Pipeline::Vertex Vertex;

    ColorCubeScene()
    {
        // Vertex Positions.
        it_list.vertices.resize(8);

        f32 half_side_length = 0.5f;

        it_list.vertices[0].position = { -half_side_length, -half_side_length, -half_side_length };
        it_list.vertices[0].color = { 0.f, 0.f, 0.f };

        it_list.vertices[1].position = { half_side_length, -half_side_length, -half_side_length };
        it_list.vertices[1].color = { 1.f, 0.f, 0.f };

        it_list.vertices[2].position = { half_side_length, -half_side_length, half_side_length };
        it_list.vertices[2].color = { 1.f, 0.f, 1.f };

        it_list.vertices[3].position = { -half_side_length, -half_side_length, half_side_length };
        it_list.vertices[3].color = { 0.f, 0.f, 1.f };

        it_list.vertices[4].position = { -half_side_length, half_side_length, half_side_length };
        it_list.vertices[4].color = { 0.f, 1.f, 1.f };

        it_list.vertices[5].position = { -half_side_length, half_side_length, -half_side_length };
        it_list.vertices[5].color = { 0.f, 1.f, 0.f };

        it_list.vertices[6].position = { half_side_length, half_side_length, -half_side_length };
        it_list.vertices[6].color = { 1.f, 1.f, 0.f };

        it_list.vertices[7].position = { half_side_length, half_side_length, half_side_length };
        it_list.vertices[7].color = { 1.f, 1.f, 1.f };

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

        it_list.indices[18] = 1; it_list.indices[19] = 5; it_list.indices[20] = 6;
        it_list.indices[21] = 1; it_list.indices[22] = 0; it_list.indices[23] = 5;

        it_list.indices[24] = 7; it_list.indices[25] = 5; it_list.indices[26] = 4;
        it_list.indices[27] = 7; it_list.indices[28] = 6; it_list.indices[29] = 5;

        it_list.indices[30] = 3; it_list.indices[31] = 0; it_list.indices[32] = 2;
        it_list.indices[33] = 0; it_list.indices[34] = 1; it_list.indices[35] = 2;

    }

    IndexedTriangleList<Vertex> it_list;
    Pipeline pipeline;

    f32 theta_x = 0.f;
    f32 theta_y = 0.f;
    f32 theta_z = 0.f;
};

#define COLOR_CUBE_SCENE_H
#endif
