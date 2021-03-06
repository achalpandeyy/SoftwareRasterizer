#ifndef PIPELINE_H

#include "Core/Types.h"
#include "IndexedTriangleList.h"
#include "Framebuffer.h"
#include "ZBuffer.h"
#include "Texture.h"
#include "Triangle.h"

#include <glm/glm.hpp>
#include <algorithm>
#include <vector>

#define TEXTURE_WRAP 1

template <typename Effect>
struct Pipeline
{
    typedef typename Effect::Vertex Vertex;
    typedef typename Effect::VertexShader::VertexOut VSOut;
    typedef typename Effect::GeometryShader::VertexOut GSOut;

    void Draw(const IndexedTriangleList<Vertex>& it_list)
    {
        f32 half_width = (f32)framebuffer->width / 2.f;
        f32 half_height = (f32)framebuffer->height / 2.f;

        std::vector<VSOut> transformed_vertices(it_list.vertices.size());
        std::transform(it_list.vertices.begin(), it_list.vertices.end(), transformed_vertices.begin(), effect.vertex_shader);

        // Assemble Triangles
        for (size_t i = 0; i < it_list.indices.size() / 3; ++i)
        {
            size_t idx0 = it_list.indices[3 * i];
            size_t idx1 = it_list.indices[3 * i + 1];
            size_t idx2 = it_list.indices[3 * i + 2];

            VSOut v0 = transformed_vertices[idx0];
            VSOut v1 = transformed_vertices[idx1];
            VSOut v2 = transformed_vertices[idx2];

            b32 should_cull = (glm::dot(glm::cross(v1.position - v0.position, v2.position - v0.position), v1.position)) >= 0;

            if (!should_cull)
            {
                Triangle<GSOut> triangle = effect.geometry_shader(&v0, &v1, &v2, i);

                // World (View) Space to Screen Space
                ToScreenSpace(&triangle.v0, half_width, half_height);
                ToScreenSpace(&triangle.v1, half_width, half_height);
                ToScreenSpace(&triangle.v2, half_width, half_height);

                DrawTriangle(&triangle);
            }
        }
    }

