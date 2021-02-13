#include <malloc.h>
#include "math.h"
#include "game_math.c"

#define PI 3.14159265

#define TILE_SIZE_PIXELS 80
#define CHUNK_SIZE_X 10
#define CHUNK_SIZE_Y 8
#define CHUNK_COUNT_X 4
#define CHUNK_COUNT_Y 4

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

i32 jump = add_timer(0);

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

Bitmap img_BackGround = win32_read_bmp("../data/backGround.bmp");

Bitmap img_Border = win32_read_bmp("../data/border.bmp");
Bitmap img_TransitionBorder = win32_read_bmp("../data/borderWithTransition.bmp");
Bitmap img_CornerBorder = win32_read_bmp("../data/cornerBorder.bmp");

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

Bitmap img_MarbleFloor[4] = {
    win32_read_bmp("../data/floor1.bmp"),
    win32_read_bmp("../data/floor2.bmp"),
    win32_read_bmp("../data/floor3.bmp"),
    win32_read_bmp("../data/floor4.bmp"),
};

Bitmap img_TiledFloor[4] = {
    win32_read_bmp("../data/floor5.bmp"),
    win32_read_bmp("../data/floor6.bmp"),
    win32_read_bmp("../data/floor7.bmp"),
    win32_read_bmp("../data/floor8.bmp"),
};

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

//камера
typedef struct
{
    V2 pos;
    V2 target;
} Camera;

Camera camera = {
    {0, 0},
    {0, 0},
};

//drawing
typedef union
{
    u32 argb;
    struct
    {
        u8 b;
        u8 g;
        u8 r;
        u8 a;
    };
} ARGB;

typedef enum
{
    DRAWING_TYPE_RECT,
    DRAWING_TYPE_BITMAP,
} Drawing_Type;

typedef enum
{
    LAYER_CLEAR,
    LAYER_BACKGROUND1,
    LAYER_BACKGROUND2,
    LAYER_BACKGROUND3,
    LAYER_BACKGROUND4,
    LAYER_GAME_OBJECT,
    LAYER_TILE,
    LAYER_FTILE,
    LAYER_FORGROUND,
} Layer;

