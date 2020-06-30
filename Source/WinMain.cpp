#include "Engine.h"

// TODO(achal): Disable all the bullshit that we don't need -- NOMINMAX etc.
#include <Windows.h>

struct Win32Framebuffer
{
    int width;
    int height;
    int bytes_per_pixel;
    int pitch;
    void* pixels;
    BITMAPINFO info;
};

void Win32DisplayImage(const Win32Framebuffer& win32_framebuffer, HDC device_context)
{
    // TODO(achal): Aspect ratio correction.
    StretchDIBits(device_context, 0, 0, win32_framebuffer.width, win32_framebuffer.height, 0, 0, win32_framebuffer.width,
        win32_framebuffer.height, win32_framebuffer.pixels, &win32_framebuffer.info, DIB_RGB_COLORS, SRCCOPY);
}

static b32 global_rotate_x_key_pressed;
static b32 global_rotate_y_key_pressed;
static b32 global_rotate_z_key_pressed;

void Win32ProcessKeyboard(WPARAM vk_code, b32 pressed)
{
    if (vk_code == 'Q')
        global_rotate_x_key_pressed = pressed;
    else if (vk_code == 'W')
        global_rotate_y_key_pressed = pressed;
    else if (vk_code == 'E')
        global_rotate_z_key_pressed = pressed;
    else if (vk_code == VK_ESCAPE)
        PostQuitMessage(0);
}

LRESULT CALLBACK Win32WindowCallback(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
    // NOTE(achal): You can get your stuff here by retrieving a pointer from HWND. 

    LRESULT result = 0;

    switch (message)
    {
        case WM_SIZE:
        {
            OutputDebugString("WM_SIZE\n");
        } break;

        case WM_DESTROY:
        {
            OutputDebugString("WM_DESTROY\n");
        } break;

        case WM_CLOSE:
        {
            OutputDebugString("WM_CLOSE\n");
            PostQuitMessage(0);
        } break;

        case WM_KEYDOWN:
        {
            Win32ProcessKeyboard(w_param, true);
        } break;

        case WM_KEYUP:
        {
            Win32ProcessKeyboard(w_param, false);
        } break;

        case WM_ACTIVATEAPP:
        {
            OutputDebugString("WM_ACTIVATEAPP\n");
        } break;

        default:
        {
            result = DefWindowProc(window, message, w_param, l_param);
        } break;
    }

    return result;
}

HWND Win32CreateWindow(int width, int height, LPCSTR name, HINSTANCE instance)
{
    WNDCLASS WindowClass = {};
    WindowClass.style = CS_CLASSDC;
    WindowClass.lpfnWndProc = Win32WindowCallback;
    WindowClass.hInstance = instance;
    WindowClass.lpszClassName = "SoftwareRendererWindowClass";

    if (!RegisterClass(&WindowClass))
    {
        // TODO(achal): Logging.
        OutputDebugString("Failed to register window class!\n");
    }

    return CreateWindowEx(0, WindowClass.lpszClassName, name, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, instance, 0);
}

b32 Win32PollEvents()
{
    MSG message;
    while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
    {
        if (message.message == WM_QUIT)
            return true;

        TranslateMessage(&message);
        DispatchMessage(&message);
    }
    return false;
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prev_instance, PSTR command_line, int show_code)
{
    int width = 768;
    int height = 768;

    HWND window = Win32CreateWindow(width, height, "Software Rendering Engine", instance);
    if (!window)
    {
        // TODO(achal): Logging.
        OutputDebugString("Unable to create window!\n");
        exit(1);
    }

    Win32Framebuffer win32_framebuffer = {};
    win32_framebuffer.width = width;
    win32_framebuffer.height = height;
    win32_framebuffer.bytes_per_pixel = 4;
    win32_framebuffer.pitch = win32_framebuffer.width * win32_framebuffer.bytes_per_pixel;;

    // NOTE(achal): Top-down Bitmap! (0, 0) at top-left.
    win32_framebuffer.info = {};
    win32_framebuffer.info.bmiHeader.biSize = sizeof(win32_framebuffer.info.bmiHeader);
    win32_framebuffer.info.bmiHeader.biWidth = win32_framebuffer.width;
    win32_framebuffer.info.bmiHeader.biHeight = -win32_framebuffer.height;
    win32_framebuffer.info.bmiHeader.biPlanes = 1;
    win32_framebuffer.info.bmiHeader.biBitCount = (WORD)(win32_framebuffer.bytes_per_pixel * 8);
    win32_framebuffer.info.bmiHeader.biCompression = BI_RGB;

    // NOTE(achal): This allocated memory is not explicitly freed anywhere in the program because
    // it'll get freed automatically when the program exits. This could, however, become a problem
    // of memory leak, when we allocate this repeatedly, for example, in a loop.
    win32_framebuffer.pixels = VirtualAlloc(0, win32_framebuffer.width * win32_framebuffer.height * win32_framebuffer.bytes_per_pixel,
        MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    if (!win32_framebuffer.pixels)
    {
        // TODO(achal): Logging.
        OutputDebugString("Unable to allocate memory for image buffer\n");
        exit(1);
    }

    Engine engine;
    engine.Initialize();
    engine.SetupFramebuffer(win32_framebuffer.width, win32_framebuffer.height, win32_framebuffer.bytes_per_pixel,
        win32_framebuffer.pitch, win32_framebuffer.pixels);

    while (true)
    {
        b32 should_quit = Win32PollEvents();
        if (should_quit)
            return 0;

        engine.buttons[0].pressed = global_rotate_x_key_pressed;
        engine.buttons[1].pressed = global_rotate_y_key_pressed;
        engine.buttons[2].pressed = global_rotate_z_key_pressed;

        engine.Render();

        Win32DisplayImage(win32_framebuffer, GetDC(window));

        // Clear the buffer to black.
        // TODO(achal): In the near future I'd like to have some sort of a swapping of buffer mechanism.
        memset(win32_framebuffer.pixels, 0, win32_framebuffer.width * win32_framebuffer.height * win32_framebuffer.bytes_per_pixel);
    }

    return 0;
}