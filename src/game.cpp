#include "game.h"
#include <malloc.h>
#include <string.h>

i32 add_timer(Game_memory *memory, i32 time)
{
    memory->timers[memory->timers_count] = time;
    memory->timers_count++;
    return memory->timers_count - 1;
}

void update_timers(Game_memory *memory)
{
    for (i32 i = 0; i < memory->timers_count; i++)
    {
        memory->timers[i]--;
    }
}

//получение перевёрнутых картинок
Bitmap turn_bitmap(Bitmap bitmap, f64 angle)
{
    Bitmap result;
    result.pitch = bitmap.pitch;
    result.size = bitmap.size;

    u64 alignment = (u32)(8 * sizeof(u32));
    u64 screen_buffer_size = (u32)(4 * (bitmap.size.x + 2) * (bitmap.size.y + 2));
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

void load_bitmaps(Game_memory *memory, READ_BMP(win32_read_bmp))
{
    memory->bitmaps[0] = win32_read_bmp("none.bmp");
    memory->bitmaps[1] = win32_read_bmp("test.bmp");
    memory->bitmaps[2] = win32_read_bmp("lexaIdle.bmp");
    memory->bitmaps[3] = win32_read_bmp("../data/lexaJump.bmp");
    memory->bitmaps[4] = win32_read_bmp("../data/lexaCrouchIdle.bmp");
    memory->bitmaps[5] = win32_read_bmp("../data/lexaWall.bmp");
    memory->bitmaps[6] = win32_read_bmp("../data/lexaWallUp.bmp");
    memory->bitmaps[7] = win32_read_bmp("../data/lexaWallDown.bmp");
    memory->bitmaps[8] = win32_read_bmp("../data/lexaLookingUp.bmp");
    memory->bitmaps[9] = win32_read_bmp("../data/lexaStep1.bmp");
    memory->bitmaps[10] = win32_read_bmp("../data/lexaStep2.bmp");
    memory->bitmaps[11] = win32_read_bmp("../data/lexaStep3.bmp");
    memory->bitmaps[12] = win32_read_bmp("../data/lexaStep4.bmp");
    memory->bitmaps[13] = win32_read_bmp("../data/lexaStep5.bmp");
    memory->bitmaps[14] = win32_read_bmp("../data/lexaStep6.bmp");
    memory->bitmaps[15] = win32_read_bmp("../data/lexaCrouchIdle1.bmp");
    memory->bitmaps[16] = win32_read_bmp("../data/lexaCrouchIdle2.bmp");
    memory->bitmaps[17] = win32_read_bmp("../data/lexaCrouchIdle3.bmp");
    memory->bitmaps[18] = win32_read_bmp("../data/lexaCrouchIdle4.bmp");
    memory->bitmaps[19] = win32_read_bmp("../data/lexaCrouchStep1.bmp");
    memory->bitmaps[20] = win32_read_bmp("../data/lexaCrouchStep2.bmp");
    memory->bitmaps[21] = win32_read_bmp("../data/lexaCrouchStep3.bmp");
    memory->bitmaps[22] = win32_read_bmp("../data/lexaCrouchStep4.bmp");
    memory->bitmaps[23] = win32_read_bmp("../data/lexaCrouchStep5.bmp");
    memory->bitmaps[24] = win32_read_bmp("../data/lexaCrouchStep6.bmp");
    memory->bitmaps[25] = win32_read_bmp("../data/zombieIdle.bmp");
    memory->bitmaps[26] = win32_read_bmp("../data/zombieStep1.bmp");
    memory->bitmaps[27] = win32_read_bmp("../data/zombieStep2.bmp");
    memory->bitmaps[28] = win32_read_bmp("../data/zombieStep3.bmp");
    memory->bitmaps[29] = win32_read_bmp("../data/zombieStep4.bmp");
    memory->bitmaps[30] = win32_read_bmp("../data/zombieStep5.bmp");
    memory->bitmaps[31] = win32_read_bmp("../data/zombieStep6.bmp");
    memory->bitmaps[32] = win32_read_bmp("../data/backGround.bmp");
    memory->bitmaps[33] = win32_read_bmp("../data/border.bmp");
    memory->bitmaps[34] = win32_read_bmp("../data/borderWithTransition.bmp");
    memory->bitmaps[35] = turn_bitmap(memory->bitmaps[Bitmap_type_TRANSITION_BORDER_0], 0.5 * PI);
    memory->bitmaps[36] = turn_bitmap(memory->bitmaps[Bitmap_type_TRANSITION_BORDER_0], PI);
    memory->bitmaps[37] = turn_bitmap(memory->bitmaps[Bitmap_type_TRANSITION_BORDER_0], 1.5 * PI);
    memory->bitmaps[38] = win32_read_bmp("../data/cornerBorder.bmp");
    memory->bitmaps[39] = turn_bitmap(memory->bitmaps[Bitmap_type_CORNER_BORDER_0], 0.5 * PI);
    memory->bitmaps[40] = turn_bitmap(memory->bitmaps[Bitmap_type_CORNER_BORDER_0], PI);
    memory->bitmaps[41] = turn_bitmap(memory->bitmaps[Bitmap_type_CORNER_BORDER_0], 1.5 * PI);
    memory->bitmaps[42] = win32_read_bmp("../data/brick1.bmp");
    memory->bitmaps[43] = win32_read_bmp("../data/brick2.bmp");
    memory->bitmaps[44] = win32_read_bmp("../data/brick3.bmp");
    memory->bitmaps[45] = win32_read_bmp("../data/brick4.bmp");
    memory->bitmaps[46] = win32_read_bmp("../data/brick5.bmp");
    memory->bitmaps[47] = win32_read_bmp("../data/brick6.bmp");
    memory->bitmaps[48] = win32_read_bmp("../data/brick7.bmp");
    memory->bitmaps[49] = win32_read_bmp("../data/brick8.bmp");
    memory->bitmaps[50] = win32_read_bmp("../data/brick9.bmp");
    memory->bitmaps[51] = win32_read_bmp("../data/brick10.bmp");
    memory->bitmaps[52] = win32_read_bmp("../data/brick11.bmp");
    memory->bitmaps[53] = win32_read_bmp("../data/brick12.bmp");
    memory->bitmaps[54] = win32_read_bmp("../data/elegantBrick.bmp");
    memory->bitmaps[55] = win32_read_bmp("../data/stone.bmp");
    memory->bitmaps[56] = win32_read_bmp("../data/floor1.bmp");
    memory->bitmaps[57] = win32_read_bmp("../data/floor5.bmp");
    memory->bitmaps[58] = win32_read_bmp("../data/door1.bmp");
    memory->bitmaps[59] = win32_read_bmp("../data/door2.bmp");
    memory->bitmaps[60] = win32_read_bmp("../data/door3.bmp");
    memory->bitmaps[61] = win32_read_bmp("../data/door4.bmp");
    memory->bitmaps[62] = win32_read_bmp("../data/door5.bmp");
    memory->bitmaps[63] = win32_read_bmp("../data/door6.bmp");
    memory->bitmaps[64] = win32_read_bmp("../data/door7.bmp");
    memory->bitmaps[65] = win32_read_bmp("../data/doorBack.bmp");
    memory->bitmaps[66] = win32_read_bmp("../data/parapet.bmp");
    memory->bitmaps[67] = win32_read_bmp("../data/health_bar.bmp");
    memory->bitmaps[68] = win32_read_bmp("../data/health.bmp");
    memory->bitmaps[69] = win32_read_bmp("../data/spikes.bmp");
}

V2 world_to_screen(Bitmap screen, Camera camera, V2 p)
{
    V2 result = (p - camera.pos) * camera.scale + screen.size * 0.5f;
    return result;
}

//drawing
void draw_rect(Game_memory *memory, V2 pos, V2 size, f32 angle, u32 color, Layer layer)
{
    Drawing drawing;
    drawing.type = DRAWING_TYPE_RECT;
    drawing.pos = pos;
    drawing.size = size;
    drawing.angle = angle;
    drawing.color = color;
    drawing.bitmap = memory->bitmaps[Bitmap_type_NONE];
    drawing.layer = layer;
    assert(memory->draw_queue_size < 1024 * 8);
    memory->draw_queue[memory->draw_queue_size] = drawing;
    memory->draw_queue_size++;
}

void draw_bitmap(Game_memory *memory, V2 pos, V2 size, f32 angle, Bitmap bitmap, f32 alpha, Layer layer)
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
    assert(memory->draw_queue_size < 1024 * 8);
    memory->draw_queue[memory->draw_queue_size] = drawing;
    memory->draw_queue_size++;
}

//очищение экрана и затемнение среды
#define DARKNESS_USUAL_LVL 0.9

void clear_screen(Game_memory *memory, Bitmap screen, Bitmap darkness)
{
    i32 pixelCount = (i32)(screen.size.x * screen.size.y);
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
            dark_pixel.a = u8(dark_pixel.a * memory->darkness_lvl);
            darkness.pixels[y * darkness.pitch + x] = dark_pixel.argb;
        }
    }
}