typedef struct
{
    Drawing_Type type;
    V2 pos;
    V2 size;
    f32 angle;
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

void draw_bitmap(V2 pos, V2 size, f32 angle, Bitmap bitmap, Layer layer)
{
    Drawing drawing;
    drawing.type = DRAWING_TYPE_BITMAP;
    drawing.pos = pos;
    drawing.size = size;
    drawing.angle = angle;
    drawing.color = NULL;
    drawing.bitmap = bitmap;
    drawing.layer = layer;
    assert(draw_queue_size < 1024 * 8);
    draw_queue[draw_queue_size] = drawing;
    draw_queue_size++;
}

typedef struct
{
    V2 min;
    V2 max;
} Rect;

void clear_screen(Bitmap screen, Bitmap darkness)
{
    i32 pixelCount = screen.size.x * screen.size.y;
    for (i32 i = 0; i < pixelCount; i++)
    {
        screen.pixels[i] = 0xFF0D400F;
    }

    //обновление темноты
    for (i32 y = 0; y < darkness.size.y; y++)
    {
        for (i32 x = 0; x < darkness.size.x; x++)
        {
            darkness.pixels[y * darkness.pitch + x] = 0xFA000000;
        }
    }
}

typedef struct
{
    ARGB a, b, c, d;
} Bilinear_Sample;

f32 min(f32 a, f32 b)
{
    f32 result;
    if (a <= b)
    {
        result = a;
    }
    else
    {
        result = b;
    }
    return result;
}

f32 max(f32 a, f32 b)
{
    f32 result;
    if (a >= b)
    {
        result = a;
    }
    else
    {
        result = b;
    }
    return result;
}

V2 get_size(Rect rect)
{
    V2 size = {rect.max.x - rect.min.x, rect.max.y - rect.min.y};
    return size;
}

V2 get_center(Rect rect)
{
    V2 result = rect.min + get_size(rect) * 0.5;
    return result;
}

Rect intersect(Rect rect1, Rect rect2)
{
    Rect result = {
        {max(rect1.min.x, rect2.min.x), max(rect1.min.y, rect2.min.y)},
        {min(rect1.max.x, rect2.max.x), min(rect1.max.y, rect2.max.y)}};
    return result;
}

bool has_area(Rect rect)
{
    V2 size = get_size(rect);
    bool result = size.x > 0 && size.y > 0;
    return result;
}

V2 fract(V2 a)
{
    V2 result = {a.x - floorf(a.x), a.y - floorf(a.y)};
    return result;
}

V2 clamp01(V2 a)
{
    V2 result = {
        min(1.0f, max(0.0f, a.x)),
        min(1.0f, max(0.0f, a.y)),
    };
    return result;
}

Bilinear_Sample get_bilinear_sample(Bitmap bitmap, V2 pos)
{
    ARGB a = {bitmap.pixels[(i32)pos.y * bitmap.pitch + (i32)pos.x]};
    ARGB b = {bitmap.pixels[(i32)pos.y * bitmap.pitch + (i32)pos.x + 1]};
    ARGB c = {bitmap.pixels[((i32)pos.y + 1) * bitmap.pitch + (i32)pos.x]};
    ARGB d = {bitmap.pixels[((i32)pos.y + 1) * bitmap.pitch + (i32)pos.x + 1]};

    Bilinear_Sample result = {a, b, c, d};
    return result;
}

ARGB lerp(ARGB a, ARGB b, f32 f)
{
    ARGB result;
    result.r = a.r * (1 - f) + b.r * f;
    result.g = a.g * (1 - f) + b.g * f;
    result.b = a.b * (1 - f) + b.b * f;
    result.a = a.a * (1 - f) + b.a * f;
    return result;
}

ARGB bilinear_blend(Bilinear_Sample sample, V2 f)
{
    ARGB ab = lerp(sample.a, sample.b, f.x);
    ARGB cd = lerp(sample.c, sample.d, f.x);
    ARGB abcd = lerp(ab, cd, f.y);
    return abcd;
}

#define PINK {0xFFff63ed}


void draw_item(Bitmap screen, Drawing drawing)
{
    if (drawing.type == DRAWING_TYPE_RECT)
    {
        Rect screen_rect = {{0, 0}, {screen.size.x, screen.size.y}};

        V2 rect_size = drawing.size;

        V2 x_axis = rotate_vector({drawing.size.x, 0}, drawing.angle);
        V2 y_axis = rotate_vector({0, drawing.size.y}, drawing.angle);
        V2 origin = drawing.pos - x_axis / 2 - y_axis / 2;

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

    if (drawing.type == DRAWING_TYPE_BITMAP)
    {
        Rect screen_rect = {{0, 0}, {screen.size.x, screen.size.y}};

        V2 rect_size = drawing.size;
        if (rect_size.x > 0) rect_size.x += 2;
        if (rect_size.y > 0) rect_size.y += 2;
        if (rect_size.x < 0) rect_size.x -= 2;
        if (rect_size.y < 0) rect_size.y -= 2;

        V2 x_axis = rotate_vector({rect_size.x, 0}, drawing.angle);
        V2 y_axis = rotate_vector({0, rect_size.y}, drawing.angle);
        V2 origin = drawing.pos - x_axis / 2 - y_axis / 2;

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

        V2 texture_size = drawing.bitmap.size;
        V2 pixel_scale = abs(drawing.size) / texture_size;
        V2 texture_size_with_apron = texture_size + 1/pixel_scale;

        V2 inverted_sqr_rect_size = 1 / (rect_size * rect_size);

        for (i32 y = paint_rect.min.y; y < paint_rect.max.y; y++)
        {
            for (i32 x = paint_rect.min.x; x < paint_rect.max.x; x++)
            {
                V2 d = V2{(f32)x, (f32)y} - origin;
                V2 uv01 = V2{dot(d, x_axis), dot(d, y_axis)} * inverted_sqr_rect_size;

                V2 uv = uv01 * texture_size_with_apron;
                V2 uv_floored = floor(uv);
                V2 uv_fract = clamp01(fract(uv) * pixel_scale);

                Bilinear_Sample sample = get_bilinear_sample(drawing.bitmap, V2(uv_floored));
                ARGB mixed_sample = bilinear_blend(sample, uv_fract);

                ARGB pixel = {screen.pixels[y * (i32)screen.size.x + x]};
                f32 alpha = (f32)mixed_sample.a / 255.0f;
                if (alpha == 0)
                {
                    i32 foo = 0;
                }
                ARGB result;
                result.r = (mixed_sample.r + pixel.r * (1 - alpha));
                result.g = (mixed_sample.g + pixel.g * (1 - alpha));
                result.b = (mixed_sample.b + pixel.b * (1 - alpha));

                screen.pixels[y * (i32)screen.size.x + x] = result.argb;
            }
        }
    }
}

Bitmap darkness;

void draw_light(V2 pos, f32 innerRadius, f32 radius)
{
    pos += darkness.size / 2 - camera.pos;

    V2 min = pos - V2{radius, radius};
    V2 max = pos + V2{radius, radius};

    i32 interval = (darkness.pitch - darkness.size.x) / 2;

    if (min.x < 0)
        min.x = 0;
    if (min.y < 0)
        min.y = 0;
    if (max.x > darkness.size.x)
        max.x = darkness.size.x;
    if (max.y > darkness.size.y)
        max.y = darkness.size.y;

    min.x = floor(min.x);
    min.y = floor(min.y);
    max.x = ceil(max.x);
    max.y = ceil(max.y);

    for (i32 y = min.y; y < max.y; y += interval)
    {
        for (i32 x = min.x; x < max.x; x += interval)
        {
            f32 distance = distance_between_points(pos, V2{f32(x), f32(y)});
            f32 alpha = (distance - innerRadius) / (radius - innerRadius);
            if (alpha < 1)
            {
                if (alpha < 0)
                {
                    alpha = 0;
                }
                for (i32 pY = 0; pY < interval; pY++)
                {
                    for (i32 pX = 0; pX < interval; pX++)
                    {
                        i32 index = (y + pY) * darkness.pitch + (x + pX);
                        ARGB pixel = {darkness.pixels[index]};
                        pixel.a *= alpha;

                        darkness.pixels[index] = pixel.argb;
                    }
                }
            }
        }
    }
}

//тайлы
typedef enum
{
    Tile_Type_NONE,
    Tile_Type_BRICK,
    Tile_Type_ELEGANT_BRICK,
    Tile_Type_MARBLE_FLOOR,
    Tile_Type_TILED_FLOOR,
    Tile_Type_PARAPET,
    Tile_Type_STONE,
    Tile_Type_BORDER,
    Tile_Type_ENTER,
    Tile_Type_EXIT,
} Tile_type;

typedef struct
{
    Tile_type type;
    Bitmap sprite;
    f32 angle;
    bool solid;
    bool interactive;
    i32 timer;
} Tile;

Tile *tile_map = NULL;

V2 get_tile_pos(i32 index)
{
    //index = y * (CHUNK_SIZE_X * (CHUNK_COUNT_X + 2)) + x;
    f32 y = floor(index / (CHUNK_SIZE_X * (CHUNK_COUNT_X + 2)));
    f32 x = index - y * (CHUNK_SIZE_X * (CHUNK_COUNT_X + 2));
    V2 result = {x, y};
    return result;
}

i32 get_index(V2 coords)
{
    i32 index = coords.y * (CHUNK_SIZE_X * (CHUNK_COUNT_X + 2)) + coords.x;
    return index;
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

    V2 pos;
    V2 hit_box;
    V2 speed;

    bool go_left;
    bool go_right;
    bool jump;

    i32 moved_through_pixels;
    Direction looking_direction;
    Condition condition;
    Bitmap sprite;

    //player
    i32 can_jump;

    i32 looking_key_held_timer;
    i32 crouching_animation_timer;
    i32 hanging_animation_timer;

    bool can_control;
    Layer layer;

} Game_Object;

i32 game_object_count = 0;
Game_Object game_objects[512];

typedef enum
{
    Side_RIGHT,
    Side_LEFT,
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

Game_Object *add_game_object(Game_Object_Type type, V2 pos)
{
    Game_Object gameObject = {
        type,

        true,

        pos,
        {0, 0},
        {0, 0},

        false,
        false,
        false,

        0,
        (Direction)random_int(0, 1),
        Condition_IDLE,
        img_None,

        NULL,

        NULL,
        NULL,
        NULL,

        true,

        LAYER_GAME_OBJECT,
    };

    if (type == Game_Object_PLAYER)
    {
        gameObject.hit_box = {30, 56};
        gameObject.can_jump = add_timer(0);
        gameObject.looking_key_held_timer = add_timer(0);
        gameObject.crouching_animation_timer = add_timer(0);
        gameObject.hanging_animation_timer = add_timer(0);
    }

    if (type == Game_Object_ZOMBIE)
    {
        gameObject.hit_box = {30, 56};
        gameObject.can_jump = add_timer(0);
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

    game_objects[slot_index] = gameObject;

    return &game_objects[slot_index];
}

Collisions check_collision(Game_Object *game_object)
{
    Game_Object *our_object = game_object;

    i8 speed_direction = -(our_object->looking_direction * 2 - 1);

    Collisions collisions;

    collisions.x.happened = false;
    collisions.y.happened = false;
    collisions.expanded_collision = false;

    V2 obj_tile_pos = our_object->pos / TILE_SIZE_PIXELS;

    V2 obj_end_tile_pos = (our_object->pos + our_object->speed) / TILE_SIZE_PIXELS;

    V2 start_tile;
    V2 finish_tile;

    if (obj_tile_pos.x <= obj_end_tile_pos.x)
    {
        start_tile.x = obj_tile_pos.x;
        finish_tile.x = obj_end_tile_pos.x;
    }
    else
    {
        start_tile.x = obj_end_tile_pos.x;
        finish_tile.x = obj_tile_pos.x;
    }

    if (obj_tile_pos.y <= obj_end_tile_pos.y)
    {
        start_tile.y = obj_tile_pos.y;
        finish_tile.y = obj_end_tile_pos.y;
    }
    else
    {
        start_tile.y = obj_end_tile_pos.y;
        finish_tile.y = obj_tile_pos.y;
    }

    start_tile = V2{floorf(start_tile.x - our_object->hit_box.x / 2 / TILE_SIZE_PIXELS), floorf(start_tile.y - our_object->hit_box.y / 2 / TILE_SIZE_PIXELS)};
    finish_tile = V2{ceilf(finish_tile.x + our_object->hit_box.x / 2 / TILE_SIZE_PIXELS), ceilf(finish_tile.y + our_object->hit_box.y / 2 / TILE_SIZE_PIXELS)};

    //проверка столкновений с тайлами
    for (i32 tile_y = start_tile.y; tile_y <= finish_tile.y; tile_y++)
    {
        for (i32 tile_x = start_tile.x; tile_x <= finish_tile.x; tile_x++)
        {
            V2 tile_pos = V2{(f32)tile_x, (f32)tile_y};
            i32 tile_index = tile_pos.y * CHUNK_SIZE_X * (CHUNK_COUNT_X + 2) + tile_pos.x;
            Tile tile = tile_map[tile_index];
            if (tile.type && tile.solid)
            {
                i32 tile_left = tile_pos.x * TILE_SIZE_PIXELS - TILE_SIZE_PIXELS / 2;
                i32 tile_right = tile_pos.x * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS / 2;
                i32 tile_bottom = tile_pos.y * TILE_SIZE_PIXELS - TILE_SIZE_PIXELS / 2;
                i32 tile_top = tile_pos.y * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS / 2;

                f32 obj_left = our_object->pos.x - (our_object->hit_box.x / 2);
                f32 obj_right = our_object->pos.x + (our_object->hit_box.x / 2);
                f32 obj_bottom = our_object->pos.y - (our_object->hit_box.y / 2);
                f32 obj_top = our_object->pos.y + (our_object->hit_box.y / 2);

                f32 obj_side;
                i32 tile_side;

                if (our_object->speed.x != 0)
                {
                    if (our_object->speed.x > 0)
                    {
                        obj_side = obj_right;
                        tile_side = tile_left;
                    }
                    else
                    {
                        obj_side = obj_left;
                        tile_side = tile_right;
                    }

                    if (!((obj_right + our_object->speed.x <= tile_left) ||
                          (obj_left + our_object->speed.x >= tile_right) ||
                          (obj_top <= tile_bottom) ||
                          (obj_bottom >= tile_top)))
                    {
                        our_object->speed.x = -obj_side + tile_side;

                        obj_left += our_object->speed.x;
                        obj_right += our_object->speed.x;

                        collisions.x.happened = true;
                        collisions.x.tile_index = tile_index;
                        if (tile_side == tile_left)
                        {
                            collisions.x.tile_side = Side_LEFT;
                        }
                        else
                        {
                            collisions.x.tile_side = Side_RIGHT;
                        }
                    }
                }
                //столкновение удлинённое (для подвешенного состояния)
                if (!collisions.x.happened &&
                    !((obj_right + our_object->speed.x + speed_direction * 7 <= tile_left) ||
                      (obj_left + our_object->speed.x + speed_direction * 7 >= tile_right) ||
                      (obj_top <= tile_bottom) ||
                      (obj_bottom >= tile_top)))
                {
                    collisions.x.tile_index = tile_index;
                    if (speed_direction == 1)
                    {
                        collisions.x.tile_side = Side_LEFT;
                    }
                    else
                    {
                        collisions.x.tile_side = Side_RIGHT;
                    }

                    collisions.expanded_collision = true;
                }

                if (our_object->speed.y != 0)
                {
                    if (our_object->speed.y > 0)
                    {
                        obj_side = obj_top;
                        tile_side = tile_bottom;
                    }
                    else
                    {
                        obj_side = obj_bottom;
                        tile_side = tile_top;
                    }

                    if (
                        !(obj_top + our_object->speed.y <= tile_bottom ||
                          obj_bottom + our_object->speed.y >= tile_top ||
                          obj_right <= tile_left ||
                          obj_left >= tile_right))
                    {
                        our_object->speed.y = -obj_side + tile_side;

                        collisions.y.happened = true;
                        collisions.y.tile_index = tile_index;
                        if (tile_side == tile_bottom)
                        {
                            collisions.y.tile_side = Side_BOTTOM;
                        }
                        else
                        {
                            collisions.y.tile_side = Side_TOP;
                        }
                    }
                }
            }
        }
    }

    game_object->speed = our_object->speed;

    return collisions;
}

bool check_vision_box(V2 obj_pos, V2 vision_vector, V2 vision_size, Game_Object_Type *triggering_objects, i32 triggering_objects_count, bool draw)
{
    V2 vision_pos = obj_pos + vision_vector;
    bool vision_triggered = false;
    for (i32 game_object_index = 0; game_object_index < game_object_count; game_object_index++)
    {
        for (i32 triggers_index = 0; triggers_index < triggering_objects_count; triggers_index++)
        {
            Game_Object game_object = game_objects[game_object_index];
            if (game_object.type == triggering_objects[triggers_index])
            {
                if (vision_pos.x - vision_size.x / 2 - game_object.hit_box.x / 2 < game_object.pos.x && vision_pos.x + vision_size.x / 2 + game_object.hit_box.x / 2 > game_object.pos.x &&
                    vision_pos.y - vision_size.y / 2 - game_object.hit_box.y / 2 < game_object.pos.y && vision_pos.y + vision_size.y / 2 + game_object.hit_box.y / 2 > game_object.pos.y)
                {
                    vision_triggered = true;
                }
            }
        }
    }
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
//             gameObject->pos.y += -gameObject->hit_box.y / 2 + hitBox.y / 2;
//         }
//         if (changePos == 1)
//         {
//             gameObject->pos.y += gameObject->hit_box.y / 2 - hitBox.y / 2;
//         }
//     }

//     for (i32 y = objTilePos.y - tileCountY; y <= objTilePos.y + tileCountY; y++)
//     {
//         for (i32 x = objTilePos.x - tileCountX; x <= objTilePos.x + tileCountX; x++)
//         {
//             i32 tileIndex = y * (CHUNK_COUNT_X + 2) * CHUNK_SIZE_X + x;
//             Tile tile = tile_map[tileIndex];

//             if (tile.solid)
//             {

//                 V2 tilePos = get_tile_pos(tileIndex);

//                 f32 recentObjTop = gameObject->pos.y + gameObject->hit_box.y / 2;
//                 f32 recentObjBottom = gameObject->pos.y - gameObject->hit_box.y / 2;
//                 f32 recentObjRight = gameObject->pos.x + gameObject->hit_box.x / 2;
//                 f32 recentObjLeft = gameObject->pos.x - gameObject->hit_box.x / 2;

//                 f32 objTop = gameObject->pos.y + hitBox.y / 2;
//                 f32 objBottom = gameObject->pos.y - hitBox.y / 2;
//                 f32 objRight = gameObject->pos.x + hitBox.x / 2;
//                 f32 objLeft = gameObject->pos.x - hitBox.x / 2;

//                 f32 tileTop = tilePos.y * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS / 2;
//                 f32 tileBottom = tilePos.y * TILE_SIZE_PIXELS - TILE_SIZE_PIXELS / 2;
//                 f32 tileRight = tilePos.x * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS / 2;
//                 f32 tileLeft = tilePos.x * TILE_SIZE_PIXELS - TILE_SIZE_PIXELS / 2;

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

    for (i32 y = obj_tile_pos.y - 1; y <= obj_tile_pos.y + 1; y++)
    {
        for (i32 x = obj_tile_pos.x - 1; x <= obj_tile_pos.x + 1; x++)
        {
            i32 index = get_index(V2{(f32)x, (f32)y});
            Tile tile = tile_map[index];
            if (tile.interactive)
            {
                result = index;
                goto end;
            }
        }
    }

end:
    return result;
}

void update_game_object(Game_Object *game_object, Input input, Bitmap screen)
{
    if (game_object->type == Game_Object_PLAYER)
    {
        //предпологаемое состояние
        Condition supposed_cond;

        //движение игрока
        if (game_object->can_control)
        {
            if (input.z.went_down)
            {
                timers[jump] = 8;
            }

            if (timers[jump] > 0)
            {
                input.z.went_down = true;
            }

            game_object->go_left = input.left.is_down;
            game_object->go_right = input.right.is_down;
            game_object->jump = input.z.went_down;
        }

        //константы ускорения
        f32 accel_const;
        if (input.shift.is_down)
        {
            accel_const = 1.9;
        }
        else
        {
            accel_const = 3.8;
        }
        if (game_object->condition == Condition_CROUCHING_IDLE || game_object->condition == Condition_CROUCHING_MOOVING)
        {
            accel_const = 0.85;
        }

        f32 friction_const = 0.75;

        f32 jump_length = 19;
        f32 jump_height = TILE_SIZE_PIXELS * 2.2 - game_object->hit_box.y;
        f32 gravity = -2 * jump_height / (jump_length * jump_length);

        //скорость по x
        game_object->speed += {(game_object->go_right - game_object->go_left) * accel_const, 0};

        //прыжок
        if (game_object->jump && timers[game_object->can_jump] > 0)
        {
            timers[jump] = 0;
            timers[game_object->can_jump] = 0;
            if (game_object->condition == Condition_HANGING || game_object->condition == Condition_HANGING_LOOKING_DOWN || game_object->condition == Condition_HANGING_LOOKING_UP)
            {
                if (timers[game_object->hanging_animation_timer] > 0)
                {
                    timers[jump] = timers[game_object->hanging_animation_timer] + 1;
                }
                else if (input.down.is_down)
                {
                    game_object->condition = Condition_FALLING;
                }
                else
                {
                    game_object->speed.y = 2 * jump_height / jump_length;
                    game_object->condition = Condition_FALLING;
                }
            }
            else
            {
                game_object->speed.y = 2 * jump_height / jump_length;
            }
        }

        if (game_object->speed.y > 0 && !input.z.is_down)
        {
            gravity = -2 * jump_height / (jump_length * jump_length) * 2;
        }

        //гравитация
        game_object->speed.y += gravity;

        if (game_object->condition == Condition_HANGING || game_object->condition == Condition_HANGING_LOOKING_DOWN || game_object->condition == Condition_HANGING_LOOKING_UP)
        {
            game_object->speed.x = 0;
            if (timers[game_object->hanging_animation_timer] <= 0)
            {
                game_object->speed.y = 0;
            }
        }

        //трение
        game_object->speed.x *= friction_const;

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

        //столкновения и движение
        Collisions collisions = check_collision(game_object);

        //состояние падает
        supposed_cond = Condition_FALLING;

        //состояние стоит
        if (collisions.y.happened)
        {
            if (collisions.y.tile_side == Side_TOP)
            {
                supposed_cond = Condition_IDLE;
                timers[game_object->can_jump] = 5;
            }
        }

        //состояние ползком и смотрит вверх
        if (supposed_cond == Condition_IDLE)
        {
            if (input.down.is_down && !input.up.is_down)
            {
                supposed_cond = Condition_CROUCHING_IDLE;
            }
            if (input.up.is_down && !input.down.is_down && !game_object->go_right && !game_object->go_left && timers[game_object->crouching_animation_timer] <= 0)
            {
                supposed_cond = Condition_LOOKING_UP;
            }
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

        if (input.left.went_down || input.right.went_down)
        {
            game_object->moved_through_pixels = 0;
        }

        //функции движения, связанные с коллизией
        V2 collided_x_tile_pos = get_tile_pos(collisions.x.tile_index);
        V2 collided_y_tile_pos = get_tile_pos(collisions.y.tile_index);

        i32 collision_up_tile_index = (collided_x_tile_pos.y + 1) * CHUNK_SIZE_X * (CHUNK_COUNT_X + 2) + collided_x_tile_pos.x;
        i32 up_tile_index = (collided_x_tile_pos.y + 1) * CHUNK_SIZE_X * (CHUNK_COUNT_X + 2) + collided_x_tile_pos.x + (game_object->looking_direction);

        //состояние весит
        if ((collisions.x.happened || collisions.expanded_collision) && !tile_map[collision_up_tile_index].solid && !tile_map[up_tile_index].solid)
        {
            if (game_object->speed.y < 0 &&
                game_object->pos.y > collided_x_tile_pos.y * TILE_SIZE_PIXELS - 2 && game_object->pos.y + game_object->speed.y <= collided_x_tile_pos.y * TILE_SIZE_PIXELS - 2)
            {
                supposed_cond = Condition_HANGING;

                game_object->speed = {0, 0};

                game_object->pos.y = collided_x_tile_pos.y * TILE_SIZE_PIXELS - 2;

                if (collisions.x.tile_side == Side_RIGHT)
                {
                    game_object->pos.x = collided_x_tile_pos.x * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS / 2 + game_object->hit_box.x / 2;
                }
                else
                {
                    game_object->pos.x = collided_x_tile_pos.x * TILE_SIZE_PIXELS - TILE_SIZE_PIXELS / 2 - game_object->hit_box.x / 2;
                }
            }
        }

        //переход на стенку из состояния ползком
        if (supposed_cond == Condition_CROUCHING_IDLE || supposed_cond == Condition_CROUCHING_MOOVING)
        {
            V2 tile_pos = collided_y_tile_pos - V2{(f32)game_object->looking_direction, 0};
            if (!tile_map[get_index(tile_pos)].solid &&
                ((game_object->pos.x + game_object->speed.x + game_object->hit_box.x / 2 <= tile_pos.x * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS / 2) && (ceilf(game_object->pos.x + game_object->hit_box.x / 2) >= tile_pos.x * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS / 2) ||
                 (game_object->pos.x + game_object->speed.x - game_object->hit_box.x / 2 >= tile_pos.x * TILE_SIZE_PIXELS - TILE_SIZE_PIXELS / 2) && (floorf(game_object->pos.x - game_object->hit_box.x / 2) <= tile_pos.x * TILE_SIZE_PIXELS - TILE_SIZE_PIXELS / 2)))
            {
                game_object->pos.x = tile_pos.x * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS / 2 * (game_object->looking_direction) + game_object->hit_box.x / 2 * -(game_object->looking_direction);
                supposed_cond = Condition_HANGING;
                game_object->looking_direction = (Direction)((-(game_object->looking_direction) + 1) / 2);
                game_object->speed = {0, 0};
                game_object->condition = Condition_IDLE;
                timers[game_object->hanging_animation_timer] = 16;
            }
        }

        //состояние смежные с состоянием весит
        if ((game_object->condition == Condition_HANGING || game_object->condition == Condition_HANGING_LOOKING_DOWN || game_object->condition == Condition_HANGING_LOOKING_UP))
        {
            if (input.up.is_down)
            {
                supposed_cond = Condition_HANGING_LOOKING_UP;
            }
            else if (input.down.is_down)
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
                timers[game_object->looking_key_held_timer] = 50;
            }

            //поднимаемся и встаём
            if ((game_object->condition != Condition_CROUCHING_IDLE && game_object->condition != Condition_CROUCHING_MOOVING) && (supposed_cond == Condition_CROUCHING_IDLE || supposed_cond == Condition_CROUCHING_MOOVING))
            {
                if (timers[game_object->crouching_animation_timer] < 0)
                {
                    timers[game_object->crouching_animation_timer] = 0;
                }
                timers[game_object->crouching_animation_timer] = (7.5 - timers[game_object->crouching_animation_timer] / 2) * 2;
            }
            if ((game_object->condition == Condition_CROUCHING_IDLE || game_object->condition == Condition_CROUCHING_MOOVING) && (supposed_cond != Condition_CROUCHING_IDLE && supposed_cond != Condition_CROUCHING_MOOVING))
            {
                if (timers[game_object->crouching_animation_timer] < 0)
                {
                    timers[game_object->crouching_animation_timer] = 0;
                }
                timers[game_object->crouching_animation_timer] = 7.5 - timers[game_object->crouching_animation_timer] / 2;
            }
            game_object->condition = supposed_cond;
        }

        //изменяем положение
        game_object->pos += game_object->speed;

        //что делают состояния
        if (game_object->condition == Condition_IDLE || game_object->condition == Condition_MOOVING)
        {
            if (timers[game_object->crouching_animation_timer] > 0)
            {
                i32 step = ceil(timers[game_object->crouching_animation_timer] / 2);
                game_object->sprite = img_PlayerStartsCrouching[step];
            }
            else if (game_object->condition == Condition_IDLE)
            {
                game_object->speed.x *= friction_const;
                game_object->sprite = img_PlayerIdle;
            }
            else if (game_object->condition == Condition_MOOVING)
            {
                i8 step = (i32)floor(fabsf(game_object->moved_through_pixels) / 20);
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
                const i32 step = 4 - ceil(timers[game_object->crouching_animation_timer] / 4);
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
                    i8 step = (i32)floor(fabsf(game_object->moved_through_pixels) / 10);
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

            timers[game_object->can_jump] = 4;
        }

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

                        game_object->can_control = false;
                    }
                }
            }
        }

        //камера
        camera.target = game_object->pos;

        //смотрим вниз и вверх
        if ((game_object->condition == Condition_CROUCHING_IDLE || game_object->condition == Condition_HANGING_LOOKING_DOWN) && timers[game_object->looking_key_held_timer] <= 0)
        {
            camera.target.y = game_object->pos.y - screen.size.y / 2 + 200;
        }

        if ((game_object->condition == Condition_LOOKING_UP || game_object->condition == Condition_HANGING_LOOKING_UP) && timers[game_object->looking_key_held_timer] <= 0)
        {
            camera.target.y = game_object->pos.y + screen.size.y / 2 - 200;
        }

        //границы для камеры
        if (!(camera.target.x - screen.size.x / 2 > -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * CHUNK_SIZE_X - 150))
        {
            camera.target.x = -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * CHUNK_SIZE_X - 150 + screen.size.x / 2;
        }

        if (!(camera.target.x + screen.size.x / 2 < -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * (CHUNK_COUNT_X + 1) * CHUNK_SIZE_X + 150))
        {
            camera.target.x = -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * (CHUNK_COUNT_X + 1) * CHUNK_SIZE_X + 150 - screen.size.x / 2;
        }

        if (!(camera.target.y - screen.size.y / 2 > -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * CHUNK_SIZE_Y - 120))
        {
            camera.target.y = -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * CHUNK_SIZE_Y - 120 + screen.size.y / 2;
        }

        if (!(camera.target.y + screen.size.y / 2 < -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * (CHUNK_COUNT_Y + 1) * CHUNK_SIZE_Y + 120))
        {
            camera.target.y = -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * (CHUNK_COUNT_Y + 1) * CHUNK_SIZE_Y + 120 - screen.size.y / 2;
        }

        //прорисовка игрока

        //хитбокс
        draw_bitmap(game_object->pos + V2{0, (game_object->sprite.size.y * 5 - game_object->hit_box.y) / 2}, V2{game_object->sprite.size.x*game_object->looking_direction, game_object->sprite.size.y} * 5, 0, game_object->sprite, LAYER_GAME_OBJECT);

        draw_light(game_object->pos, -200, 300);
    }

    if (game_object->type == Game_Object_ZOMBIE)
    {
        //константы скорости
        f32 accel_const = 3.2;
        f32 friction_const = 0.75;

        f32 jump_length = 15;
        f32 jump_height = TILE_SIZE_PIXELS * 1.1;
        f32 gravity = -2 * jump_height / jump_length / jump_length;

        //движение
        game_object->speed.x += (game_object->go_right - game_object->go_left) * accel_const;

        //прыжок
        if (game_object->condition != Condition_FALLING)
        {
            if (timers[game_object->can_jump] == 0)
            {
                game_object->speed.y = 2 * jump_height / jump_length;
                game_object->speed.x += (game_object->looking_direction) * 18;
            }
            else if (timers[game_object->can_jump] > 0)
            {
                game_object->go_left = false;
                game_object->go_right = false;
            }
        }

        //гравитация и трение
        game_object->speed.y += gravity;

        game_object->speed.x *= friction_const;

        //направление
        if (!(game_object->go_left && game_object->go_right))
        {
            if (game_object->go_left)
            {
                game_object->looking_direction = Direction_RIGHT;
            }
            if (game_object->go_right)
            {
                game_object->looking_direction = Direction_LEFT;
            }
        }

        //поведение, связанное с тайлами
        Collisions collisions = check_collision(game_object);

        //если стоит
        if (collisions.y.happened && collisions.y.tile_side == Side_TOP)
        {
            //состояния
            game_object->condition = Condition_IDLE;
            if (fabs(game_object->speed.x) > 1)
            {
                game_object->condition = Condition_MOOVING;
            }

            //если не заряжает прыжок
            if (timers[game_object->can_jump] < 0)
            {
                V2 collided_y_tile_pos = get_tile_pos(collisions.y.tile_index);

                Tile downleft_tile = tile_map[get_index(collided_y_tile_pos + V2{-1, 0})];
                Tile downright_tile = tile_map[get_index(collided_y_tile_pos + V2{1, 0})];
                Tile left_tile = tile_map[get_index(collided_y_tile_pos + V2{-1, 1})];
                Tile right_tile = tile_map[get_index(collided_y_tile_pos + V2{1, 1})];
                //если доходит до края тайла, то разворачивается
                if ((!downleft_tile.solid || left_tile.solid) && game_object->pos.x - game_object->hit_box.x * 0.5 - TILE_SIZE_PIXELS * 0.37 <= collided_y_tile_pos.x * TILE_SIZE_PIXELS - TILE_SIZE_PIXELS / 2)
                {
                    if (left_tile.solid)
                    {
                        game_object->looking_direction = Direction_RIGHT;
                    }
                    game_object->go_right = true;
                    game_object->go_left = false;
                }
                if ((!downright_tile.solid || right_tile.solid) && game_object->pos.x + game_object->hit_box.x * 0.5 + TILE_SIZE_PIXELS * 0.37 >= collided_y_tile_pos.x * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS / 2)
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
                                game_object->looking_direction = Direction_LEFT;
                            }
                            if (right_tile.solid)
                            {
                                game_object->looking_direction = Direction_RIGHT;
                            }
                        }
                    }
                }
            }
        }
        else
        {
            game_object->condition = Condition_FALLING;
            timers[game_object->can_jump] = -1;
        }

        //эффекты состояний

        if (game_object->condition == Condition_IDLE)
        {
            game_object->speed.x *= friction_const;
        }

        if (game_object->condition == Condition_FALLING)
        {
            game_object->go_right = game_object->looking_direction;
            game_object->go_left = !game_object->go_right;
        }

        game_object->pos += game_object->speed;

        Game_Object_Type triggers[1];
        triggers[0] = Game_Object_PLAYER;

        f32 vision_length = TILE_SIZE_PIXELS * 5;
        V2 obj_tile_pos = V2{roundf(game_object->pos.x / TILE_SIZE_PIXELS), roundf(game_object->pos.y / TILE_SIZE_PIXELS)};
        for (i32 tile_index = 1; tile_index * TILE_SIZE_PIXELS <= vision_length; tile_index++)
        {
            Tile tile = tile_map[get_index(obj_tile_pos + V2{(f32)tile_index * (game_object->looking_direction), 0})];
            if (tile.solid)
            {
                vision_length = fabs((obj_tile_pos.x + tile_index * (game_object->looking_direction)) * TILE_SIZE_PIXELS - game_object->pos.x - TILE_SIZE_PIXELS / 2 * (game_object->looking_direction));
            }
        }

        if (check_vision_box(game_object->pos, V2{vision_length / 2 * (f32)(game_object->looking_direction), 0}, V2{vision_length, 30}, triggers, 1, true) && timers[game_object->can_jump] < 0 && game_object->condition != Condition_FALLING)
        {
            timers[game_object->can_jump] = 1;
        }

        draw_rect(game_object->pos, game_object->hit_box, 0, 0xFFFF0000, LAYER_GAME_OBJECT);
    }
}