    void DrawTriangle(Triangle<GSOut>* triangle)
    {
        VSOut* v0 = &triangle->v0;
        VSOut* v1 = &triangle->v1;
        VSOut* v2 = &triangle->v2;

        // NOTE(achal): Sort the vertices so that v0 will be at the top (lowest y) and v2 will be at the bottom (highest y).
        if (v0->position.y > v1->position.y) std::swap(v0, v1);
        if (v1->position.y > v2->position.y) std::swap(v1, v2);
        if (v0->position.y > v1->position.y) std::swap(v0, v1);

        if (v0->position.y == v1->position.y)
        {
            // NOTE(achal): Sort the vertices so that v0 is left of v1
            if (v0->position.x > v1->position.x)
                std::swap(v0, v1);

            DrawFlatTopTriangle(*v0, *v1, *v2);
        }
        else if (v1->position.y == v2->position.y)
        {
            // NOTE(achal): Sort the vertices so that v1 is left of v2
            if (v1->position.x > v2->position.x)
                std::swap(v1, v2);

            DrawFlatBottomTriangle(*v0, *v1, *v2);
        }
        else
        {
            f32 alpha = (f32)(v1->position.y - v0->position.y) / (f32)(v2->position.y - v0->position.y);
            VSOut split_vertex = *v0 + (*v2 - *v0) * alpha;

            if (split_vertex.position.x > v1->position.x)
            {
                // Major Right
                DrawFlatBottomTriangle(*v0, *v1, split_vertex);
                DrawFlatTopTriangle(*v1, split_vertex, *v2);
            }
            else
            {
                // Major Left
                DrawFlatBottomTriangle(*v0, split_vertex, *v1);
                DrawFlatTopTriangle(split_vertex, *v1, *v2);
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

    void DrawFlatBottomTriangle(const GSOut& v0, const GSOut& v1, const GSOut& v2)
    {
        f32 rcp_dy = 1.f / (v2.position.y - v0.position.y);

        GSOut dv0 = (v1 - v0) * rcp_dy;
        GSOut dv1 = (v2 - v0) * rcp_dy;

        // Initialize right edge interpolant
        GSOut interp_right = v0;

        DrawFlatTriangle(v0, v1, v2, dv0, dv1, interp_right);
    }

    // NOTE(achal): Vertex Order Assumption:
    //
    //   v0 ------ v1
    //      \    /
    //       \  /
    //        \/
    //        v2
    //

    void DrawFlatTopTriangle(const GSOut& v0, const GSOut& v1, const GSOut& v2)
    {
        f32 rcp_dy = 1.f / (v2.position.y - v0.position.y);

        GSOut dv0 = (v2 - v0) * rcp_dy;
        GSOut dv1 = (v2 - v1) * rcp_dy;

        // Initialize right edge interpolant.
        GSOut interp_right = v1;

        DrawFlatTriangle(v0, v1, v2, dv0, dv1, interp_right);
    }

    void DrawFlatTriangle(const GSOut& v0, const GSOut& v1, const GSOut& v2, const GSOut& dv0,
        const GSOut& dv1, GSOut interp_right)
    {
        // Initialize left edge interpolant.
        GSOut interp_left = v0;

        int y_start = (int)std::ceilf(v0.position.y - 0.5f);
        int y_end = (int)std::ceilf(v2.position.y - 0.5f);

        // Add pre-step.
        //
        // NOTE(achal): While it is true that the position of v0 is not the same in both flat-top and flat-bottom
        // triangle, what makes this work is, for the right edge interpolant in the flat-top triangle both v0 and
        // v1 have the same y coordinate! So, substracting v0.position.y would work for all the cases, whereas
        // subtracing v1.position.y would NOT work for all the cases.
        interp_left += dv0 * ((f32)y_start + 0.5f - v0.position.y);
        interp_right += dv1 * ((f32)y_start + 0.5f - v0.position.y);

        for (int y = y_start; y < y_end; ++y, interp_left += dv0, interp_right += dv1)
        {
            int x_start = (int)std::ceilf(interp_left.position.x - 0.5f);
            int x_end = (int)std::ceilf(interp_right.position.x - 0.5f);

            DrawScanLine(y, x_start, x_end, interp_left, interp_right);
        }
    }

    void DrawScanLine(int y, int start, int end, const GSOut& interp_left, const GSOut& interp_right)
    {
        f32 dx = interp_right.position.x - interp_left.position.x;

        GSOut d_interp = (interp_right - interp_left) / dx;
        GSOut interp = interp_left + d_interp * ((f32)start + 0.5f - interp_left.position.x);

        for (int x = start; x < end; ++x, interp += d_interp)
        {
            // NOTE(achal): We're doing some unnecessary computations here by multiplying the z value to
            // every vertex attribute of interp.
            f32 z = 1.f / interp.position.z;
            if (z_buffer->TestAndSet(x, y, z))
            {
                framebuffer->PutPixel(x, y, effect.pixel_shader(interp * z));
            }
        }
    }

    inline static void ToScreenSpace(VSOut* v, f32 half_width, f32 half_height)
    {
        // NOTE(achal): Since I'm looking down the negative z axis, all the z-coordinates would be negative.
        // We do not want to mirror the x and y coordinates about the y = x line when we do perspective divide
        // by z, so we take the absolute value of it.
        f32 rcp_abs_z = 1.f / glm::abs(v->position.z);

        // NOTE(achal): This will bring all the attributes in screen-space, even the texture coordinates.
        // Bringing texture coordinates in screen-space is required so that texture maps
        // correctly to the object in perspective space (basically, avoid texture warping).
        *v *= rcp_abs_z;

        v->position.x = (v->position.x + 1.f) * half_width;
        v->position.y = (-v->position.y + 1.f) * half_height;
        v->position.z = rcp_abs_z;
    }

#if 0
    void DrawLine(glm::vec2& v0, glm::vec2& v1, u32 color)
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
            if (steep)
                framebuffer->PutPixel((int)y, (int)x, color);
            else
                framebuffer->PutPixel((int)x, (int)y, color);

            error += derror;
            if (error > dx)
            {
                y += v1.y > v0.y ? 1 : -1;
                error -= 2 * dx;
            }
        }
    }
#endif

    Effect effect;
    Framebuffer* framebuffer;
    ZBuffer* z_buffer;
};

#define PIPELINE_H
#endif