#include "Engine.h"
#include "CubeScene.h"
#include "ColorCubeScene.h"
#include "CubeSkinScene.h"
#include "FaceColorCubeScene.h"
#include "CubeVertexPositionColorScene.h"
#include "WavyPlaneScene.h"

#include <glm/gtc/matrix_transform.hpp>
#include <stb_image/stb_image.h>
#include <cmath>
#include <utility>

/*
    NOTE(achal): I'm using Direct3D's Coordinate System and Rasterization Rules.
    The pixel (0, 0) includes the _area_ [0, 1) x [0, 1). (0, 0)th pixel is the one
    at the top-left corner.
    Reference: https://docs.microsoft.com/en-us/windows/win32/direct3d10/d3d10-graphics-programming-guide-resources-coordinates
*/

void Engine::Initialize(int width, int height, int channel_count, void* pixels)
{
    scene = std::make_unique<WavyPlaneScene>();

    framebuffer.width = width;
    framebuffer.height = height;
    framebuffer.channel_count = channel_count;
    framebuffer.pixels = pixels;
    scene->SetFramebuffer(&framebuffer);

    z_buffer.width = width;
    z_buffer.height = height;
    z_buffer.z_values = (f32*)malloc((size_t)width * (size_t)height * sizeof(f32));
    scene->SetZBuffer(&z_buffer);
}

// Wraps the given angle in the range -PI to PI
inline f32 WrapAngle(f32 angle)
{
    f32 modded_angle = fmod(angle, 2.f * PI32);
    return modded_angle > PI32 ? modded_angle - 2.f * PI32 : modded_angle;
}

void Engine::UpdateModel()
{
    f32 angular_speed = PI32 / 10.f;
    f32 dt = 1.f / 60.f;
    f32 dtheta = angular_speed * dt;

    if (buttons[0].pressed)
    {
        scene->theta_x = WrapAngle(scene->theta_x + dtheta);
    }
    if (buttons[1].pressed)
    {
        scene->theta_y = WrapAngle(scene->theta_y + dtheta);
    }
    if (buttons[2].pressed)
    {
        scene->theta_z = WrapAngle(scene->theta_z + dtheta);
    }

    glm::mat4 model(1.f);
    model = glm::translate(model, glm::vec3(0.f, 0.f, -2.f));
    model = glm::rotate(model, scene->theta_z, glm::vec3(0.f, 0.f, 1.f));
    model = glm::rotate(model, scene->theta_y, glm::vec3(0.f, 1.f, 0.f));
    model = glm::rotate(model, scene->theta_x, glm::vec3(1.f, 0.f, 0.f));

    scene->SetModel(model);
    time += dt;
    scene->SetTime(time);
}

void Engine::Render()
{
    framebuffer.Clear();
    z_buffer.Clear();
    UpdateModel();
    scene->Draw();
}