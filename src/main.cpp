#include "stdio.h"
#include "game.h"
#include "Windows.h"

void foo(const char *str)
{
    OutputDebugStringA(str);
}

typedef struct
{
    byte *data;
    u32 size;
} File_Buffer;

bool win32_write_entire_file(char *file_name)
{
    bool result = false;

    HANDLE file = CreateFileA(
        file_name,
        GENERIC_WRITE,
        NULL,
        NULL,
        CREATE_ALWAYS,
        NULL,
        NULL);

    assert(file != INVALID_HANDLE_VALUE);

    DWORD file_size_high;
    DWORD file_size = GetFileSize(file, &file_size_high);

    byte *file_memory = (byte *)malloc(file_size);
    DWORD bytes_written;

    if (WriteFile(file, file_memory, file_size, &bytes_written, NULL))
    {
        result = true;
    }

    return result;
}

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

//pack убирает промежутки между данными в struct
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

READ_BMP(win32_read_bmp)
{
    File_Buffer file = win32_read_entire_file(file_name);
    Bmp_Header *header = (Bmp_Header *)file.data;
    Bmp_Info *info = (Bmp_Info *)(file.data + sizeof(Bmp_Header));
    u32 *pixels = (u32 *)(file.data + header->data_offset);

    u64 alignment = 8 * sizeof(u32);
    u64 screen_buffer_size = 4 * (info->width + 2) * (info->height + 2);
    screen_buffer_size += alignment - (screen_buffer_size % alignment);

    u32 *new_pixels = (u32 *)_aligned_malloc(screen_buffer_size, alignment);
    memset(new_pixels, 0, screen_buffer_size);
    Bitmap result;
    result.pitch = info->width + 2;
    result.size = {(f32)info->width, (f32)info->height};

    u32 bitmap_start_offset = result.pitch + 1;

    for (u32 y = 0; y < info->height; y++)
    {
        for (u32 x = 0; x < info->width; x++)
        {
            ARGB pixel;
            pixel.argb = pixels[y * info->width + x];
            f32 alpha = (f32)(pixel.a / 0xFF);
            pixel.r = (u8)roundf(pixel.r * alpha);
            pixel.g = (u8)roundf(pixel.g * alpha);
            pixel.b = (u8)roundf(pixel.b * alpha);

            new_pixels[y * result.pitch + x + bitmap_start_offset] = pixel.argb;
        }
    }

    // AA RR GG BB
    result.pixels = new_pixels;

    free(file.data);

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
    case WM_ACTIVATE:
    {
        // if (wParam == WA_ACTIVE)
        // {
        //     SetLayeredWindowAttributes(window, RGB(0, 0, 0), 255, LWA_ALPHA);
        // }
        // else
        // {
        //     SetLayeredWindowAttributes(window, RGB(0, 0, 0), 50, LWA_ALPHA);
        // }
    }
    break;

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

void process_messages(HWND window, Input *input)
{
    for (i32 button_index = 0; button_index < BUTTON_COUNT; button_index++)
    {
        Button *button = &input->buttons[button_index];
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
            DWORD key_code = (DWORD)message.wParam;
            bool key_went_up = (message.lParam & (1 << 31)) != 0;
            switch (key_code)
            {
            case VK_LEFT:
            {
                handle_button(&input->left, key_went_up);
            }
            break;
            case VK_RIGHT:
            {
                handle_button(&input->right, key_went_up);
            }
            break;
            case VK_DOWN:
            {
                handle_button(&input->down, key_went_up);
            }
            break;
            case VK_UP:
            {
                handle_button(&input->up, key_went_up);
            }
            break;
            case 'Z':
            {
                handle_button(&input->z, key_went_up);
            }
            break;
            case 'X':
            {
                handle_button(&input->x, key_went_up);
            }
            break;
            case 'R':
            {
                handle_button(&input->r, key_went_up);
            }
            break;
            case 'L':
            {
                handle_button(&input->l, key_went_up);
            }
            break;
            case VK_SHIFT:
            {
                handle_button(&input->shift, key_went_up);
            }
            break;
            case VK_SPACE:
            {
                handle_button(&input->space, key_went_up);
            }
            break;
            case VK_F11:
            {
                handle_button(&input->F11, key_went_up);
            }
            break;
            }
        }
        break;
        default:
            DefWindowProc(window, message.message, message.wParam, message.lParam);
        }
    }
}

