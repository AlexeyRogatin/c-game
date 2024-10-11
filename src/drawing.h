//очищение экрана и затемнение среды
#define DARKNESS_USUAL_LVL 0.90f

void clear_screen(Game_memory *memory, Bitmap screen, Bitmap darkness)
{
    i32 pixel_count = (i32)(screen.size.x * screen.size.y);
    memset(screen.pixels, 0, sizeof(u32) * pixel_count);

    //обновление темноты
    for (i32 y = 0; y <= darkness.size.y; y++)
    {
        for (i32 x = 0; x <= darkness.size.x; x++)
        {
            ARGB dark_pixel = {0xFF000000};
            dark_pixel.a = (u8)((dark_pixel.a * (DARKNESS_USUAL_LVL + (1.00f - DARKNESS_USUAL_LVL) * fabsf(memory->transition))) * random_float(&memory->__global_random_state, 0.996f + 0.004f * fabsf(memory->transition), 1.0f));
            darkness.pixels[y * darkness.pitch + x] = dark_pixel.argb;
        }
    }
}

//drawing
void add_rect_to_queue(Game_memory *memory, V2 pos, V2 size, f32 angle, u32 color, Layer layer)
{
    Drawing drawing;
    drawing.type = DRAWING_TYPE_RECT;
    drawing.pos = pos;
    drawing.size = size;
    drawing.angle = angle;
    drawing.color = color;
    drawing.layer = layer;
    assert(memory->draw_queue_size < 1024 * 8);
    memory->draw_queue[memory->draw_queue_size] = drawing;
    memory->draw_queue_size++;
}

void add_bitmap_to_queue(Game_memory *memory, V2 pos, V2 size, f32 angle, Bitmap bitmap, f32 alpha, u32 color, Layer layer)
{
    Drawing drawing;
    drawing.type = DRAWING_TYPE_BITMAP;
    drawing.pos = pos;
    drawing.size = size;
    drawing.angle = angle;
    drawing.color = color;
    drawing.bitmap = bitmap;
    drawing.layer = layer;
    drawing.alpha = alpha;
    assert(memory->draw_queue_size < 1024 * 8);
    memory->draw_queue[memory->draw_queue_size] = drawing;
    assert(!(memory->draw_queue[memory->draw_queue_size].bitmap.pixels == 0 && memory->draw_queue[memory->draw_queue_size].type == DRAWING_TYPE_BITMAP));
    memory->draw_queue_size++;
}

void add_light_to_queue(Game_memory *memory, V2 pos, f32 world_radius, u32 color)
{
    Drawing drawing;
    drawing.type = DRAWING_TYPE_LIGHT;
    drawing.pos = pos;
    drawing.size = V2{world_radius, world_radius};
    drawing.color = color;
    drawing.layer = LAYER_BACKGROUND_MAIN;
    assert(memory->draw_queue_size < 1024 * 8);
    memory->light_queue[memory->light_queue_size] = drawing;
    memory->light_queue_size++;
}

void add_text_to_queue(Game_memory *memory, char *string, V2 pos, V2 size, f32 height, f32 angle, u32 color, f32 alpha, Layer layer)
{
    Drawing drawing;
    drawing.type = DRAWING_TYPE_TEXT;
    drawing.pos = pos;
    drawing.size = size;
    drawing.angle = angle;
    drawing.color = color;
    drawing.layer = layer;
    drawing.alpha = alpha;
    drawing.string = string;
    assert(memory->draw_queue_size < 1024 * 8);
    memory->draw_queue[memory->draw_queue_size] = drawing;
    memory->draw_queue_size++;
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

//(to do) отключил пока Серёга не поможет разобраться с кодом
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

void render_rect(Bitmap screen, Drawing drawing)
{
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

    Rect screen_rect = {
        V2{0, 0},
        screen.size + V2{1, 1},
    };

    Rect paint_rect = intersect(drawn_rect, drawing.clip_rect);
    paint_rect = intersect(paint_rect, screen_rect);

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

void render_bitmap(Bitmap screen, Drawing drawing)
{
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

    Rect screen_rect = {
        V2{0, 0},
        screen.size + V2{1, 1},
    };

    Rect paint_rect = intersect(drawn_rect, drawing.clip_rect);
    paint_rect = intersect(paint_rect, screen_rect);
#if 0

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
#else
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
    V4 color = argb_to_v4({drawing.color});
    color = color * color.a;
    V4_8x color_8x = V4_8x{set1_f32(color.r), set1_f32(color.g), set1_f32(color.b), set1_f32(0x00)};

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

                i32_8x mask = uv01.x >= zero_8x & uv01.x < one_8x & uv01.y >= zero_8x & uv01.y < one_8x;
                V2_8x uv_floored = clamp(floor(uv), zero_vector_8x, bitmap_size_8x);
                V2_8x uv_fract = clamp01(fract(uv) * pixel_scale_8x);
                Bilinear_Sample_8x sample = get_bilinear_sample(drawing.bitmap, uv_floored, mask);
                V4_8x texel = bilinear_blend(sample, uv_fract);
                V4_8x pixel = argb_to_v4_8x(load(pixel_ptr));

                texel = texel * alpha_8x;
                texel = clamp01(texel + color_8x * texel.a);

                f32_8x inverted_alpha = one_8x - texel.a;
                V4_8x result = inverted_alpha * pixel + texel;

                mask_store(pixel_ptr, v4_to_argb_8x(result), mask);

                pixel_ptr += 8;
                d.x += eight_8x;
            }
        }
    }
