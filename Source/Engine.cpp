#include "Engine.h"

#define WIREFRAME 0

#include "glm/glm/gtc/matrix_transform.hpp"
#include "stb_image/stb_image.h"

// TODO(achal): Swap macro and ceil function.
#include <cmath>
#include <utility>

/*
    TODO(achal): All of the drawing functions below take their coordinates in raster space and as a result
    should take integral vectors. They represent pixels after all! I currently don't want to track all the
    floating-point math and see that it is safely converted to ints. So I'm currently making them all floats.
    I will see to it later.

    DrawLine should be integer bresenham.

    TODO(achal): All of the triangle drawing routines below can do with 2D positions, but still I'm passing
    3D position because they have texture coordinates associated with them in the struct "Vertex". So essentially,
    the z-component is wasted space.
    Workaround for this?
*/

/*
    NOTE(achal): I'm using Direct3D's Coordinate System and Rasterization Rules.
    The pixel (0, 0) includes the _area_ [0, 1) x [0, 1). "Center" of the pixel (0, 0) is
    at the top-left corner.
    Reference: https://docs.microsoft.com/en-us/windows/win32/direct3d10/d3d10-graphics-programming-guide-resources-coordinates
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

// TODO(achal): DrawFlatTopTriangle & DrawFlatBottomTriangle seems suspiciously similar, collapse 'em into one??
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

void DrawFlatBottomTriangleTex(const Vertex& v0, const Vertex& v1, const Vertex& v2, Framebuffer& framebuffer, const Texture& texture)
{
    #if 0
    f32 dy = v2_pos.y - v0_pos.y;
    
    glm::vec2 dv1_v0_pos = (v1_pos - v0_pos) / dy;
    glm::vec2 dv1_v0_tex = (v1_tex - v0_tex) / dy;

    glm::vec2 dv2_v0_pos = (v2_pos - v0_pos) / dy;
    glm::vec2 dv2_v0_tex = (v2_tex - v0_tex) / dy;

    glm::vec2 right_interpolant_pos = ;
    glm::vec2 right_interpolant_tex = ;
    #endif


    f32 rcp_slope_1_0 = (f32)(v1.position.x - v0.position.x) / (f32)(v1.position.y - v0.position.y);
    f32 rcp_slope_2_0 = (f32)(v2.position.x - v0.position.x) / (f32)(v2.position.y - v0.position.y);

    int pixel_y_start = (int)std::ceilf(v0.position.y - 0.5f);
    int pixel_y_end = (int)std::ceilf(v1.position.y - 0.5f);

    glm::vec2 tex_edge_step_l = (v1.texture_coordinates - v0.texture_coordinates) / (v1.position.y - v0.position.y);
    glm::vec2 tex_edge_step_r = (v2.texture_coordinates - v0.texture_coordinates) / (v2.position.y - v0.position.y);

    // Add pre-step
    glm::vec2 tex_edge_l = v0.texture_coordinates + tex_edge_step_l * ((f32)pixel_y_start + 0.5f - v0.position.y);
    glm::vec2 tex_edge_r = v0.texture_coordinates + tex_edge_step_r * ((f32)pixel_y_start + 0.5f - v0.position.y);

    for (int pixel_y = pixel_y_start; pixel_y < pixel_y_end; ++pixel_y,
        tex_edge_l += tex_edge_step_l, tex_edge_r += tex_edge_step_r)
    {
        f32 point_x1 = rcp_slope_1_0 * (f32(pixel_y) + 0.5f - v1.position.y) + v1.position.x;
        f32 point_x2 = rcp_slope_2_0 * (f32(pixel_y) + 0.5f - v2.position.y) + v2.position.x;

        int pixel_x_start = (int)std::ceilf(point_x1 - 0.5f);
        int pixel_x_end = (int)std::ceilf(point_x2 - 0.5f);

        glm::vec2 tex_scan_step = (tex_edge_r - tex_edge_l) / (point_x2 - point_x1);

        // Add pre-step
        glm::vec2 tex_scan = tex_edge_l + tex_scan_step * ((f32)pixel_x_start + 0.5f - point_x1);

        for (int pixel_x = pixel_x_start; pixel_x < pixel_x_end; ++pixel_x, tex_scan += tex_scan_step)
        {
            u32* pixel = framebuffer.GetPixelPointer(pixel_x, pixel_y);
            u32 texel = texture.GetTexel(tex_scan.x, tex_scan.y);
            *pixel = texel;
        }
    }
}

void DrawFlatTopTriangleTex(const Vertex& v0, const Vertex& v1, const Vertex& v2, Framebuffer& framebuffer, const Texture& texture)
{
    f32 rcp_slope_2_0 = (f32)(v2.position.x - v0.position.x) / (f32)(v2.position.y - v0.position.y);
    f32 rcp_slope_2_1 = (f32)(v2.position.x - v1.position.x) / (f32)(v2.position.y - v1.position.y);

    int pixel_y_start = (int)std::ceilf(v0.position.y - 0.5f);
    int pixel_y_end = (int)std::ceilf(v2.position.y - 0.5f);

    glm::vec2 tex_edge_step_l = (v2.texture_coordinates - v0.texture_coordinates) / (v2.position.y - v0.position.y);
    glm::vec2 tex_edge_step_r = (v2.texture_coordinates - v1.texture_coordinates) / (v2.position.y - v1.position.y);

    // Add pre-step
    glm::vec2 tex_edge_l = v0.texture_coordinates + tex_edge_step_l * ((f32)pixel_y_start + 0.5f - v0.position.y);
    glm::vec2 tex_edge_r = v1.texture_coordinates + tex_edge_step_r * ((f32)pixel_y_start + 0.5f - v1.position.y);

    for (int pixel_y = pixel_y_start; pixel_y < pixel_y_end; ++pixel_y,
        tex_edge_l += tex_edge_step_l, tex_edge_r += tex_edge_step_r)
    {
        f32 point_x0 = rcp_slope_2_0 * (f32(pixel_y) + 0.5f - v0.position.y) + v0.position.x;
        f32 point_x1 = rcp_slope_2_1 * (f32(pixel_y) + 0.5f - v1.position.y) + v1.position.x;

        int pixel_x_start = (int)std::ceilf(point_x0 - 0.5f);
        int pixel_x_end = (int)std::ceilf(point_x1 - 0.5f);

        glm::vec2 tex_scan_step = (tex_edge_r - tex_edge_l) / (point_x1 - point_x0);

        // Add pre-step
        glm::vec2 tex_scan = tex_edge_l + tex_scan_step * ((f32)pixel_x_start + 0.5f - point_x0);

        for (int pixel_x = pixel_x_start; pixel_x < pixel_x_end; ++pixel_x, tex_scan += tex_scan_step)
        {
            u32* pixel = framebuffer.GetPixelPointer(pixel_x, pixel_y);
            u32 texel = texture.GetTexel(tex_scan.x, tex_scan.y);
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
void Engine::Initialize()
{
    f32 half_side_length = 0.5f;

    // In Object Space.
    glm::vec3 vertex_positions[8] =
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

    glm::vec2 texture_coordinates[8] =
    {
        { 0.f, 1.f },
        { 1.f, 1.f },
        { 0.f, 1.f },
        { 1.f, 1.f },

        { 1.f, 0.f },
        { 0.f, 0.f },
        { 1.f, 0.f },
        { 0.f, 0.f }
    };

    cube.vertices.resize(8);
    for (int i = 0; i < cube.vertices.size(); ++i)
    {
        cube.vertices[i].position = vertex_positions[i];
        cube.vertices[i].texture_coordinates = texture_coordinates[i];
    }

    // NOTE(achal): Freeing texture memory is not necessary for now.
    texture.texels = (u8*)stbi_load("../sauron-bhole-100x100.png", &texture.width, &texture.height, &texture.channel_count, 0);

#if WIREFRAME
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
#else

    // Assemble your cube in such a way that all the face normals face outside -- this constraint 
    // gives you the winding of the triangles.
    cube.indices = 
    {
        4, 3, 2,
        4, 2, 7,

        4, 7, 5,
        5, 7, 6,

        6, 1, 5,
        1, 0, 5,

        0, 3, 4,
        0, 4, 5,

        1, 6, 7,
        1, 7, 2,

        1, 2, 3,
        0, 1, 3
    };
#endif
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

void Engine::UpdateAndRender()
{
    // TODO(achal): Is my rotation speed too high?

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

#if WIREFRAME
    // Draw the damn lined cube
    for (u32 i = 0; i < cube.indices.size() / 2; ++i)
    {
        size_t idx0 = cube.indices[2 * i];
        size_t idx1 = cube.indices[2 * i + 1];

        // TODO(achal): This is a lot of wasted effort, essentially I'm transforming the same vertex twice.
        glm::vec3 v0 = cube.vertices[idx0];

        // Object Space to World Space
        v0 = glm::vec3(rotate * glm::vec4(v0, 1.f));
        v0 += glm::vec3(0.f, 0.f, -2.f);

        // World Space to Screen Space
        v0.x = ((v0.x / v0.z) + 1.f) * half_width;
        v0.y = ((-v0.y / v0.z) + 1.f) * half_height;

        glm::vec3 v1 = cube.vertices[idx1];

        // Object Space to World Space
        v1 = glm::vec3(rotate * glm::vec4(v1, 1.f));;
        v1 += glm::vec3(0.f, 0.f, -2.f);

        // World Space to Screen Space
        v1.x = ((v1.x / v1.z) + 1.f) * half_width;
        v1.y = ((-v1.y / v1.z) + 1.f) * half_height;

        // TODO(achal): assert that raster_v0 and raster_v1 are always positive.
        // NOTE(achal): Raster Space: [0, width] x [0, height]
        // TODO(achal): Can I put it into a matrix transfrom and probably combine it with other transforms, like
        // from World space to NDC, for example??
        glm::vec2 raster_v0(v0.x, v0.y);
        glm::vec2 raster_v1(v1.x, v1.y);
        DrawLine(raster_v0, raster_v1, framebuffer, 0xFFFFFFFF);
    }
#else
    u32 colors[4] = {0xFFFFFFFF, 0xFFFF0000, 0xFF00FF00, 0xFF0000FF};
    for (int i = 0; i < cube.indices.size() / 3; ++i)
    {
        size_t idx0 = cube.indices[3 * i];
        size_t idx1 = cube.indices[3 * i + 1];
        size_t idx2 = cube.indices[3 * i + 2];

        // Object Space to World (View) Space
        Vertex v0 = cube.vertices[idx0];
        v0.position = glm::vec3(rotate * glm::vec4(v0.position, 1.f));
        v0.position += glm::vec3(0.f, 0.f, -2.f);

        Vertex v1 = cube.vertices[idx1];
        v1.position = glm::vec3(rotate * glm::vec4(v1.position, 1.f));
        v1.position += glm::vec3(0.f, 0.f, -2.f);

        Vertex v2 = cube.vertices[idx2];
        v2.position = glm::vec3(rotate * glm::vec4(v2.position, 1.f));
        v2.position += glm::vec3(0.f, 0.f, -2.f);

        b32 should_cull = glm::dot(v0.position, glm::cross(v1.position - v0.position, v2.position - v0.position)) >= 0.f;

        if (!should_cull)
        {
            // World (View) Space to Screen Space
            v0.position.x = ((v0.position.x / -v0.position.z) + 1.f) * half_width;
            v0.position.y = ((-v0.position.y / -v0.position.z) + 1.f) * half_height;

            v1.position.x = ((v1.position.x / -v1.position.z) + 1.f) * half_width;
            v1.position.y = ((-v1.position.y / -v1.position.z) + 1.f) * half_height;

            v2.position.x = ((v2.position.x / -v2.position.z) + 1.f) * half_width;
            v2.position.y = ((-v2.position.y / -v2.position.z) + 1.f) * half_height;

            // DrawTriangle(v0.position, v1.position, v2.position, framebuffer, colors[i % 4]);
            DrawTriangleTex(v0, v1, v2, framebuffer, texture);
        }
    }
#endif
}