//границы камеры
void border_camera(Game_memory *memory, Bitmap screen)
{
    if (!(memory->camera.target.x - screen.size.x / memory->camera.scale.x * 0.5 > -TILE_SIZE_PIXELS * 0.5))
    {
        memory->camera.target.x = f32(-TILE_SIZE_PIXELS * 0.5 + screen.size.x / memory->camera.scale.x * 0.5);
    }

    if (!(memory->camera.target.x + screen.size.x / memory->camera.scale.x * 0.5 < -TILE_SIZE_PIXELS * 0.5 + TILE_SIZE_PIXELS * CHUNK_COUNT_X * CHUNK_SIZE_X + 2 * BORDER_SIZE * TILE_SIZE_PIXELS))
    {
        memory->camera.target.x = f32(-TILE_SIZE_PIXELS * 0.5 + TILE_SIZE_PIXELS * CHUNK_COUNT_X * CHUNK_SIZE_X + 2 * BORDER_SIZE * TILE_SIZE_PIXELS - screen.size.x / memory->camera.scale.x * 0.5);
    }

    if (!(memory->camera.target.y - screen.size.y / memory->camera.scale.y * 0.5 > -TILE_SIZE_PIXELS * 0.5))
    {
        memory->camera.target.y = f32(-TILE_SIZE_PIXELS * 0.5 + screen.size.y / memory->camera.scale.y * 0.5);
    }

    if (!(memory->camera.target.y + screen.size.y / memory->camera.scale.y * 0.5 < -TILE_SIZE_PIXELS * 0.5 + TILE_SIZE_PIXELS * CHUNK_COUNT_Y * CHUNK_SIZE_Y + 2 * BORDER_SIZE * TILE_SIZE_PIXELS))
    {
        memory->camera.target.y = f32(-TILE_SIZE_PIXELS * 0.5 + TILE_SIZE_PIXELS * CHUNK_COUNT_Y * CHUNK_SIZE_Y + 2 * BORDER_SIZE * TILE_SIZE_PIXELS - screen.size.y / memory->camera.scale.y * 0.5);
    }
}

V2 get_tile_pos(i32 index)
{
    //size_x = CHUNK_SIZE_X * CHUNK_COUNT_X + BORDER_SIZE * 2;
    //index = y * (CHUNK_SIZE_X * CHUNK_COUNT_X + BORDER_SIZE * 2) + x;
    f32 y = (f32)floor(index / (CHUNK_SIZE_X * CHUNK_COUNT_X + BORDER_SIZE * 2));
    f32 x = index - y * (CHUNK_SIZE_X * CHUNK_COUNT_X + BORDER_SIZE * 2);
    V2 result = {x, y};
    return result;
}

i32 get_index(V2 coords)
{
    i32 index = (i32)(coords.y * (CHUNK_SIZE_X * CHUNK_COUNT_X + BORDER_SIZE * 2) + coords.x);
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
    result.pitch = (i32)size.x + 2;
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
    i32_8x offsets = to_i32_8x(pos.y * set1_f32((f32)bitmap.pitch) + pos.x);
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

//отключил пока Серёга не поможет разобраться с кодом
// struct Timed_Scope
// {
//     u64 stamp;
//     char *name;
//     u64 times;

//     ~Timed_Scope()
//     {
//         f64 time_since = (f64)(__rdtsc() - stamp) / times;
//         char buffer[256];
//         sprintf_s(buffer, 256, "%s: %.02f\n", name, time_since);
//         foo(buffer);
//     }
// };

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

        for (i32 y = (i32)paint_rect.min.y; y < paint_rect.max.y; y++)
        {
            for (i32 x = (i32)paint_rect.min.x; x < paint_rect.max.x; x++)
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

    if (drawing.type == DRAWING_TYPE_BITMAP)
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

            for (i32 y = (i32)paint_rect.min.y; y < paint_rect.max.y; y++)
            {
                for (i32 x = (i32)paint_rect.min.x; x < paint_rect.max.x; x++)
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

    if (drawing.type == DRAWING_TYPE_OLD_BITMAP)
    {
        bool is_tile = drawing.layer == LAYER_TILE || drawing.layer == LAYER_BACKGROUND_MAIN;
        f32_8x is_tile_multiplier = set1_f32(f32(1 - is_tile));

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
            paint_rect.min.x = (f32)((i32)paint_rect.min.x & ~7);
        }

        if ((i32)paint_rect.max.x & 7)
        {
            paint_rect.max.x = (f32)(((i32)paint_rect.max.x & ~7) + 8);
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
            for (i32 y = (i32)paint_rect.min.y; y < paint_rect.max.y; y++)
            {
                u32 *pixel_ptr = screen.pixels + y * (i32)screen.size.x + (i32)paint_rect.min.x;
                V2_8x d = V2_8x{
                              set1_f32(paint_rect.min.x) + zero_to_seven,
                              set1_f32((f32)y)} -
                          origin_8x;

                for (i32 x = (i32)paint_rect.min.x; x < paint_rect.max.x; x += 8)
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
void draw_light(Game_memory *memory, Bitmap screen, Camera camera, V2 world_pos, f32 inner_radius, f32 world_radius)
{
    inner_radius *= memory->light_lvl;
    world_radius *= memory->light_lvl;
    f32 darkness_scale = memory->darkness.size.x / screen.size.x;
    f32 radius = world_radius * camera.scale.x * darkness_scale;
    V2 center = world_to_screen(screen, camera, world_pos) * darkness_scale;

    Rect rect = {
        center - V2{radius, radius},
        center + V2{radius, radius},
    };
    Rect screen_rect = {
        V2{0, 0},
        memory->darkness.size,
    };
    rect = intersect(rect, screen_rect);

    f32 radius_sqr = radius * radius;

    for (f32 y = rect.min.y; y <= rect.max.y; y++)
    {
        for (f32 x = rect.min.x; x <= rect.max.x; x++)
        {
            V2 d = {x, y};
            f32 dist_sqr = length_sqr(d - center);

            ARGB pixel = {memory->darkness.pixels[(i32)y * (i32)memory->darkness.pitch + (i32)x]};
            f32 intensity = min(pixel.a / 255.0f, dist_sqr / radius_sqr);
            memory->darkness.pixels[(i32)y * (i32)memory->darkness.pitch + (i32)x] = v4_to_argb({0, 0, 0, intensity}).argb;
        }
    }
}

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
    Side tile_side;
} Collision;

typedef struct
{
    Collision x;
    Collision y;
    bool expanded_collision;
} Collisions;

Game_Object *add_game_object(Game_memory *memory, Game_Object_Type type, V2 pos)
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

        0,                                                                     //пройденный путь в пикселях
        (Direction)(random_int(&memory->__global_random_state, 0, 1) * 2 - 1), //направление
        Condition_IDLE,                                                        //состояние
        memory->bitmaps[Bitmap_type_NONE],                                     //спрайт

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
        game_object.jump_timer = add_timer(memory, 0);
        game_object.jump = add_timer(memory, 0);
        game_object.can_jump = add_timer(memory, 0);
        game_object.looking_key_held_timer = add_timer(memory, 0);
        game_object.crouching_animation_timer = add_timer(memory, 0);
        game_object.hanging_animation_timer = add_timer(memory, 0);
        game_object.cant_control_timer = add_timer(memory, 0);
        game_object.invulnerable_timer = add_timer(memory, 0);

        game_object.jump_height = (f32)(TILE_SIZE_PIXELS * 2.2) - game_object.collision_box.y;
    }

    if (type == Game_Object_ZOMBIE)
    {
        game_object.healthpoints = 1;

        game_object.jump = add_timer(memory, -1);
        game_object.can_jump = add_timer(memory, 0);

        game_object.jump_duration = 15;

        game_object.go_left = random_int(&memory->__global_random_state, 0, 1);
        game_object.go_right = !game_object.go_left;
    }

    i32 slot_index = memory->game_object_count;

    for (i32 objectIndex = 0; objectIndex < memory->game_object_count; objectIndex++)
    {
        if (memory->game_objects[objectIndex].exists == false)
        {
            slot_index = objectIndex;
            break;
        }
    }

    if (slot_index == memory->game_object_count)
    {
        memory->game_object_count++;
    }

    memory->game_objects[slot_index] = game_object;

    return &memory->game_objects[slot_index];
}

#define DISTANT_HANGING_VALUE 7

bool test_wall(f32 wall_x, f32 obj_speed_x, f32 obj_speed_y, f32 obj_rel_pos_x, f32 obj_rel_pos_y, f32 *min_time, f32 min_y, f32 max_y)
{
    bool hit = false;

    f32 time_epsilon = 0.001f;
    if (obj_speed_x != 0.0f)
    {
        f32 time = (wall_x - obj_rel_pos_x) / obj_speed_x;
        f32 y = obj_rel_pos_y + time * obj_speed_y;
        if ((time >= 0.0f) && (time < *min_time))
        {
            if (y >= min_y && y <= max_y)
            {
                *min_time = max(0.0f, time - time_epsilon);
                hit = true;
            }
        }
    }

    return hit;
}

void check_collision(Game_memory *memory, Game_Object *game_object)
{
    V2 old_pos = game_object->pos + game_object->collision_box_pos;
    V2 new_pos = old_pos + game_object->speed;
    V2 start_tile = min(old_pos - game_object->collision_box, new_pos - game_object->collision_box);
    V2 finish_tile = max(old_pos + game_object->collision_box, new_pos + game_object->collision_box);

    start_tile = floor(start_tile / TILE_SIZE_PIXELS);
    finish_tile = ceil(finish_tile / TILE_SIZE_PIXELS);

    f32 remaining_time = 1.0f;
    V2 total_speed = game_object->speed;
    for (i32 iterations = 0; iterations < 4 && remaining_time > 0.0f; iterations++)
    {
        f32 min_time = 1.0f;
        V2 wall_normal = V2{};

        for (i32 tile_y = (i32)start_tile.y; tile_y <= finish_tile.y; tile_y++)
        {
            for (i32 tile_x = (i32)start_tile.x; tile_x <= finish_tile.x; tile_x++)
            {
                i32 index = get_index(V2{(f32)tile_x, (f32)tile_y});
                Tile tile = memory->tile_map[index];
                if (tile.solid)
                {
                    V2 tile_pos = V2{(f32)tile_x, (f32)tile_y} * TILE_SIZE_PIXELS;
                    V2 tile_min = (V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS} + game_object->collision_box) * (-0.5);
                    V2 tile_max = (V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS} + game_object->collision_box) * 0.5;

                    V2 obj_rel_pos = game_object->pos + game_object->collision_box_pos - tile_pos;

                    if (test_wall(tile_min.x, total_speed.x, total_speed.y, obj_rel_pos.x, obj_rel_pos.y, &min_time, tile_min.y, tile_max.y))
                    {
                        wall_normal = V2{-1, 0};
                    }
                    if (test_wall(tile_max.x, total_speed.x, total_speed.y, obj_rel_pos.x, obj_rel_pos.y, &min_time, tile_min.y, tile_max.y))
                    {
                        wall_normal = V2{1, 0};
                    }
                    if (test_wall(tile_min.y, total_speed.y, total_speed.x, obj_rel_pos.y, obj_rel_pos.x, &min_time, tile_min.x, tile_max.x))
                    {
                        wall_normal = V2{0, -1};
                    }
                    if (test_wall(tile_max.y, total_speed.y, total_speed.x, obj_rel_pos.y, obj_rel_pos.x, &min_time, tile_min.x, tile_max.x))
                    {
                        wall_normal = V2{0, 1};
                    }
                }
            }
        }

        //передвигаем персонажа до точки столкновения
        game_object->pos += min_time * total_speed;

        //убираем часть скорости, которую мы уже прошли
        total_speed *= (1.0f - min_time) * remaining_time;

        //удаляем скорость столкновения
        game_object->speed -= dot(game_object->speed, wall_normal) * wall_normal;
        total_speed -= dot(total_speed, wall_normal) * wall_normal;

        remaining_time -= min_time * remaining_time;
    }
}

