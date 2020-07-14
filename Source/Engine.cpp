#include "Engine.h"

#include <glm/gtc/matrix_transform.hpp>
#include <stb_image/stb_image.h>

// TODO(achal): Swap macro and ceil function.
#include <cmath>
#include <utility>

/*
    DrawLine should be integer bresenham. (I think!)

    TODO(achal): All of the triangle drawing routines below can do with 2D positions, but still I'm passing
    3D position because they have texture coordinates associated with them in the struct "Vertex". So essentially,
    the z-component is wasted space.
    Workaround for this?

    NOTE(achal): I'm using Direct3D's Coordinate System and Rasterization Rules.
    The pixel (0, 0) includes the _area_ [0, 1) x [0, 1). (0, 0)th pixel is the one
    at the top-left corner.
    Reference: https://docs.microsoft.com/en-us/windows/win32/direct3d10/d3d10-graphics-programming-guide-resources-coordinates

    All of the following drawing functions take input in raster space, but that doesn't mean that
    they should have integral coordinates. We use the aforementioned rasterization rules to get
    the exact (integral) pixel coordinates on the fly.
*/

// TODO(achal): Make helper function for hard-coded skinned cube and "normal" cube?
void Engine::Initialize(int width, int height, int channel_count, void* pixels)
{
    framebuffer.width = width;
    framebuffer.height = height;
    framebuffer.channel_count = channel_count;
    framebuffer.pixels = pixels;

    scene.pipeline.framebuffer = &framebuffer;
}

// Wraps the given angle in the range -PI to PI
inline f32 WrapAngle(f32 angle)
{
    f32 modded_angle = fmod(angle, 2.f * PI32);
    return modded_angle > PI32 ? modded_angle - 2.f * PI32 : modded_angle;
}

inline void ToScreenSpace(Vertex* v, f32 half_width, f32 half_height)
{
    v->position.x = ((v->position.x / -v->position.z) + 1.f) * half_width;
    v->position.y = ((-v->position.y / -v->position.z) + 1.f) * half_height;
}

void Engine::UpdateModel()
{
    // TODO(achal): Is my rotation speed too high or maybe I was just cycling through a lot frames earlier?

    f32 angular_speed = PI32 / 10.f;
    f32 dt = 1.f / 60.f;
    f32 dtheta = angular_speed * dt;

    if (buttons[0].pressed)
    {
        scene.theta_x = WrapAngle(scene.theta_x + dtheta);
    }
    if (buttons[1].pressed)
    {
        scene.theta_y = WrapAngle(scene.theta_y + dtheta);
    }
    if (buttons[2].pressed)
    {
        scene.theta_z = WrapAngle(scene.theta_z + dtheta);
    }

    glm::mat4 model(1.f);
    model = glm::translate(model, glm::vec3(0.f, 0.f, -2.f));
    model = glm::rotate(model, scene.theta_z, glm::vec3(0.f, 0.f, 1.f));
    model = glm::rotate(model, scene.theta_y, glm::vec3(0.f, 1.f, 0.f));
    model = glm::rotate(model, scene.theta_x, glm::vec3(1.f, 0.f, 0.f));
    scene.pipeline.BindModelMatrix(model);
}

void Engine::Render()
{
    framebuffer.Clear();
    UpdateModel();
    scene.pipeline.Draw(scene.it_list);
}