void generate_map()
{
    //карта тайлов
    V2 map_size = {(CHUNK_COUNT_X + 2) * CHUNK_SIZE_X, (CHUNK_COUNT_Y + 2) * CHUNK_SIZE_Y};
    i32 tile_count = map_size.x * map_size.y;

    tile_map = (Tile *)malloc(sizeof(Tile) * tile_count);

    //удаляем объекты
    for (i32 i = 0; i < game_object_count; i++)
    {
        game_objects[i].exists = false;
    }

    //удаляем таймеры
    timers_count = 1;

    //создание чанков
    char *chunk_strings[(CHUNK_COUNT_X + 2) * (CHUNK_COUNT_Y + 2)];

    //путь через тайлы
    V2 enter_chunk_pos = {(f32)random_int(1, CHUNK_COUNT_X), 1};
    V2 chunk_pos = enter_chunk_pos;
    V2 end_chunk_pos = {(f32)random_int(1, CHUNK_COUNT_X), CHUNK_COUNT_Y};

    //заполняем чанки
    for (i32 y = 0; y < CHUNK_COUNT_Y + 2; y++)
    {
        for (i32 x = 0; x < CHUNK_COUNT_X + 2; x++)
        {
            if (y == 0 || y == CHUNK_COUNT_Y + 1 || x == 0 || x == CHUNK_COUNT_Y + 1)
            {
                //чанк-граница
                chunk_strings[y * (CHUNK_COUNT_X + 2) + x] =
                    "----------"
                    "----------"
                    "----------"
                    "----------"
                    "----------"
                    "----------"
                    "----------"
                    "----------";
            }
            else
            {
                //чанк-наполнитель
                chunk_strings[y * (CHUNK_COUNT_X + 2) + x] =
                    "##########"
                    "##########"
                    "##########"
                    "##########"
                    "##########"
                    "##########"
                    "##########"
                    "##########";
            }
        }
    }

    i8 direction = random_int(-1, 1);
    if ((direction == 1 && chunk_pos.x == CHUNK_COUNT_X) || (direction == -1 && chunk_pos.x == 1))
    {
        direction = -direction;
    }

    while (chunk_pos != end_chunk_pos)
    {
        if ((random_float(0, 1) < 0.25 || direction == 0) && chunk_pos.y < CHUNK_COUNT_Y)
        {
            if (chunk_pos == enter_chunk_pos)
            {
                //чанк-вход с проходом вниз
                chunk_strings[(i32)(chunk_pos.y * (CHUNK_COUNT_X + 2) + chunk_pos.x)] =
                    "8      8  "
                    "8        8"
                    "       #  "
                    "     N  # "
                    "     T    "
                    "        # "
                    "M8MM   MMM"
                    "##### ####";
            }
            else
            {
                //чанк-проход вниз
                chunk_strings[(i32)(chunk_pos.y * (CHUNK_COUNT_X + 2) + chunk_pos.x)] =
                    "          "
                    "          "
                    "          "
                    "##     TT="
                    "   8      "
                    "     MMM8M"
                    "M8MM   8##"
                    "##### ####";
            }
            chunk_pos.y++;

            bool chance = random_int(0, 1);
            if (chance == 0)
            {
                direction = 1;
            }
            else if (chance == 1)
            {
                direction = -1;
            }
        }
        else
        {
            if ((direction == 1 && chunk_pos.x == CHUNK_COUNT_X) || (direction == -1 && chunk_pos.x == 1))
            {
                direction = 0;
            }

            if (chunk_pos.y == CHUNK_COUNT_Y)
            {
                direction = end_chunk_pos.x - chunk_pos.x;
                if (direction > 0)
                {
                    direction = 1;
                }
                else
                {
                    direction = -1;
                }
            }

            if (direction != 0)
            {
                if (chunk_pos == enter_chunk_pos)
                {
                    //чанк-вход
                    chunk_strings[(i32)(chunk_pos.y * (CHUNK_COUNT_X + 2) + chunk_pos.x)] =
                        "          "
                        "          "
                        "    N     "
                        "   TTTT   "
                        "   =###=  "
                        "  =#####8 "
                        "MMMM##### "
                        "##########";
                }
                else
                {
                    //обычный чанк
                    chunk_strings[(i32)(chunk_pos.y * (CHUNK_COUNT_X + 2) + chunk_pos.x)] =
                        " PPPPPPP  "
                        " #######  "
                        "          "
                        "         ="
                        "          "
                        "   TT     "
                        "MMMMMMMMMM"
                        "#======###";
                }
                chunk_pos.x += direction;
            }
        }
        //чанк-выход
        if (chunk_pos == end_chunk_pos)
        {
            chunk_strings[(i32)(chunk_pos.y * (CHUNK_COUNT_X + 2) + chunk_pos.x)] =
                "          "
                " 8        "
                "8         "
                "      T  8"
                "          "
                " #   X    "
                "  TTTTTT  "
                " ######## ";
        }
    }

    char *chunk_string;
    //заполняем чанки
    for (i32 chunk_index_y = 0; chunk_index_y < CHUNK_COUNT_Y + 2; chunk_index_y++)
    {
        for (i32 chunk_index_x = 0; chunk_index_x < CHUNK_COUNT_X + 2; chunk_index_x++)
        {
            chunk_string = chunk_strings[(CHUNK_COUNT_Y + 2 - chunk_index_y - 1) * (CHUNK_COUNT_X + 2) + chunk_index_x];
            for (i32 y = 0; y < CHUNK_SIZE_Y; y++)
            {
                for (i32 x = 0; x < CHUNK_SIZE_X; x++)
                {
                    Tile_type type = Tile_Type_NONE;
                    switch (chunk_string[(CHUNK_SIZE_Y - y - 1) * CHUNK_SIZE_X + x])
                    {
                    case '#':
                    {
                        type = Tile_Type_BRICK;
                        break;
                    };
                    case '=':
                    {
                        type = Tile_Type_ELEGANT_BRICK;
                        break;
                    };
                    case '-':
                    {
                        type = Tile_Type_BORDER;
                        break;
                    };
                    case 'N':
                    {
                        type = Tile_Type_ENTER;
                        break;
                    };
                    case 'X':
                    {
                        type = Tile_Type_EXIT;
                        break;
                    };
                    case 'M':
                    {
                        type = Tile_Type_MARBLE_FLOOR;
                        break;
                    };
                    case 'T':
                    {
                        type = Tile_Type_TILED_FLOOR;
                        break;
                    };
                    case 'P':
                    {
                        type = Tile_Type_PARAPET;
                        break;
                    };
                    case '8':
                    {
                        type = Tile_Type_STONE;
                        break;
                    };
                    }
                    V2 tile_pos = {(f32)(x + chunk_index_x * CHUNK_SIZE_X), (f32)(y + chunk_index_y * CHUNK_SIZE_Y)};
                    i32 index = get_index(tile_pos);
                    tile_map[index].type = type;
                }
            }
        }
    }

    //налаживаем свойства тайлов
    for (i32 index = 0; index < tile_count; index++)
    {
        V2 tile_pos = get_tile_pos(index);
        Bitmap sprite = img_None;
        f32 angle = 0;
        bool solid = true;
        switch (tile_map[index].type)
        {
        case Tile_Type_NONE:
        {
            solid = false;
            break;
        };
        case Tile_Type_BRICK:
        {
            f32 chance = random_float(0, 1);
            if (chance < 0.95)
            {
                sprite = img_Bricks[(i32)random_int(0, 6)];
            }
            else
            {
                sprite = img_Bricks[(i32)random_int(7, 11)];
            }
            break;
        };
        case Tile_Type_ELEGANT_BRICK:
        {
            sprite = img_ElegantBrick;
            break;
        };
        case Tile_Type_BORDER:
        {
            sprite = img_Border;
            i32 left_tile = get_index(tile_pos + V2{-1, 0});
            i32 right_tile = get_index(tile_pos + V2{1, 0});
            i32 top_tile = get_index(tile_pos + V2{0, 1});
            i32 bottom_tile = get_index(tile_pos + V2{0, -1});
            i32 bottomright_tile = get_index(tile_pos + V2{1, -1});
            i32 topright_tile = get_index(tile_pos + V2{1, 1});
            i32 bottomleft_tile = get_index(tile_pos + V2{-1, -1});
            i32 topleft_tile = get_index(tile_pos + V2{-1, 1});
            if (tile_pos.y != 0 && tile_map[bottom_tile].type != Tile_Type_BORDER)
            {
                sprite = img_TransitionBorder;
            }
            else if (tile_pos.x != map_size.x - 1 && tile_map[right_tile].type != Tile_Type_BORDER)
            {
                sprite = img_TransitionBorder;
                angle = PI * 0.5;
            }
            else if (tile_pos.y != map_size.y - 1 && tile_map[top_tile].type != Tile_Type_BORDER)
            {
                sprite = img_TransitionBorder;
                angle = PI;
            }
            else if (tile_pos.x != 0 && tile_map[left_tile].type != Tile_Type_BORDER)
            {
                sprite = img_TransitionBorder;
                angle = PI * 1.5;
            }
            else if (tile_pos.y != 0 && tile_pos.x != map_size.x - 1 && tile_map[bottomright_tile].type != Tile_Type_BORDER)
            {
                sprite = img_CornerBorder;
            }
            else if (tile_pos.y != map_size.y - 1 && tile_pos.x != map_size.x - 1 && tile_map[topright_tile].type != Tile_Type_BORDER)
            {
                sprite = img_CornerBorder;
                angle = PI * 0.5;
            }
            else if (tile_pos.y != map_size.y - 1 && tile_pos.x != 0 && tile_map[topleft_tile].type != Tile_Type_BORDER)
            {
                sprite = img_CornerBorder;
                angle = PI;
            }
            else if (tile_pos.y != 0 && tile_pos.x != 0 && tile_map[bottomleft_tile].type != Tile_Type_BORDER)
            {
                sprite = img_CornerBorder;
                angle = PI * 1.5;
            }
            break;
        };
        case Tile_Type_ENTER:
        {
            //addPlayer
            V2 spawn_pos = tile_pos * TILE_SIZE_PIXELS;
            add_game_object(Game_Object_PLAYER, spawn_pos);
            add_game_object(Game_Object_ZOMBIE, spawn_pos);
            camera.pos = spawn_pos;
            camera.target = spawn_pos;
            solid = false;
            sprite = img_Door[0];
            break;
        };
        case Tile_Type_EXIT:
        {
            solid = false;
            sprite = img_Door[6];
            tile_map[index].interactive = true;
            break;
        };
        case Tile_Type_MARBLE_FLOOR:
        {
            i16 chance = random_int(0, 3);
            sprite = img_MarbleFloor[chance];
            break;
        };
        case Tile_Type_TILED_FLOOR:
        {
            i16 chance = random_int(0, 3);
            sprite = img_TiledFloor[chance];
            break;
        };
        case Tile_Type_PARAPET:
        {
            sprite = img_Parapet;
            solid = false;
            break;
        };
        case Tile_Type_STONE:
        {
            sprite = img_Stone;
            break;
        };
        }
        tile_map[index].sprite = sprite;
        tile_map[index].angle = angle;
        tile_map[index].solid = solid;
    }
}

