#include "stdio.h"
#include "game.h"
#include "Windows.h"
#include "assets.h"

void foo(const char *str)
{
    OutputDebugStringA(str);
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
    case WM_ACTIVATEAPP:
    {
        DWORD window_ex_style = GetWindowLong(window, GWL_EXSTYLE);
        if (window_ex_style & WS_EX_LAYERED)
        {
            if (wParam)
            {
                SetLayeredWindowAttributes(window, RGB(0, 0, 0), 255, LWA_ALPHA);
            }
            else
            {
                SetLayeredWindowAttributes(window, RGB(0, 0, 0), 100, LWA_ALPHA);
            }
        }
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

f64 win32_get_time(void)
{
    LARGE_INTEGER ticks;
    QueryPerformanceCounter(&ticks);
    f64 result = (f64)ticks.QuadPart / (f64)performance_frequency;
    return result;
}

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

typedef enum
{
    Replay_State_NONE,
    Replay_State_RECORDING,
    Replay_State_REPLAYING,
} Replay_State;

typedef struct
{
    HANDLE handle;
    Replay_State replay_state;
    Game_memory *memory;
    char exe_file_name[MAX_PATH];
    char *exe_file_one_past_last_slash;
} win32_State;

void get_file_path(win32_State *win32_state, char *file_name, i32 dest_count, char *dest)
{
    cat_strings(win32_state->exe_file_one_past_last_slash - win32_state->exe_file_name, win32_state->exe_file_name, string_length(file_name), file_name, dest_count, dest);
}

void begin_record_input(win32_State *win32_state)
{
    //открываем файл для записи движений
    win32_state->replay_state = Replay_State_RECORDING;

    char file_name[256];
    sprintf_s(file_name, 256, "loop_edit.gmi");

    char file_path[MAX_PATH];
    get_file_path(win32_state, file_name, sizeof(file_path), file_path);

    win32_state->handle = CreateFileA(
        file_path,
        GENERIC_WRITE,
        NULL,
        NULL,
        CREATE_ALWAYS,
        NULL,
        NULL);

    DWORD bytes_written;
    WriteFile(win32_state->handle, win32_state->memory, sizeof(Game_memory), &bytes_written, 0);
}

void end_record_input(win32_State *win32_state)
{
    //закрываем файл для записи движений
    CloseHandle(win32_state->handle);
    win32_state->replay_state = Replay_State_NONE;
}

void begin_input_play_back(win32_State *win32_state)
{
    //открываем файл для чтения движений
    win32_state->replay_state = Replay_State_REPLAYING;

    char file_name[256];
    sprintf_s(file_name, 256, "loop_edit.gmi");

    char file_path[MAX_PATH];
    get_file_path(win32_state, file_name, sizeof(file_path), file_path);

    win32_state->handle = CreateFileA(
        file_path,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    DWORD bytes_read;
    assert(ReadFile(win32_state->handle, win32_state->memory, sizeof(Game_memory), &bytes_read, 0));
    assert(bytes_read == sizeof(Game_memory));
}

void end_input_play_back(win32_State *win32_state)
{
    //закрываем файл для чтения движений
    CloseHandle(win32_state->handle);
    win32_state->replay_state = Replay_State_NONE;
}

void win32_record_input(win32_State *win32_state, Input *input)
{
    //продолжаем записывать движения
    DWORD bytes_written;
    WriteFile(win32_state->handle, input, sizeof(Input), &bytes_written, 0);
}

void win32_play_back_input(win32_State *win32_state, Input *input)
{
    DWORD bytes_read;
    if (ReadFile(win32_state->handle, input, sizeof(Input), &bytes_read, 0))
    {
        if (bytes_read == 0)
        {
            //повторяем проход
            end_input_play_back(win32_state);
            begin_input_play_back(win32_state);
            ReadFile(win32_state->handle, input, sizeof(Input), &bytes_read, 0);
        }
    }
}

void loop_editing_render(win32_State *win32_state, Input *input)
{
    if (win32_state->replay_state == Replay_State_RECORDING)
    {
        win32_record_input(win32_state, input);
    }
    if (win32_state->replay_state == Replay_State_REPLAYING)
    {
        win32_play_back_input(win32_state, input);
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

void process_messages(HWND window, win32_State *win32_state, Input *input, WINDOWPLACEMENT *g_wpPrev)
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

                if (key_went_up)
                {

                    if (win32_state->replay_state == Replay_State_NONE)
                    {
                        begin_record_input(win32_state);
                    }
                    else if (win32_state->replay_state == Replay_State_RECORDING)
                    {
                        end_record_input(win32_state);
                    }
                }
            }
            break;
            case 'P':
            {
                handle_button(&input->p, key_went_up);

                if (key_went_up)
                {

                    if (win32_state->replay_state == Replay_State_NONE)
                    {
                        begin_input_play_back(win32_state);
                    }
                    else if (win32_state->replay_state == Replay_State_REPLAYING)
                    {
                        end_input_play_back(win32_state);
                        *input = {0};
                    }
                }
            }
            break;
            case 'F':
            {
                if (key_went_up)
                {
                    // Raymond (How do I switch a window between normal and fullscreen?) https://devblogs.microsoft.com/oldnewthing/20100412-00/?p=14353 link in the description
                    DWORD window_style = GetWindowLong(window, GWL_STYLE);
                    DWORD window_ex_style = GetWindowLong(window, GWL_EXSTYLE);
                    if (window_style & WS_OVERLAPPEDWINDOW)
                    {
                        MONITORINFO monitor_info = {sizeof(monitor_info)};
                        if (GetWindowPlacement(window, g_wpPrev) &&
                            GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &monitor_info))
                        {
                            SetWindowLong(window, GWL_STYLE,
                                          window_style & ~WS_OVERLAPPEDWINDOW);
                            SetWindowLong(window, GWL_EXSTYLE,
                                          window_ex_style & ~WS_EX_LAYERED);

                            SetWindowPos(window, HWND_NOTOPMOST,
                                         monitor_info.rcMonitor.left, monitor_info.rcMonitor.top,
                                         monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
                                         monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
                                         SWP_FRAMECHANGED);
                        }
                    }
                    else
                    {
                        SetWindowLong(window, GWL_STYLE,
                                      window_style | WS_OVERLAPPEDWINDOW);
                        SetWindowLong(window, GWL_EXSTYLE,
                                      window_ex_style | WS_EX_LAYERED);
                        SetWindowPlacement(window, g_wpPrev);
                        SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0,
                                     SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
                    }
                }
            }
            break;
            case 'T':
            {
                handle_button(&input->t, key_went_up);
            }
            break;
            case 'S':
            {
                handle_button(&input->s, key_went_up);
            }
            break;
            case 'A':
            {
                handle_button(&input->a, key_went_up);
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
            case VK_ESCAPE:
            {
                running = false;
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

void win32_add_entry(Platform_work_queue *queue, Platform_work_queue_callback callback, void *data)
{
    u32 new_next_entry_to_write = (queue->next_entry_to_write + 1) % ARRAY_COUNT(queue->entries);
    assert(new_next_entry_to_write != queue->next_entry_to_read);
    Platform_work_queue_entry *entry = &queue->entries[queue->next_entry_to_write];
    entry->data = data;
    entry->callback = callback;
    _WriteBarrier();
    // _mm_sfence();
    queue->completion_goal++;
    queue->next_entry_to_write = new_next_entry_to_write;
    ReleaseSemaphore(queue->semaphore_handle, 1, 0);
}

bool win32_do_next_queue_entry(Platform_work_queue *queue)
{
    bool we_should_sleep = false;

    u32 original_next_entry_to_read = queue->next_entry_to_read;
    u32 new_next_entry_to_read = (original_next_entry_to_read + 1) % ARRAY_COUNT(queue->entries);
    if (original_next_entry_to_read != queue->next_entry_to_write)
    {
        u32 index = (u32)InterlockedCompareExchange((LONG volatile *)&queue->next_entry_to_read, new_next_entry_to_read, original_next_entry_to_read);
        if (index == original_next_entry_to_read)
        {
            Platform_work_queue_entry entry = queue->entries[index];

            entry.callback(entry.data);

            InterlockedIncrement((u64 volatile *)&queue->completion_count);
        }
    }
    else
    {
        we_should_sleep = true;
    }

    return we_should_sleep;
}

void win32_complete_all_work(Platform_work_queue *queue)
{
    while (queue->completion_goal != queue->completion_count)
    {
        win32_do_next_queue_entry(queue);
    }

    queue->completion_count = 0;
    queue->completion_goal = 0;
}

typedef struct
{
    i32 logical_thread_index;
    Platform_work_queue *queue;
} win32_thread_info;

DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
    win32_thread_info *thread_info = (win32_thread_info *)lpParameter;

    while (running)
    {
        if (win32_do_next_queue_entry(thread_info->queue))
        {
            WaitForSingleObjectEx(thread_info->queue->semaphore_handle, INFINITE, FALSE);
        }
    }

    return 0;
};

PLATFORM_WORK_QUEUE_CALLBACK(do_worker_work)
{
    char buffer[256];
    sprintf_s(buffer, "Thread %u: %s\n", GetCurrentThreadId(), (char *)data);
    OutputDebugStringA(buffer);
}

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
            PSTR lpCmdLine, INT nCmdShow)
{
    win32_State win32_state = {};
    win32_state.memory = (Game_memory *)malloc(sizeof(Game_memory));
    memset(win32_state.memory, 0, sizeof(Game_memory));
    win32_state.memory->win32_read_bmp = &win32_read_bmp;
    win32_state.memory->stbtt_read_font = &stbtt_read_font;
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

    // multithreading
    Platform_work_queue queue = {};

    win32_thread_info info[THREADS_COUNT] = {};
    i32 thread_count = ARRAY_COUNT(info);
    i32 initial_count = 0;

    queue.semaphore_handle = CreateSemaphoreExA(
        0,
        initial_count,
        thread_count,
        0,
        0,
        SEMAPHORE_ALL_ACCESS);

    for (i32 thread_index = 0; thread_index < thread_count; thread_index++)
    {
        info[thread_index].logical_thread_index = thread_index;
        info[thread_index].queue = &queue;

        CreateThread(0, 0, ThreadProc, &info[thread_index], 0, 0);
    }

    // win32_add_entry(&queue, do_worker_work, "String 0\n");
    // win32_add_entry(&queue, do_worker_work, "String 1\n");
    // win32_add_entry(&queue, do_worker_work, "String 2\n");
    // win32_add_entry(&queue, do_worker_work, "String 3\n");
    // win32_add_entry(&queue, do_worker_work, "String 4\n");
    // win32_add_entry(&queue, do_worker_work, "String 5\n");
    // win32_add_entry(&queue, do_worker_work, "String 6\n");
    // win32_add_entry(&queue, do_worker_work, "String 7\n");
    // win32_add_entry(&queue, do_worker_work, "String 8\n");

    // win32_complete_all_work(&queue);

    win32_state.memory->platform_add_work_queue_entry = win32_add_entry;
    win32_state.memory->platform_complete_all_work = win32_complete_all_work;
    win32_state.memory->high_priority_queue = &queue;

    WNDCLASSA wndClass = {0};
    wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndClass.lpfnWndProc = WindowProc;
    wndClass.hInstance = hInstance;
    wndClass.lpszClassName = "Auch";

    ATOM registeredClass = RegisterClassA(&wndClass);

    HWND window = CreateWindowExA(
        WS_EX_LAYERED | WS_EX_TOPMOST,
        wndClass.lpszClassName,
        "A real game",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        920, // x
        380, // y
        512, // width
        325, // height
        NULL,
        NULL,
        wndClass.hInstance,
        NULL);

    HDC device_context = GetDC(window);
    ReleaseDC(window, device_context);

    RECT rect;
    i32 game_width = 960;
    i32 game_height = 540;

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
        i32 window_height = rect.bottom - rect.top;
        i32 window_width = window_height / 9 * 16;

        FILETIME new_write_time = get_last_write_time(source_dll_full_path);
        if (new_write_time.dwLowDateTime != game_code.last_write_time.dwLowDateTime)
        {
            char buffer[256];
            sprintf_s(buffer, sizeof(buffer), "saved time: %ld; new time: %ld\n", game_code.last_write_time.dwLowDateTime, new_write_time.dwLowDateTime);
            OutputDebugStringA(buffer);
            win32_unload_game_code(&game_code);
            game_code = win32_load_game_code(source_dll_full_path, temp_dll_full_path);
        }

        process_messages(window, &win32_state, &input, &g_wpPrev);

        loop_editing_render(&win32_state, &input);

        game_code.game_update(game_screen, win32_state.memory, input);

        SetStretchBltMode(device_context, COLORONCOLOR);

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

        OutputDebugStringA(buffer);
    }
}