#define TASK_QUEUE_SIZE 1024

typedef struct
{
    void (*fn)(void *);
    void *data;
} Task;

typedef struct
{
    Task tasks[TASK_QUEUE_SIZE];
    u32 start_cursor;
    u32 end_cursor;
} Queue;

void add_task(Queue *queue, Task task)
{
    queue->tasks[queue->end_cursor] = task;
    queue->end_cursor = (queue->end_cursor + 1) % TASK_QUEUE_SIZE;
    assert(queue->end_cursor != queue->start_cursor);
}

Task *take_task(Queue *queue)
{
    Task *result = NULL;
    if (queue->end_cursor != queue->start_cursor)
    {
        result = &queue->tasks[queue->start_cursor];
        queue->start_cursor = (queue->start_cursor + 1) % TASK_QUEUE_SIZE;
    }
    return result;
}

// Queue queue = {0};

// DWORD WINAPI ThreadProc(LPVOID lpParameter)
// {
//     Queue *queue = (Queue *)lpParameter;
//     while (true)
//     {
//         Task *task = take_task(queue);
//         if (task)
//         {
//             task->fn(task->data);
//         }
//     }

//     return 0;
// };

// void print_smth(void *data)
// {
//     char buffer[256];
//     //sprintf_s(buffer, 256, "%d\n", (i32)data);

//     OutputDebugStringA(buffer);
// }

typedef struct
{
    FILETIME last_write_time;
    HMODULE game_code_dll;
    Game_Update *game_update;

    bool is_valid;
} win32_game_code;

FILETIME get_last_write_time(char *src)
{
    FILETIME last_write_time = {};

    WIN32_FIND_DATAA find_data;
    HANDLE find_handle = FindFirstFileA(src, &find_data);
    if (find_handle != INVALID_HANDLE_VALUE)
    {
        last_write_time = find_data.ftLastWriteTime;
        FindClose(find_handle);
    }
    return last_write_time;
}

win32_game_code win32_load_game_code(char *source_dll_name, char *temp_dll_name)
{
    win32_game_code result = {};

    result.last_write_time = get_last_write_time(source_dll_name);
    CopyFileA(source_dll_name, temp_dll_name, FALSE);
    result.game_code_dll = LoadLibraryA(temp_dll_name);
    if (result.game_code_dll)
    {
        result.game_update = (Game_Update *)GetProcAddress(result.game_code_dll, "game_update");

        result.is_valid = result.game_update;
    }

    if (!result.is_valid)
    {
        result.game_update = Game_Update_Stub;
    }

    return result;
}

void win32_unload_game_code(win32_game_code *game_code)
{
    if (game_code->game_code_dll)
    {
        FreeLibrary(game_code->game_code_dll);
        game_code->game_code_dll = 0;
    }
    game_code->is_valid = false;
    game_code->game_update = Game_Update_Stub;
}

void cat_strings(i64 source_a_count, char *source_a, i64 source_b_count, char *source_b, i64 dest_count, char *dest)
{
    for (i32 index = 0; index < source_a_count; index++)
    {
        *dest++ = *source_a++;
    }

    for (i32 index = 0; index < source_b_count; index++)
    {
        *dest++ = *source_b++;
    }

    *dest++ = 0;
}

typedef struct
{
    HANDLE recording_handle;
    HANDLE play_back_handle;
    i32 input_recording_index;
    i32 input_playing_index;
    Game_memory *memory;
    char exe_file_name[MAX_PATH];
    char *exe_file_one_past_last_slash;
} win32_State;

void get_file_path(win32_State *win32_state, char *file_name, i32 dest_count, char *dest)
{
    cat_strings(win32_state->exe_file_one_past_last_slash - win32_state->exe_file_name, win32_state->exe_file_name, string_length(file_name), file_name, dest_count, dest);
}

void begin_record_input(win32_State *win32_state, i32 input_recording_index)
{
    //открываем файл для записи движений
    win32_state->input_recording_index = input_recording_index;

    char file_name[256];
    sprintf_s(file_name, 256, "loop_edit%i.gmi", input_recording_index);

    char file_path[MAX_PATH];
    get_file_path(win32_state, file_name, sizeof(file_path), file_path);

    win32_state->recording_handle = CreateFileA(
        file_path,
        GENERIC_WRITE,
        NULL,
        NULL,
        CREATE_ALWAYS,
        NULL,
        NULL);

    DWORD bytes_written;
    WriteFile(win32_state->recording_handle, win32_state->memory, sizeof(Game_memory), &bytes_written, 0);
}

