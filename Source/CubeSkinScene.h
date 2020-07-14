#ifndef CUBE_SKIN_SCENE_H

#include "Core/Types.h"
#include "IndexedTriangleList.h"
#include "Pipeline.h"
#include "TextureEffect.h"

#include <glm/glm.hpp>

// NOTE(achal): Triangle Winding Assumption: Anticlock-wise
//
//        v0
//        /\
//       /  \
//      /    \
//   v1 ------ v2
//
// Normal of the triangle is given by: (v1 - v0) x (v2 - v0)
// Cull the triangle when face normal points in the same direction as view vector (any_point_on_the_triangle - focal_point).

struct CubeSkinScene
{
    typedef Pipeline<TextureEffect> Pipeline;
    typedef Pipeline::Vertex Vertex;

    CubeSkinScene()
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
        it_list.indices.resize(14);

        it_list.vertices[0].texture_coordinates = { 2.f / 3.f, 0.f / 4.f };
        it_list.vertices[1].texture_coordinates = { 2.f / 3.f, 3.f / 4.f };
        it_list.vertices[2].texture_coordinates = { 2.f / 3.f, 2.f / 4.f };
        it_list.vertices[3].texture_coordinates = { 2.f / 3.f, 1.f / 4.f };

        it_list.vertices[4].texture_coordinates = { 1.f / 3.f, 1.f / 4.f };
        it_list.vertices[5].texture_coordinates = { 1.f / 3.f, 0.f / 4.f };
        it_list.vertices[6].texture_coordinates = { 1.f / 3.f, 3.f / 4.f };
        it_list.vertices[7].texture_coordinates = { 1.f / 3.f, 2.f / 4.f };

        it_list.vertices[8].texture_coordinates = { 0.f / 3.f, 1.f / 4.f };  // 5"
        it_list.vertices[9].texture_coordinates = { 0.f / 3.f, 2.f / 4.f };  // 6'
        it_list.vertices[10].texture_coordinates = { 1.f / 3.f, 4.f / 4.f }; // 5'
        it_list.vertices[11].texture_coordinates = { 2.f / 3.f, 4.f / 4.f }; // 0'
        it_list.vertices[12].texture_coordinates = { 3.f / 3.f, 1.f / 4.f }; // 0"
        it_list.vertices[13].texture_coordinates = { 3.f / 3.f, 2.f / 4.f }; // 1'

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

        pipeline.effect.pixel_shader.BindTexture("../Resources/dice_skin.png");

// TODO(achal): Put this into its own Scene.
#if 0
        // For sauron.png
        glm::vec2 texture_coordinates[14] =
        {
            { 0.f, 2.f },   // 0
            { 3.f, 2.f },   // 1
            { 2.f, 2.f },   // 2
            { 1.f, 2.f },   // 3

            { 1.f, 1.f },   // 4
            { 0.f, 1.f },   // 5
            { 3.f, 1.f },   // 6
            { 2.f, 1.f },   // 7

            { 1.f, 0.f },   // 8 (5")
            { 2.f, 0.f },   // 9 (6')

            { 4.f, 1.f },   // 10 (5')
            { 4.f, 2.f },   // 11 (0')

            { 1.f, 3.f },   // 12 (0")
            { 2.f, 3.f }    // 13 (1')
        };

        // NOTE(achal): Freeing texture memory is not necessary for now.
        texture.texels = (u8*)stbi_load("../sauron.png", &texture.width, &texture.height, &texture.channel_count, 0);
#endif
    }

    IndexedTriangleList<Vertex> it_list;
    Pipeline pipeline;
    f32 theta_x = 0.f;
    f32 theta_y = 0.f;
    f32 theta_z = 0.f;
};

#define CUBE_SKIN_SCENE_H
#endif