#endif
}

void render_light(Game_memory *memory, Drawing drawing)
{
    V2 darkness_scale = memory->bitmaps[Bitmap_type_BRICKS].size.x / TILE_SIZE_PIXELS / memory->camera.scale;
    f32 radius = drawing.size.x * darkness_scale.x;
    drawing.pos *= darkness_scale;
    radius *= 1.0f - fabsf(memory->transition);

    Rect drawn_rect = {
        drawing.pos - V2{radius, radius},
        drawing.pos + V2{radius, radius},
    };

    Rect screen_rect = {
        V2{0, 0},
        memory->darkness.size + V2{1, 1},
    };

    Rect paint_rect = intersect(drawn_rect, drawing.clip_rect);
    paint_rect = intersect(paint_rect, screen_rect);
#if 0

    for (f32 y = rect.min.y; y <= rect.max.y; y++)
    {
        for (f32 x = rect.min.x; x <= rect.max.x; x++)
        {
            V2 d = {x, y};

            ARGB pixel = {memory->darkness.pixels[(i32)y * (i32)memory->darkness.pitch + (i32)x]};
            f32 intensity = min(pixel.a / 255.0f, length(d - drawing.pos) / radius);
            memory->darkness.pixels[(i32)y * (i32)memory->darkness.pitch + (i32)x] = v4_to_argb({0, 0, 0, intensity}).argb;
        }
    }
#else
    if ((i32)paint_rect.min.x & 7)
    {
        paint_rect.min.x = (f32)((i32)paint_rect.min.x & ~7);
    }

    if ((i32)paint_rect.max.x & 7)
    {
        paint_rect.max.x = (f32)(((i32)paint_rect.max.x & ~7) + 8);
    }

    f32_8x zero_to_seven = set8_f32(0, 1, 2, 3, 4, 5, 6, 7);
    V2_8x drawing_pos_8x = V2_8x{set1_f32(drawing.pos.x), set1_f32(drawing.pos.y)};
    f32_8x radius_8x = set1_f32(radius);
    f32_8x zero_8x = set1_f32(0.0f);
    f32_8x one_8x = set1_f32(1.0f);
    V2_8x max_rect_8x = V2_8x{set1_f32(screen_rect.max.x), set1_f32(screen_rect.max.y)};

    V4 color = argb_to_v4({drawing.color});
    color = color * color.a;
    V4_8x color_8x = V4_8x{set1_f32(color.r), set1_f32(color.g), set1_f32(color.b), set1_f32(0x00)};

    for (i32 y = (i32)paint_rect.min.y; y < (i32)paint_rect.max.y; y++)
    {
        u32 *pixel_ptr = memory->darkness.pixels + y * memory->darkness.pitch + (i32)paint_rect.min.x;
        f32_8x y_8x = set1_f32((f32)y);
        for (i32 x = (i32)paint_rect.min.x; x < (i32)paint_rect.max.x; x += 8)
        {
            V2_8x d = V2_8x{set1_f32((f32)x) + zero_to_seven, y_8x};

            i32_8x mask = d.x < max_rect_8x.x;
            V4_8x pixel = argb_to_v4_8x(load(pixel_ptr));
            f32_8x intensity = min(one_8x, (length_8x(d - drawing_pos_8x) + one_8x) / radius_8x);
            V4_8x texel = pixel * intensity + color_8x * pixel.a * (one_8x - intensity) * intensity;
            mask_store(pixel_ptr, v4_to_argb_8x(texel), mask);

            pixel_ptr += 8;
        }
    }
#endif
}

