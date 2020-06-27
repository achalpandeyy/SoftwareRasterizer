#include "Engine.h"

#include "glm/glm/gtc/matrix_transform.hpp"

// TODO(achal): Swap macro and ceil function.
#include <cmath>
#include <utility>

/*
    TODO(achal): All of the drawing functions below take their coordinates in raster space and as a result
    should take integral vectors. They represent pixels after all! I currently don't want to track all the
    floating-point math and see that it is safely converted to ints. So I'm currently making them all floats.
    I will see to it later.

    DrawLine should be integer bresenham.
*/

void DrawLine(glm::vec2& v0, glm::vec2& v1, Framebuffer& framebuffer, u32 color)
{
    b32 steep = false;
    if (std::abs(v0.x - v1.x) < std::abs(v0.y - v1.y))
    {
        steep = true;
        std::swap(v0.x, v0.y);
        std::swap(v1.x, v1.y);
    }

    // NOTE(achal): The following swapping of coordinates (x_0, y_0) by (x_1, y_1) doesn't mean that
    // dy will always be positive! dx will always be positive though.

    if (v0.x > v1.x)
    {
        std::swap(v0.x, v1.x);
        std::swap(v0.y, v1.y);
    }

    f32 dx = v1.x - v0.x;
    f32 dy = v1.y - v0.y;

    f32 derror = 2 * std::abs(dy);
    f32 error = 0;

    f32 y = v0.y;
    for (f32 x = v0.x; x <= v1.x; ++x)
    {
        u32* pixel;
        if (steep)
            pixel = framebuffer.GetPixelPointer((int)y, (int)x);
        else
            pixel = framebuffer.GetPixelPointer((int)x, (int)y);

        *pixel = color;

        error += derror;
        if (error > dx)
        {
            y += v1.y > v0.y ? 1 : -1;
            error -= 2 * dx;
        }
    }
}

// NOTE(achal): I'm using Direct3D's Coordinate System and Rasterization Rules.
// The pixel (0, 0) includes the _area_ [0, 1) x [0, 1). "Center" of the pixel (0, 0) is
// at the top-left corner.
// Reference: https://docs.microsoft.com/en-us/windows/win32/direct3d10/d3d10-graphics-programming-guide-resources-coordinates

// NOTE(achal): Vertex Order Assumption:
//
//        v0
//        /\
//       /  \
//      /    \
//   v1 ------ v2
//
void DrawFlatBottomTriangle(const glm::vec2& v0, const glm::vec2& v1, const glm::vec2& v2, Framebuffer& framebuffer, u32 color)
{
    f32 rcp_slope_1_0 = (f32)(v1.x - v0.x) / (f32)(v1.y - v0.y);
    f32 rcp_slope_2_0 = (f32)(v2.x - v0.x) / (f32)(v2.y - v0.y);

    int pixel_y_start = (int)std::ceilf(v0.y - 0.5f);
    int pixel_y_end = (int)std::ceilf(v1.y - 0.5f);

    for (int pixel_y = pixel_y_start; pixel_y < pixel_y_end; ++pixel_y)
    {
        f32 point_x1 = rcp_slope_1_0 * (f32(pixel_y) + 0.5f - v1.y) + v1.x;
        f32 point_x2 = rcp_slope_2_0 * (f32(pixel_y) + 0.5f - v2.y) + v2.x;

        int pixel_x_start = (int)std::ceilf(point_x1 - 0.5f);
        int pixel_x_end = (int)std::ceilf(point_x2 - 0.5f);

        for (int pixel_x = pixel_x_start; pixel_x < pixel_x_end; ++pixel_x)
        {
            u32* pixel = framebuffer.GetPixelPointer(pixel_x, pixel_y);
            *pixel = color;
        }
    }
}

