#include "math.h"
#include "game_math.c"
#include <malloc.h>
#include <string.h>

#define PI 3.14159265359

#define TILE_SIZE_PIXELS 80
#define CHUNK_SIZE_X 10
#define CHUNK_SIZE_Y 8
#define CHUNK_COUNT_X 4
#define CHUNK_COUNT_Y 4
#define BORDER_SIZE 2

#define SPRITE_SCALE 5

//fps
f64 target_time_per_frame = 1.0f / 60.0f;
f32 dt = (f32)target_time_per_frame;

//таймеры
i32 timers[512];
i32 timers_count = 0;

i32 add_timer(i32 time)
{
    timers[timers_count] = time;
    timers_count++;
    return timers_count - 1;
}

void update_timers()
{
    for (i32 i = 0; i < timers_count; i++)
    {
        timers[i]--;
    }
}

//ввод
typedef struct
{
    bool is_down;
    bool went_down;
    bool went_up;
} Button;

#define BUTTON_COUNT 9

typedef union
{
    struct
    {
        Button left;
        Button right;
        Button up;
        Button down;
        Button z;
        Button x;
        Button r;
        Button shift;
        Button space;
    };
    Button buttons[BUTTON_COUNT];
} Input;

//получение картинки
typedef struct
{
    u32 *pixels;
    V2 size;
    i32 pitch;
} Bitmap;

Bitmap win32_read_bmp(char *file_name);

//вставляем картинки
Bitmap img_None = win32_read_bmp("../data/none.bmp");

Bitmap img_Test = win32_read_bmp("../data/test.bmp");

Bitmap img_PlayerIdle = win32_read_bmp("../data/lexaIdle.bmp");
Bitmap img_PlayerJump = win32_read_bmp("../data/lexaJump.bmp");
Bitmap img_PlayerCrouchIdle = win32_read_bmp("../data/lexaCrouchIdle.bmp");
Bitmap img_PlayerHanging = win32_read_bmp("../data/lexaWall.bmp");
Bitmap img_PlayerHangingUp = win32_read_bmp("../data/lexaWallUp.bmp");
Bitmap img_PlayerHangingDown = win32_read_bmp("../data/lexaWallDown.bmp");
Bitmap img_PlayerLookingUp = win32_read_bmp("../data/lexaLookingUp.bmp");

Bitmap img_PlayerStep[6] = {
    win32_read_bmp("../data/lexaStep1.bmp"),
    win32_read_bmp("../data/lexaStep2.bmp"),
    win32_read_bmp("../data/lexaStep3.bmp"),
    win32_read_bmp("../data/lexaStep4.bmp"),
    win32_read_bmp("../data/lexaStep5.bmp"),
    win32_read_bmp("../data/lexaStep6.bmp"),
};

Bitmap img_PlayerStartsCrouching[4] = {
    win32_read_bmp("../data/lexaCrouchIdle1.bmp"),
    win32_read_bmp("../data/lexaCrouchIdle2.bmp"),
    win32_read_bmp("../data/lexaCrouchIdle3.bmp"),
    win32_read_bmp("../data/lexaCrouchIdle4.bmp"),
};

Bitmap img_PlayerCrouchStep[6] = {
    win32_read_bmp("../data/lexaCrouchStep1.bmp"),
    win32_read_bmp("../data/lexaCrouchStep2.bmp"),
    win32_read_bmp("../data/lexaCrouchStep3.bmp"),
    win32_read_bmp("../data/lexaCrouchStep4.bmp"),
    win32_read_bmp("../data/lexaCrouchStep5.bmp"),
    win32_read_bmp("../data/lexaCrouchStep6.bmp"),
};

Bitmap img_ZombieIdle = win32_read_bmp("../data/zombieIdle.bmp");

Bitmap img_ZombieStep[6] = {
    win32_read_bmp("../data/zombieStep1.bmp"),
    win32_read_bmp("../data/zombieStep2.bmp"),
    win32_read_bmp("../data/zombieStep3.bmp"),
    win32_read_bmp("../data/zombieStep4.bmp"),
    win32_read_bmp("../data/zombieStep5.bmp"),
    win32_read_bmp("../data/zombieStep6.bmp"),
};

Bitmap img_BackGround = win32_read_bmp("../data/backGround.bmp");

Bitmap img_Border = win32_read_bmp("../data/border.bmp");

Bitmap img_Bricks[12] = {
    win32_read_bmp("../data/brick1.bmp"),
    win32_read_bmp("../data/brick2.bmp"),
    win32_read_bmp("../data/brick3.bmp"),
    win32_read_bmp("../data/brick4.bmp"),
    win32_read_bmp("../data/brick5.bmp"),
    win32_read_bmp("../data/brick6.bmp"),
    win32_read_bmp("../data/brick7.bmp"),
    win32_read_bmp("../data/brick8.bmp"),
    win32_read_bmp("../data/brick9.bmp"),
    win32_read_bmp("../data/brick10.bmp"),
    win32_read_bmp("../data/brick11.bmp"),
    win32_read_bmp("../data/brick12.bmp"),
};
Bitmap img_ElegantBrick = win32_read_bmp("../data/elegantBrick.bmp");

Bitmap img_Stone = win32_read_bmp("../data/stone.bmp");

Bitmap img_MarbleFloor = win32_read_bmp("../data/floor1.bmp");

Bitmap img_TiledFloor = win32_read_bmp("../data/floor5.bmp");

Bitmap img_Door[7] = {
    win32_read_bmp("../data/door1.bmp"),
    win32_read_bmp("../data/door2.bmp"),
    win32_read_bmp("../data/door3.bmp"),
    win32_read_bmp("../data/door4.bmp"),
    win32_read_bmp("../data/door5.bmp"),
    win32_read_bmp("../data/door6.bmp"),
    win32_read_bmp("../data/door7.bmp"),
};

Bitmap img_DoorBack = win32_read_bmp("../data/doorBack.bmp");

Bitmap img_Parapet = win32_read_bmp("../data/parapet.bmp");

Bitmap img_HealthBar = win32_read_bmp("../data/health_bar.bmp");
Bitmap img_Health = win32_read_bmp("../data/health.bmp");

Bitmap img_Spikes = win32_read_bmp("../data/spikes.bmp");

//получение перевёрнутых картинок
Bitmap turn_bitmap(Bitmap bitmap, f64 angle)
{
    Bitmap result;
    result.pitch = bitmap.pitch;
    result.size = bitmap.size;

    u64 alignment = 8 * sizeof(u32);
    u64 screen_buffer_size = 4 * (bitmap.size.x + 2) * (bitmap.size.y + 2);
    screen_buffer_size += alignment - (screen_buffer_size % alignment);
    u32 *pixels = (u32 *)_aligned_malloc(screen_buffer_size, alignment);
    memset(pixels, 0, screen_buffer_size);

    result.pixels = pixels;

    for (i32 y = 0; y < bitmap.size.y + 2; y++)
    {
        for (i32 x = 0; x < bitmap.size.x + 2; x++)
        {
            i32 pixel = bitmap.pixels[y * bitmap.pitch + x];
            if (angle == 0)
            {
                result.pixels[y * result.pitch + x] = pixel;
            }
            if (angle == PI * 0.5)
            {
                result.pixels[x * ((i32)result.size.x + 2) + ((i32)result.size.y + 1 - y)] = pixel;
            }
            if (angle == PI)
            {
                result.pixels[((i32)result.size.y + 1 - y) * result.pitch + ((i32)result.size.x + 1 - x)] = pixel;
            }
            if (angle == PI * 1.5)
            {
                result.pixels[((i32)result.size.x + 1 - x) * ((i32)result.size.x + 2) + y] = pixel;
            }
        }
    }
    return result;
}

Bitmap img_TransitionBorder_0 = win32_read_bmp("../data/borderWithTransition.bmp");
Bitmap img_TransitionBorder_05PI = turn_bitmap(img_TransitionBorder_0, 0.5 * PI);
Bitmap img_TransitionBorder_PI = turn_bitmap(img_TransitionBorder_0, PI);
Bitmap img_TransitionBorder_15PI = turn_bitmap(img_TransitionBorder_0, 1.5 * PI);

Bitmap img_CornerBorder_0 = win32_read_bmp("../data/cornerBorder.bmp");
Bitmap img_CornerBorder_05PI = turn_bitmap(img_CornerBorder_0, 0.5 * PI);
Bitmap img_CornerBorder_PI = turn_bitmap(img_CornerBorder_0, PI);
Bitmap img_CornerBorder_15PI = turn_bitmap(img_CornerBorder_0, 1.5 * PI);

//камера
typedef struct
{
    V2 pos;
    V2 target;
    V2 scale;
} Camera;

Camera camera = {
    {0, 0},
    {0, 0},
    {0, 0},
};

V2 world_to_screen(Bitmap screen, Camera camera, V2 p)
{
    V2 result = (p - camera.pos) * camera.scale + screen.size * 0.5f;
    return result;
}

//прорисовка

typedef enum
{
    DRAWING_TYPE_RECT,
    DRAWING_TYPE_BITMAP,
    DRAWING_TYPE_OLD_BITMAP,
} Drawing_Type;

typedef enum
{
    LAYER_BACKGROUND_MAIN,
    LAYER_TILE,
    LAYER_ON_BACKGROUND,
    LAYER_ON_ON_BACKGROUND,
    LAYER_GAME_OBJECT,
    LAYER_FORGROUND,
    LAYER_DARKNESS,
    LAYER_UI,
} Layer;

typedef struct
{
    Drawing_Type type;
    V2 pos;
    V2 size;
    f32 angle;
    f32 alpha;
    u32 color;
    Bitmap bitmap;
    Layer layer;
} Drawing;

Drawing draw_queue[1024 * 8];
i32 draw_queue_size = 0;

//drawing
void draw_rect(V2 pos, V2 size, f32 angle, u32 color, Layer layer)
{
    Drawing drawing;
    drawing.type = DRAWING_TYPE_RECT;
    drawing.pos = pos;
    drawing.size = size;
    drawing.angle = angle;
    drawing.color = color;
    drawing.bitmap = img_None;
    drawing.layer = layer;
    assert(draw_queue_size < 1024 * 8);
    draw_queue[draw_queue_size] = drawing;
    draw_queue_size++;
}

void draw_bitmap(V2 pos, V2 size, f32 angle, Bitmap bitmap, f32 alpha, Layer layer)
{
    Drawing drawing;
    drawing.type = DRAWING_TYPE_BITMAP;
    drawing.pos = pos;
    drawing.size = size;
    drawing.angle = angle;
    drawing.color = NULL;
    drawing.bitmap = bitmap;
    drawing.layer = layer;
    drawing.alpha = alpha;
    assert(draw_queue_size < 1024 * 8);
    draw_queue[draw_queue_size] = drawing;
    draw_queue_size++;
}

//очищение экрана и затемнение среды
bool transition_is_on = false;
#define DARKNESS_USUAL_LVL 0.9
f32 darkness_lvl = DARKNESS_USUAL_LVL;

void clear_screen(Bitmap screen, Bitmap darkness)
{
    i32 pixelCount = screen.size.x * screen.size.y;
    for (i32 i = 0; i < pixelCount; i++)
    {
        screen.pixels[i] = 0;
    }

    //обновление темноты
    for (i32 y = 0; y <= darkness.size.y; y++)
    {
        for (i32 x = 0; x <= darkness.size.x; x++)
        {
            ARGB dark_pixel = {0xFF000000};
            dark_pixel.a *= darkness_lvl;
            darkness.pixels[y * darkness.pitch + x] = dark_pixel.argb;
        }
    }
}

//границы камеры
void border_camera(Bitmap screen)
{
    if (!(camera.target.x - screen.size.x / camera.scale.x * 0.5 > -TILE_SIZE_PIXELS * 0.5))
    {
        camera.target.x = -TILE_SIZE_PIXELS * 0.5 + screen.size.x / camera.scale.x * 0.5;
    }

    if (!(camera.target.x + screen.size.x / camera.scale.x * 0.5 < -TILE_SIZE_PIXELS * 0.5 + TILE_SIZE_PIXELS * CHUNK_COUNT_X * CHUNK_SIZE_X + 2 * BORDER_SIZE * TILE_SIZE_PIXELS))
    {
        camera.target.x = -TILE_SIZE_PIXELS * 0.5 + TILE_SIZE_PIXELS * CHUNK_COUNT_X * CHUNK_SIZE_X + 2 * BORDER_SIZE * TILE_SIZE_PIXELS - screen.size.x / camera.scale.x * 0.5;
    }

    if (!(camera.target.y - screen.size.y / camera.scale.y * 0.5 > -TILE_SIZE_PIXELS * 0.5))
    {
        camera.target.y = -TILE_SIZE_PIXELS * 0.5 + screen.size.y / camera.scale.y * 0.5;
    }

    if (!(camera.target.y + screen.size.y / camera.scale.y * 0.5 < -TILE_SIZE_PIXELS * 0.5 + TILE_SIZE_PIXELS * CHUNK_COUNT_Y * CHUNK_SIZE_Y + 2 * BORDER_SIZE * TILE_SIZE_PIXELS))
    {
        camera.target.y = -TILE_SIZE_PIXELS * 0.5 + TILE_SIZE_PIXELS * CHUNK_COUNT_Y * CHUNK_SIZE_Y + 2 * BORDER_SIZE * TILE_SIZE_PIXELS - screen.size.y / camera.scale.y * 0.5;
    }
}

