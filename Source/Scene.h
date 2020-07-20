#ifndef SCENE_H

#include "Core/Types.h"

#include <glm/glm.hpp>

struct Framebuffer;
struct ZBuffer;

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

struct Scene
{
    virtual ~Scene() {}

    virtual void Draw() = 0;

    virtual void SetFramebuffer(Framebuffer* framebuffer) = 0;
    virtual void SetZBuffer(ZBuffer* z_buffer) = 0;
    virtual void SetModel(const glm::mat4& model) = 0;
    virtual void SetTime(f32 t) {}

    f32 theta_x = 0.f;
    f32 theta_y = 0.f;
    f32 theta_z = 0.f;
};

#define SCENE_H
#endif