void update_tile(i32 tile_index)
{
    Tile *tile = &tile_map[tile_index];
    V2 tilePos = get_tile_pos(tile_index);

    //обновление тайлов
    if (tile->type == Tile_Type_PARAPET)
    {
        if (tile_map[get_index(tilePos + V2{0, -1})].type == Tile_Type_NONE)
        {
            tile->type = Tile_Type_NONE;
        }
    }

    if (tile->type == Tile_Type_EXIT)
    {
        draw_bitmap(tilePos * TILE_SIZE_PIXELS + V2{0, (f32)(tile_map[tile_index].sprite.size.y * 2.5 - TILE_SIZE_PIXELS / 2)}, tile_map[tile_index].sprite.size * 5, tile_map[tile_index].angle, tile_map[tile_index].sprite, LAYER_BACKGROUND4);
        draw_bitmap(tilePos * TILE_SIZE_PIXELS + V2{0, (f32)(tile_map[tile_index].sprite.size.y * 2.5 - TILE_SIZE_PIXELS / 2)}, img_DoorBack.size * 5, tile_map[tile_index].angle, img_DoorBack, LAYER_BACKGROUND2);
    }
    else if (tile->type == Tile_Type_NONE || tile->type == Tile_Type_PARAPET)
    {
        draw_bitmap(tilePos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS}, 0, img_BackGround, LAYER_BACKGROUND1);
    }
    else if (tile->type == Tile_Type_ENTER)
    {
        draw_bitmap(tilePos * TILE_SIZE_PIXELS + V2{0, (f32)(tile_map[tile_index].sprite.size.y * 2.5 - TILE_SIZE_PIXELS / 2)}, tile_map[tile_index].sprite.size * 5, tile_map[tile_index].angle, tile_map[tile_index].sprite, LAYER_BACKGROUND4);
    }
    else if (tile->type != Tile_Type_NONE)
    {
        draw_bitmap(tilePos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS}, tile_map[tile_index].angle, tile_map[tile_index].sprite, LAYER_TILE);
    }
}