//тайлы
typedef enum
{
    Tile_Type_NONE,
    Tile_Type_NORMAL,
    Tile_Type_FLOOR,
    Tile_Type_BORDER,
    Tile_Type_ENTER,
    Tile_Type_EXIT,
    Tile_Type_PARAPET,
    Tile_Type_SPIKES,
} Tile_type;

typedef struct
{
    Tile_type type;
    Bitmap sprite;
    bool solid;
    i32 interactive;
} Tile;

Tile *tile_map = NULL;

V2 get_tile_pos(i32 index)
{
    //size_x = CHUNK_SIZE_X * CHUNK_COUNT_X + BORDER_SIZE * 2;
    //index = y * (CHUNK_SIZE_X * CHUNK_COUNT_X + BORDER_SIZE * 2) + x;
    f32 y = floor(index / (CHUNK_SIZE_X * CHUNK_COUNT_X + BORDER_SIZE * 2));
    f32 x = index - y * (CHUNK_SIZE_X * CHUNK_COUNT_X + BORDER_SIZE * 2);
    V2 result = {x, y};
    return result;
}

i32 get_index(V2 coords)
{
    i32 index = coords.y * (CHUNK_SIZE_X * CHUNK_COUNT_X + BORDER_SIZE * 2) + coords.x;
    return index;
}

//смешивание цветов
typedef struct
{
    V4 a, b, c, d;
} Bilinear_Sample;

typedef struct
{
    V4_8x a, b, c, d;
} Bilinear_Sample_8x;

Bitmap create_empty_bitmap(V2 size)
{
    Bitmap result;
    result.size = size;
    result.pitch = size.x + 2;
    result.pixels = (u32 *)malloc((u32)(result.pitch * (size.y + 2) * sizeof(u32)));
    return result;
}

Bilinear_Sample get_bilinear_sample(Bitmap bitmap, V2 pos)
{
    V4 a = argb_to_v4({bitmap.pixels[(i32)pos.y * bitmap.pitch + (i32)pos.x]});
    V4 b = argb_to_v4({bitmap.pixels[(i32)pos.y * bitmap.pitch + (i32)pos.x + 1]});
    V4 c = argb_to_v4({bitmap.pixels[((i32)pos.y + 1) * bitmap.pitch + (i32)pos.x]});
    V4 d = argb_to_v4({bitmap.pixels[((i32)pos.y + 1) * bitmap.pitch + (i32)pos.x + 1]});

    Bilinear_Sample result = {a, b, c, d};
    return result;
}

Bilinear_Sample_8x get_bilinear_sample(Bitmap bitmap, V2_8x pos, i32_8x mask)
{
    i32_8x x = to_i32_8x(pos.x);
    i32_8x y = to_i32_8x(pos.y);
    i32_8x offsets = to_i32_8x(pos.y * set1_f32(bitmap.pitch) + pos.x);
    i32_8x pixel_a = gather((const int *)bitmap.pixels, offsets, mask);
    i32_8x pixel_b = gather((const int *)bitmap.pixels + 1, offsets, mask);
    i32_8x pixel_c = gather((const int *)bitmap.pixels + bitmap.pitch, offsets, mask);
    i32_8x pixel_d = gather((const int *)bitmap.pixels + bitmap.pitch + 1, offsets, mask);

    V4_8x a = argb_to_v4_8x(pixel_a);
    V4_8x b = argb_to_v4_8x(pixel_b);
    V4_8x c = argb_to_v4_8x(pixel_c);
    V4_8x d = argb_to_v4_8x(pixel_d);

    Bilinear_Sample_8x result = {a, b, c, d};
    return result;
}

V4 bilinear_blend(Bilinear_Sample sample, V2 f)
{
    V4 ab = lerp(sample.a, sample.b, f.x);
    V4 cd = lerp(sample.c, sample.d, f.x);
    V4 abcd = lerp(ab, cd, f.y);
    return abcd;
}

V4_8x bilinear_blend(Bilinear_Sample_8x sample, V2_8x f)
{
    V4_8x ab = lerp(sample.a, sample.b, f.x);
    V4_8x cd = lerp(sample.c, sample.d, f.x);
    V4_8x abcd = lerp(ab, cd, f.y);
    return abcd;
}

void foo(const char *);

struct Timed_Scope
{
    u64 stamp;
    char *name;
    u64 times;

    ~Timed_Scope()
    {
        f64 time_since = (f64)(__rdtsc() - stamp) / times;
        char buffer[256];
        sprintf_s(buffer, 256, "%s: %.02f\n", name, time_since);
        foo(buffer);
    }
};

// #define TIMED_BLOCK(name, times) Timed_Scope __scope_##name = {__rdtsc(), #name, times};

void draw_item(Bitmap screen, Drawing drawing)
{
    if (drawing.type == DRAWING_TYPE_RECT)
    {
        Rect screen_rect = {{0, 0}, {screen.size.x, screen.size.y}};

        V2 rect_size = drawing.size;
        if (rect_size.x >= 0)
            rect_size.x += 1;
        else
            rect_size.x -= 1;
        if (rect_size.y >= 0)
            rect_size.y += 1;
        else
            rect_size.y -= 1;

        V2 x_axis = rotate_vector({drawing.size.x, 0}, drawing.angle);
        V2 y_axis = rotate_vector({0, drawing.size.y}, drawing.angle);
        V2 origin = drawing.pos - x_axis * 0.5 - y_axis * 0.5;

        V2 vertices[] = {
            origin,
            origin + x_axis,
            origin + y_axis,
            origin + x_axis + y_axis,
        };

        Rect drawn_rect = {{INFINITY, INFINITY}, {-INFINITY, -INFINITY}};
        for (u32 vertex_index = 0; vertex_index < 4; vertex_index++)
        {
            V2 vertex = vertices[vertex_index];

            drawn_rect.min.x = min(drawn_rect.min.x, vertex.x);
            drawn_rect.min.y = min(drawn_rect.min.y, vertex.y);
            drawn_rect.max.x = max(drawn_rect.max.x, vertex.x);
            drawn_rect.max.y = max(drawn_rect.max.y, vertex.y);
        }

        Rect paint_rect = intersect(screen_rect, drawn_rect);

        V2 inverted_sqr_rect_size = 1 / (rect_size * rect_size);

        for (i32 y = paint_rect.min.y; y < paint_rect.max.y; y++)
        {
            for (i32 x = paint_rect.min.x; x < paint_rect.max.x; x++)
            {
                V2 d = V2{(f32)x, (f32)y} - origin;
                V2 uv01 = V2{dot(d, x_axis), dot(d, y_axis)} * inverted_sqr_rect_size;

                if (uv01.x >= 0 && uv01.x < 1 && uv01.y >= 0 && uv01.y < 1)
                {
                    screen.pixels[y * (i32)screen.size.x + x] = drawing.color;
                }
            }
        }
    }

    if (drawing.type == DRAWING_TYPE_OLD_BITMAP)
    {

        bool is_tile = drawing.layer == LAYER_TILE || drawing.layer == LAYER_BACKGROUND_MAIN;

        Rect screen_rect = {{0, 0}, {screen.size.x, screen.size.y}};

        V2 rect_size = drawing.size;
        if (rect_size.x >= 0)
            rect_size.x += 1;
        else
            rect_size.x -= 1;
        if (rect_size.y >= 0)
            rect_size.y += 1;
        else
            rect_size.y -= 1;

        V2 x_axis = rotate_vector({rect_size.x, 0}, drawing.angle);
        V2 y_axis = rotate_vector({0, rect_size.y}, drawing.angle);
        V2 origin = drawing.pos - x_axis * 0.5 - y_axis * 0.5;

        V2 vertices[] = {
            origin,
            origin + x_axis,
            origin + y_axis,
            origin + x_axis + y_axis,
        };

        Rect drawn_rect = {{INFINITY, INFINITY}, {-INFINITY, -INFINITY}};
        for (u32 vertex_index = 0; vertex_index < 4; vertex_index++)
        {
            V2 vertex = vertices[vertex_index];

            drawn_rect.min.x = min(drawn_rect.min.x, vertex.x);
            drawn_rect.min.y = min(drawn_rect.min.y, vertex.y);
            drawn_rect.max.x = max(drawn_rect.max.x, vertex.x);
            drawn_rect.max.y = max(drawn_rect.max.y, vertex.y);
        }

        Rect paint_rect = intersect(screen_rect, drawn_rect);

        V2 pixel_scale = abs(drawing.size) / drawing.bitmap.size;
        V2 texture_size = drawing.bitmap.size + 1 / pixel_scale;

        V2 inverted_sqr_rect_size = 1 / (rect_size * rect_size);
        if (has_area(paint_rect))
        {
            // TIMED_BLOCK(draw_pixel_slowly, (u64)get_area(paint_rect));

            for (i32 y = paint_rect.min.y; y < paint_rect.max.y; y++)
            {
                for (i32 x = paint_rect.min.x; x < paint_rect.max.x; x++)
                {
                    V2 d = V2{(f32)x, (f32)y} - origin;
                    V2 uv01 = V2{dot(d, x_axis), dot(d, y_axis)} * inverted_sqr_rect_size;
                    if (uv01.x >= 0 && uv01.x < 1 && uv01.y >= 0 && uv01.y < 1)
                    {
                        V2 uv = uv01 * (texture_size);
                        V2 uv_floored = clamp(floor(uv), V2{0, 0}, drawing.bitmap.size);
                        V2 uv_fract = clamp01(fract(uv) * pixel_scale);

                        Bilinear_Sample sample = get_bilinear_sample(drawing.bitmap, V2(uv_floored));
                        V4 texel = bilinear_blend(sample, uv_fract);
                        V4 pixel = argb_to_v4({screen.pixels[y * (i32)screen.size.x + x]});

                        texel = texel * drawing.alpha;

                        f32 inverted_alpha = (1 - texel.a * (1 - is_tile));
                        V4 result = inverted_alpha * pixel + texel;

                        screen.pixels[y * (i32)screen.size.x + x] = v4_to_argb(result).argb;
                    }
                }
            }
        }
    }

    if (drawing.type == DRAWING_TYPE_BITMAP)
    {
        bool is_tile = drawing.layer == LAYER_TILE || drawing.layer == LAYER_BACKGROUND_MAIN;
        f32_8x is_tile_multiplier = set1_f32(1 - is_tile);

        Rect screen_rect = {{0, 0}, {screen.size.x, screen.size.y}};

        V2 rect_size = drawing.size;
        if (rect_size.x >= 0)
            rect_size.x += 1;
        else
            rect_size.x -= 1;
        if (rect_size.y >= 0)
            rect_size.y += 1;
        else
            rect_size.y -= 1;

        V2 x_axis = rotate_vector({rect_size.x, 0}, drawing.angle);
        V2 y_axis = rotate_vector({0, rect_size.y}, drawing.angle);
        V2 origin = drawing.pos - x_axis * 0.5 - y_axis * 0.5;

        V2 vertices[] = {
            origin,
            origin + x_axis,
            origin + y_axis,
            origin + x_axis + y_axis,
        };

        Rect drawn_rect = {{INFINITY, INFINITY}, {-INFINITY, -INFINITY}};
        for (u32 vertex_index = 0; vertex_index < 4; vertex_index++)
        {
            V2 vertex = vertices[vertex_index];

            drawn_rect.min.x = floorf(min(drawn_rect.min.x, vertex.x));
            drawn_rect.min.y = floorf(min(drawn_rect.min.y, vertex.y));
            drawn_rect.max.x = ceilf(max(drawn_rect.max.x, vertex.x));
            drawn_rect.max.y = ceilf(max(drawn_rect.max.y, vertex.y));
        }

        Rect paint_rect = intersect(screen_rect, drawn_rect);

        if ((i32)paint_rect.min.x & 7)
        {
            paint_rect.min.x = (i32)paint_rect.min.x & ~7;
        }

        if ((i32)paint_rect.max.x & 7)
        {
            paint_rect.max.x = ((i32)paint_rect.max.x & ~7) + 8;
        }

        V2 pixel_scale = abs(drawing.size) / drawing.bitmap.size;
        V2 texture_size = drawing.bitmap.size + 1 / pixel_scale;
        V2 inverted_sqr_rect_size = 1 / (rect_size * rect_size);

        V2_8x x_axis_8x = set1(x_axis);
        V2_8x y_axis_8x = set1(y_axis);
        V2_8x inverted_sqr_rect_size_8x = set1(inverted_sqr_rect_size);
        V2_8x origin_8x = set1(origin);
        f32_8x zero_to_seven = set8_f32(0, 1, 2, 3, 4, 5, 6, 7);
        V2_8x texture_size_8x = set1(texture_size);
        V2_8x zero_vector_8x = set1(V2{0, 0});
        V2_8x bitmap_size_8x = set1(drawing.bitmap.size);
        V2_8x pixel_scale_8x = set1(pixel_scale);
        f32_8x one_8x = set1_f32(1.0f);
        f32_8x zero_8x = set1_f32(0);
        f32_8x eight_8x = set1_f32(8.0f);
        f32_8x alpha_8x = set1_f32(drawing.alpha);

        if (has_area(paint_rect))
        {
            // TIMED_BLOCK(draw_pixel, (u64)get_area(paint_rect));
            for (i32 y = paint_rect.min.y; y < paint_rect.max.y; y++)
            {
                u32 *pixel_ptr = screen.pixels + y * (i32)screen.size.x + (i32)paint_rect.min.x;
                V2_8x d = V2_8x{
                              set1_f32(paint_rect.min.x) + zero_to_seven,
                              set1_f32(y)} -
                          origin_8x;

                for (i32 x = paint_rect.min.x; x < paint_rect.max.x; x += 8)
                {
                    V2_8x uv01 = V2_8x{dot(d, x_axis_8x), dot(d, y_axis_8x)} * inverted_sqr_rect_size_8x;
                    V2_8x uv = uv01 * (texture_size_8x);
                    V2_8x uv_floored = clamp(floor(uv), zero_vector_8x, bitmap_size_8x);
                    V2_8x uv_fract = clamp01(fract(uv) * pixel_scale_8x);

                    i32_8x mask = uv01.x >= zero_8x & uv01.x < one_8x & uv01.y >= zero_8x & uv01.y < one_8x;
                    Bilinear_Sample_8x sample = get_bilinear_sample(drawing.bitmap, uv_floored, mask);
                    V4_8x texel = bilinear_blend(sample, uv_fract);
                    V4_8x pixel = argb_to_v4_8x(load(pixel_ptr));

                    texel = texel * alpha_8x;

                    f32_8x inverted_alpha = one_8x - texel.a * is_tile_multiplier;
                    V4_8x result = inverted_alpha * pixel + texel;

                    store(pixel_ptr, v4_to_argb_8x(result), mask);

                    pixel_ptr += 8;
                    d.x += eight_8x;
                }
            }
        }
    }
}

