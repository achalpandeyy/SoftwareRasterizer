#include "Engine.h"

#define TEXTURE_WRAP 1

#include "glm/glm/gtc/matrix_transform.hpp"
#include "stb_image/stb_image.h"

// TODO(achal): Swap macro and ceil function.
#include <cmath>
#include <utility>

/*
    DrawLine should be integer bresenham. (I think!)

    TODO(achal): All of the triangle drawing routines below can do with 2D positions, but still I'm passing
    3D position because they have texture coordinates associated with them in the struct "Vertex". So essentially,
    the z-component is wasted space.
    Workaround for this?
*/

/*
    NOTE(achal): I'm using Direct3D's Coordinate System and Rasterization Rules.
    The pixel (0, 0) includes the _area_ [0, 1) x [0, 1). (0, 0)th pixel is the one
    at the top-left corner.
    Reference: https://docs.microsoft.com/en-us/windows/win32/direct3d10/d3d10-graphics-programming-guide-resources-coordinates

    All of the following drawing functions take input in raster space, but that doesn't mean that
    they should have integral coordinates. We use the aforementioned rasterization rules to get
    the exact (integral) pixel coordinates on the fly.
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

// TODO(achal): Maybe remove the following solid color triangle drawing routines in favour of a single
// triangle routine which is smart enough to know that when to use a (supplied) solid color and when
// to do a texture lookup??
#if 0
// NOTE(achal): Vertex Order Assumption:
//
//        v0
//        /\
//       /  \
//      /    \
//   v1 ------ v2
//
void DrawFlatBottomTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, Framebuffer& framebuffer, u32 color)
{
    f32 rcp_slope_1_0 = (f32)(v1.position.x - v0.position.x) / (f32)(v1.position.y - v0.position.y);
    f32 rcp_slope_2_0 = (f32)(v2.position.x - v0.position.x) / (f32)(v2.position.y - v0.position.y);

    int pixel_y_start = (int)std::ceilf(v0.position.y - 0.5f);
    int pixel_y_end = (int)std::ceilf(v1.position.y - 0.5f);

    for (int pixel_y = pixel_y_start; pixel_y < pixel_y_end; ++pixel_y)
    {
        f32 point_x1 = rcp_slope_1_0 * (f32(pixel_y) + 0.5f - v1.position.y) + v1.position.x;
        f32 point_x2 = rcp_slope_2_0 * (f32(pixel_y) + 0.5f - v2.position.y) + v2.position.x;

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
void DrawFlatTopTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, Framebuffer& framebuffer, u32 color)
{
    // NOTE(achal): Here we calculate the reciprocal of the slope of the lines because, we _could_ have a 
    // line with infinte slope which would throw a divison by zero error.

    f32 rcp_slope_2_0 = (f32)(v2.position.x - v0.position.x) / (f32)(v2.position.y - v0.position.y);
    f32 rcp_slope_2_1 = (f32)(v2.position.x - v1.position.x) / (f32)(v2.position.y - v1.position.y);

    int pixel_y_start = (int)std::ceilf(v0.position.y - 0.5f);
    int pixel_y_end = (int)std::ceilf(v2.position.y - 0.5f);

    for (int pixel_y = pixel_y_start; pixel_y < pixel_y_end; ++pixel_y)
    {
        f32 point_x0 = rcp_slope_2_0 * (f32(pixel_y) + 0.5f - v0.position.y) + v0.position.x;
        f32 point_x1 = rcp_slope_2_1 * (f32(pixel_y) + 0.5f - v1.position.y) + v1.position.x;

        int pixel_x_start = (int)std::ceilf(point_x0 - 0.5f);
        int pixel_x_end = (int)std::ceilf(point_x1 - 0.5f);

        for (int pixel_x = pixel_x_start; pixel_x < pixel_x_end; ++pixel_x)
        {
            u32* pixel = framebuffer.GetPixelPointer(pixel_x, pixel_y);
            *pixel = color;
        }
    }
}

// NOTE(achal): This takes shit is raster space.
void DrawTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, Framebuffer& framebuffer, u32 color)
{
    const Vertex* vertex_0 = &v0;
    const Vertex* vertex_1 = &v1;
    const Vertex* vertex_2 = &v2;

    // NOTE(achal): Sort the vertices so that v0 will be at the top (lowest y) and v2 will be at the bottom (highest y).

    if (vertex_0->position.y > vertex_1->position.y) std::swap(vertex_0, vertex_1);
    if (vertex_1->position.y > vertex_2->position.y) std::swap(vertex_1, vertex_2);
    if (vertex_0->position.y > vertex_1->position.y) std::swap(vertex_0, vertex_1);

    if (vertex_0->position.y == vertex_1->position.y)
    {
        // NOTE(achal): Sort the vertices so that v0 is left of v1
        if (vertex_0->position.x > vertex_1->position.x)
            std::swap(vertex_0, vertex_1);

        DrawFlatTopTriangle(*vertex_0, *vertex_1, *vertex_2, framebuffer, color);
    }
    else if (vertex_1->position.y == vertex_2->position.y)
    {
        // NOTE(achal): Sort the vertices so that v1 is left of v2
        if (vertex_1->position.x > vertex_2->position.x)
            std::swap(vertex_1, vertex_2);

        DrawFlatBottomTriangle(*vertex_0, *vertex_1, *vertex_2, framebuffer, color);
    }
    else
    {
        f32 alpha = (f32)(vertex_1->position.y - vertex_0->position.y) / (f32)(vertex_2->position.y - vertex_0->position.y);
        Vertex split_vertex = {};
        split_vertex.position = vertex_0->position + (vertex_2->position - vertex_0->position) * alpha;

        if (split_vertex.position.x > vertex_1->position.x)
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
#endif

// TODO(achal): DrawFlatTopTriangleTex & DrawFlatBottomTriangleTex seems suspiciously similar, collapse 'em into one??
void DrawFlatBottomTriangleTex(const Vertex& v0, const Vertex& v1, const Vertex& v2, Framebuffer& framebuffer, const Texture& texture)
{
    f32 rcp_dy = 1.f / (v2.position.y - v0.position.y);

    Vertex dv10dy = (v1 - v0) * rcp_dy;
    Vertex dv20dy = (v2 - v0) * rcp_dy;

    int pixel_y_start = (int)std::ceilf(v0.position.y - 0.5f);
    int pixel_y_end = (int)std::ceilf(v2.position.y - 0.5f);

    // Add pre-step
    Vertex left_edge_interpolant = v0 + dv10dy * ((f32)pixel_y_start + 0.5f - v0.position.y);
    Vertex right_edge_interpolant = v0 + dv20dy * ((f32)pixel_y_start + 0.5f - v0.position.y);

    for (int pixel_y = pixel_y_start; pixel_y < pixel_y_end; ++pixel_y,
        left_edge_interpolant += dv10dy, right_edge_interpolant += dv20dy)
    {
        int pixel_x_start = (int)std::ceilf(left_edge_interpolant.position.x - 0.5f);
        int pixel_x_end = (int)std::ceilf(right_edge_interpolant.position.x - 0.5f);

        glm::vec2 dtexdx = (right_edge_interpolant.texture_coordinates - left_edge_interpolant.texture_coordinates)
            / (right_edge_interpolant.position.x - left_edge_interpolant.position.x);

        glm::vec2 scan_tex_interpolant = left_edge_interpolant.texture_coordinates + dtexdx
            * ((f32)pixel_x_start + 0.5f - left_edge_interpolant.position.x);

        for (int pixel_x = pixel_x_start; pixel_x < pixel_x_end; ++pixel_x, scan_tex_interpolant += dtexdx)
        {
            u32* pixel = framebuffer.GetPixelPointer(pixel_x, pixel_y);
            u32 texel = texture.GetTexel(scan_tex_interpolant.x, scan_tex_interpolant.y, TEXTURE_WRAP);
            *pixel = texel;
        }
    }
}

void DrawFlatTopTriangleTex(const Vertex& v0, const Vertex& v1, const Vertex& v2, Framebuffer& framebuffer, const Texture& texture)
{
    f32 rcp_dy = 1.f / (v2.position.y - v0.position.y);

    Vertex dv20dy = (v2 - v0) * rcp_dy;
    Vertex dv21dy = (v2 - v1) * rcp_dy;

    int pixel_y_start = (int)std::ceilf(v0.position.y - 0.5f);
    int pixel_y_end = (int)std::ceilf(v2.position.y - 0.5f);

    // Add pre-step
    Vertex left_edge_interpolant = v0 + dv20dy * ((f32)pixel_y_start + 0.5f - v0.position.y);
    Vertex right_edge_interpolant = v1 + dv21dy * ((f32)pixel_y_start + 0.5f - v1.position.y);

    for (int pixel_y = pixel_y_start; pixel_y < pixel_y_end; ++pixel_y,
        left_edge_interpolant += dv20dy, right_edge_interpolant += dv21dy)
    {
        int pixel_x_start = (int)std::ceilf(left_edge_interpolant.position.x - 0.5f);
        int pixel_x_end = (int)std::ceilf(right_edge_interpolant.position.x - 0.5f);

        glm::vec2 dtexdx = (right_edge_interpolant.texture_coordinates - left_edge_interpolant.texture_coordinates)
            / (right_edge_interpolant.position.x - left_edge_interpolant.position.x);

        glm::vec2 scan_tex_interpolant = left_edge_interpolant.texture_coordinates + dtexdx
            * ((f32)pixel_x_start + 0.5f - left_edge_interpolant.position.x);

        for (int pixel_x = pixel_x_start; pixel_x < pixel_x_end; ++pixel_x, scan_tex_interpolant += dtexdx)
        {
            u32* pixel = framebuffer.GetPixelPointer(pixel_x, pixel_y);
            u32 texel = texture.GetTexel(scan_tex_interpolant.x, scan_tex_interpolant.y, TEXTURE_WRAP);
            *pixel = texel;
        }
    }
}

void DrawTriangleTex(const Vertex& v0, const Vertex& v1, const Vertex& v2, Framebuffer& framebuffer, const Texture& texture)
{
    const Vertex* vertex0 = &v0;
    const Vertex* vertex1 = &v1;
    const Vertex* vertex2 = &v2;

    // NOTE(achal): Sort the vertices so that v0 will be at the top (lowest y) and v2 will be at the bottom (highest y).
    if (vertex0->position.y > vertex1->position.y) std::swap(vertex0, vertex1);
    if (vertex1->position.y > vertex2->position.y) std::swap(vertex1, vertex2);
    if (vertex0->position.y > vertex1->position.y) std::swap(vertex0, vertex1);

    if (vertex0->position.y == vertex1->position.y)
    {
        // NOTE(achal): Sort the vertices so that v0 is left of v1
        if (vertex0->position.x > vertex1->position.x)
            std::swap(vertex0, vertex1);

        DrawFlatTopTriangleTex(*vertex0, *vertex1, *vertex2, framebuffer, texture);
    }
    else if (vertex1->position.y == vertex2->position.y)
    {
        // NOTE(achal): Sort the vertices so that v1 is left of v2
        if (vertex1->position.x > vertex2->position.x)
            std::swap(vertex1, vertex2);

        DrawFlatBottomTriangleTex(*vertex0, *vertex1, *vertex2, framebuffer, texture);
    }
    else
    {
        f32 alpha = (f32)(vertex1->position.y - vertex0->position.y) / (f32)(vertex2->position.y - vertex0->position.y);
        Vertex split_vertex = {};
        split_vertex.position = vertex0->position + (vertex2->position - vertex0->position) * alpha;
        split_vertex.texture_coordinates = vertex0->texture_coordinates
            + (vertex2->texture_coordinates - vertex0->texture_coordinates) * alpha;

        if (split_vertex.position.x > vertex1->position.x)
        {
            // Major Right
            DrawFlatBottomTriangleTex(*vertex0, *vertex1, split_vertex, framebuffer, texture);
            DrawFlatTopTriangleTex(*vertex1, split_vertex, *vertex2, framebuffer, texture);    
        }
        else
        {
            // Major Left
            DrawFlatBottomTriangleTex(*vertex0, split_vertex, *vertex1, framebuffer, texture);
            DrawFlatTopTriangleTex(split_vertex, *vertex1, *vertex2, framebuffer, texture);
        }
    }
}

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

// TODO(achal): Make helper function for hard-coded skinned cube and "normal" cube?
void Engine::Initialize()
{
    f32 half_side_length = 0.5f;

    // In Object Space.
    glm::vec3 vertex_positions[14] =
    {
        { -half_side_length, -half_side_length, -half_side_length },    // 0
        { half_side_length, -half_side_length, -half_side_length },     // 1
        { half_side_length, -half_side_length, half_side_length },      // 2
        { -half_side_length, -half_side_length, half_side_length },     // 3

        { -half_side_length, half_side_length, half_side_length },      // 4
        { -half_side_length, half_side_length, -half_side_length },     // 5
        { half_side_length, half_side_length, -half_side_length },      // 6
        { half_side_length, half_side_length, half_side_length },       // 7

        { -half_side_length, half_side_length, -half_side_length },     // 8 (5")
        { half_side_length, half_side_length, -half_side_length },      // 9 (6')

        { -half_side_length, half_side_length, -half_side_length },     // 10 (5')
        { -half_side_length, -half_side_length, -half_side_length },    // 11 (0')

        { -half_side_length, -half_side_length, -half_side_length },    // 12 (0")
        { half_side_length, -half_side_length, -half_side_length },     // 13 (1')
    };

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
#else
    // For dice_skin.png
    glm::vec2 texture_coordinates[14] =
    {
        { 2.f / 3.f, 0.f / 4.f },   // 0
        { 2.f / 3.f, 3.f / 4.f },   // 1
        { 2.f / 3.f, 2.f / 4.f },   // 2
        { 2.f / 3.f, 1.f / 4.f },   // 3

        { 1.f / 3.f, 1.f / 4.f },   // 4
        { 1.f / 3.f, 0.f / 4.f },   // 5
        { 1.f / 3.f, 3.f / 4.f },   // 6
        { 1.f / 3.f, 2.f / 4.f },   // 7

        { 0.f / 3.f, 1.f / 4.f },   // 8 (5")
        { 0.f / 3.f, 2.f / 4.f },   // 9 (6')

        { 1.f / 3.f, 4.f / 4.f },   // 10 (5')
        { 2.f / 3.f, 4.f / 4.f },   // 11 (0')

        { 3.f / 3.f, 1.f / 4.f },   // 12 (0")
        { 3.f / 3.f, 2.f / 4.f }    // 13 (1')
    };

    // NOTE(achal): Freeing texture memory is not necessary for now.
    texture.texels = (u8*)stbi_load("../dice_skin.png", &texture.width, &texture.height, &texture.channel_count, 0);
#endif

    cube.vertices.resize(14);
    for (int i = 0; i < cube.vertices.size(); ++i)
    {
        cube.vertices[i].position = vertex_positions[i];
        cube.vertices[i].texture_coordinates = texture_coordinates[i];
    }

    // Assemble your cube in such a way that all the face normals face outside -- this constraint 
    // gives you the winding of the triangles.
    cube.indices = 
    {
        3, 5, 0,
        3, 4, 5,

        2, 4, 3,
        2, 7, 4,

        1, 7, 2,
        1, 6, 7,

        1, 10, 6,
        1, 11, 10,

        7, 8, 4,
        7, 9, 8,

        3, 12, 2,
        12, 13, 2
    };
}

void Engine::SetupFramebuffer(int width, int height, int channel_count, int pitch, void* pixels)
{
    framebuffer.width = width;
    framebuffer.height = height;
    framebuffer.channels = channel_count;
    framebuffer.pitch = pitch;
    framebuffer.pixels = pixels;
}

// Wraps the given angle in the range -PI to PI
inline f32 WrapAngle(f32 angle)
{
    f32 modded_angle = fmod(angle, 2.f * PI32);
    return modded_angle > PI32 ? modded_angle - 2.f * PI32 : modded_angle;
}

void Engine::Render()
{
    // TODO(achal): Is my rotation speed too high or maybe I was just cycling through a lot frames earlier?

    // Take user input
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

    glm::mat4 object_to_world(1.f);
    object_to_world = glm::translate(object_to_world, glm::vec3(0.f, 0.f, -2.f));
    object_to_world = glm::rotate(object_to_world, theta_z, glm::vec3(0.f, 0.f, 1.f));
    object_to_world = glm::rotate(object_to_world, theta_y, glm::vec3(0.f, 1.f, 0.f));
    object_to_world = glm::rotate(object_to_world, theta_x, glm::vec3(1.f, 0.f, 0.f));

    f32 half_width = (f32)framebuffer.width / 2.f;
    f32 half_height = (f32)framebuffer.height / 2.f;

    // Transform vertices to World Space
    std::vector<Vertex> transformed_vertices(cube.vertices.size());
    for (int i = 0; i < cube.vertices.size(); ++i)
    {
        transformed_vertices[i].position = glm::vec3(object_to_world * glm::vec4(cube.vertices[i].position, 1.f));
        transformed_vertices[i].texture_coordinates = cube.vertices[i].texture_coordinates;
    }

    // Assemble Triangles
    for (int i = 0; i < cube.indices.size() / 3; ++i)
    {
        size_t idx0 = cube.indices[3 * i];
        size_t idx1 = cube.indices[3 * i + 1];
        size_t idx2 = cube.indices[3 * i + 2];

        Vertex v0 = transformed_vertices[idx0];
        Vertex v1 = transformed_vertices[idx1];
        Vertex v2 = transformed_vertices[idx2];

        b32 should_cull = glm::dot(v0.position, glm::cross(v1.position - v0.position, v2.position - v0.position)) >= 0.f;

        if (!should_cull)
        {
            // World (View) Space to Screen Space

            // TODO(achal): Pull this repeated projection code out!
            v0.position.x = ((v0.position.x / -v0.position.z) + 1.f) * half_width;
            v0.position.y = ((-v0.position.y / -v0.position.z) + 1.f) * half_height;

            v1.position.x = ((v1.position.x / -v1.position.z) + 1.f) * half_width;
            v1.position.y = ((-v1.position.y / -v1.position.z) + 1.f) * half_height;

            v2.position.x = ((v2.position.x / -v2.position.z) + 1.f) * half_width;
            v2.position.y = ((-v2.position.y / -v2.position.z) + 1.f) * half_height;

            DrawTriangleTex(v0, v1, v2, framebuffer, texture);
        }
    }
}