bool initialized = false;
void game_update(Bitmap screen, Input input)
{
    //очистка экрана
    clear_screen(screen, darkness);
    V2 map_size = {(CHUNK_COUNT_X + 2) * CHUNK_SIZE_X, (CHUNK_COUNT_Y + 2) * CHUNK_SIZE_Y};
    i32 tile_count = map_size.x * map_size.y;

    //выполняется один раз
    if (!initialized)
    {
        initialized = true;

        //темнота
        i32 interval = floor(TILE_SIZE_PIXELS / img_PlayerIdle.size.x);
        darkness.pitch = screen.size.x + interval * 2;
        darkness.size = screen.size;
        u32 *pixels = (u32 *)malloc(sizeof(u32) * (darkness.size.x + interval * 2) * (darkness.size.y + interval * 2));
        darkness.pixels = pixels + (i32)darkness.pitch + interval;

        generate_map();
    }

    if (input.r.went_down)
    {
        generate_map();
    }

    //обновление сущностей
    for (i32 object_index = 0; object_index < game_object_count; object_index++)
    {
        if (game_objects[object_index].exists)
        {
            update_game_object(&game_objects[object_index], input, screen);
        }
    }

    camera.pos += (camera.target - camera.pos) * 0.25f;

    // //прорисовка темноты
    // f32 intervalx2 = darkness.pitch - darkness.size.x;
    // draw_bitmap(camera.pos, screen.size + V2{intervalx2, intervalx2}, 0, darkness, LAYER_FORGROUND);

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
        for (i32 drawingIndex = 0; drawingIndex < draw_queue_size; drawingIndex++)
        {
            if (draw_queue[drawingIndex].layer == layer)
            {
                draw_queue[drawingIndex].pos += screen.size / 2 - camera.pos;
                new_draw_queue[new_draw_queue_size] = draw_queue[drawingIndex];
                new_draw_queue_size++;
            }
        }
        layer = (Layer)((i32)layer + 1);
    }

    draw_queue_size = 0;

    for (i32 i = 0; i <= new_draw_queue_size; i++)
    {
        draw_item(screen, new_draw_queue[i]);
    }

    update_timers();
}