//темнота и уровень освещения
Bitmap darkness;
f32 light_lvl = 1;

void draw_light(Bitmap screen, Camera camera, V2 world_pos, f32 inner_radius, f32 world_radius)
{
    inner_radius *= light_lvl;
    world_radius *= light_lvl;
    f32 darkness_scale = darkness.size.x / screen.size.x;
    f32 radius = world_radius * camera.scale.x * darkness_scale;
    V2 center = world_to_screen(screen, camera, world_pos) * darkness_scale;

    Rect rect = {
        center - V2{radius, radius},
        center + V2{radius, radius},
    };
    Rect screen_rect = {
        V2{0, 0},
        darkness.size,
    };
    rect = intersect(rect, screen_rect);

    f32 radius_sqr = radius * radius;

    for (f32 y = rect.min.y; y <= rect.max.y; y++)
    {
        for (f32 x = rect.min.x; x <= rect.max.x; x++)
        {
            V2 d = {x, y};
            f32 dist_sqr = length_sqr(d - center);

            ARGB pixel = {darkness.pixels[(i32)y * (i32)darkness.pitch + (i32)x]};
            f32 intensity = min(pixel.a / 255.0f, dist_sqr / radius_sqr);
            darkness.pixels[(i32)y * (i32)darkness.pitch + (i32)x] = v4_to_argb({0, 0, 0, intensity}).argb;
        }
    }
}

//сущности
typedef enum
{
    Game_Object_PLAYER,
    Game_Object_ZOMBIE,
} Game_Object_Type;

typedef enum
{
    Direction_RIGHT = 1,
    Direction_LEFT = -1,
    Direction_VERTICAL = 0,
} Direction;

typedef enum
{
    Condition_IDLE,
    Condition_MOOVING,
    Condition_CROUCHING_IDLE,
    Condition_CROUCHING_MOOVING,
    Condition_LOOKING_UP,
    Condition_FALLING,
    Condition_STUNNED,
    Condition_HANGING,
    Condition_HANGING_LOOKING_UP,
    Condition_HANGING_LOOKING_DOWN,
} Condition;

typedef struct
{
    Game_Object_Type type;

    bool exists;

    f32 healthpoints;
    f32 max_healthpoints;

    V2 pos;
    V2 collision_box_pos;
    V2 collision_box;
    V2 hit_box_pos;
    V2 hit_box;
    V2 speed;
    V2 deflection;
    V2 target_deflection;

    bool go_left;
    bool go_right;
    i32 jump;

    f32 accel;
    f32 friction;
    f32 jump_height;
    f32 jump_duration;

    i32 moved_through_pixels;
    Direction looking_direction;
    Condition condition;
    Bitmap sprite;

    //player
    i32 can_jump;

    i32 jump_timer;
    i32 looking_key_held_timer;
    i32 crouching_animation_timer;
    i32 hanging_animation_timer;
    i32 cant_control_timer;
    i32 invulnerable_timer;

    Layer layer;

} Game_Object;

i32 game_object_count = 0;
Game_Object game_objects[512];

typedef enum
{
    Side_LEFT,
    Side_RIGHT,
    Side_BOTTOM,
    Side_TOP,
} Side;

typedef struct
{
    bool happened;
    i32 tile_index;
    f32 tile_side;
} Collision;

typedef struct
{
    Collision x;
    Collision y;
    bool expanded_collision;
} Collisions;

Game_Object *add_game_object(Game_Object_Type type, V2 pos)
{
    Game_Object game_object = {
        type, //тип

        true, //существует

        3, //жизни
        3, //максимальные жизни

        pos,      //позиция
        {0, -12}, //коробка столкновения
        {25, 56},
        {0, -5}, //коробка удара
        {46, 70},
        {0, 0}, //скорость
        {0, 0}, //скривление
        {0, 0},

        false, //инпут
        false,
        NULL,

        1,                    //ускорение
        0.75,                 //трение
        TILE_SIZE_PIXELS * 2, //высота прыжка
        19,                   //время прыжка

        0,                                     //пройденный путь в пикселях
        (Direction)(random_int(0, 1) * 2 - 1), //направление
        Condition_IDLE,                        //состояние
        img_None,                              //спрайт

        NULL, //таймеры

        NULL,
        NULL,
        NULL,
        NULL,
        NULL, //может контролировать
        NULL,

        LAYER_GAME_OBJECT, //слой прорисовки
    };

    if (type == Game_Object_PLAYER)
    {
        game_object.jump_timer = add_timer(0);
        game_object.jump = add_timer(0);
        game_object.can_jump = add_timer(0);
        game_object.looking_key_held_timer = add_timer(0);
        game_object.crouching_animation_timer = add_timer(0);
        game_object.hanging_animation_timer = add_timer(0);
        game_object.cant_control_timer = add_timer(0);
        game_object.invulnerable_timer = add_timer(0);

        game_object.jump_height = TILE_SIZE_PIXELS * 2.2 - game_object.collision_box.y;
    }

    if (type == Game_Object_ZOMBIE)
    {
        game_object.healthpoints = 1;

        game_object.jump = add_timer(-1);
        game_object.can_jump = add_timer(0);

        game_object.jump_duration = 15;

        game_object.go_left = random_int(0, 1);
        game_object.go_right = !game_object.go_left;
    }

    i32 slot_index = game_object_count;

    for (i32 objectIndex = 0; objectIndex < game_object_count; objectIndex++)
    {
        if (game_objects[objectIndex].exists == false)
        {
            slot_index = objectIndex;
            break;
        }
    }

    if (slot_index == game_object_count)
    {
        game_object_count++;
    }

    game_objects[slot_index] = game_object;

    return &game_objects[slot_index];
}

#define DISTANT_HANGING_VALUE 7

void test_wall(f32 wall_x, f32 obj_speed_x, f32 obj_speed_y, f32 obj_rel_pos_x, f32 obj_rel_pos_y, f32 *min_time, f32 min_y, f32 max_y)
{
    if (obj_speed_x != 0.0f)
    {
        f32 time = (wall_x - obj_rel_pos_x) / obj_speed_x;
        f32 y = obj_rel_pos_y + time * obj_speed_y;
        if ((time >= 0.0f) && (time < *min_time))
        {
            if (y >= min_y && y <= max_y)
            {
                *min_time = time;
            }
        }
    }
}

Collisions check_collision(Game_Object *game_object)
{
    Collisions collisions;
    collisions.x.happened = false;
    collisions.y.happened = false;
    collisions.expanded_collision = false;

    V2 start_tile = min(game_object->pos, game_object->pos + game_object->speed);
    V2 finish_tile = max(game_object->pos, game_object->pos + game_object->speed);

    start_tile = floor(start_tile / TILE_SIZE_PIXELS);
    finish_tile = ceil(finish_tile / TILE_SIZE_PIXELS);

    f32 min_time = 1.0f;

    if (length(game_object->speed) != 0)
    {
        i32 foo = 0;
    }

    for (i32 tile_y = start_tile.y; tile_y <= finish_tile.y + 1; tile_y++)
    {
        for (i32 tile_x = start_tile.x; tile_x <= finish_tile.x + 1; tile_x++)
        {
            i32 index = get_index(V2{(f32)tile_x, (f32)tile_y});
            Tile tile = tile_map[index];
            if (tile.solid)
            {
                V2 tile_pos = V2{(f32)tile_x, (f32)tile_y} * TILE_SIZE_PIXELS;
                V2 tile_min = V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS} * (-0.5) - V2{1, 1};
                V2 tile_max = V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS} * 0.5 + V2{1, 1};

                V2 obj_rel_pos = game_object->pos - tile_pos;

                test_wall(tile_min.x, game_object->speed.x, game_object->speed.y, obj_rel_pos.x, obj_rel_pos.y, &min_time, tile_min.y, tile_max.y);
                test_wall(tile_max.x, game_object->speed.x, game_object->speed.y, obj_rel_pos.x, obj_rel_pos.y, &min_time, tile_min.y, tile_max.y);
                test_wall(tile_min.y, game_object->speed.y, game_object->speed.x, obj_rel_pos.y, obj_rel_pos.x, &min_time, tile_min.x, tile_max.x);
                test_wall(tile_max.y, game_object->speed.y, game_object->speed.x, obj_rel_pos.y, obj_rel_pos.x, &min_time, tile_min.x, tile_max.x);
            }
        }
    }

    game_object->speed *= min_time;

    return collisions;
}

bool deal_damage(Game_Object *dealing_object, Game_Object *taking_object, f32 damage)
{
    bool result = false;
    if (timers[taking_object->invulnerable_timer] < 0)
    {
        taking_object->healthpoints -= damage;
        result = true;
    }
    return result;
}

#define JUMP_ON_ENEMY_BOOST 15
#define KNOCKBACK \
    V2 { 25, 10 }

