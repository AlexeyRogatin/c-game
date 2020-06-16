typedef char i8;
typedef unsigned char u8;

typedef short i16;
typedef unsigned short u16;

typedef int i32;
typedef unsigned int u32;

typedef long long int i64;
typedef unsigned long long int u64;

typedef float f32;
typedef double f64;

typedef u8 byte;

#define true 1
#define false 0

#define assert(cond) \
    if (!(cond))     \
        *(int *)NULL = 12;

#include <stdio.h>
#include "game.c"
#include "Windows.h"

typedef struct
{
    byte *data;
    u32 size;
} File_Buffer;

File_Buffer win32_read_entire_file(char *file_name)
{
    HANDLE file = CreateFileA(
        file_name,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    assert(file != INVALID_HANDLE_VALUE);

    DWORD file_size_high;
    DWORD file_size = GetFileSize(file, &file_size_high);

    byte *file_memory = (byte *)malloc(file_size);
    DWORD bytes_read = 0;

    ReadFile(file, file_memory, file_size, &bytes_read, NULL);

    File_Buffer result;
    result.data = file_memory;
    result.size = file_size;
    return result;
}

#pragma pack(push, 1)

typedef struct
{
    u16 signature;
    u32 file_size;
    u32 unused;
    u32 data_offset;
} Bmp_Header;

typedef struct
{
    u32 header_size;
    u32 width;
    u32 height;
    u16 planes;
    u16 bits_per_pixel;
    u32 compression;
    u32 image_size;
    u32 x_pixels_per_meter;
    u32 y_pixels_per_meter;
    u32 colors_used;
    u32 important_colors;
} Bmp_Info;
#pragma pack(pop)

Bitmap win32_read_bmp(char *file_name)
{
    File_Buffer file = win32_read_entire_file(file_name);
    Bmp_Header *header = (Bmp_Header *)file.data;
    Bmp_Info *info = (Bmp_Info *)(file.data + sizeof(Bmp_Header));

    u32 *pixels = (u32 *)(file.data + header->data_offset);

    // AA RR GG BB
    Bitmap result;
    result.pixels = pixels;
    result.width = info->width;
    result.height = info->height;

    return result;
}

bool running = true;

LRESULT CALLBACK WindowProc(
    _In_ HWND window,
    _In_ UINT message,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam)
{
    LRESULT result = 0;

    switch (message)
    {
    case WM_CLOSE:
    {
        running = false;
    }
    break;

    default:
    {
        result = DefWindowProc(window, message, wParam, lParam);
    }
    }

    return result;
}

void handle_button(Button *button, bool key_went_up)
{
    if (key_went_up && button->is_down)
    {
        button->went_up = true;
        button->is_down = false;
    }
    else if (!button->is_down)
    {
        button->went_down = true;
        button->is_down = true;
    }
}

u64 performance_frequency = 1;

f64 win32_get_time()
{
    LARGE_INTEGER ticks;
    QueryPerformanceCounter(&ticks);
    f64 result = (f64)ticks.QuadPart / (f64)performance_frequency;
    return result;
}

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
            PSTR lpCmdLine, INT nCmdShow)
{
    time_t curTime = time(NULL);
    srand((u32)curTime);

    LARGE_INTEGER perf_frequency_li;
    QueryPerformanceFrequency(&perf_frequency_li);
    performance_frequency = perf_frequency_li.QuadPart;

    WNDCLASSA wndClass = {0};
    wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndClass.lpfnWndProc = WindowProc;
    wndClass.hInstance = hInstance;
    wndClass.lpszClassName = "WHat a crap";

    ATOM registeredClass = RegisterClassA(&wndClass);

    HWND window = CreateWindowExA(
        0,
        wndClass.lpszClassName,
        "Auch",
        WS_POPUPWINDOW | WS_VISIBLE | WS_MAXIMIZE,
        CW_USEDEFAULT, //x
        CW_USEDEFAULT, //y
        CW_USEDEFAULT, //width
        CW_USEDEFAULT, //height
        NULL,
        NULL,
        wndClass.hInstance,
        NULL);

    RECT rect;
    GetClientRect(window, &rect);
    HDC device_context = GetDC(window);
    i32 window_width = rect.right - rect.left;
    i32 window_height = rect.bottom - rect.top;

    BITMAPINFO bitmap_info = {0};
    bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmap_info.bmiHeader.biWidth = window_width;
    bitmap_info.bmiHeader.biHeight = window_height;
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = 32;
    bitmap_info.bmiHeader.biCompression = BI_RGB;

    u32 *pixels = (u32 *)malloc(4 * window_width * window_height);

    Bitmap game_screen;
    game_screen.pixels = pixels;
    game_screen.width = window_width;
    game_screen.height = window_height;

    Input input = {0};

    f64 prev_time = win32_get_time();

    while (running)
    {
        for (i32 button_index = 0; button_index < BUTTON_COUNT; button_index++)
        {
            Button *button = &input.buttons[button_index];
            button->went_up = false;
            button->went_down = false;
        }

        MSG message;
        while (PeekMessageA(&message, window, 0, 0, PM_REMOVE))
        {
            switch (message.message)
            {
            case WM_KEYUP:
            case WM_KEYDOWN:
            {
                DWORD key_code = message.wParam;
                bool key_went_up = (message.lParam & (1 << 31)) != 0;
                switch (key_code)
                {
                case VK_LEFT:
                {
                    handle_button(&input.left, key_went_up);
                }
                break;
                case VK_RIGHT:
                {
                    handle_button(&input.right, key_went_up);
                }
                break;
                case VK_DOWN:
                {
                    handle_button(&input.down, key_went_up);
                }
                break;
                case VK_UP:
                {
                    handle_button(&input.up, key_went_up);
                }
                break;
                case VK_SPACE:
                {
                    handle_button(&input.space, key_went_up);
                }
                break;
                }
            }
            break;
            default:
                DefWindowProc(window, message.message, message.wParam, message.lParam);
            }
        }

        game_update(game_screen, input);

        StretchDIBits(
            device_context,
            0,
            0,
            window_width,
            window_height,

            0,
            0,
            window_width,
            window_height,

            pixels,
            &bitmap_info,

            DIB_RGB_COLORS,
            SRCCOPY);

        f64 time_per_frame = win32_get_time() - prev_time;

        while (time_per_frame < target_time_per_frame)
        {
            time_per_frame = win32_get_time() - prev_time;
        }

        prev_time = win32_get_time();

        char buffer[256];
        sprintf_s(buffer, 256, "time_per_frame: %f; fps: %f\n", time_per_frame, 1 / time_per_frame);

        OutputDebugStringA(buffer);
    }
}