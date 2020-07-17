#ifndef ENGINE_H

#include "Core/Types.h"

// TODO(achal): Can't include all them together for some reason!!
// #include "CubeSkinScene.h"
// #include "CubeScene.h"
// #include "ColorCubeScene.h"
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
    // CubeSkinScene scene;
    // CubeScene scene;
    FaceColorCubeScene scene;
};

#define ENGINE_H
#endif