bool deal_damage(Game_memory *memory, Game_Object *dealing_object, Game_Object *taking_object, f32 damage)
{
    bool result = false;
    if (memory->timers[taking_object->invulnerable_timer] < 0)
    {
        taking_object->healthpoints -= damage;
        result = true;
    }
    return result;
}

#define JUMP_ON_ENEMY_BOOST 15
#define KNOCKBACK \
    V2 { 25, 10 }

void check_object_collision(Game_memory *memory, Game_Object *game_object, Game_Object_Type *triggering_objects, i32 triggering_objects_count)
{
    for (i32 game_object_index = 0; game_object_index < memory->game_object_count; game_object_index++)
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
                            if (deal_damage(memory, game_object, &memory->game_objects[game_object_index], 1))
                            {
                                game_object->speed.y = JUMP_ON_ENEMY_BOOST;
                            }
                        }
                        else
                        {
                            if (deal_damage(memory, &memory->game_objects[game_object_index], game_object, 1))
                            {
                                memory->timers[game_object->cant_control_timer] = 30;
                                memory->timers[game_object->invulnerable_timer] = 60;
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
                        if (deal_damage(memory, &memory->game_objects[game_object_index], game_object, 1))
                        {
                            memory->timers[game_object->cant_control_timer] = 30;
                            memory->timers[game_object->invulnerable_timer] = 60;
                            if (game_object->pos.x > collided_object.pos.x)
                            {
                                game_object->speed.x = KNOCKBACK.x;
                                memory->game_objects[game_object_index].looking_direction = Direction_RIGHT;
                            }
                            else
                            {
                                game_object->speed.x = -KNOCKBACK.x;
                                memory->game_objects[game_object_index].looking_direction = Direction_LEFT;
                            }
                            game_object->speed.y = KNOCKBACK.y;
                        }
                    }
                }
            }
        }
    }
}