void check_object_collision(Game_Object *game_object, Game_Object_Type *triggering_objects, i32 triggering_objects_count)
{
    for (i32 game_object_index = 0; game_object_index < game_object_count; game_object_index++)
    {
        for (i32 triggers_index = 0; triggers_index < triggering_objects_count; triggers_index++)
        {
            Game_Object collided_object = game_object[game_object_index];
            if (collided_object.type == triggering_objects[triggers_index] && collided_object.exists)
            {
                V2 obj_min = game_object->pos + game_object->hit_box_pos - game_object->hit_box * 0.5;
                V2 obj_max = game_object->pos + game_object->hit_box_pos + game_object->hit_box * 0.5;

                V2 collided_obj_min = collided_object.pos + game_object->hit_box_pos - collided_object.hit_box * 0.5;
                V2 collided_obj_max = collided_object.pos + game_object->hit_box_pos + collided_object.hit_box * 0.5;

                V2 obj_side;
                V2 collided_obj_side;
                V2 speed = game_object->speed - collided_object.speed;

                //по у
                if (speed.y > 0)
                {
                    obj_side.y = obj_max.y;
                    collided_obj_side.y = collided_obj_min.y;
                }
                else
                {
                    obj_side.y = obj_min.y;
                    collided_obj_side.y = collided_obj_max.y;
                }

                if (speed.x > 0)
                {
                    obj_side.x = obj_max.x;
                    collided_obj_side.x = collided_obj_min.x;
                }
                else
                {
                    obj_side.x = obj_min.x;
                    collided_obj_side.x = collided_obj_max.x;
                }

                if (fabs(obj_side.y + speed.y - collided_obj_side.y) < fabs(obj_side.x + speed.x - collided_obj_side.x))
                {
                    if (!((obj_max.y + speed.y <= collided_obj_min.y) ||
                          (obj_min.y + speed.y >= collided_obj_max.y) ||
                          (obj_max.x <= collided_obj_min.x) ||
                          (obj_min.x >= collided_obj_max.x)))
                    {
                        if (obj_side.y == obj_min.y)
                        {
                            if (deal_damage(game_object, &game_objects[game_object_index], 1))
                            {
                                game_object->speed.y = JUMP_ON_ENEMY_BOOST;
                            }
                        }
                        else
                        {
                            if (deal_damage(&game_objects[game_object_index], game_object, 1))
                            {
                                timers[game_object->cant_control_timer] = 30;
                                timers[game_object->invulnerable_timer] = 60;
                                game_object->speed = KNOCKBACK;
                            }
                        }
                    }
                }
                else
                {

                    if (!((obj_max.x + speed.y <= collided_obj_min.x) ||
                          (obj_min.x + speed.y >= collided_obj_max.x) ||
                          (obj_max.y <= collided_obj_min.y) ||
                          (obj_min.y >= collided_obj_max.y)))
                    {
                        if (deal_damage(&game_objects[game_object_index], game_object, 1))
                        {
                            timers[game_object->cant_control_timer] = 30;
                            timers[game_object->invulnerable_timer] = 60;
                            if (game_object->pos.x > collided_object.pos.x)
                            {
                                game_object->speed.x = KNOCKBACK.x;
                                game_objects[game_object_index].looking_direction = Direction_RIGHT;
                            }
                            else
                            {
                                game_object->speed.x = -KNOCKBACK.x;
                                game_objects[game_object_index].looking_direction = Direction_LEFT;
                            }
                            game_object->speed.y = KNOCKBACK.y;
                        }
                    }
                }
            }
        }
    }
}