void end_record_input(win32_State *win32_state)
{
    //закрываем файл для записи движений
    CloseHandle(win32_state->recording_handle);
    win32_state->input_recording_index = 0;
}

void begin_input_play_back(win32_State *win32_state, i32 input_playing_index)
{
    //открываем файл для чтения движений
    win32_state->input_playing_index = input_playing_index;

    char file_name[256];
    sprintf_s(file_name, 256, "loop_edit%i.gmi", input_playing_index);

    char file_path[MAX_PATH];
    get_file_path(win32_state, file_name, sizeof(file_path), file_path);

    win32_state->play_back_handle = CreateFileA(
        file_path,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    DWORD bytes_read;
    assert(ReadFile(win32_state->play_back_handle, win32_state->memory, sizeof(Game_memory), &bytes_read, 0));
    assert(bytes_read == sizeof(Game_memory));
}

void end_input_play_back(win32_State *win32_state)
{
    //закрываем файл для чтения движений
    CloseHandle(win32_state->play_back_handle);
    win32_state->input_playing_index = 0;
}

void win32_record_input(win32_State *win32_state, Input *input)
{
    //продолжаем записывать движения
    DWORD bytes_written;
    WriteFile(win32_state->recording_handle, input, sizeof(Input), &bytes_written, 0);
}

void win32_play_back_input(win32_State *win32_state, Input *input)
{
    DWORD bytes_read;
    if (ReadFile(win32_state->play_back_handle, input, sizeof(Input), &bytes_read, 0))
    {
        if (bytes_read == 0)
        {
            //повторяем проход
            i32 playing_index = win32_state->input_playing_index;
            end_input_play_back(win32_state);
            begin_input_play_back(win32_state, playing_index);
            ReadFile(win32_state->play_back_handle, input, sizeof(Input), &bytes_read, 0);
        }
    }
}

void win32_get_exe_file_name(win32_State *win32_state)
{
    DWORD sizeof_file_name = GetModuleFileNameA(NULL, win32_state->exe_file_name, MAX_PATH);
    win32_state->exe_file_one_past_last_slash = win32_state->exe_file_name;
    for (char *scan = win32_state->exe_file_one_past_last_slash; *scan; scan++)
    {
        if (*scan == '\\')
        {
            win32_state->exe_file_one_past_last_slash = scan + 1;
        }
    }
}

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
            PSTR lpCmdLine, INT nCmdShow)
{
    win32_State win32_state = {};
    win32_state.memory = (Game_memory *)malloc(sizeof(Game_memory));
    memset(win32_state.memory, 0, sizeof(Game_memory));
    win32_state.memory->win32_read_bmp = &win32_read_bmp;
    win32_state.memory->__global_random_state = xorshift256_init(time(NULL));

    //файлы для динамичной перезагрузки
    win32_get_exe_file_name(&win32_state);
    char source_dll_full_path[MAX_PATH];
    get_file_path(&win32_state, "game.dll", sizeof(source_dll_full_path), source_dll_full_path);
    char temp_dll_full_path[MAX_PATH];
    get_file_path(&win32_state, "temp_game.dll", sizeof(temp_dll_full_path), temp_dll_full_path);

    LARGE_INTEGER perf_frequency_li;
    QueryPerformanceFrequency(&perf_frequency_li);
    performance_frequency = perf_frequency_li.QuadPart;

    // for (i32 num = 0; num <= 100; num++)
    // {
    //     Task task;
    //     task.fn = print_smth;
    //     task.data = (void *)num;
    //     add_task(&queue, task);
    // }

    // //поток
    // for (i32 i = 0; i < 50; i++)
    // {
    //     CreateThread(
    //         NULL,
    //         0,
    //         ThreadProc,
    //         &queue,
    //         0,
    //         0);
    // }

    WNDCLASSA wndClass = {0};
    wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndClass.lpfnWndProc = WindowProc;
    wndClass.hInstance = hInstance;
    wndClass.lpszClassName = "Auch";

    ATOM registeredClass = RegisterClassA(&wndClass);

    HWND window = CreateWindowExA(
        0,
        wndClass.lpszClassName,
        "A real game",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, //x
        CW_USEDEFAULT, //y
        CW_USEDEFAULT, //width
        CW_USEDEFAULT, //height
        NULL,
        NULL,
        wndClass.hInstance,
        NULL);

    HDC device_context = GetDC(window);
    ReleaseDC(window, device_context);

    RECT rect;
    i32 game_width = 512;
    i32 game_height = 288;

    BITMAPINFO bitmap_info = {0};
    bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmap_info.bmiHeader.biWidth = game_width;
    bitmap_info.bmiHeader.biHeight = game_height;
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = 32;
    bitmap_info.bmiHeader.biCompression = BI_RGB;

    u64 alignment = 8 * sizeof(u32);
    u64 screen_buffer_size = 4 * game_width * game_height;
    screen_buffer_size += alignment - (screen_buffer_size % alignment);

    u32 *pixels = (u32 *)_aligned_malloc(screen_buffer_size, alignment);

    Bitmap game_screen;
    game_screen.pixels = pixels;
    game_screen.size = {(f32)game_width, (f32)game_height};

    Input input = {0};

    f64 prev_time = win32_get_time();

    win32_game_code game_code = win32_load_game_code(source_dll_full_path, temp_dll_full_path);

    //переменная для getWindowPlacement и setWindowPlacement
    WINDOWPLACEMENT g_wpPrev = {sizeof(g_wpPrev)};

    while (running)
    {
        GetClientRect(window, &rect);
        i32 window_width = rect.right - rect.left;
        i32 window_height = rect.bottom - rect.top;

        FILETIME new_write_time = get_last_write_time(source_dll_full_path);
        if (new_write_time.dwLowDateTime != game_code.last_write_time.dwLowDateTime)
        {
            char buffer[256];
            sprintf_s(buffer, sizeof(buffer), "saved time: %ld; new time: %ld\n", game_code.last_write_time.dwLowDateTime, new_write_time.dwLowDateTime);
            OutputDebugStringA(buffer);
            win32_unload_game_code(&game_code);
            game_code = win32_load_game_code(source_dll_full_path, temp_dll_full_path);
        }

        process_messages(window, &input);

        if (input.F11.went_down)
        {
            //Raymond (How do I switch a window between normal and fullscreen?) https://devblogs.microsoft.com/oldnewthing/20100412-00/?p=14353 link in the description
            DWORD window_style = GetWindowLong(window, GWL_STYLE);
            if (window_style & WS_OVERLAPPEDWINDOW)
            {
                MONITORINFO mi = {sizeof(mi)};
                if (GetWindowPlacement(window, &g_wpPrev) &&
                    GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &mi))
                {
                    SetWindowLong(window, GWL_STYLE,
                                  window_style & ~WS_OVERLAPPEDWINDOW);
                    SetWindowPos(window, HWND_TOP,
                                 mi.rcMonitor.left, mi.rcMonitor.top,
                                 mi.rcMonitor.right - mi.rcMonitor.left,
                                 mi.rcMonitor.bottom - mi.rcMonitor.top,
                                 SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
                }
            }
            else
            {
                SetWindowLong(window, GWL_STYLE,
                              window_style | WS_OVERLAPPEDWINDOW);
                SetWindowPlacement(window, &g_wpPrev);
                SetWindowPos(window, NULL, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                                 SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
            }
        }

        if (input.l.went_down)
        {
            if (win32_state.input_recording_index == 0)
            {
                begin_record_input(&win32_state, 1);
            }
            else
            {
                end_record_input(&win32_state);
                begin_input_play_back(&win32_state, 1);
            }
        }

        if (win32_state.input_recording_index)
        {
            win32_record_input(&win32_state, &input);
        }
        if (win32_state.input_playing_index)
        {
            win32_play_back_input(&win32_state, &input);
        }

        game_code.game_update(game_screen, win32_state.memory, input);

        StretchDIBits(
            device_context,
            0,
            0,
            window_width,
            window_height,

            0,
            0,
            game_width,
            game_height,

            pixels,
            &bitmap_info,

            DIB_RGB_COLORS,
            SRCCOPY);

        f64 time_per_frame = win32_get_time() - prev_time;

        while (time_per_frame < TARGET_TIME_PER_FRAME)
        {
            time_per_frame = win32_get_time() - prev_time;
        }

        prev_time = win32_get_time();

        char buffer[256];
        sprintf_s(buffer, 256, "time_per_frame: %f; fps: %f\n", time_per_frame, 1 / time_per_frame);

        // OutputDebugStringA(buffer);
    }
}