// NOTE(achal): Vertex Order Assumption:
//
//   v0 ------ v1
//      \    /
//       \  /
//        \/
//        v2
//
void DrawFlatTopTriangle(const glm::vec2& v0, const glm::vec2& v1, const glm::vec2& v2, Framebuffer& framebuffer, u32 color)
{
    // NOTE(achal): Here we calculate the reciprocal of the slope of the lines because, we _could_ have a 
    // line with infinte slope which would throw a divison by zero error.

    f32 rcp_slope_2_0 = (f32)(v2.x - v0.x) / (f32)(v2.y - v0.y);
    f32 rcp_slope_2_1 = (f32)(v2.x - v1.x) / (f32)(v2.y - v1.y);

    int pixel_y_start = (int)std::ceilf(v0.y - 0.5f);
    int pixel_y_end = (int)std::ceilf(v2.y - 0.5f);

    for (int pixel_y = pixel_y_start; pixel_y < pixel_y_end; ++pixel_y)
    {
        f32 point_x0 = rcp_slope_2_0 * (f32(pixel_y) + 0.5f - v0.y) + v0.x;
        f32 point_x1 = rcp_slope_2_1 * (f32(pixel_y) + 0.5f - v1.y) + v1.x;

        int pixel_x_start = (int)std::ceilf(point_x0 - 0.5f);
        int pixel_x_end = (int)std::ceilf(point_x1 - 0.5f);

        for (int pixel_x = pixel_x_start; pixel_x < pixel_x_end; ++pixel_x)
        {
            u32* pixel = framebuffer.GetPixelPointer(pixel_x, pixel_y);
            *pixel = color;
        }
    }
}

// TODO(achal): DrawFlatTopTriangle & DrawFlatBottomTriangle seems suspiciously similar, collapse 'em into one??
// NOTE(achal): This takes shit is raster space.
void DrawTriangle(const glm::vec2& v0, const glm::vec2& v1, const glm::vec2& v2, Framebuffer& framebuffer, u32 color)
{
    const glm::vec2* vertex_0 = &v0;
    const glm::vec2* vertex_1 = &v1;
    const glm::vec2* vertex_2 = &v2;

    // NOTE(achal): Sort the vertices so that v0 will be at the top (lowest y) and v2 will be at the bottom (highest y).

    if (vertex_0->y > vertex_1->y) std::swap(vertex_0, vertex_1);
    if (vertex_1->y > vertex_2->y) std::swap(vertex_1, vertex_2);
    if (vertex_0->y > vertex_1->y) std::swap(vertex_0, vertex_1);

    if (vertex_0->y == vertex_1->y)
    {
        // NOTE(achal): Sort the vertices so that v0 is left of v1
        if (vertex_0->x > vertex_1->x)
            std::swap(vertex_0, vertex_1);

        DrawFlatTopTriangle(*vertex_0, *vertex_1, *vertex_2, framebuffer, color);
    }
    else if (vertex_1->y == vertex_2->y)
    {
        // NOTE(achal): Sort the vertices so that v1 is left of v2
        if (vertex_1->x > vertex_2->x)
            std::swap(vertex_1, vertex_2);

        DrawFlatBottomTriangle(*vertex_0, *vertex_1, *vertex_2, framebuffer, color);
    }
    else
    {
        f32 alpha = (f32)(vertex_1->y - vertex_0->y) / (f32)(vertex_2->y - vertex_0->y);
        glm::vec2 split_vertex = *vertex_0 + (*vertex_2 - *vertex_0) * alpha;

        if (split_vertex.x > vertex_1->x)
        {
            // Major Right
            DrawFlatBottomTriangle(*vertex_0, *vertex_1, split_vertex, framebuffer, color);
            DrawFlatTopTriangle(*vertex_1, split_vertex, *vertex_2, framebuffer, color);
        }
        else
        {
            // Major Left
            DrawFlatBottomTriangle(*vertex_0, split_vertex, *vertex_1, framebuffer, color);
            DrawFlatTopTriangle(split_vertex, *vertex_1, *vertex_2, framebuffer, color);
        }
    }
}

void Engine::Initialize()
{
    f32 half_side_length = 0.5f;

    cube.vertices =
    {
        { -half_side_length, -half_side_length, -half_side_length },
        { half_side_length, -half_side_length, -half_side_length },
        { half_side_length, -half_side_length, half_side_length },
        { -half_side_length, -half_side_length, half_side_length },

        { -half_side_length, half_side_length, half_side_length },
        { -half_side_length, half_side_length, -half_side_length },
        { half_side_length, half_side_length, -half_side_length },
        { half_side_length, half_side_length, half_side_length },
    };

#if 0
    cube.indices = 
    {
        3, 4, 2,
        4, 7, 2,
        4, 5, 7,
        5, 6, 7,
        6, 1, 5,
        1, 0, 5,
        4, 3, 0,
        0, 5, 4,
        7, 6, 1,
        1, 2, 7
    };
#endif
    cube.indices = 
    {
        0, 1,
        1, 2,
        2, 3,
        3, 0,

        3, 4,
        4, 7,
        7, 2,

        5, 4,
        5, 6,
        6, 7,

        6, 1,
        5, 0
    };
}