bool check_vision_box(i32 *trigger_index, V2 vision_point, V2 vision_pos, V2 vision_size, Game_Object_Type *triggering_objects, i32 triggering_objects_count, bool goes_through_walls, bool draw)
{
    bool vision_triggered = false;
    for (i32 game_object_index = 0; game_object_index < game_object_count; game_object_index++)
    {
        for (i32 triggers_index = 0; triggers_index < triggering_objects_count; triggers_index++)
        {
            Game_Object game_object = game_objects[game_object_index];
            if (game_object.type == triggering_objects[triggers_index])
            {
                if (vision_pos.x - vision_size.x * 0.5 - game_object.hit_box.x * 0.5 < game_object.pos.x && vision_pos.x + vision_size.x * 0.5 + game_object.hit_box.x * 0.5 > game_object.pos.x &&
                    vision_pos.y - vision_size.y * 0.5 - game_object.hit_box.y * 0.5 < game_object.pos.y && vision_pos.y + vision_size.y * 0.5 + game_object.hit_box.y * 0.5 > game_object.pos.y)
                {
                    vision_triggered = true;
                    trigger_index = &game_object_index;
                    if (!goes_through_walls)
                    {
                        f32 k = (vision_point.y - game_object.pos.y) / (vision_point.x - game_object.pos.x);
                        f32 b = vision_point.y - k * vision_point.x;
                        //y=kx+b
                        V2 min_point = round(V2{min(vision_point.x, game_object.pos.x), min(vision_point.y, game_object.pos.y)} / TILE_SIZE_PIXELS);
                        V2 max_point = round(V2{max(vision_point.x, game_object.pos.x), max(vision_point.y, game_object.pos.y)} / TILE_SIZE_PIXELS);
                        for (i32 y = min_point.y; y <= max_point.y; y++)
                        {
                            for (i32 x = min_point.x; x <= max_point.x; x++)
                            {
                                V2 tile_pos = V2{(f32)x, (f32)y};
                                if (tile_map[get_index(tile_pos)].solid)
                                {
                                    tile_pos *= TILE_SIZE_PIXELS;
                                    i32 collisions = 0;
                                    f32 x1 = ((tile_pos.y - TILE_SIZE_PIXELS * 0.5) - b) / k;
                                    f32 x2 = ((tile_pos.y + TILE_SIZE_PIXELS * 0.5) - b) / k;
                                    f32 y1 = k * (tile_pos.x - TILE_SIZE_PIXELS * 0.5) + b;
                                    f32 y2 = k * (tile_pos.x + TILE_SIZE_PIXELS * 0.5) + b;
                                    bool bx1 = x1 >= tile_pos.x - TILE_SIZE_PIXELS * 0.5 && x1 <= tile_pos.x + TILE_SIZE_PIXELS * 0.5;
                                    bool bx2 = x2 >= tile_pos.x - TILE_SIZE_PIXELS * 0.5 && x2 <= tile_pos.x + TILE_SIZE_PIXELS * 0.5;
                                    bool by1 = y1 >= tile_pos.y - TILE_SIZE_PIXELS * 0.5 && y1 <= tile_pos.y + TILE_SIZE_PIXELS * 0.5;
                                    bool by2 = y2 >= tile_pos.y - TILE_SIZE_PIXELS * 0.5 && y2 <= tile_pos.y + TILE_SIZE_PIXELS * 0.5;
                                    if (bx1 || bx2 || by1 || by2)
                                    {
                                        vision_triggered = false;
                                        goto Result;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
Result:
    if (draw)
    {
        draw_rect(vision_pos, vision_size, 0, 0xFF00FF00, LAYER_FORGROUND);
    }
    return vision_triggered;
}

// //changePos принимает три значения: -1, 0, 1.
// //-1 хитбокс перемещается к низу
// //0 xитбокс не перемещается
// //1 хитбокс перемещается к верху
// void changeHitBox(Game_Object *gameObject, V2 hitBox, i8 changePos)
// {
//     V2 objTilePos = {roundf(gameObject->pos.x / TILE_SIZE_PIXELS),
//                      roundf(gameObject->pos.y / TILE_SIZE_PIXELS)};

//     i32 tileCountX = ceilf((hitBox.x - gameObject->hit_box.x) / TILE_SIZE_PIXELS);
//     i32 tileCountY = ceilf((hitBox.y - gameObject->hit_box.y) / TILE_SIZE_PIXELS);

//     if (changePos)
//     {
//         if (changePos == -1)
//         {
//             gameObject->pos.y += -gameObject->hit_box.y *0.5 + hitBox.y *0.5;
//         }
//         if (changePos == 1)
//         {
//             gameObject->pos.y += gameObject->hit_box.y *0.5 - hitBox.y *0.5;
//         }
//     }

//     for (i32 y = objTilePos.y - tileCountY; y <= objTilePos.y + tileCountY; y++)
//     {
//         for (i32 x = objTilePos.x - tileCountX; x <= objTilePos.x + tileCountX; x++)
//         {
//             i32 tileIndex = get_index(x,y);
//             Tile tile = tile_map[tileIndex];

//             if (tile.solid)
//             {

//                 V2 tilePos = get_tile_pos(tileIndex);

//                 f32 recentObjTop = gameObject->pos.y + gameObject->hit_box.y *0.5;
//                 f32 recentObjBottom = gameObject->pos.y - gameObject->hit_box.y *0.5;
//                 f32 recentObjRight = gameObject->pos.x + gameObject->hit_box.x *0.5;
//                 f32 recentObjLeft = gameObject->pos.x - gameObject->hit_box.x *0.5;

//                 f32 objTop = gameObject->pos.y + hitBox.y *0.5;
//                 f32 objBottom = gameObject->pos.y - hitBox.y *0.5;
//                 f32 objRight = gameObject->pos.x + hitBox.x *0.5;
//                 f32 objLeft = gameObject->pos.x - hitBox.x *0.5;

//                 f32 tileTop = tilePos.y * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS *0.5;
//                 f32 tileBottom = tilePos.y * TILE_SIZE_PIXELS - TILE_SIZE_PIXELS *0.5;
//                 f32 tileRight = tilePos.x * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS *0.5;
//                 f32 tileLeft = tilePos.x * TILE_SIZE_PIXELS - TILE_SIZE_PIXELS *0.5;

//                 if (recentObjLeft >= tileRight && objLeft <= tileRight)
//                 {
//                     gameObject->pos.x -= objLeft - tileRight;
//                 }

//                 if (recentObjRight <= tileLeft && objRight >= tileLeft)
//                 {
//                     gameObject->pos.x -= objRight - tileLeft;
//                 }

//                 if (recentObjBottom >= tileTop && objBottom <= tileTop)
//                 {
//                     gameObject->pos.y -= objBottom - tileTop;
//                 }

//                 if (recentObjTop <= tileBottom && objTop >= tileBottom)
//                 {
//                     gameObject->pos.y -= objTop - tileBottom;
//                 }
//             }
//         }
//     }
//     gameObject->hit_box = hitBox;
// }

i32 check_for_interactive_tiles(Game_Object *game_object)
{
    i32 result = -1;
    V2 obj_tile_pos = V2{roundf(game_object->pos.x / TILE_SIZE_PIXELS), roundf(game_object->pos.y / TILE_SIZE_PIXELS)};

    i32 index = get_index(obj_tile_pos);
    Tile tile = tile_map[index];
    if (tile.interactive != -1)
    {
        result = index;
    }
    return result;
}

f32 UI_alpha = 1.00f;

void update_game_object(Game_Object *game_object, Input input, Bitmap screen)
{
#define RUNNING_ACCEL 3.8
#define NORMAL_ACCEL 1.9
#define CROUCHING_ACCEL 0.85
#define JUMP_BUTTON_DURATION 15
#define ADDITIONAL_JUMP_FRAMES 6
#define HANGING_ANIMATION_TIME 16
#define LOOKING_KEY_HOLDING 40
#define CROUCHING_ANIMATION_TIME 8
#define LOOKING_DISTANCE 200

    if (game_object->type == Game_Object_PLAYER)
    {
        //предпологаемое состояние
        Condition supposed_cond;

        V2 recent_speed = game_object->speed;

        //движение игрока
        if (timers[game_object->cant_control_timer] < 0)
        {
            if (input.z.went_down)
            {
                timers[game_object->jump] = JUMP_BUTTON_DURATION;
            }

            if (timers[game_object->jump] > 0)
            {
                input.z.went_down = true;
            }

            game_object->go_left = input.left.is_down;
            game_object->go_right = input.right.is_down;
        }
        else
        {
            game_object->go_left = false;
            game_object->go_right = false;
            timers[game_object->jump] = 0;
        }

        //константы ускорения
        if (input.shift.is_down)
        {
            game_object->accel = NORMAL_ACCEL;
        }
        else
        {
            game_object->accel = RUNNING_ACCEL;
        }
        if (game_object->condition == Condition_CROUCHING_IDLE || game_object->condition == Condition_CROUCHING_MOOVING)
        {
            game_object->accel = CROUCHING_ACCEL;
        }

        f32 gravity = -2 * game_object->jump_height / (game_object->jump_duration * game_object->jump_duration);

        //скорость по x
        game_object->speed += {(game_object->go_right - game_object->go_left) * game_object->accel, (input.up.is_down - input.down.is_down) * game_object->accel};

        //прыжок
        if (input.z.went_down && timers[game_object->can_jump] > 0)
        {
            timers[game_object->jump] = 0;
            timers[game_object->can_jump] = 0;
            if (game_object->condition == Condition_HANGING || game_object->condition == Condition_HANGING_LOOKING_DOWN || game_object->condition == Condition_HANGING_LOOKING_UP)
            {
                if (timers[game_object->hanging_animation_timer] > 0)
                {
                    timers[game_object->jump] = timers[game_object->hanging_animation_timer] + 2;
                }
                else
                {
                    if (!input.down.is_down)
                    {
                        game_object->speed.y = 2 * game_object->jump_height / game_object->jump_duration;
                        timers[game_object->jump_timer] = game_object->jump_duration;
                    }
                    game_object->condition = Condition_FALLING;
                }
            }
            else
            {
                game_object->speed.y = 2 * game_object->jump_height / game_object->jump_duration;
                timers[game_object->jump_timer] = game_object->jump_duration;
            }
        }

        if (timers[game_object->jump_timer] > 0)
        {
            if (!input.z.is_down)
            {
                timers[game_object->jump_timer] = 0;
            }
        }

        if (game_object->speed.y > 0 && timers[game_object->jump_timer] <= 0)
        {
            gravity *= 2;
        }

        //гравитация
        if (game_object->speed.y < TILE_SIZE_PIXELS / 2)
        {
            // game_object->speed.y += gravity;
        }

        f32 sliding_speed = -(game_object->hit_box.y + TILE_SIZE_PIXELS) / 2 / (f32)(HANGING_ANIMATION_TIME);

        if (game_object->condition == Condition_HANGING || game_object->condition == Condition_HANGING_LOOKING_DOWN || game_object->condition == Condition_HANGING_LOOKING_UP)
        {
            game_object->speed.x = 0;
            if (timers[game_object->hanging_animation_timer] <= 0)
            {
                game_object->speed.y = 0;
            }
            else
            {
                game_object->speed.y = sliding_speed;
            }
        }

        //трение
        game_object->speed *= game_object->friction;

        //направление
        if (game_object->condition != Condition_HANGING && game_object->condition != Condition_HANGING_LOOKING_DOWN && game_object->condition != Condition_HANGING_LOOKING_UP)
        {
            if (!(game_object->go_left && game_object->go_right))
            {
                if (game_object->go_right)
                {
                    game_object->looking_direction = Direction_RIGHT;
                }
                if (game_object->go_left)
                {
                    game_object->looking_direction = Direction_LEFT;
                }
            }
        }

        Collisions collisions = check_collision(game_object);

        //состояние падает
        supposed_cond = Condition_FALLING;

        //состояние стоит
        if (collisions.y.happened && collisions.y.tile_side == TILE_SIZE_PIXELS * 0.5)
        {
            supposed_cond = Condition_IDLE;
            timers[game_object->can_jump] = ADDITIONAL_JUMP_FRAMES;

            //состояние ползком и смотрит вверх
            if (input.down.is_down && !input.up.is_down)
            {
                supposed_cond = Condition_CROUCHING_IDLE;
            }
            else if (input.up.is_down && !input.down.is_down && fabs(game_object->speed.x) <= 1 && timers[game_object->crouching_animation_timer] <= 0)
            {
                supposed_cond = Condition_LOOKING_UP;
            }

            //состояния движения
            if (fabs(game_object->speed.x) > 1 && (supposed_cond == Condition_IDLE || supposed_cond == Condition_CROUCHING_IDLE))
            {

                game_object->moved_through_pixels += game_object->speed.x;

                if (supposed_cond == Condition_IDLE)
                {
                    supposed_cond = Condition_MOOVING;
                }
                else
                {
                    supposed_cond = Condition_CROUCHING_MOOVING;
                }
            }
        }

        if (input.left.went_down || input.right.went_down)
        {
            game_object->moved_through_pixels = 0;
        }

        //функции движения, связанные с коллизией
        V2 collided_x_tile_pos = get_tile_pos(collisions.x.tile_index);
        V2 collided_y_tile_pos = get_tile_pos(collisions.y.tile_index);

        i32 collision_up_tile_index = get_index(V2{collided_x_tile_pos.x, collided_x_tile_pos.y + 1});
        i32 collision_frontup_tile_index = get_index(V2{collided_x_tile_pos.x - game_object->looking_direction, collided_x_tile_pos.y + 1});

        //состояние весит
        if ((collisions.x.happened || collisions.expanded_collision) && !tile_map[collision_up_tile_index].solid && !tile_map[collision_frontup_tile_index].solid)
        {
            if (game_object->speed.y < 0 &&
                game_object->pos.y + game_object->collision_box_pos.y > collided_x_tile_pos.y * TILE_SIZE_PIXELS - 3 &&
                game_object->pos.y + game_object->collision_box_pos.y + game_object->speed.y <= collided_x_tile_pos.y * TILE_SIZE_PIXELS - 3)
            {
                supposed_cond = Condition_HANGING;

                game_object->speed = {0, 0};

                game_object->pos.y = collided_x_tile_pos.y * TILE_SIZE_PIXELS - game_object->collision_box_pos.y - 3;

                game_object->pos.x = collided_x_tile_pos.x * TILE_SIZE_PIXELS - game_object->looking_direction * (TILE_SIZE_PIXELS * 0.5 + game_object->collision_box.x * 0.5) + game_object->collision_box_pos.x;
            }
        }

        //переход на стенку из состояния ползком
        if (supposed_cond == Condition_CROUCHING_IDLE || supposed_cond == Condition_CROUCHING_MOOVING)
        {
            V2 collisionY_front_tile_pos = collided_y_tile_pos + V2{(f32)game_object->looking_direction, 0};
            if (!tile_map[get_index(collisionY_front_tile_pos)].solid &&
                ((game_object->pos.x + game_object->speed.x + game_object->collision_box.x * 0.5 + game_object->collision_box_pos.x <= collisionY_front_tile_pos.x * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS * 0.5) && (ceilf(game_object->pos.x + game_object->collision_box_pos.x + game_object->collision_box.x * 0.5) >= collisionY_front_tile_pos.x * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS * 0.5) ||
                 (game_object->pos.x + game_object->speed.x - game_object->collision_box.x * 0.5 + game_object->collision_box_pos.x >= collisionY_front_tile_pos.x * TILE_SIZE_PIXELS - TILE_SIZE_PIXELS * 0.5) && (floorf(game_object->pos.x + game_object->collision_box_pos.x - game_object->collision_box.x * 0.5) <= collisionY_front_tile_pos.x * TILE_SIZE_PIXELS - TILE_SIZE_PIXELS * 0.5)))
            {
                game_object->pos.x = collisionY_front_tile_pos.x * TILE_SIZE_PIXELS + game_object->collision_box_pos.x - TILE_SIZE_PIXELS * 0.5 * game_object->looking_direction + game_object->collision_box.x * 0.5 * game_object->looking_direction;
                supposed_cond = Condition_HANGING;
                game_object->looking_direction = (Direction)(-game_object->looking_direction);
                game_object->speed.x = 0;
                game_object->speed.y = sliding_speed;
                game_object->condition = Condition_IDLE;
                timers[game_object->hanging_animation_timer] = HANGING_ANIMATION_TIME;
            }
        }

        //состояние смежные с состоянием весит
        if ((game_object->condition == Condition_HANGING || game_object->condition == Condition_HANGING_LOOKING_DOWN || game_object->condition == Condition_HANGING_LOOKING_UP))
        {

            if (input.up.is_down && !input.down.is_down)
            {
                supposed_cond = Condition_HANGING_LOOKING_UP;
            }
            else if (input.down.is_down && !input.up.is_down)
            {
                supposed_cond = Condition_HANGING_LOOKING_DOWN;
            }
            else
            {
                supposed_cond = Condition_HANGING;
            }
        }

        //работа с предполагаемым состоянием

        if ((game_object->condition != Condition_HANGING && game_object->condition != Condition_HANGING_LOOKING_DOWN && game_object->condition != Condition_HANGING_LOOKING_UP) ||
            (game_object->condition == Condition_HANGING || game_object->condition == Condition_HANGING_LOOKING_DOWN || game_object->condition == Condition_HANGING_LOOKING_UP) &&
                (supposed_cond == Condition_HANGING || supposed_cond == Condition_HANGING_LOOKING_DOWN || supposed_cond == Condition_HANGING_LOOKING_UP))
        {
            //начинаем смотреть вверх и вниз
            if ((game_object->condition != Condition_CROUCHING_IDLE && supposed_cond == Condition_CROUCHING_IDLE) ||
                (game_object->condition != Condition_LOOKING_UP && supposed_cond == Condition_LOOKING_UP) ||
                (game_object->condition != Condition_HANGING_LOOKING_UP && supposed_cond == Condition_HANGING_LOOKING_UP) ||
                (game_object->condition != Condition_HANGING_LOOKING_DOWN && supposed_cond == Condition_HANGING_LOOKING_DOWN))
            {
                timers[game_object->looking_key_held_timer] = LOOKING_KEY_HOLDING;
            }

            //поднимаемся и встаём
            if ((game_object->condition != Condition_CROUCHING_IDLE && game_object->condition != Condition_CROUCHING_MOOVING) && (supposed_cond == Condition_CROUCHING_IDLE || supposed_cond == Condition_CROUCHING_MOOVING))
            {
                if (timers[game_object->crouching_animation_timer] < 0)
                {
                    timers[game_object->crouching_animation_timer] = 0;
                }
                timers[game_object->crouching_animation_timer] = (CROUCHING_ANIMATION_TIME - timers[game_object->crouching_animation_timer] * 0.5) * 2;
            }
            if ((game_object->condition == Condition_CROUCHING_IDLE || game_object->condition == Condition_CROUCHING_MOOVING) && (supposed_cond != Condition_CROUCHING_IDLE && supposed_cond != Condition_CROUCHING_MOOVING))
            {
                if (timers[game_object->crouching_animation_timer] < 0)
                {
                    timers[game_object->crouching_animation_timer] = 0;
                }
                timers[game_object->crouching_animation_timer] = CROUCHING_ANIMATION_TIME - timers[game_object->crouching_animation_timer] * 0.5;
            }
            game_object->condition = supposed_cond;
        }

        //изменяем положение
        game_object->pos += game_object->speed;

        //столкновения и движение
        Game_Object_Type triggers[1];
        triggers[0] = Game_Object_ZOMBIE;
        check_object_collision(game_object, triggers, 1);

        //что делают состояния
        if (game_object->condition == Condition_IDLE || game_object->condition == Condition_MOOVING)
        {
            if (timers[game_object->crouching_animation_timer] > 0)
            {
                i32 step = ceil(timers[game_object->crouching_animation_timer] * 0.5);
                game_object->sprite = img_PlayerStartsCrouching[step];
            }
            else if (game_object->condition == Condition_IDLE)
            {
                game_object->speed.x *= game_object->friction;
                game_object->sprite = img_PlayerIdle;
            }
            else if (game_object->condition == Condition_MOOVING)
            {
                i8 step = (i32)floor(fabsf(game_object->moved_through_pixels) / 35);
                while (step > 5)
                {
                    step -= 6;
                }

                game_object->sprite = img_PlayerStep[step];
            }
        }

        if (game_object->condition == Condition_CROUCHING_MOOVING || game_object->condition == Condition_CROUCHING_IDLE)
        {
            if (timers[game_object->crouching_animation_timer] > 0)
            {
                const i32 step = 4 - ceil(timers[game_object->crouching_animation_timer] * 0.25);
                game_object->sprite = img_PlayerStartsCrouching[step];
            }
            else
            {
                if (game_object->condition == Condition_CROUCHING_IDLE)
                {
                    game_object->sprite = img_PlayerCrouchIdle;
                }
                if (game_object->condition == Condition_CROUCHING_MOOVING)
                {
                    i8 step = (i32)floor(fabsf(game_object->moved_through_pixels) * 0.1);
                    while (step > 5)
                    {
                        step -= 6;
                    }
                    game_object->sprite = img_PlayerCrouchStep[step];
                }
            }
        }

        if (game_object->condition == Condition_LOOKING_UP)
        {
            game_object->sprite = img_PlayerLookingUp;
        }

        if (game_object->condition == Condition_FALLING)
        {
            game_object->sprite = img_PlayerJump;
        }

        if (game_object->condition == Condition_HANGING || game_object->condition == Condition_HANGING_LOOKING_UP || game_object->condition == Condition_HANGING_LOOKING_DOWN)
        {
            if (game_object->condition == Condition_HANGING)
            {
                game_object->sprite = img_PlayerHanging;
            }
            else if (game_object->condition == Condition_HANGING_LOOKING_DOWN)
            {
                game_object->sprite = img_PlayerHangingDown;
            }
            else
            {
                game_object->sprite = img_PlayerHangingUp;
            }

            if ((timers[game_object->hanging_animation_timer] <= 1 && (length(game_object->speed) > 0)) ||
                (timers[game_object->hanging_animation_timer] > 1 && game_object->speed.y != sliding_speed))
            {
                game_object->condition = Condition_FALLING;
                timers[game_object->hanging_animation_timer] = 0;
            }

            timers[game_object->can_jump] = 4;
        }

        //камера
        camera.target = game_object->pos;

        //смотрим вниз и вверх
        if ((game_object->condition == Condition_CROUCHING_IDLE || game_object->condition == Condition_HANGING_LOOKING_DOWN) && timers[game_object->looking_key_held_timer] <= 0)
        {
            camera.target.y = game_object->pos.y - screen.size.y / camera.scale.y * 0.5 + LOOKING_DISTANCE;
        }

        if ((game_object->condition == Condition_LOOKING_UP || game_object->condition == Condition_HANGING_LOOKING_UP) && timers[game_object->looking_key_held_timer] <= 0)
        {
            camera.target.y = game_object->pos.y + screen.size.y / camera.scale.y * 0.5 - LOOKING_DISTANCE;
        }

        //искривление
        if (recent_speed.y != 0 && game_object->speed.y == 0)
        {
            game_object->target_deflection = V2{-recent_speed.y, recent_speed.y};
        }
        if (game_object->speed.y != 0)
        {
            game_object->target_deflection = V2{game_object->speed.y, -game_object->speed.y};
        }

        game_object->deflection += (game_object->target_deflection - game_object->deflection) * 0.3;

        if (length(game_object->target_deflection - game_object->deflection) < 0.1)
        {
            game_object->target_deflection = V2{0, 0};
        }

        //границы для камеры
        border_camera(screen);

        //камера
        camera.pos += (camera.target - camera.pos) * 0.25f;

        //интерфейс

#define BAR_HEIGHT 50
#define BAR_WIDTH 150
#define INTERVAL 10

        V2 camera_shake = V2{0, 0};

        if (game_object->healthpoints / game_object->max_healthpoints <= 0.5)
        {
            camera_shake = {random_float(-10 * (0.5 - game_object->healthpoints / game_object->max_healthpoints), 10 * (0.5 - game_object->healthpoints / game_object->max_healthpoints)), random_float(-5 * (0.5 - game_object->healthpoints / game_object->max_healthpoints), 5 * (0.5 - game_object->healthpoints / game_object->max_healthpoints))};
        }

        draw_bitmap(
            camera.pos + camera_shake + V2{-screen.size.x, screen.size.y} / camera.scale * 0.5 + V2{BAR_WIDTH / 2 + INTERVAL, -(BAR_HEIGHT / 2 + INTERVAL)} - V2{(1 - game_object->healthpoints / game_object->max_healthpoints) * BAR_WIDTH, 0} / 2,
            V2{game_object->healthpoints / game_object->max_healthpoints * BAR_WIDTH, BAR_HEIGHT},
            0, img_Health, UI_alpha, LAYER_UI);

        draw_bitmap(camera.pos + camera_shake + V2{-screen.size.x, screen.size.y} / camera.scale * 0.5 + V2{BAR_WIDTH / 2 + INTERVAL, -(BAR_HEIGHT / 2 + INTERVAL)},
                    V2{BAR_WIDTH, BAR_HEIGHT}, 0, img_HealthBar, UI_alpha, LAYER_UI);

        //взаимодействие с тайлами

        if (input.space.went_down)
        {
            i32 tile_index = check_for_interactive_tiles(game_object);
            if (tile_index != -1)
            {
                Tile tile = tile_map[tile_index];
                V2 tile_pos = get_tile_pos(tile_index);
                if (tile.type == Tile_Type_EXIT)
                {
                    if (game_object->condition != Condition_FALLING)
                    {
                        game_object->pos.x = tile_pos.x * TILE_SIZE_PIXELS;
                        game_object->speed = V2{0, 0};

                        timers[game_object->cant_control_timer] = 9999;
                        timers[game_object->invulnerable_timer] = 9999;
                        timers[tile.interactive] = 60;

                        game_object->layer = LAYER_ON_ON_BACKGROUND;
                    }
                }
            }
        }

        //прорисовка игрока

        //хитбокс
        // draw_rect(game_object->pos + game_object->hit_box_pos, game_object->hit_box, 0, 0xFFFF0000, LAYER_FORGROUND);

        draw_bitmap(game_object->pos + V2{0, game_object->deflection.y * 0.5f}, V2{(game_object->sprite.size.x * SPRITE_SCALE + game_object->deflection.x) * game_object->looking_direction, game_object->sprite.size.y * SPRITE_SCALE + game_object->deflection.y}, 0, game_object->sprite, 1, game_object->layer);

        draw_light(screen, camera, game_object->pos, 200, 300);
    }

#define ZOMBIE_ACCEL 6
#define JUMP_LATENCY 20
#define VISION_BOX V2{5, 3} * TILE_SIZE_PIXELS

    if (game_object->type == Game_Object_ZOMBIE)
    {
        V2 recent_speed = game_object->speed;

        //константы скорости
        f32 gravity = -2 * game_object->jump_height / (game_object->jump_duration * game_object->jump_duration);

        //движение
        game_object->speed.x += (game_object->go_right - game_object->go_left) * game_object->accel;

        //прыжок
        if (game_object->condition != Condition_FALLING)
        {
            if (timers[game_object->jump] == 0)
            {
                game_object->speed.y = 2 * game_object->jump_height / game_object->jump_duration * random_float(0.4, 1);
                game_object->accel = ZOMBIE_ACCEL;
            }
            else if (timers[game_object->jump] > 0)
            {
                game_object->go_left = false;
                game_object->go_right = false;
            }
        }

        //гравитация и трение
        game_object->speed.y += gravity;

        game_object->speed.x *= game_object->friction;

        //направление
        if (!(game_object->go_left && game_object->go_right))
        {
            if (game_object->go_left)
            {
                game_object->looking_direction = Direction_LEFT;
            }
            if (game_object->go_right)
            {
                game_object->looking_direction = Direction_RIGHT;
            }
        }

        //поведение, связанное с тайлами
        Collisions collisions = check_collision(game_object);

        //если стоит
        if (collisions.y.happened && collisions.y.tile_side == TILE_SIZE_PIXELS * 0.5)
        {
            game_object->accel = 1;
            //состояния стоит и движение
            if (fabs(game_object->speed.x) > 1)
            {
                if (game_object->moved_through_pixels > 0 && game_object->speed.x < 0 || game_object->moved_through_pixels < 0 && game_object->speed.x > 0)
                {
                    game_object->moved_through_pixels = 0;
                }
                game_object->moved_through_pixels += game_object->speed.x;
                game_object->condition = Condition_MOOVING;
            }
            else
            {
                game_object->condition = Condition_IDLE;
            }

            //если не заряжает прыжок
            if (timers[game_object->jump] < 0 && game_object->speed.y <= 0)
            {
                V2 collided_y_tile_pos = get_tile_pos(collisions.y.tile_index);

                Tile downleft_tile = tile_map[get_index(collided_y_tile_pos + V2{-1, 0})];
                Tile downright_tile = tile_map[get_index(collided_y_tile_pos + V2{1, 0})];
                Tile left_tile = tile_map[get_index(collided_y_tile_pos + V2{-1, 1})];
                Tile right_tile = tile_map[get_index(collided_y_tile_pos + V2{1, 1})];
                //если доходит до края тайла, то разворачивается
                if ((!downleft_tile.solid || left_tile.solid) && game_object->pos.x + game_object->hit_box_pos.x - game_object->hit_box.x * 0.25 <= collided_y_tile_pos.x * TILE_SIZE_PIXELS)
                {
                    if (left_tile.solid)
                    {
                        game_object->looking_direction = Direction_RIGHT;
                    }
                    game_object->go_right = true;
                    game_object->go_left = false;
                }
                if ((!downright_tile.solid || right_tile.solid) && game_object->pos.x + game_object->hit_box_pos.x + game_object->hit_box.x * 0.25 >= collided_y_tile_pos.x * TILE_SIZE_PIXELS)
                {
                    if (right_tile.solid)
                    {
                        game_object->looking_direction = Direction_LEFT;
                    }
                    game_object->go_right = false;
                    game_object->go_left = true;
                }
                //если находится на одиночном тайле, то останавливается посередине
                if ((!downleft_tile.solid || left_tile.solid) && (!downright_tile.solid || right_tile.solid))
                {
                    if (game_object->pos.x + TILE_SIZE_PIXELS * 0.2 <= collided_y_tile_pos.x * TILE_SIZE_PIXELS)
                    {
                        game_object->go_right = true;
                    }
                    else if (game_object->pos.x - TILE_SIZE_PIXELS * 0.2 >= collided_y_tile_pos.x * TILE_SIZE_PIXELS)
                    {
                        game_object->go_left = true;
                    }
                    else
                    {
                        game_object->go_right = false;
                        game_object->go_left = false;
                        if (!(left_tile.solid && right_tile.solid))
                        {
                            if (left_tile.solid)
                            {
                                game_object->looking_direction = Direction_RIGHT;
                            }
                            if (right_tile.solid)
                            {
                                game_object->looking_direction = Direction_LEFT;
                            }
                        }
                    }
                }
            }
        }
        else
        {
            game_object->condition = Condition_FALLING;
            timers[game_object->jump] = -1;
        }

        //эффекты состояний
        if (game_object->condition == Condition_IDLE)
        {
            game_object->speed.x *= game_object->friction;
            game_object->sprite = img_ZombieIdle;
        }

        if (game_object->condition == Condition_MOOVING)
        {
            i8 step = (i32)floor(fabsf(game_object->moved_through_pixels) * 0.05);
            while (step > 5)
            {
                step -= 6;
            }

            game_object->sprite = img_ZombieStep[step];
        }

        if (game_object->condition == Condition_FALLING)
        {
            bool bool_direction = (game_object->looking_direction + 1) * 0.5;
            game_object->go_right = bool_direction;
            game_object->go_left = !bool_direction;
            game_object->sprite = img_ZombieIdle;
        }

        game_object->pos += game_object->speed;

        Game_Object_Type triggers[1];
        triggers[0] = Game_Object_PLAYER;

        i32 trigger_index = -1;

        if (check_vision_box(&trigger_index, game_object->pos, game_object->pos + V2{(TILE_SIZE_PIXELS * 5 + game_object->hit_box.x) * (f32)(game_object->looking_direction), 0} * 0.5, VISION_BOX, triggers, 1, false, false) &&
            timers[game_object->jump] < 0)
        {
            timers[game_object->jump] = JUMP_LATENCY;
            if (trigger_index != -1)
            {
                Game_Object trigger = game_objects[trigger_index];
                if (trigger.pos.x > game_object->pos.x)
                {
                    game_object->looking_direction = Direction_RIGHT;
                }
                if (trigger.pos.x < game_object->pos.x)
                {
                    game_object->looking_direction = Direction_LEFT;
                }
            }
        }

        //искривление
        if (recent_speed.y != 0 && game_object->speed.y == 0)
        {
            game_object->target_deflection = V2{-recent_speed.y, recent_speed.y};
        }
        if (game_object->speed.y != 0)
        {
            game_object->target_deflection = V2{game_object->speed.y, -game_object->speed.y};
        }

        game_object->deflection += (game_object->target_deflection - game_object->deflection) * 0.3;

        if (length(game_object->target_deflection - game_object->deflection) < 0.1)
        {
            game_object->target_deflection = V2{0, 0};
        }

        // draw_rect(game_object->pos + game_object->hit_box_pos, game_object->hit_box, 0, 0xFFFF0000, LAYER_GAME_OBJECT);
        // draw_rect(game_object->pos, game_object->collision_box, 0, 0xFF00FF00, LAYER_FORGROUND);
        draw_bitmap(game_object->pos + V2{0, game_object->deflection.y * 0.5f}, V2{(game_object->sprite.size.x * SPRITE_SCALE + game_object->deflection.x) * game_object->looking_direction, (game_object->sprite.size.y * SPRITE_SCALE + game_object->deflection.y)}, 0, game_object->sprite, 1, game_object->layer);
    }

    if (game_object->healthpoints <= 0)
    {
        game_object->exists = false;
    }
}

#define NORMAL_CHUNKS_COUNT 1
char *normal_chunks[NORMAL_CHUNKS_COUNT] = {
    "          "
    "          "
    "          "
    "          "
    "          "
    "          "
    "          "
    "          ",
    // "###  #####"
    // "#     ####"
    // "TTT   ####"
    // "=== T  ###"
    // "##      ##"
    // "##    TT##"
    // "#    ===##"
    // "#  #######",

    // "##    ## #"
    // "#       ##"
    // "##      ##"
    // "#      ###"
    // "##       #"
    // "#       ##"
    // "###    ###"
    // "###TTTT###",

    // "          "
    // "          "
    // "     ##   "
    // " TT TTTT  "
    // "          "
    // "#T TTTTTT "
    // "##        "
    // "###======#",
};

#define DOWN_PASSAGE_CHUNKS_COUNT 1
char *down_passage_chunks[DOWN_PASSAGE_CHUNKS_COUNT] = {
    "          "
    "          "
    "          "
    "          "
    "          "
    "          "
    "          "
    "          ",
    // "          "
    // "          "
    // "          "
    // "##     TT="
    // "   #      "
    // "     TTT#T"
    // "T#TT   #=="
    // "##### ####",

    // "##    ## #"
    // "        ##"
    // "##      ##"
    // "       ## "
    // "          "
    // "  === ### "
    // "###      ="
    // "###T   ###",

    // "          "
    // " =        "
    // "          "
    // "          "
    // "   #   #  "
    // "= ##   ## "
    // "####   ## "
    // "####   ###",
};

#define ENTER_DOWN_PASSAGE_CHUNKS_COUNT 1
char *enter_down_passage_chunks[ENTER_DOWN_PASSAGE_CHUNKS_COUNT] = {
    "          "
    "          "
    "          "
    "     N    "
    "     #    "
    "          "
    "          "
    "          ",
    // "#      #  "
    // "#        #"
    // "       #  "
    // "     N  # "
    // "     T    "
    // " ^     TTT"
    // "T#TT   ==="
    // "####   ###",

    // "          "
    // "  N       "
    // "TTTT      "
    // "       ## "
    // "    =     "
    // "####=  TT#"
    // "####=  ###"
    // "####=  ###",

    // "       #  "
    // "TT      ##"
    // "          "
    // "    N     "
    // "    ##    "
    // "#TT=#  ###"
    // "###=    ##"
    // "#####  ###",
};

#define ENTER_CHUNKS_COUNT 1
char *enter_chunks[ENTER_CHUNKS_COUNT] = {
    "          "
    "          "
    "     N    "
    "     #    "
    "          "
    "          "
    "          "
    "          ",
    // "          "
    // "          "
    // "    N     "
    // "   TTTT   "
    // "   =###=  "
    // "  =###### "
    // "TT########"
    // "##########",

    // "          "
    // "  N       "
    // " ###      "
    // "       ## "
    // "  ^ #     "
    // "####   ###"
    // "####  ####"
    // "#####  ###",

    // "       #  "
    // " ##     # "
    // "          "
    // "    N     "
    // "    ##    "
    // "#TT##  ## "
    // "####   ###"
    // "#####  ###",
};

#define PASSAGE_CHUNKS_COUNT 1
char *passage_chunks[PASSAGE_CHUNKS_COUNT] = {
    "          "
    "          "
    "          "
    "          "
    "          "
    "          "
    "          "
    "          ",
    // " PPPPPPP  "
    // " #######  "
    // "          "
    // "         ="
    // "          "
    // "   TT     "
    // "TTTTTTTTTT"
    // "#======###",

    // "  ##   ## "
    // "#         "
    // "        # "
    // "       ## "
    // "  #    ###"
    // "TTTT  ####"
    // "##########"
    // "##########",

    // "##     #  "
    // " #      ##"
    // "          "
    // "          "
    // "          "
    // "#TT#  ## #"
    // "###======#"
    // "###T######",
};

#define END_CHUNKS_COUNT 1
char *exit_chunks[END_CHUNKS_COUNT] = {
    "          "
    "          "
    "          "
    "          "
    "          "
    "          "
    "          "
    "          ",
    // "          "
    // " #        "
    // "#         "
    // "      T  #"
    // "          "
    // " #   X    "
    // "  TTTTTT  "
    // " ######## ",
    // "          "
    // "#         "
    // "#         "
    // "         #"
    // "  X       "
    // " ####     "
    // "##TTTTTT  "
    // "##########",
    // "##      # "
    // "         #"
    // "          "
    // "   X      "
    // " #####    "
    // "          "
    // "  TTTTTT  "
    // " ======== ",
};

i32 danger_points = random_int(8, 12);

xoshiro256ss_state world_state;

void generate_new_map(Bitmap screen)
{
    // __global_random_state.s[0] = 14002478324315578667;
    // __global_random_state.s[1] = 14922209480584404284;
    // __global_random_state.s[2] = 1922425429350644560;
    // __global_random_state.s[3] = 9604386734951883359;

    //удаляем объекты
    game_object_count = 0;

    //удаляем таймеры
    timers_count = 0;

    //генерация
    f32 down_chunk_chance = 0.25;

    //карта тайлов
    V2 map_size = {CHUNK_COUNT_X * CHUNK_SIZE_X + BORDER_SIZE * 2, CHUNK_COUNT_Y * CHUNK_SIZE_Y + BORDER_SIZE * 2};
    i32 tile_count = map_size.x * map_size.y;

    tile_map = (Tile *)malloc(sizeof(Tile) * tile_count);
    for (i32 index = 0; index < tile_count; index++)
    {
        tile_map[index].type = Tile_Type_BORDER;
        tile_map[index].solid = true;
        tile_map[index].interactive = -1;
        tile_map[index].sprite = img_None;
    }

    //создание чанков
    char *chunks_strings[CHUNK_COUNT_X * CHUNK_COUNT_Y];

    //путь через тайлы
    V2 enter_chunk_pos = {(f32)random_int(0, CHUNK_COUNT_X - 1), CHUNK_COUNT_Y - 1};
    V2 chunk_pos = enter_chunk_pos;
    V2 end_chunk_pos = {(f32)random_int(0, CHUNK_COUNT_X - 1), 0};

    //заполняем чанки
    for (i32 index = 0; index < CHUNK_COUNT_X * CHUNK_COUNT_Y; index++)
    {
        chunks_strings[index] = normal_chunks[random_int(1, NORMAL_CHUNKS_COUNT) - 1];
    }

    i8 direction = random_int(Direction_LEFT, Direction_RIGHT);
    if ((direction == Direction_RIGHT && chunk_pos.x == CHUNK_COUNT_X - 1) || (direction == Direction_LEFT && chunk_pos.x == 0))
    {
        direction = -direction;
    }

    while (chunk_pos != end_chunk_pos)
    {
        if ((random_float(0, 1) <= down_chunk_chance || direction == Direction_VERTICAL) && chunk_pos.y > 0)
        {
            if (chunk_pos == enter_chunk_pos)
            {
                //чанк-вход с проходом вниз
                chunks_strings[(i32)(chunk_pos.y * CHUNK_COUNT_X + chunk_pos.x)] = enter_down_passage_chunks[random_int(1, ENTER_DOWN_PASSAGE_CHUNKS_COUNT) - 1];
            }
            else
            {
                //чанк-проход вниз
                chunks_strings[(i32)(chunk_pos.y * CHUNK_COUNT_X + chunk_pos.x)] = down_passage_chunks[random_int(1, DOWN_PASSAGE_CHUNKS_COUNT) - 1];
            }
            chunk_pos.y--;

            direction = random_int(0, 1) * 2 - 1;
        }
        else
        {
            if ((direction == Direction_RIGHT && chunk_pos.x == CHUNK_COUNT_X - 1) || (direction == Direction_LEFT && chunk_pos.x == 0))
            {
                direction = Direction_VERTICAL;
            }

            if (chunk_pos.y == 0)
            {
                direction = end_chunk_pos.x - chunk_pos.x;
                if (direction > 0)
                {
                    direction = Direction_RIGHT;
                }
                else
                {
                    direction = Direction_LEFT;
                }
            }

            if (direction != Direction_VERTICAL)
            {
                if (chunk_pos == enter_chunk_pos)
                {
                    //чанк-вход
                    chunks_strings[(i32)(chunk_pos.y * CHUNK_COUNT_X + chunk_pos.x)] = enter_chunks[random_int(1, ENTER_CHUNKS_COUNT) - 1];
                }
                else
                {
                    //проходной чанк
                    chunks_strings[(i32)(chunk_pos.y * CHUNK_COUNT_X + chunk_pos.x)] = passage_chunks[random_int(1, PASSAGE_CHUNKS_COUNT) - 1];
                }
                chunk_pos.x += direction;
            }
        }
        //чанк-выход
        if (chunk_pos == end_chunk_pos)
        {
            chunks_strings[(i32)(chunk_pos.y * CHUNK_COUNT_X + chunk_pos.x)] = exit_chunks[random_int(1, END_CHUNKS_COUNT) - 1];
        }
    }

    char *chunk_string;
    //заполняем чанки
    for (i32 chunk_index_y = 0; chunk_index_y < CHUNK_COUNT_Y; chunk_index_y++)
    {
        for (i32 chunk_index_x = 0; chunk_index_x < CHUNK_COUNT_X; chunk_index_x++)
        {
            chunk_string = chunks_strings[chunk_index_y * CHUNK_COUNT_X + chunk_index_x];
            for (i32 y = 0; y < CHUNK_SIZE_Y; y++)
            {
                for (i32 x = 0; x < CHUNK_SIZE_X; x++)
                {
                    char tile_char = chunk_string[(CHUNK_SIZE_Y - y - 1) * CHUNK_SIZE_X + x];
                    V2 tile_pos = {(f32)(x + chunk_index_x * CHUNK_SIZE_X + BORDER_SIZE), (f32)(y + chunk_index_y * CHUNK_SIZE_Y + BORDER_SIZE)};
                    i32 index = get_index(tile_pos);

                    Tile_type type = Tile_Type_NONE;
                    bool solid = false;
                    i32 interactive = -1;
                    Bitmap sprite = img_None;

                    switch (tile_char)
                    {
                    case ' ':
                    {
                        type = Tile_Type_NONE;
                        solid = false;
                        interactive = -1;
                        sprite = img_None;
                    }
                    break;
                    case '#':
                    {
                        type = Tile_Type_NORMAL;
                        solid = true;
                        interactive = -1;

                        f32 chance = random_float(0, 1);
                        if (chance < 0.95)
                        {
                            if (chance > 0.15)
                            {
                                sprite = img_Bricks[(i32)random_int(0, 6)];
                            }
                            else
                            {
                                sprite = img_Stone;
                            }
                        }
                        else
                        {
                            sprite = img_Bricks[(i32)random_int(7, 11)];
                        }
                    }
                    break;
                    case '=':
                    {
                        type = Tile_Type_NORMAL;
                        solid = true;
                        interactive = -1;
                        sprite = img_ElegantBrick;
                    }
                    break;
                    case '-':
                    {
                        type = Tile_Type_BORDER;
                        solid = true;
                        interactive = -1;

                        sprite = img_Border;
                        Tile_type left_tile = tile_map[get_index(tile_pos + V2{-1, 0})].type;
                        Tile_type right_tile = tile_map[get_index(tile_pos + V2{1, 0})].type;
                        Tile_type top_tile = tile_map[get_index(tile_pos + V2{0, 1})].type;
                        Tile_type bottom_tile = tile_map[get_index(tile_pos + V2{0, -1})].type;
                        Tile_type bottomright_tile = tile_map[get_index(tile_pos + V2{1, -1})].type;
                        Tile_type topright_tile = tile_map[get_index(tile_pos + V2{1, 1})].type;
                        Tile_type bottomleft_tile = tile_map[get_index(tile_pos + V2{-1, -1})].type;
                        Tile_type topleft_tile = tile_map[get_index(tile_pos + V2{-1, 1})].type;

                        if (bottom_tile != Tile_Type_BORDER)
                        {
                            sprite = img_TransitionBorder_0;
                        }
                        else
                        {
                            if (right_tile != Tile_Type_BORDER)
                            {
                                sprite = img_TransitionBorder_05PI;
                            }
                            else if (bottomright_tile != Tile_Type_BORDER)
                            {
                                sprite = img_CornerBorder_0;
                            }

                            if (left_tile != Tile_Type_BORDER)
                            {
                                sprite = img_TransitionBorder_15PI;
                            }
                            else if (bottomright_tile != Tile_Type_BORDER)
                            {
                                sprite = img_CornerBorder_15PI;
                            }
                        }

                        if (top_tile != Tile_Type_BORDER)
                        {
                            sprite = img_TransitionBorder_PI;
                        }
                        else
                        {
                            if (right_tile != Tile_Type_BORDER)
                            {
                                sprite = img_TransitionBorder_05PI;
                            }
                            else if (topright_tile != Tile_Type_BORDER)
                            {
                                sprite = img_CornerBorder_05PI;
                            }

                            if (left_tile != Tile_Type_BORDER)
                            {
                                sprite = img_TransitionBorder_15PI;
                            }
                            else if (topleft_tile != Tile_Type_BORDER)
                            {
                                sprite = img_CornerBorder_PI;
                            }
                        }
                    }
                    break;
                    case 'N':
                    {
                        type = Tile_Type_ENTER;
                        solid = false;
                        interactive = -1;

                        sprite = img_Door[0];

                        //addPlayer
                        V2 spawn_pos = tile_pos * TILE_SIZE_PIXELS;
                        add_game_object(Game_Object_PLAYER, spawn_pos);
                        camera.target = spawn_pos;
                        border_camera(screen);
                        camera.pos = camera.target;
                    }
                    break;
                    case 'X':
                    {
                        type = Tile_Type_EXIT;
                        solid = false;
                        interactive = add_timer(-1);
                        sprite = img_Door[6];
                    }
                    break;
                    case 'T':
                    {
                        type = Tile_Type_FLOOR;
                        solid = true;
                        interactive = -1;
                        sprite = img_None;
                    }
                    break;
                    case 'P':
                    {
                        type = Tile_Type_PARAPET;
                        solid = false;
                        interactive = -1;
                        sprite = img_Parapet;
                    }
                    break;
                    case '^':
                    {
                        type = Tile_Type_SPIKES;
                        solid = false;
                        interactive = -1;
                        sprite = img_Spikes;
                    }
                    break;
                    }

                    tile_map[index].type = type;
                    tile_map[index].solid = solid;
                    tile_map[index].interactive = interactive;
                    tile_map[index].sprite = sprite;
                }
            }
        }
    }

    //налаживаем свойства тайлов после полной расстановки
    for (i32 index = 0; index < tile_count; index++)
    {
        V2 tile_pos = get_tile_pos(index);
        Tile_type type = tile_map[index].type;
        Bitmap sprite = tile_map[index].sprite;

        if (type == Tile_Type_FLOOR)
        {
            Bitmap left_tile = tile_map[get_index(tile_pos + V2{-1, 0})].sprite;
            Bitmap bottom_tile = tile_map[get_index(tile_pos + V2{0, -1})].sprite;
            Bitmap bottomleft_tile = tile_map[get_index(tile_pos + V2{-1, -1})].sprite;
            Bitmap bottomright_tile = tile_map[get_index(tile_pos + V2{1, -1})].sprite;

            i32 bitmap_size = img_TiledFloor.pitch * (img_TiledFloor.size.y + 2) * sizeof(u32);

            if (left_tile.pixels == img_TiledFloor.pixels ||
                bottom_tile.pixels == img_TiledFloor.pixels ||
                bottomleft_tile.pixels == img_TiledFloor.pixels ||
                bottomright_tile.pixels == img_TiledFloor.pixels)
            {
                sprite = img_TiledFloor;
            }
            else if (left_tile.pixels == img_MarbleFloor.pixels ||
                     bottom_tile.pixels == img_MarbleFloor.pixels ||
                     bottomleft_tile.pixels == img_MarbleFloor.pixels ||
                     bottomright_tile.pixels == img_MarbleFloor.pixels)
            {
                sprite = img_MarbleFloor;
            }
            else
            {
                if (random_int(0, 1) == 0)
                {
                    sprite = img_TiledFloor;
                }
                else
                {
                    sprite = img_MarbleFloor;
                }
            }
        }

        if (type == Tile_Type_BORDER)
        {
            sprite = img_Border;
            if (tile_pos.x != 0 && tile_pos.y != 0 && tile_pos.x != (map_size.x - 1) && tile_pos.y != (map_size.y - 1))
            {
                Tile_type left_tile = tile_map[get_index(tile_pos + V2{-1, 0})].type;
                Tile_type right_tile = tile_map[get_index(tile_pos + V2{1, 0})].type;
                Tile_type top_tile = tile_map[get_index(tile_pos + V2{0, 1})].type;
                Tile_type bottom_tile = tile_map[get_index(tile_pos + V2{0, -1})].type;
                Tile_type bottomright_tile = tile_map[get_index(tile_pos + V2{1, -1})].type;
                Tile_type topright_tile = tile_map[get_index(tile_pos + V2{1, 1})].type;
                Tile_type bottomleft_tile = tile_map[get_index(tile_pos + V2{-1, -1})].type;
                Tile_type topleft_tile = tile_map[get_index(tile_pos + V2{-1, 1})].type;

                if (bottom_tile != Tile_Type_BORDER)
                {
                    sprite = img_TransitionBorder_0;
                }
                else
                {
                    if (right_tile != Tile_Type_BORDER)
                    {
                        sprite = img_TransitionBorder_05PI;
                    }
                    else if (bottomleft_tile != Tile_Type_BORDER)
                    {
                        sprite = img_CornerBorder_15PI;
                    }

                    if (left_tile != Tile_Type_BORDER)
                    {
                        sprite = img_TransitionBorder_15PI;
                    }
                    else if (bottomright_tile != Tile_Type_BORDER)
                    {
                        sprite = img_CornerBorder_0;
                    }
                }

                if (top_tile != Tile_Type_BORDER)
                {
                    sprite = img_TransitionBorder_PI;
                }
                else
                {
                    if (right_tile != Tile_Type_BORDER)
                    {
                        sprite = img_TransitionBorder_05PI;
                    }
                    else if (topright_tile != Tile_Type_BORDER)
                    {
                        sprite = img_CornerBorder_05PI;
                    }

                    if (left_tile != Tile_Type_BORDER)
                    {
                        sprite = img_TransitionBorder_15PI;
                    }
                    else if (topleft_tile != Tile_Type_BORDER)
                    {
                        sprite = img_CornerBorder_PI;
                    }
                }
            }
        }

        i32 bottom_tile_index = get_index(tile_pos - V2{0, 1});
        V2 pixel_tile_pos = tile_pos * TILE_SIZE_PIXELS;

        if (distance_between_points(pixel_tile_pos, game_objects[0].pos) > 400)
        {
            if (!tile_map[index].solid && random_float(0, 1) < 0.15)
            {
                if (tile_map[bottom_tile_index].solid)
                {
                    i32 right_tile_index = get_index(tile_pos + V2{1, 0});
                    i32 left_tile_index = get_index(tile_pos + V2{-1, 0});

                    if (!(tile_map[right_tile_index].solid && tile_map[left_tile_index].solid))
                    {
                        V2 spawn_pos = pixel_tile_pos;
                        // add_game_object(Game_Object_ZOMBIE, spawn_pos);
                    }
                }
            }
        }

        tile_map[index].sprite = sprite;
    }
}

void update_tile(i32 tile_index)
{
    Tile *tile = &tile_map[tile_index];
    V2 tilePos = get_tile_pos(tile_index);

    if (tile->sprite.pixels == img_Parapet.pixels)
    {
        i32 foo = 0;
    }

    //обновление тайлов
    switch (tile->type)
    {
    case Tile_Type_NONE:
    {
    }
    break;
    case Tile_Type_NORMAL:
    {
        draw_bitmap(tilePos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS}, 0, tile_map[tile_index].sprite, 1, LAYER_TILE);
    }
    break;
    case Tile_Type_BORDER:
    {
        // draw_bitmap(tilePos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS}, 0, tile_map[tile_index].sprite, 1, LAYER_TILE);
    }
    break;
    case Tile_Type_FLOOR:
    {
        draw_bitmap(tilePos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS}, 0, tile_map[tile_index].sprite, 1, LAYER_TILE);
    }
    break;
    case Tile_Type_PARAPET:
    {
        if (tile_map[get_index(tilePos + V2{0, -1})].type == Tile_Type_NONE)
        {
            tile->type = Tile_Type_NONE;
            tile->sprite = img_BackGround;
        }
        else
        {
            draw_bitmap(tilePos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS}, 0, tile_map[tile_index].sprite, 1, LAYER_FORGROUND);
        }
    }
    break;
    case Tile_Type_ENTER:
    {
        draw_bitmap(tilePos * TILE_SIZE_PIXELS + V2{0, (tile_map[tile_index].sprite.size.y * SPRITE_SCALE - TILE_SIZE_PIXELS) * 0.5f}, tile_map[tile_index].sprite.size * SPRITE_SCALE, 0, tile_map[tile_index].sprite, 1, LAYER_ON_ON_BACKGROUND);
    }
    break;
    case Tile_Type_EXIT:
    {
        draw_bitmap(tilePos * TILE_SIZE_PIXELS + V2{0, (tile_map[tile_index].sprite.size.y * SPRITE_SCALE - TILE_SIZE_PIXELS) * 0.5f}, img_DoorBack.size * SPRITE_SCALE, 0, img_DoorBack, 1, LAYER_ON_BACKGROUND);
        draw_bitmap(tilePos * TILE_SIZE_PIXELS + V2{0, (tile_map[tile_index].sprite.size.y * SPRITE_SCALE - TILE_SIZE_PIXELS) * 0.5f}, tile_map[tile_index].sprite.size * SPRITE_SCALE, 0, tile_map[tile_index].sprite, 1, LAYER_ON_ON_BACKGROUND);

        if (timers[tile->interactive] > 0)
        {
            i32 step = floor(timers[tile->interactive] / 60.0f * 6.0f);
            tile->sprite = img_Door[step];
        }
        else if (timers[tile->interactive] == 0)
        {
            transition_is_on = true;
        }
    }
    break;
    case Tile_Type_SPIKES:
    {
        draw_bitmap(tilePos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS}, 0, tile_map[tile_index].sprite, 1, LAYER_ON_BACKGROUND);
    }
    break;
    }

    if (!tile->solid)
    {
        // draw_bitmap(tilePos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS}, 0, img_BackGround, 1, LAYER_BACKGROUND_MAIN);
    }
}

bool draw_darkness = false;

bool initialized = false;
void game_update(Bitmap screen, Input input)
{
    V2 map_size = {CHUNK_COUNT_X * CHUNK_SIZE_X + BORDER_SIZE * 2, CHUNK_COUNT_Y * CHUNK_SIZE_Y + BORDER_SIZE * 2};
    i32 tile_count = map_size.x * map_size.y;

    i32 interval = SPRITE_SCALE;

    //выполняется один раз
    if (!initialized)
    {
        initialized = true;

        camera.scale = V2{1, 1} * 0.4f;

        //темнота
        darkness = create_empty_bitmap(screen.size / camera.scale);

        generate_new_map(screen);
    }

    if (input.r.went_down)
    {
        // if (game_objects[0].exists == true)
        // {

        //     draw_darkness = !draw_darkness;
        // }
        // else
        {
            generate_new_map(screen);
        }
    }

    //очистка экрана
    clear_screen(screen, darkness);

    //обновление сущностей
    for (i32 object_index = 0; object_index < game_object_count; object_index++)
    {
        if (game_objects[object_index].exists)
        {
            update_game_object(&game_objects[object_index], input, screen);
        }
    }

    //плавный переход
    if (transition_is_on)
    {
        darkness_lvl += (1 - darkness_lvl) * 0.1;
        light_lvl += (0 - light_lvl) * 0.1;
        UI_alpha += (0 - UI_alpha) * 0.1;
        if (light_lvl < 0.001)
        {
            transition_is_on = false;
            generate_new_map(screen);
        }
    }
    else
    {
        darkness_lvl += (DARKNESS_USUAL_LVL - darkness_lvl) * 0.1;
        light_lvl += (1 - light_lvl) * 0.1;
        UI_alpha += (1 - UI_alpha) * 0.1;
    }

    //прорисовка темноты
    if (draw_darkness)
    {
        draw_bitmap(camera.pos, darkness.size, 0, darkness, 1, LAYER_DARKNESS);
    }

    //обновление тайлов
    for (i32 tile_index = 0; tile_index < tile_count; tile_index++)
    {
        update_tile(tile_index);
    }

    //сортируем qrawQueue
    Drawing new_draw_queue[1024 * 8];
    i32 new_draw_queue_size = 0;
    Layer layer = (Layer)0;

    //to do переделать под insertion sort
    while (new_draw_queue_size != draw_queue_size)
    {
        for (i32 drawing_index = 0; drawing_index < draw_queue_size; drawing_index++)
        {
            if (draw_queue[drawing_index].layer == layer)
            {
                draw_queue[drawing_index].pos = world_to_screen(screen, camera, draw_queue[drawing_index].pos);
                draw_queue[drawing_index].size *= camera.scale;

                new_draw_queue[new_draw_queue_size] = draw_queue[drawing_index];
                new_draw_queue_size++;
            }
        }
        layer = (Layer)((i32)layer + 1);
    }

    draw_queue_size = 0;

    for (i32 i = 0; i < new_draw_queue_size; i++)
    {
        draw_item(screen, new_draw_queue[i]);
    }

    update_timers();
}