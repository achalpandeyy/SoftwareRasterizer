#ifndef ENGINE_H

#include "Core/Types.h"

#include "CubeSkinScene.h"
#include "CubeScene.h"
#include "ColorCubeScene.h"
#include "FaceColorCubeScene.h"

#include <cmath>
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
    // CubeSkinScene scene;
    // TODO(achal): Sauron's texture looks a bit off (just a teeny-tiny bit misplaced on faces of the cube).
    // CubeScene scene;
    ColorCubeScene scene;
    // FaceColorCubeScene scene;
};

#define ENGINE_H
#endif