void Engine::SetupFramebuffer(int width, int height, int bytes_per_pixel, int pitch, void* pixels)
{
    framebuffer.width = width;
    framebuffer.height = height;
    framebuffer.bytes_per_pixel = bytes_per_pixel;
    framebuffer.pitch = pitch;
    framebuffer.pixels = pixels;
}

// Wraps the given angle in the range -PI to PI
inline f32 WrapAngle(f32 angle)
{
    f32 modded_angle = fmod(angle, 2.f * PI32);
    return modded_angle > PI32 ? modded_angle - 2.f * PI32 : modded_angle;
}

void Engine::UpdateAndRender()
{
    // TODO(achal): There could be a bug in how I handle the input linked with the rotation.
    // Chilli seemingly had all the same params but his rotation speed was much slower. I think,
    // this could be due to the way I take key input or just that I render many more frames than
    // him, in the duration of the key being pressed.
    // I'm not really sure at this moment so I'm leaving it for now.

    f32 angular_speed = PI32 / 10.f;
    f32 dt = 1.f / 60.f;
    f32 dtheta = angular_speed * dt;
    
    if (buttons[0].pressed)
    {
        theta_x = WrapAngle(theta_x + dtheta);
    }
    if (buttons[1].pressed)
    {
        theta_y = WrapAngle(theta_y + dtheta);
    }
    if (buttons[2].pressed)
    {
        theta_z = WrapAngle(theta_z + dtheta);
    }

    glm::mat4 rotate = glm::rotate(glm::mat4(1.f), theta_x, glm::vec3(1.f, 0.f, 0.f));
    rotate = glm::rotate(rotate, theta_y, glm::vec3(0.f, 1.f, 0.f));
    rotate = glm::rotate(rotate, theta_z, glm::vec3(0.f, 0.f, 1.f));

    f32 half_width = (f32)framebuffer.width / 2.f;
    f32 half_height = (f32)framebuffer.height / 2.f;

    // Draw the damn lined cube
    for (u32 i = 0; i < cube.indices.size() / 2; ++i)
    {
        size_t idx0 = cube.indices[2 * i];
        size_t idx1 = cube.indices[2 * i + 1];

        glm::vec3 v0 = cube.vertices[idx0];
        v0 = glm::vec3(rotate * glm::vec4(v0, 1.f));
        v0 += glm::vec3(0.f, 0.f, 10.f);

        v0.x = (v0.x + 1.f) * half_width;
        v0.y = (-v0.y + 1.f) * half_height;

        glm::vec3 v1 = cube.vertices[idx1];
        v1 = glm::vec3(rotate * glm::vec4(v1, 1.f));;
        v1 += glm::vec3(0.f, 0.f, 10.f);

        v1.x = (v1.x + 1.f) * half_width;
        v1.y = (-v1.y + 1.f) * half_height;

        // TODO(achal): assert that raster_v0 and raster_v1 are always positive.
        // NOTE(achal): Raster Space: [0, width] x [0, height]
        // TODO(achal): Can I put it into a matrix transfrom and probably combine it with other transforms, like
        // from World space to NDC, for example??
        glm::vec2 v0_2dim(v0.x, v0.y);
        glm::vec2 v1_2dim(v1.x, v1.y);
        DrawLine(v0_2dim, v1_2dim, framebuffer, 0xFFFFFFFF);
    }

    #if 0
    // Draw the damn cube
    for (u32 i = 0; i < cube.index_count / 3; ++i)
    {
        size_t idx0 = cube.indices[3 * i];
        size_t idx1 = cube.indices[3 * i + 1];
        size_t idx2 = cube.indices[3 * i + 2];

        Vector3f v0 = cube.vertices[idx0];
        Vector3f v1 = cube.vertices[idx1];
        Vector3f v2 = cube.vertices[idx2];

        Vector2f raster_v0 = ToRasterSpace(v0, half_width, half_height);
        Vector2f raster_v1 = ToRasterSpace(v1, half_width, half_height);
        Vector2f raster_v2 = ToRasterSpace(v2, half_width, half_height);

        DrawTriangle(raster_v0, raster_v1, raster_v2, framebuffer, 0xFFFFFFFF);
    }
    #endif
}