void render_text(Game_memory *memory, Bitmap screen, Drawing drawing)
{
    V2 letter_pos = drawing.pos;
    for (i32 char_index = 0; char_index < string_length(drawing.string); char_index++)
    {
        char string_char = drawing.string[char_index];
        Letter letter = memory->letters[string_char];

        Drawing letter_drawing;
        letter_drawing.type = DRAWING_TYPE_BITMAP;
        letter_drawing.size = letter.bitmap.size;
        letter_drawing.pos = letter_pos + letter.bitmap.size * 0.5f - letter.offset;
        letter_drawing.layer = drawing.layer;
        letter_drawing.bitmap = letter.bitmap;
        letter_drawing.angle = 0;
        letter_drawing.alpha = 1;
        letter_drawing.clip_rect = drawing.clip_rect;
        render_bitmap(screen, letter_drawing);
        if (drawing.string[char_index + 1])
        {
            letter_pos += V2{(f32)letter.advance, 0};
        }
    }
}

void draw_item(Game_memory *memory, Bitmap screen, Drawing drawing)
{
    if (drawing.type == DRAWING_TYPE_RECT)
    {
        render_rect(screen, drawing);
    }

    if (drawing.type == DRAWING_TYPE_BITMAP)
    {
        render_bitmap(screen, drawing);
    }
	
    if (drawing.type == DRAWING_TYPE_LIGHT)
    {
        render_light(memory, drawing);
    }

    if (drawing.type == DRAWING_TYPE_TEXT)
    {
        render_text(memory, screen, drawing);
    }
}

typedef struct
{
    Game_memory *memory;
    Bitmap screen;
    Rect clip_rect;
    Drawing *queue;
    i32 queue_size;
} Clip_rect_render_work;

PLATFORM_WORK_QUEUE_CALLBACK(draw_items_in_rect)
{
    Clip_rect_render_work *work = (Clip_rect_render_work *)data;
    for (i32 i = 0; i < work->queue_size; i++)
    {
        Drawing item = work->queue[i];

        item.pos = world_to_screen(work->screen, work->memory->camera, item.pos);
        item.size *= work->memory->camera.scale;

        item.clip_rect = work->clip_rect;
        draw_item(work->memory, work->screen, item);

        item.pos = screen_to_world(work->screen, work->memory->camera, item.pos);
        item.size /= work->memory->camera.scale;
    }
}

void clip_rect_items_render(Platform_work_queue *render_queue,
                            Game_memory *memory, Bitmap screen, Bitmap bitmap, Drawing *queue, i32 queue_size)
{
    f32 TILE_WIDTH = ceilf(bitmap.size.x / CLIP_RECTS_COUNT_X);
    f32 TILE_HEIGHT = ceilf(bitmap.size.y / CLIP_RECTS_COUNT_Y);

    Clip_rect_render_work work_queue[CLIP_RECTS_COUNT_X * CLIP_RECTS_COUNT_Y] = {};

    Clip_rect_render_work work = {};
    work.memory = memory;
    work.screen = screen;
    work.queue = queue;
    work.queue_size = queue_size;
    work.clip_rect.min.y = 0;
    work.clip_rect.max.y = TILE_HEIGHT;

    for (u32 clip_rect_y = 0; clip_rect_y < CLIP_RECTS_COUNT_Y; clip_rect_y++)
    {
        work.clip_rect.min.x = 0;
        work.clip_rect.max.x = TILE_WIDTH;
        for (u32 clip_rect_x = 0; clip_rect_x < CLIP_RECTS_COUNT_X; clip_rect_x++)
        {

            i32 clip_rect_index = clip_rect_y * CLIP_RECTS_COUNT_X + clip_rect_x;

            work_queue[clip_rect_index] = work;
            // draw_items_in_rect((void *)&work);
            memory->platform_add_work_queue_entry(render_queue, draw_items_in_rect, &work_queue[clip_rect_index]);

            work.clip_rect.min.x = work.clip_rect.max.x;
            work.clip_rect.max.x += TILE_WIDTH;
        }
        work.clip_rect.min.y = work.clip_rect.max.y;
        work.clip_rect.max.y += TILE_HEIGHT;
    }

    memory->platform_complete_all_work(render_queue);
}