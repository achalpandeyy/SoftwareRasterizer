#ifndef ENGINE_H

#include "Core/Types.h"
#include "Framebuffer.h"
#include "ZBuffer.h"
#include "Scene.h"

#include <cmath>
#include <memory>
#include <vector>

#define PI32 3.14159265f

struct Button
{
    b32 pressed;
    u32 code;
};

struct Engine
{
    void Initialize(int width, int height, int channel_count, void* pixels);
    void UpdateModel();
    void Render();

    Button buttons[3];
    
    Framebuffer framebuffer;
    ZBuffer z_buffer;
    std::unique_ptr<Scene> scene = NULL;
    f32 time = 0.f;
};

#define ENGINE_H
#endif