bool check_vision_box(Game_memory *memory, i32 *trigger_index, V2 vision_point, V2 vision_pos, V2 vision_size, Game_Object_Type *triggering_objects, i32 triggering_objects_count, bool goes_through_walls, bool draw)
{
    bool vision_triggered = false;
    for (i32 game_object_index = 0; game_object_index < memory->game_object_count; game_object_index++)
    {
        for (i32 triggers_index = 0; triggers_index < triggering_objects_count; triggers_index++)
        {
            Game_Object game_object = memory->game_objects[game_object_index];
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
                        for (i32 y = (i32)min_point.y; y <= max_point.y; y++)
                        {
                            for (i32 x = (i32)min_point.x; x <= max_point.x; x++)
                            {
                                V2 tile_pos = V2{(f32)x, (f32)y};
                                if (memory->tile_map[get_index(tile_pos)].solid)
                                {
                                    tile_pos *= TILE_SIZE_PIXELS;
                                    i32 collisions = 0;
                                    f32 x1 = (f32)(((tile_pos.y - TILE_SIZE_PIXELS * 0.5) - b) / k);
                                    f32 x2 = (f32)(((tile_pos.y + TILE_SIZE_PIXELS * 0.5) - b) / k);
                                    f32 y1 = (f32)(k * (tile_pos.x - TILE_SIZE_PIXELS * 0.5) + b);
                                    f32 y2 = (f32)(k * (tile_pos.x + TILE_SIZE_PIXELS * 0.5) + b);
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
        draw_rect(memory, vision_pos, vision_size, 0, 0xFF00FF00, LAYER_FORGROUND);
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

i32 check_for_interactive_tiles(Game_memory *memory, Game_Object *game_object)
{
    i32 result = -1;
    V2 obj_tile_pos = V2{roundf(game_object->pos.x / TILE_SIZE_PIXELS), roundf(game_object->pos.y / TILE_SIZE_PIXELS)};

    i32 index = get_index(obj_tile_pos);
    Tile tile = memory->tile_map[index];
    if (tile.interactive != -1)
    {
        result = index;
    }
    return result;
}

void update_game_object(Game_memory *memory, Game_Object *game_object, Input input, Bitmap screen)
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
        // Condition supposed_cond;

        V2 recent_speed = game_object->speed;

        //движение игрока
        if (memory->timers[game_object->cant_control_timer] < 0)
        {
            if (input.z.went_down)
            {
                memory->timers[game_object->jump] = JUMP_BUTTON_DURATION;
            }

            if (memory->timers[game_object->jump] > 0)
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
            memory->timers[game_object->jump] = 0;
        }

        //константы ускорения
        if (input.shift.is_down)
        {
            game_object->accel = (f32)NORMAL_ACCEL;
        }
        else
        {
            game_object->accel = (f32)RUNNING_ACCEL;
        }
        if (game_object->condition == Condition_CROUCHING_IDLE || game_object->condition == Condition_CROUCHING_MOOVING)
        {
            game_object->accel = (f32)CROUCHING_ACCEL;
        }

        f32 gravity = -2 * game_object->jump_height / (game_object->jump_duration * game_object->jump_duration);

        //скорость по x
        game_object->speed += {(game_object->go_right - game_object->go_left) * game_object->accel, (input.up.is_down - input.down.is_down) * game_object->accel};

        //прыжок
        if (input.z.went_down && memory->timers[game_object->can_jump] > 0)
        {
            memory->timers[game_object->jump] = 0;
            memory->timers[game_object->can_jump] = 0;
            if (game_object->condition == Condition_HANGING || game_object->condition == Condition_HANGING_LOOKING_DOWN || game_object->condition == Condition_HANGING_LOOKING_UP)
            {
                if (memory->timers[game_object->hanging_animation_timer] > 0)
                {
                    memory->timers[game_object->jump] = memory->timers[game_object->hanging_animation_timer] + 2;
                }
                else
                {
                    if (!input.down.is_down)
                    {
                        game_object->speed.y = 2 * game_object->jump_height / game_object->jump_duration;
                        memory->timers[game_object->jump_timer] = (i32)ceilf(game_object->jump_duration);
                    }
                    game_object->condition = Condition_FALLING;
                }
            }
            else
            {
                game_object->speed.y = 2 * game_object->jump_height / game_object->jump_duration;
                memory->timers[game_object->jump_timer] = (i32)ceilf(game_object->jump_duration);
            }
        }

        if (memory->timers[game_object->jump_timer] > 0)
        {
            if (!input.z.is_down)
            {
                memory->timers[game_object->jump_timer] = 0;
            }
        }

        if (game_object->speed.y > 0 && memory->timers[game_object->jump_timer] <= 0)
        {
            gravity *= 2;
        }

        //гравитация
        // game_object->speed.y += gravity;

        if (game_object->speed.y > TILE_SIZE_PIXELS * 0.5)
        {
            game_object->speed.y = TILE_SIZE_PIXELS * 0.5;
        }

        f32 sliding_speed = -(game_object->hit_box.y + TILE_SIZE_PIXELS) / 2 / (f32)(HANGING_ANIMATION_TIME);

        if (game_object->condition == Condition_HANGING || game_object->condition == Condition_HANGING_LOOKING_DOWN || game_object->condition == Condition_HANGING_LOOKING_UP)
        {
            game_object->speed.x = 0;
            if (memory->timers[game_object->hanging_animation_timer] <= 0)
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

        //столкновения
        Game_Object old_object = *game_object;
        check_collision(memory, game_object);

        // //функции движения, связанные с коллизией
        // //состояние падает
        // supposed_cond = Condition_FALLING;

        // //состояние стоит
        // if (collisions.y.happened && collisions.y.tile_side == Side_TOP)
        // {
        //     supposed_cond = Condition_IDLE;
        //     timers[game_object->can_jump] = ADDITIONAL_JUMP_FRAMES;

        //     //состояние ползком и смотрит вверх
        //     if (input.down.is_down && !input.up.is_down)
        //     {
        //         supposed_cond = Condition_CROUCHING_IDLE;
        //     }
        //     else if (input.up.is_down && !input.down.is_down && fabs(game_object->speed.x) <= 1 && timers[game_object->crouching_animation_timer] <= 0)
        //     {
        //         supposed_cond = Condition_LOOKING_UP;
        //     }

        //     //состояния движения
        //     if (fabs(game_object->speed.x) > 1 && (supposed_cond == Condition_IDLE || supposed_cond == Condition_CROUCHING_IDLE))
        //     {

        //         game_object->moved_through_pixels += round(game_object->speed.x);

        //         if (supposed_cond == Condition_IDLE)
        //         {
        //             supposed_cond = Condition_MOOVING;
        //         }
        //         else
        //         {
        //             supposed_cond = Condition_CROUCHING_MOOVING;
        //         }
        //     }
        // }

        // if (input.left.went_down || input.right.went_down)
        // {
        //     game_object->moved_through_pixels = 0;
        // }

        // V2 player_delta = game_object->pos - old_object.pos;

        // //состояние весит
        // if (collisions.x.happened || collisions.expanded_collision)
        // {
        //     V2 collided_x_tile_pos = get_tile_pos(collisions.x.tile_index);
        //     i32 collision_up_tile_index = get_index(V2{collided_x_tile_pos.x, collided_x_tile_pos.y + 1});
        //     i32 collision_frontup_tile_index = get_index(V2{collided_x_tile_pos.x - game_object->looking_direction, collided_x_tile_pos.y + 1});

        //     if (!tile_map[collision_up_tile_index].solid && !tile_map[collision_frontup_tile_index].solid)
        //     {
        //         if (player_delta.y < 0 &&
        //             old_object.pos.y + game_object->collision_box_pos.y > collided_x_tile_pos.y * TILE_SIZE_PIXELS - 3 &&
        //             old_object.pos.y + game_object->collision_box_pos.y + player_delta.y <= collided_x_tile_pos.y * TILE_SIZE_PIXELS - 3)
        //         {
        //             supposed_cond = Condition_HANGING;

        //             game_object->speed = {0, 0};

        //             game_object->pos.y = collided_x_tile_pos.y * TILE_SIZE_PIXELS - game_object->collision_box_pos.y - 3;

        //             game_object->pos.x = collided_x_tile_pos.x * TILE_SIZE_PIXELS - game_object->looking_direction * (TILE_SIZE_PIXELS * 0.5 + game_object->collision_box.x * 0.5 + 0.001f) + game_object->collision_box_pos.x;
        //         }
        //     }
        // }

        // //переход на стенку из состояния ползком
        // if (supposed_cond == Condition_CROUCHING_IDLE || supposed_cond == Condition_CROUCHING_MOOVING)
        // {
        //     V2 collided_y_tile_pos = get_tile_pos(collisions.y.tile_index);
        //     V2 collisionY_front_tile_pos = collided_y_tile_pos + V2{(f32)game_object->looking_direction, 0};
        //     i32 collisionY_front_tile_pos_index = get_index(collisionY_front_tile_pos);

        //     if (!tile_map[collisionY_front_tile_pos_index].solid &&
        //         ((old_object.pos.x + player_delta.x + game_object->collision_box.x * 0.5 + game_object->collision_box_pos.x <= collisionY_front_tile_pos.x * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS * 0.5) && (ceilf(old_object.pos.x + game_object->collision_box_pos.x + game_object->collision_box.x * 0.5) >= collisionY_front_tile_pos.x * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS * 0.5) ||
        //          (old_object.pos.x + player_delta.x - game_object->collision_box.x * 0.5 + game_object->collision_box_pos.x >= collisionY_front_tile_pos.x * TILE_SIZE_PIXELS - TILE_SIZE_PIXELS * 0.5) && (floorf(old_object.pos.x + game_object->collision_box_pos.x - game_object->collision_box.x * 0.5) <= collisionY_front_tile_pos.x * TILE_SIZE_PIXELS - TILE_SIZE_PIXELS * 0.5)))
        //     {
        //         game_object->pos.x = collisionY_front_tile_pos.x * TILE_SIZE_PIXELS - ((TILE_SIZE_PIXELS - game_object->collision_box.x) * 0.5 - 0.001f) * game_object->looking_direction;

        //         supposed_cond = Condition_HANGING;

        //         game_object->looking_direction = (Direction)(-game_object->looking_direction);

        //         game_object->speed.x = 0;
        //         game_object->speed.y = sliding_speed;

        //         game_object->condition = Condition_IDLE;
        //         timers[game_object->hanging_animation_timer] = HANGING_ANIMATION_TIME;
        //     }
        // }

        // //состояние смежные с состоянием весит
        // if ((game_object->condition == Condition_HANGING || game_object->condition == Condition_HANGING_LOOKING_DOWN || game_object->condition == Condition_HANGING_LOOKING_UP))
        // {

        //     if (input.up.is_down && !input.down.is_down)
        //     {
        //         supposed_cond = Condition_HANGING_LOOKING_UP;
        //     }
        //     else if (input.down.is_down && !input.up.is_down)
        //     {
        //         supposed_cond = Condition_HANGING_LOOKING_DOWN;
        //     }
        //     else
        //     {
        //         supposed_cond = Condition_HANGING;
        //     }
        // }

        //работа с предполагаемым состоянием (возможно нужно убрать)

        // if ((game_object->condition != Condition_HANGING && game_object->condition != Condition_HANGING_LOOKING_DOWN && game_object->condition != Condition_HANGING_LOOKING_UP) ||
        //     (game_object->condition == Condition_HANGING || game_object->condition == Condition_HANGING_LOOKING_DOWN || game_object->condition == Condition_HANGING_LOOKING_UP) &&
        //         (supposed_cond == Condition_HANGING || supposed_cond == Condition_HANGING_LOOKING_DOWN || supposed_cond == Condition_HANGING_LOOKING_UP))
        // {
        //     //начинаем смотреть вверх и вниз
        //     if ((game_object->condition != Condition_CROUCHING_IDLE && supposed_cond == Condition_CROUCHING_IDLE) ||
        //         (game_object->condition != Condition_LOOKING_UP && supposed_cond == Condition_LOOKING_UP) ||
        //         (game_object->condition != Condition_HANGING_LOOKING_UP && supposed_cond == Condition_HANGING_LOOKING_UP) ||
        //         (game_object->condition != Condition_HANGING_LOOKING_DOWN && supposed_cond == Condition_HANGING_LOOKING_DOWN))
        //     {
        //         timers[game_object->looking_key_held_timer] = LOOKING_KEY_HOLDING;
        //     }

        //     //поднимаемся и встаём
        //     if ((game_object->condition != Condition_CROUCHING_IDLE && game_object->condition != Condition_CROUCHING_MOOVING) && (supposed_cond == Condition_CROUCHING_IDLE || supposed_cond == Condition_CROUCHING_MOOVING))
        //     {
        //         if (timers[game_object->crouching_animation_timer] < 0)
        //         {
        //             timers[game_object->crouching_animation_timer] = 0;
        //         }
        //         timers[game_object->crouching_animation_timer] = (CROUCHING_ANIMATION_TIME - timers[game_object->crouching_animation_timer] * 0.5) * 2;
        //     }
        //     if ((game_object->condition == Condition_CROUCHING_IDLE || game_object->condition == Condition_CROUCHING_MOOVING) && (supposed_cond != Condition_CROUCHING_IDLE && supposed_cond != Condition_CROUCHING_MOOVING))
        //     {
        //         if (timers[game_object->crouching_animation_timer] < 0)
        //         {
        //             timers[game_object->crouching_animation_timer] = 0;
        //         }
        //         timers[game_object->crouching_animation_timer] = CROUCHING_ANIMATION_TIME - timers[game_object->crouching_animation_timer] * 0.5;
        //     }
        //     game_object->condition = supposed_cond;
        // }

        //изменяем положение
        // game_object->pos += game_object->speed;

        //столкновения и движение
        Game_Object_Type triggers[1];
        triggers[0] = Game_Object_ZOMBIE;
        check_object_collision(memory, game_object, triggers, 1);

        //что делают состояния
        if (game_object->condition == Condition_IDLE || game_object->condition == Condition_MOOVING)
        {
            if (memory->timers[game_object->crouching_animation_timer] > 0)
            {
                i32 step = (i32)ceil(memory->timers[game_object->crouching_animation_timer] * 0.5);
                game_object->sprite = memory->bitmaps[Bitmap_type_PLAYER_CROUCH_START + step];
            }
            else if (game_object->condition == Condition_IDLE)
            {
                game_object->speed.x *= game_object->friction;
                game_object->sprite = memory->bitmaps[Bitmap_type_PLAYER_IDLE];
            }
            else if (game_object->condition == Condition_MOOVING)
            {
                i8 step = (i8)floor(fabsf((f32)game_object->moved_through_pixels) / 35);
                while (step > 5)
                {
                    step -= 6;
                }

                game_object->sprite = memory->bitmaps[Bitmap_type_PLAYER_STEP + step];
            }
        }

        if (game_object->condition == Condition_CROUCHING_MOOVING || game_object->condition == Condition_CROUCHING_IDLE)
        {
            if (memory->timers[game_object->crouching_animation_timer] > 0)
            {
                const i8 step = 4 - (i8)ceil(memory->timers[game_object->crouching_animation_timer] * 0.25);
                game_object->sprite = memory->bitmaps[Bitmap_type_PLAYER_CROUCH_START + step];
            }
            else
            {
                if (game_object->condition == Condition_CROUCHING_IDLE)
                {
                    game_object->sprite = memory->bitmaps[Bitmap_type_PLAYER_CROUCH_IDLE];
                }
                if (game_object->condition == Condition_CROUCHING_MOOVING)
                {
                    i8 step = (i8)floor(fabsf((f32)game_object->moved_through_pixels) * 0.1);
                    while (step > 5)
                    {
                        step -= 6;
                    }
                    game_object->sprite = memory->bitmaps[Bitmap_type_PLAYER_CROUCH_STEP + step];
                }
            }
        }

        if (game_object->condition == Condition_LOOKING_UP)
        {
            game_object->sprite = memory->bitmaps[Bitmap_type_PLAYER_LOOKING_UP];
        }

        if (game_object->condition == Condition_FALLING)
        {
            game_object->sprite = memory->bitmaps[Bitmap_type_PLAYER_JUMP];
        }

        if (game_object->condition == Condition_HANGING || game_object->condition == Condition_HANGING_LOOKING_UP || game_object->condition == Condition_HANGING_LOOKING_DOWN)
        {
            if (game_object->condition == Condition_HANGING)
            {
                game_object->sprite = memory->bitmaps[Bitmap_type_PLAYER_HANGING];
            }
            else if (game_object->condition == Condition_HANGING_LOOKING_DOWN)
            {
                game_object->sprite = memory->bitmaps[Bitmap_type_PLAYER_HANGING_DOWN];
            }
            else
            {
                game_object->sprite = memory->bitmaps[Bitmap_type_PLAYER_HANGING_UP];
            }

            if ((memory->timers[game_object->hanging_animation_timer] <= 1 && (length(game_object->speed) > 0)) ||
                (memory->timers[game_object->hanging_animation_timer] > 1 && game_object->speed.y != sliding_speed))
            {
                game_object->condition = Condition_FALLING;
                memory->timers[game_object->hanging_animation_timer] = 0;
            }

            memory->timers[game_object->can_jump] = 4;
        }

        //камера
        memory->camera.target = game_object->pos;

        //смотрим вниз и вверх
        if ((game_object->condition == Condition_CROUCHING_IDLE || game_object->condition == Condition_HANGING_LOOKING_DOWN) && memory->timers[game_object->looking_key_held_timer] <= 0)
        {
            memory->camera.target.y = game_object->pos.y - screen.size.y / memory->camera.scale.y * 0.5f + f32(LOOKING_DISTANCE);
        }

        if ((game_object->condition == Condition_LOOKING_UP || game_object->condition == Condition_HANGING_LOOKING_UP) && memory->timers[game_object->looking_key_held_timer] <= 0)
        {
            memory->camera.target.y = game_object->pos.y + screen.size.y / memory->camera.scale.y * 0.5f - f32(LOOKING_DISTANCE);
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

        game_object->deflection += (game_object->target_deflection - game_object->deflection) * 0.3f;

        if (length(game_object->target_deflection - game_object->deflection) < 0.1f)
        {
            game_object->target_deflection = V2{0, 0};
        }

        //границы для камеры
        border_camera(memory, screen);

        //камера
        memory->camera.pos = memory->camera.target; // += (camera.target - camera.pos) * 0.25f;

        //интерфейс

#define BAR_HEIGHT 50
#define BAR_WIDTH 150
#define INTERVAL 10

        V2 camera_shake = V2{0, 0};

        if (game_object->healthpoints / game_object->max_healthpoints <= 0.5)
        {
            camera_shake = {random_float(&memory->__global_random_state, -10 * (0.5f - game_object->healthpoints / game_object->max_healthpoints), 10 * (0.5f - game_object->healthpoints / game_object->max_healthpoints)), random_float(&memory->__global_random_state, -5 * (0.5f - game_object->healthpoints / game_object->max_healthpoints), 5 * (0.5f - game_object->healthpoints / game_object->max_healthpoints))};
        }

        draw_bitmap(memory,
                    memory->camera.pos + camera_shake + V2{-screen.size.x, screen.size.y} / memory->camera.scale * 0.5f + V2{BAR_WIDTH / 2 + INTERVAL, -(BAR_HEIGHT / 2 + INTERVAL)} - V2{(1 - game_object->healthpoints / game_object->max_healthpoints) * BAR_WIDTH, 0} / 2,
                    V2{game_object->healthpoints / game_object->max_healthpoints * BAR_WIDTH, BAR_HEIGHT},
                    0, memory->bitmaps[Bitmap_type_HEALTH], memory->UI_alpha, LAYER_UI);

        draw_bitmap(memory,
                    memory->camera.pos + camera_shake + V2{-screen.size.x, screen.size.y} / memory->camera.scale * 0.5f + V2{BAR_WIDTH / 2 + INTERVAL, -(BAR_HEIGHT / 2 + INTERVAL)},
                    V2{BAR_WIDTH, BAR_HEIGHT}, 0, memory->bitmaps[Bitmap_type_HEALTH_BAR], memory->UI_alpha, LAYER_UI);

        //взаимодействие с тайлами

        if (input.space.went_down)
        {
            i32 tile_index = check_for_interactive_tiles(memory, game_object);
            if (tile_index != -1)
            {
                Tile tile = memory->tile_map[tile_index];
                V2 tile_pos = get_tile_pos(tile_index);
                if (tile.type == Tile_Type_EXIT)
                {
                    if (game_object->condition != Condition_FALLING)
                    {
                        game_object->pos.x = tile_pos.x * TILE_SIZE_PIXELS;
                        game_object->speed = V2{0, 0};

                        memory->timers[game_object->cant_control_timer] = 9999;
                        memory->timers[game_object->invulnerable_timer] = 9999;
                        memory->timers[tile.interactive] = 60;

                        game_object->layer = LAYER_ON_ON_BACKGROUND;
                    }
                }
            }
        }

        //прорисовка игрока

        //хитбокс
        draw_rect(memory, game_object->pos + game_object->collision_box_pos, game_object->collision_box, 0, 0xFFFF0000, LAYER_FORGROUND);

        draw_bitmap(memory, game_object->pos + V2{0, game_object->deflection.y * 0.5f},
                    V2{(game_object->sprite.size.x * SPRITE_SCALE + game_object->deflection.x) * game_object->looking_direction,
                       game_object->sprite.size.y * SPRITE_SCALE + game_object->deflection.y},
                    0, game_object->sprite, 1, game_object->layer);

        draw_light(memory, screen, memory->camera, game_object->pos, 200, 300);
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
            if (memory->timers[game_object->jump] == 0)
            {
                game_object->speed.y = f32(2 * game_object->jump_height / game_object->jump_duration * random_float(&memory->__global_random_state, 0.4f, 1));
                game_object->accel = ZOMBIE_ACCEL;
            }
            else if (memory->timers[game_object->jump] > 0)
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
        Collisions collisions = {};
        check_collision(memory, game_object);

        //если стоит
        if (collisions.y.happened && collisions.y.tile_side == Side_TOP)
        {
            game_object->accel = 1;
            //состояния стоит и движение
            if (fabs(game_object->speed.x) > 1)
            {
                if (game_object->moved_through_pixels > 0 && game_object->speed.x < 0 || game_object->moved_through_pixels < 0 && game_object->speed.x > 0)
                {
                    game_object->moved_through_pixels = 0;
                }
                game_object->moved_through_pixels += (i32)roundf(game_object->speed.x);
                game_object->condition = Condition_MOOVING;
            }
            else
            {
                game_object->condition = Condition_IDLE;
            }

            //если не заряжает прыжок
            if (memory->timers[game_object->jump] < 0 && game_object->speed.y <= 0)
            {
                V2 collided_y_tile_pos = get_tile_pos(collisions.y.tile_index);

                Tile downleft_tile = memory->tile_map[get_index(collided_y_tile_pos + V2{-1, 0})];
                Tile downright_tile = memory->tile_map[get_index(collided_y_tile_pos + V2{1, 0})];
                Tile left_tile = memory->tile_map[get_index(collided_y_tile_pos + V2{-1, 1})];
                Tile right_tile = memory->tile_map[get_index(collided_y_tile_pos + V2{1, 1})];
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
            memory->timers[game_object->jump] = -1;
        }

        //эффекты состояний
        if (game_object->condition == Condition_IDLE)
        {
            game_object->speed.x *= game_object->friction;
            game_object->sprite = memory->bitmaps[Bitmap_type_ZOMBIE_IDLE];
        }

        if (game_object->condition == Condition_MOOVING)
        {
            i8 step = (i8)floor(fabsf((f32)game_object->moved_through_pixels) * 0.05);
            while (step > 5)
            {
                step -= 6;
            }

            game_object->sprite = memory->bitmaps[Bitmap_type_ZOMBIE_STEP + step];
        }

        if (game_object->condition == Condition_FALLING)
        {
            bool bool_direction = (game_object->looking_direction + 1) * 0.5;
            game_object->go_right = bool_direction;
            game_object->go_left = !bool_direction;
            game_object->sprite = memory->bitmaps[Bitmap_type_ZOMBIE_IDLE];
        }

        game_object->pos += game_object->speed;

        Game_Object_Type triggers[1];
        triggers[0] = Game_Object_PLAYER;

        i32 trigger_index = -1;

        if (check_vision_box(memory, &trigger_index, game_object->pos, game_object->pos + V2{(TILE_SIZE_PIXELS * 5 + game_object->hit_box.x) * (f32)(game_object->looking_direction), 0} * 0.5, VISION_BOX, triggers, 1, false, false) &&
            memory->timers[game_object->jump] < 0)
        {
            memory->timers[game_object->jump] = JUMP_LATENCY;
            if (trigger_index != -1)
            {
                Game_Object trigger = memory->game_objects[trigger_index];
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

        game_object->deflection += (game_object->target_deflection - game_object->deflection) * 0.3f;

        if (length(game_object->target_deflection - game_object->deflection) < 0.1f)
        {
            game_object->target_deflection = V2{0, 0};
        }

        // draw_rect(game_object->pos + game_object->hit_box_pos, game_object->hit_box, 0, 0xFFFF0000, LAYER_GAME_OBJECT);
        // draw_rect(game_object->pos, game_object->collision_box, 0, 0xFF00FF00, LAYER_FORGROUND);
        draw_bitmap(memory, game_object->pos + V2{0, game_object->deflection.y * 0.5f}, V2{(game_object->sprite.size.x * SPRITE_SCALE + game_object->deflection.x) * game_object->looking_direction, (game_object->sprite.size.y * SPRITE_SCALE + game_object->deflection.y)}, 0, game_object->sprite, 1, game_object->layer);
    }

    if (game_object->healthpoints <= 0)
    {
        game_object->exists = false;
    }
}

void generate_new_map(Game_memory *memory, Bitmap screen)
{
    char *normal_chunks[] = {
        // "          "
        // "          "
        // "          "
        // "          "
        // "          "
        // "          "
        // "          "
        // "          ",
        "###  #####"
        "#     ####"
        "TTT   ####"
        "=== T  ###"
        "##      ##"
        "##    TT##"
        "#    ===##"
        "#  #######",

        "##    ## #"
        "#       ##"
        "##      ##"
        "#      ###"
        "##       #"
        "#       ##"
        "###    ###"
        "###TTTT###",

        "          "
        "          "
        "     ##   "
        " TT TTTT  "
        "          "
        "#T TTTTTT "
        "##        "
        "###======#",
    };

    char *down_passage_chunks[] = {
        // "          "
        // "          "
        // "          "
        // "          "
        // "          "
        // "          "
        // "          "
        // "          ",
        "          "
        "          "
        "          "
        "##     TT="
        "   #      "
        "     TTT#T"
        "T#TT   #=="
        "##### ####",

        "##    ## #"
        "        ##"
        "##      ##"
        "       ## "
        "          "
        "  === ### "
        "###      ="
        "###T   ###",

        "          "
        " =        "
        "          "
        "          "
        "   #   #  "
        "= ##   ## "
        "####   ## "
        "####   ###",
    };

    char *enter_down_passage_chunks[] = {
        // "          "
        // "          "
        // "       # N"
        // "        # "
        // "          "
        // "          "
        // "          "
        // "          ",
        "#      #  "
        "#        #"
        "       #  "
        "     N  # "
        "     T    "
        " ^     TTT"
        "T#TT   ==="
        "####   ###",

        "          "
        "  N       "
        "TTTT      "
        "       ## "
        "    =     "
        "####=  TT#"
        "####=  ###"
        "####=  ###",

        "       #  "
        "TT      ##"
        "          "
        "    N     "
        "    ##    "
        "#TT=#  ###"
        "###=    ##"
        "#####  ###",
    };

    char *enter_chunks[] = {
        // "          "
        // "          "
        // "       # N"
        // "        # "
        // "          "
        // "          "
        // "          "
        // "          ",
        // "          "
        // "          "
        // "     N    "
        // "     #    "
        // "          "
        // "          "
        // "          "
        // "          ",

        "          "
        "          "
        "    N     "
        "   TTTT   "
        "   =###=  "
        "  =###### "
        "TT########"
        "##########",

        "          "
        "  N       "
        " ###      "
        "       ## "
        "  ^ #     "
        "####   ###"
        "####  ####"
        "#####  ###",

        "       #  "
        " ##     # "
        "          "
        "    N     "
        "    ##    "
        "#TT##  ## "
        "####   ###"
        "#####  ###",
    };

    char *passage_chunks[] = {
        // "          "
        // "          "
        // "          "
        // "          "
        // "          "
        // "          "
        // "          "
        // "          ",
        " PPPPPPP  "
        " #######  "
        "          "
        "         ="
        "          "
        "   TT     "
        "TTTTTTTTTT"
        "#======###",

        "  ##   ## "
        "#         "
        "        # "
        "       ## "
        "  #    ###"
        "TTTT  ####"
        "##########"
        "##########",

        "##     #  "
        " #      ##"
        "          "
        "          "
        "          "
        "#TT#  ## #"
        "###======#"
        "###T######",
    };

    char *exit_chunks[] = {
        // "          "
        // "          "
        // "          "
        // "          "
        // "          "
        // "          "
        // "          "
        // "          ",
        "          "
        " #        "
        "#         "
        "      T  #"
        "          "
        " #   X    "
        "  TTTTTT  "
        " ######## ",
        "          "
        "#         "
        "#         "
        "         #"
        "  X       "
        " ####     "
        "##TTTTTT  "
        "##########",
        "##      # "
        "         #"
        "          "
        "   X      "
        " #####    "
        "          "
        "  TTTTTT  "
        " ======== ",
    };

    //удаляем объекты
    memory->game_object_count = 0;

    //удаляем таймеры
    memory->timers_count = 0;

    //генерация
    f32 down_chunk_chance = 0.25;

    //карта тайлов
    V2 map_size = {CHUNK_COUNT_X * CHUNK_SIZE_X + BORDER_SIZE * 2, CHUNK_COUNT_Y * CHUNK_SIZE_Y + BORDER_SIZE * 2};
    i32 tile_count = i32(map_size.x * map_size.y);

    for (i32 index = 0; index < tile_count; index++)
    {
        memory->tile_map[index].type = Tile_Type_BORDER;
        memory->tile_map[index].solid = true;
        memory->tile_map[index].interactive = -1;
        memory->tile_map[index].sprite = memory->bitmaps[Bitmap_type_NONE];
    }

    //создание чанков
    char *chunks_strings[CHUNK_COUNT_X * CHUNK_COUNT_Y];

    //путь через тайлы
    V2 enter_chunk_pos = {(f32)random_int(&memory->__global_random_state, 0, CHUNK_COUNT_X - 1), CHUNK_COUNT_Y - 1};
    V2 chunk_pos = enter_chunk_pos;
    V2 end_chunk_pos = {(f32)random_int(&memory->__global_random_state, 0, CHUNK_COUNT_X - 1), 0};

    //заполняем чанки
    for (i32 index = 0; index < CHUNK_COUNT_X * CHUNK_COUNT_Y; index++)
    {
        chunks_strings[index] = normal_chunks[random_int(&memory->__global_random_state, 1, ARRAY_COUNT(normal_chunks)) - 1];
    }

    i8 direction = (i8)random_int(&memory->__global_random_state, Direction_LEFT, Direction_RIGHT);
    if ((direction == Direction_RIGHT && chunk_pos.x == CHUNK_COUNT_X - 1) || (direction == Direction_LEFT && chunk_pos.x == 0))
    {
        direction = -direction;
    }

    while (chunk_pos != end_chunk_pos)
    {
        if ((random_float(&memory->__global_random_state, 0, 1) <= down_chunk_chance || direction == Direction_VERTICAL) && chunk_pos.y > 0)
        {
            if (chunk_pos == enter_chunk_pos)
            {
                //чанк-вход с проходом вниз
                chunks_strings[(i32)(chunk_pos.y * CHUNK_COUNT_X + chunk_pos.x)] = enter_down_passage_chunks[random_int(&memory->__global_random_state, 1, ARRAY_COUNT(enter_down_passage_chunks)) - 1];
            }
            else
            {
                //чанк-проход вниз
                chunks_strings[(i32)(chunk_pos.y * CHUNK_COUNT_X + chunk_pos.x)] = down_passage_chunks[random_int(&memory->__global_random_state, 1, ARRAY_COUNT(down_passage_chunks)) - 1];
            }
            chunk_pos.y--;

            direction = (i8)random_int(&memory->__global_random_state, 0, 1) * 2 - 1;
        }
        else
        {
            if ((direction == Direction_RIGHT && chunk_pos.x == CHUNK_COUNT_X - 1) || (direction == Direction_LEFT && chunk_pos.x == 0))
            {
                direction = Direction_VERTICAL;
            }

            if (chunk_pos.y == 0)
            {
                direction = (i8)unit(V2{end_chunk_pos.x - chunk_pos.x, 0}).x;
            }

            if (direction != Direction_VERTICAL)
            {
                if (chunk_pos == enter_chunk_pos)
                {
                    //чанк-вход
                    chunks_strings[(i32)(chunk_pos.y * CHUNK_COUNT_X + chunk_pos.x)] = enter_chunks[random_int(&memory->__global_random_state, 1, ARRAY_COUNT(enter_chunks)) - 1];
                }
                else
                {
                    //проходной чанк
                    chunks_strings[(i32)(chunk_pos.y * CHUNK_COUNT_X + chunk_pos.x)] = passage_chunks[random_int(&memory->__global_random_state, 1, ARRAY_COUNT(passage_chunks)) - 1];
                }
                chunk_pos.x += direction;
            }
        }
        //чанк-выход
        if (chunk_pos == end_chunk_pos)
        {
            chunks_strings[(i32)(chunk_pos.y * CHUNK_COUNT_X + chunk_pos.x)] = exit_chunks[random_int(&memory->__global_random_state, 1, ARRAY_COUNT(exit_chunks)) - 1];
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
                    Bitmap sprite = memory->bitmaps[Bitmap_type_NONE];

                    switch (tile_char)
                    {
                    case ' ':
                    {
                        type = Tile_Type_NONE;
                        solid = false;
                        interactive = -1;
                    }
                    break;
                    case '#':
                    {
                        type = Tile_Type_NORMAL;
                        solid = true;
                        interactive = -1;

                        f32 chance = random_float(&memory->__global_random_state, 0, 1);
                        if (chance < 0.95)
                        {
                            if (chance > 0.15)
                            {
                                sprite = memory->bitmaps[Bitmap_type_BRICKS + random_int(&memory->__global_random_state, 0, 6)];
                            }
                            else
                            {
                                sprite = memory->bitmaps[Bitmap_type_STONE];
                            }
                        }
                        else
                        {
                            sprite = memory->bitmaps[Bitmap_type_BRICKS + random_int(&memory->__global_random_state, 7, 11)];
                        }
                    }
                    break;
                    case '=':
                    {
                        type = Tile_Type_NORMAL;
                        solid = true;
                        interactive = -1;
                        sprite = memory->bitmaps[Bitmap_type_ELEGANT_BRICK];
                    }
                    break;
                    case '-':
                    {
                        type = Tile_Type_BORDER;
                        solid = true;
                        interactive = -1;

                        sprite = memory->bitmaps[Bitmap_type_BORDER];
                    }
                    break;
                    case 'N':
                    {
                        type = Tile_Type_ENTER;
                        solid = false;
                        interactive = -1;

                        sprite = memory->bitmaps[Bitmap_type_DOOR];

                        //addPlayer
                        V2 spawn_pos = tile_pos * TILE_SIZE_PIXELS + V2{0, 100};
                        add_game_object(memory, Game_Object_PLAYER, spawn_pos);
                        memory->camera.target = spawn_pos;
                        border_camera(memory, screen);
                        memory->camera.pos = memory->camera.target;
                    }
                    break;
                    case 'X':
                    {
                        type = Tile_Type_EXIT;
                        solid = false;
                        interactive = add_timer(memory, -1);
                        sprite = memory->bitmaps[Bitmap_type_DOOR + 6];
                    }
                    break;
                    case 'T':
                    {
                        type = Tile_Type_FLOOR;
                        solid = true;
                        interactive = -1;
                    }
                    break;
                    case 'P':
                    {
                        type = Tile_Type_PARAPET;
                        solid = false;
                        interactive = -1;
                        sprite = memory->bitmaps[Bitmap_type_PARAPET];
                    }
                    break;
                    case '^':
                    {
                        type = Tile_Type_SPIKES;
                        solid = false;
                        interactive = -1;
                        sprite = memory->bitmaps[Bitmap_type_SPIKES];
                    }
                    break;
                    }

                    memory->tile_map[index].type = type;
                    memory->tile_map[index].solid = solid;
                    memory->tile_map[index].interactive = interactive;
                    memory->tile_map[index].sprite = sprite;
                }
            }
        }
    }

    //налаживаем свойства тайлов после полной расстановки
    for (i32 index = 0; index < tile_count; index++)
    {
        V2 tile_pos = get_tile_pos(index);
        Tile_type type = memory->tile_map[index].type;
        Bitmap sprite = memory->tile_map[index].sprite;

        if (type == Tile_Type_FLOOR)
        {
            Bitmap left_tile = memory->tile_map[get_index(tile_pos + V2{-1, 0})].sprite;
            Bitmap bottom_tile = memory->tile_map[get_index(tile_pos + V2{0, -1})].sprite;
            Bitmap bottomleft_tile = memory->tile_map[get_index(tile_pos + V2{-1, -1})].sprite;
            Bitmap bottomright_tile = memory->tile_map[get_index(tile_pos + V2{1, -1})].sprite;

            if (left_tile.pixels == memory->bitmaps[Bitmap_type_TILED_FLOOR].pixels ||
                bottom_tile.pixels == memory->bitmaps[Bitmap_type_TILED_FLOOR].pixels ||
                bottomleft_tile.pixels == memory->bitmaps[Bitmap_type_TILED_FLOOR].pixels ||
                bottomright_tile.pixels == memory->bitmaps[Bitmap_type_TILED_FLOOR].pixels)
            {
                sprite = memory->bitmaps[Bitmap_type_TILED_FLOOR];
            }
            else if (left_tile.pixels == memory->bitmaps[Bitmap_type_MARBLE_FLOOR].pixels ||
                     bottom_tile.pixels == memory->bitmaps[Bitmap_type_MARBLE_FLOOR].pixels ||
                     bottomleft_tile.pixels == memory->bitmaps[Bitmap_type_MARBLE_FLOOR].pixels ||
                     bottomright_tile.pixels == memory->bitmaps[Bitmap_type_MARBLE_FLOOR].pixels)
            {
                sprite = memory->bitmaps[Bitmap_type_MARBLE_FLOOR];
            }
            else
            {
                if (random_int(&memory->__global_random_state, 0, 1) == 0)
                {
                    sprite = memory->bitmaps[Bitmap_type_TILED_FLOOR];
                }
                else
                {
                    sprite = memory->bitmaps[Bitmap_type_MARBLE_FLOOR];
                }
            }
        }

        if (type == Tile_Type_BORDER)
        {
            sprite = memory->bitmaps[Bitmap_type_BORDER];
            if (tile_pos.x != 0 && tile_pos.y != 0 && tile_pos.x != (map_size.x - 1) && tile_pos.y != (map_size.y - 1))
            {
                Tile_type left_tile = memory->tile_map[get_index(tile_pos + V2{-1, 0})].type;
                Tile_type right_tile = memory->tile_map[get_index(tile_pos + V2{1, 0})].type;
                Tile_type top_tile = memory->tile_map[get_index(tile_pos + V2{0, 1})].type;
                Tile_type bottom_tile = memory->tile_map[get_index(tile_pos + V2{0, -1})].type;
                Tile_type bottomright_tile = memory->tile_map[get_index(tile_pos + V2{1, -1})].type;
                Tile_type topright_tile = memory->tile_map[get_index(tile_pos + V2{1, 1})].type;
                Tile_type bottomleft_tile = memory->tile_map[get_index(tile_pos + V2{-1, -1})].type;
                Tile_type topleft_tile = memory->tile_map[get_index(tile_pos + V2{-1, 1})].type;

                if (bottom_tile != Tile_Type_BORDER)
                {
                    sprite = memory->bitmaps[Bitmap_type_TRANSITION_BORDER_0];
                }
                else
                {
                    if (right_tile != Tile_Type_BORDER)
                    {
                        sprite = memory->bitmaps[Bitmap_type_TRANSITION_BORDER_05];
                    }
                    else if (bottomleft_tile != Tile_Type_BORDER)
                    {
                        sprite = memory->bitmaps[Bitmap_type_CORNER_BORDER_15];
                    }

                    if (left_tile != Tile_Type_BORDER)
                    {
                        sprite = memory->bitmaps[Bitmap_type_TRANSITION_BORDER_15];
                    }
                    else if (bottomright_tile != Tile_Type_BORDER)
                    {
                        sprite = memory->bitmaps[Bitmap_type_CORNER_BORDER_0];
                    }
                }

                if (top_tile != Tile_Type_BORDER)
                {
                    sprite = memory->bitmaps[Bitmap_type_TRANSITION_BORDER_1];
                }
                else
                {
                    if (right_tile != Tile_Type_BORDER)
                    {
                        sprite = memory->bitmaps[Bitmap_type_TRANSITION_BORDER_05];
                    }
                    else if (topright_tile != Tile_Type_BORDER)
                    {
                        sprite = memory->bitmaps[Bitmap_type_CORNER_BORDER_05];
                    }

                    if (left_tile != Tile_Type_BORDER)
                    {
                        sprite = memory->bitmaps[Bitmap_type_TRANSITION_BORDER_15];
                    }
                    else if (topleft_tile != Tile_Type_BORDER)
                    {
                        sprite = memory->bitmaps[Bitmap_type_CORNER_BORDER_1];
                    }
                }
            }
        }

        i32 bottom_tile_index = get_index(tile_pos - V2{0, 1});
        V2 pixel_tile_pos = tile_pos * TILE_SIZE_PIXELS;

        if (distance_between_points(pixel_tile_pos, memory->game_objects[0].pos) > 400)
        {
            if (!memory->tile_map[index].solid && random_float(&memory->__global_random_state, 0, 1) < 0.15)
            {
                if (memory->tile_map[bottom_tile_index].solid)
                {
                    i32 right_tile_index = get_index(tile_pos + V2{1, 0});
                    i32 left_tile_index = get_index(tile_pos + V2{-1, 0});

                    if (!(memory->tile_map[right_tile_index].solid && memory->tile_map[left_tile_index].solid))
                    {
                        V2 spawn_pos = pixel_tile_pos;
                        add_game_object(memory, Game_Object_ZOMBIE, spawn_pos);
                    }
                }
            }
        }

        memory->tile_map[index].sprite = sprite;
    }
}

void update_tile(Game_memory *memory, i32 tile_index)
{
    Tile *tile = &memory->tile_map[tile_index];
    V2 tilePos = get_tile_pos(tile_index);

    //обновление тайлов
    switch (tile->type)
    {
    case Tile_Type_NONE:
    {
    }
    break;
    case Tile_Type_NORMAL:
    {
        draw_bitmap(memory, tilePos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS}, 0, memory->tile_map[tile_index].sprite, 1, LAYER_TILE);
    }
    break;
    case Tile_Type_BORDER:
    {
        draw_bitmap(memory, tilePos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS}, 0, memory->tile_map[tile_index].sprite, 1, LAYER_TILE);
    }
    break;
    case Tile_Type_FLOOR:
    {
        draw_bitmap(memory, tilePos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS}, 0, memory->tile_map[tile_index].sprite, 1, LAYER_TILE);
    }
    break;
    case Tile_Type_PARAPET:
    {
        if (memory->tile_map[get_index(tilePos + V2{0, -1})].type == Tile_Type_NONE)
        {
            tile->type = Tile_Type_NONE;
            tile->sprite = memory->bitmaps[Bitmap_type_BACKGROUND];
        }
        else
        {
            draw_bitmap(memory, tilePos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS}, 0, memory->tile_map[tile_index].sprite, 1, LAYER_FORGROUND);
        }
    }
    break;
    case Tile_Type_ENTER:
    {
        draw_bitmap(memory, tilePos * TILE_SIZE_PIXELS + V2{0, (memory->tile_map[tile_index].sprite.size.y * SPRITE_SCALE - TILE_SIZE_PIXELS) * 0.5f}, memory->tile_map[tile_index].sprite.size * SPRITE_SCALE, 0, memory->tile_map[tile_index].sprite, 1, LAYER_ON_ON_BACKGROUND);
    }
    break;
    case Tile_Type_EXIT:
    {
        draw_bitmap(memory, tilePos * TILE_SIZE_PIXELS + V2{0, (memory->tile_map[tile_index].sprite.size.y * SPRITE_SCALE - TILE_SIZE_PIXELS) * 0.5f}, memory->bitmaps[Bitmap_type_DOOR_BACK].size * SPRITE_SCALE, 0, memory->bitmaps[Bitmap_type_DOOR_BACK], 1, LAYER_ON_BACKGROUND);
        draw_bitmap(memory, tilePos * TILE_SIZE_PIXELS + V2{0, (memory->tile_map[tile_index].sprite.size.y * SPRITE_SCALE - TILE_SIZE_PIXELS) * 0.5f}, memory->tile_map[tile_index].sprite.size * SPRITE_SCALE, 0, memory->tile_map[tile_index].sprite, 1, LAYER_ON_ON_BACKGROUND);

        if (memory->timers[tile->interactive] > 0)
        {
            i8 step = (i8)floor(memory->timers[tile->interactive] / 60.0f * 6.0f);
            tile->sprite = memory->bitmaps[Bitmap_type_DOOR + step];
        }
        else if (memory->timers[tile->interactive] == 0)
        {
            memory->transition_is_on = true;
        }
    }
    break;
    case Tile_Type_SPIKES:
    {
        draw_bitmap(memory, tilePos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS}, 0, memory->tile_map[tile_index].sprite, 1, LAYER_ON_BACKGROUND);
    }
    break;
    }

    if (!tile->solid)
    {
        draw_bitmap(memory, tilePos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS}, 0, memory->bitmaps[Bitmap_type_BACKGROUND], 1, LAYER_BACKGROUND_MAIN);
    }
}

extern "C" GAME_UPDATE(game_update)
{
    V2 map_size = {CHUNK_COUNT_X * CHUNK_SIZE_X + BORDER_SIZE * 2, CHUNK_COUNT_Y * CHUNK_SIZE_Y + BORDER_SIZE * 2};
    i32 tile_count = i32(map_size.x * map_size.y);

    i32 interval = SPRITE_SCALE;

    //выполняется один раз
    if (!memory->initialized)
    {
        memory->initialized = true;

        load_bitmaps(memory, memory->win32_read_bmp);

        memory->camera.pos = V2{0, 0};
        memory->camera.target = V2{0, 0};
        memory->camera.scale = V2{1, 1} * 0.4f;

        memory->darkness = create_empty_bitmap(screen.size / memory->camera.scale);

        memory->light_lvl = 1;
        memory->darkness_lvl = (f32)DARKNESS_USUAL_LVL;
        memory->UI_alpha = 1;
        memory->draw_darkness = false;

        memory->draw_queue_size = 0;
        memory->game_object_count = 0;
        memory->timers_count = 0;

        memory->transition_is_on = false;

        generate_new_map(memory, screen);
    }

    if (input.r.went_down)
    {
        // if (game_objects[0].exists == true)
        // {

        //     draw_darkness = !draw_darkness;
        // }
        // else
        {
            generate_new_map(memory, screen);
        }
    }

    //очистка экрана
    clear_screen(memory, screen, memory->darkness);

    //обновление сущностей
    for (i32 object_index = 0; object_index < memory->game_object_count; object_index++)
    {
        if (memory->game_objects[object_index].exists)
        {
            update_game_object(memory, &memory->game_objects[object_index], input, screen);
        }
    }

    //плавный переход
    if (memory->transition_is_on)
    {
        memory->darkness_lvl += (1 - memory->darkness_lvl) * 0.1f;
        memory->light_lvl += (0 - memory->light_lvl) * 0.1f;
        memory->UI_alpha += (0 - memory->UI_alpha) * 0.1f;
        if (memory->light_lvl < 0.001f)
        {
            memory->transition_is_on = false;
            generate_new_map(memory, screen);
        }
    }
    else
    {
        memory->darkness_lvl += (f32)(DARKNESS_USUAL_LVL - memory->darkness_lvl) * 0.1f;
        memory->light_lvl += (1 - memory->light_lvl) * 0.1f;
        memory->UI_alpha += (1 - memory->UI_alpha) * 0.1f;
    }

    //прорисовка темноты
    if (memory->draw_darkness)
    {
        draw_bitmap(memory, memory->camera.pos, memory->darkness.size, 0, memory->darkness, 1, LAYER_DARKNESS);
    }

    //обновление тайлов
    for (i32 tile_index = 0; tile_index < tile_count; tile_index++)
    {
        update_tile(memory, tile_index);
    }

    //сортируем qrawQueue
    Drawing new_draw_queue[1024 * 8];
    i32 new_draw_queue_size = 0;
    Layer layer = (Layer)0;

    //to do переделать под insertion sort
    while (new_draw_queue_size != memory->draw_queue_size)
    {
        for (i32 drawing_index = 0; drawing_index < memory->draw_queue_size; drawing_index++)
        {
            if (memory->draw_queue[drawing_index].layer == layer)
            {
                memory->draw_queue[drawing_index].pos = world_to_screen(screen, memory->camera, memory->draw_queue[drawing_index].pos);
                memory->draw_queue[drawing_index].size *= memory->camera.scale;

                new_draw_queue[new_draw_queue_size] = memory->draw_queue[drawing_index];
                new_draw_queue_size++;
            }
        }
        layer = (Layer)((i32)layer + 1);
    }

    memory->draw_queue_size = 0;

    for (i32 i = 0; i < new_draw_queue_size; i++)
    {
        draw_item(screen, new_draw_queue[i]);
    }

    update_timers(memory);
}