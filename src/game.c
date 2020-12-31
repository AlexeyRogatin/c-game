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

Bitmap img_Door = win32_read_bmp("../data/door_closed.bmp");

Bitmap img_DoorOpens[6] = {
    win32_read_bmp("../data/door1.bmp"),
    win32_read_bmp("../data/door2.bmp"),
    win32_read_bmp("../data/door3.bmp"),
    win32_read_bmp("../data/door4.bmp"),
    win32_read_bmp("../data/door5.bmp"),
    win32_read_bmp("../data/door6.bmp"),
};

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
    LAYER_BACKGROUND,
    LAYER_FBACKGROUND,
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

Bilinear_Sample get_bilinear_sample(Bitmap bitmap, i32 x, i32 y)
{
    ARGB a = {bitmap.pixels[y * bitmap.pitch + x]};
    ARGB b = {bitmap.pixels[y * bitmap.pitch + x + 1]};
    ARGB c = {bitmap.pixels[(y + 1) * bitmap.pitch + x]};
    ARGB d = {bitmap.pixels[(y + 1) * bitmap.pitch + x + 1]};

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

ARGB bilinear_blend(Bilinear_Sample sample, f32 x_f, f32 y_f)
{
    ARGB ab = lerp(sample.a, sample.b, x_f);
    ARGB cd = lerp(sample.c, sample.d, x_f);
    ARGB abcd = lerp(ab, cd, y_f);
    return abcd;
}

void draw_item(Bitmap screen, Drawing drawing)
{
    if (drawing.type == DRAWING_TYPE_RECT)
    {
        V2 x_axis = rotate_vector({drawing.size.x, 0}, drawing.angle);
        V2 y_axis = rotate_vector({0, drawing.size.y}, drawing.angle);

        V2 origin = drawing.pos - x_axis / 2 - y_axis / 2;

        f32 x_axis_length = drawing.size.x;
        f32 y_axis_length = drawing.size.y;

        V2 vertices[] = {
            origin,
            origin + x_axis,
            origin + y_axis,
            origin + x_axis + y_axis,
        };

        V2 min = {INFINITY, INFINITY};
        V2 max = {-INFINITY, -INFINITY};

        for (i32 vertex_index = 0; vertex_index < 4; vertex_index++)
        {
            V2 vertex = vertices[vertex_index];
            if (vertex.x < min.x)
                min.x = vertex.x;
            if (vertex.y < min.y)
                min.y = vertex.y;
            if (vertex.x > max.x)
                max.x = vertex.x;
            if (vertex.y > max.y)
                max.y = vertex.y;
        }

        if (min.x < 0)
            min.x = 0;
        if (min.y < 0)
            min.y = 0;
        if (max.x > screen.size.x)
            max.x = screen.size.x;
        if (max.y > screen.size.y)
            max.y = screen.size.y;

        min.x = floor(min.x);
        min.y = floor(min.y);
        max.x = ceil(max.x);
        max.y = ceil(max.y);

        for (i32 y = min.y; y < max.y; y++)
        {
            for (i32 x = min.x; x < max.x; x++)
            {
                V2 pixel_coords = {(f32)x, (f32)y};
                V2 d = pixel_coords - origin;

                f32 dot_x = dot(d, x_axis) / x_axis_length;
                f32 dot_y = dot(d, y_axis) / y_axis_length;

                if (dot_x >= 0 && dot_x < x_axis_length && dot_y >= 0 && dot_y < y_axis_length)
                {
                    screen.pixels[y * (i32)screen.size.x + x] = drawing.color;
                }
            }
        }
    }

    if (drawing.type == DRAWING_TYPE_BITMAP)
    {
        V2 x_axis = rotate_vector({drawing.size.x, 0}, drawing.angle);
        V2 y_axis = rotate_vector({0, drawing.size.y}, drawing.angle);

        V2 origin = drawing.pos - x_axis / 2 - y_axis / 2;

        f32 x_axis_length = length(x_axis);
        f32 y_axis_length = length(y_axis);

        V2 vertices[] = {
            origin,
            origin + x_axis,
            origin + y_axis,
            origin + x_axis + y_axis,
        };

        V2 min = {INFINITY, INFINITY};
        V2 max = {-INFINITY, -INFINITY};

        for (i32 vertex_index = 0; vertex_index < 4; vertex_index++)
        {
            V2 vertex = vertices[vertex_index];
            if (vertex.x < min.x)
                min.x = vertex.x;
            if (vertex.y < min.y)
                min.y = vertex.y;
            if (vertex.x > max.x)
                max.x = vertex.x;
            if (vertex.y > max.y)
                max.y = vertex.y;
        }

        if (min.x < 0)
            min.x = 0;
        if (min.y < 0)
            min.y = 0;
        if (max.x > screen.size.x)
            max.x = screen.size.x;
        if (max.y > screen.size.y)
            max.y = screen.size.y;

        min.x = floor(min.x);
        min.y = floor(min.y);
        max.x = ceil(max.x);
        max.y = ceil(max.y);

        for (i32 y = min.y; y < max.y; y++)
        {
            for (i32 x = min.x; x < max.x; x++)
            {
                V2 pixel_coords = {(f32)x, (f32)y};
                V2 d = pixel_coords - origin;

                f32 dot_x = dot(d, x_axis) / x_axis_length;
                f32 dot_y = dot(d, y_axis) / y_axis_length;

                if (dot_x >= 0 && dot_x <= x_axis_length && dot_y >= 0 && dot_y <= y_axis_length)
                {

                    f32 u = dot_x / x_axis_length;
                    f32 v = dot_y / y_axis_length;

                    f32 tex_x = u * drawing.bitmap.size.x;
                    f32 tex_y = v * drawing.bitmap.size.y;

                    i32 x_int = (i32)tex_x;
                    i32 y_int = (i32)tex_y;

                    f32 x_f = tex_x - x_int;
                    f32 y_f = tex_y - y_int;

                    ARGB pixel = {screen.pixels[y * (i32)screen.size.x + x]};

                    // Bilinear_Sample sample = get_bilinear_sample(drawing.bitmap, x_int, y_int);
                    // ARGB blended_sample = bilinear_blend(sample, x_f, y_f);
                    // ARGB result;

                    // f32 alpha = (0xFF - blended_sample.a) / 255.0f;

                    // result.r = blended_sample.r + pixel.r * alpha;
                    // result.g = blended_sample.g + pixel.g * alpha;
                    // result.b = blended_sample.b + pixel.b * alpha;
                    // result.a = 0xFF;

                    // screen.pixels[y * (i32)screen.size.x + x] = result.argb;

                    ARGB sample = {drawing.bitmap.pixels[y_int * drawing.bitmap.pitch + x_int]};

                    ARGB result;

                    f32 alpha = (0xFF - sample.a) / 255.0f;

                    result.r = sample.r + pixel.r * alpha;
                    result.g = sample.g + pixel.g * alpha;
                    result.b = sample.b + pixel.b * alpha;
                    result.a = 0xFF;

                    screen.pixels[y * (i32)screen.size.x + x] = result.argb;
                }
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
} Tile;

Tile *tile_map = NULL;

V2 index_to_pos(i32 index)
{
    //index = y * (CHUNK_SIZE_X * (CHUNK_COUNT_X + 2)) + x;
    f32 y = floor(index / (CHUNK_SIZE_X * (CHUNK_COUNT_X + 2)));
    f32 x = index - y * (CHUNK_SIZE_X * (CHUNK_COUNT_X + 2));
    V2 result = {x, y};
    return result;
}

i32 pos_to_index(V2 coords)
{
    i32 index = coords.y * (CHUNK_SIZE_X * (CHUNK_COUNT_X + 2)) + coords.x;
    return index;
}

V2 pixel_to_pos(V2 pos)
{
    V2 result = V2{roundf(pos.x / TILE_SIZE_PIXELS), roundf(pos.y / TILE_SIZE_PIXELS)};
    return result;
}

V2 pos_to_pixel(V2 pos)
{
    V2 result = pos * TILE_SIZE_PIXELS;
    return result;
}

Tile index_to_tile(i32 index)
{
    Tile result = tile_map[index];
    return result;
}

Tile pos_to_tile(V2 pos)
{
    Tile result = tile_map[pos_to_index(pos)];
    return result;
}

//сущности
typedef enum
{
    Game_Object_PLAYER,
    Game_Object_ZOMBIE,
} Game_Object_Type;

typedef enum
{
    Direction_RIGHT,
    Direction_LEFT,
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

struct Rect2
{
    V2 min;
    V2 max;
};

Rect2 get_bbox(Game_Object *o)
{
    Rect2 result;
    result.max.y = o->pos.y + o->hit_box.y / 2;
    result.min.y = o->pos.y - o->hit_box.y / 2;
    result.max.x = o->pos.x + o->hit_box.x / 2;
    result.min.x = o->pos.x - o->hit_box.x / 2;

    return result;
}

Rect2 get_tile_bbox(V2 tile_pos)
{
    Rect2 result;
    result.max.y = tile_pos.y + TILE_SIZE_PIXELS * 0.5;
    result.min.y = tile_pos.y - TILE_SIZE_PIXELS * 0.5;
    result.max.x = tile_pos.x + TILE_SIZE_PIXELS * 0.5;
    result.min.x = tile_pos.x - TILE_SIZE_PIXELS * 0.5;

    return result;
}

Collisions check_collision(Game_Object *game_object)
{
    Collisions result;
    result.x.happened = false;
    result.y.happened = false;
    result.expanded_collision = false;

    Rect2 bbox = get_bbox(game_object);

    f32 obj_side;

    if (game_object->speed.x >= 0)
    {
        obj_side = bbox.max.x;
    }
    else
    {
        obj_side = bbox.min.x;
    }

    if (pos_to_tile(pixel_to_pos(V2{obj_side + game_object->speed.x, bbox.max.y})).solid || pos_to_tile(pixel_to_pos(V2{obj_side + game_object->speed.x, bbox.min.y})).solid)
    {
        if (game_object->speed.x >= 0)
        {
            f32 tile_left = pixel_to_pos(V2{obj_side + game_object->speed.x, bbox.max.y}).x * TILE_SIZE_PIXELS - TILE_SIZE_PIXELS * 0.5f;
            game_object->pos.x = tile_left - game_object->hit_box.x * 0.5f;
        }
        else
        {
            f32 tile_right = pixel_to_pos(V2{obj_side + game_object->speed.x, bbox.max.y}).x * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS * 0.5f;
            game_object->pos.x = tile_right + game_object->hit_box.x * 0.5f;
        }
        game_object->speed.x = 0;
    }

    game_object->pos.x += game_object->speed.x;

    bbox = get_bbox(game_object);

    if (game_object->speed.y >= 0)
    {
        obj_side = bbox.max.y;
    }
    else
    {
        obj_side = bbox.min.y;
    }

    if (pos_to_tile(pixel_to_pos(V2{bbox.max.x, obj_side + game_object->speed.y})).solid || pos_to_tile(pixel_to_pos(V2{bbox.min.x, obj_side + game_object->speed.y})).solid)
    {
        if (game_object->speed.y >= 0)
        {
            f32 tile_bottom = pixel_to_pos(V2{bbox.max.x, obj_side + game_object->speed.y}).y * TILE_SIZE_PIXELS - TILE_SIZE_PIXELS * 0.5f;
            game_object->pos.y = tile_bottom - game_object->hit_box.y * 0.5f;
        }
        else
        {
            f32 tile_top = pixel_to_pos(V2{bbox.max.x, obj_side + game_object->speed.y}).y * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS * 0.5f;
            game_object->pos.y = tile_top + game_object->hit_box.y * 0.5f;
            result.y.tile_side = Side_TOP;
        }
        game_object->speed.y = 0;
        result.y.happened = true;
    }

    game_object->pos.y += game_object->speed.y;

    //столкновение
    //x - произошло, с чем, с какой стороной
    //y - ...
    //удлинённое столкновение

    return result;
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

void update_game_object(Game_Object *game_object, Input input, Bitmap screen)
{
    if (game_object->type == Game_Object_PLAYER)
    {
        //предпологаемое состояние
        Condition supposed_cond;

        //движение игрока
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
        V2 collided_x_tile_pos = index_to_pos(collisions.x.tile_index);
        V2 collided_y_tile_pos = index_to_pos(collisions.y.tile_index);

        i32 collision_up_tile_index = (collided_x_tile_pos.y + 1) * CHUNK_SIZE_X * (CHUNK_COUNT_X + 2) + collided_x_tile_pos.x;
        i32 up_tile_index = (collided_x_tile_pos.y + 1) * CHUNK_SIZE_X * (CHUNK_COUNT_X + 2) + collided_x_tile_pos.x + (game_object->looking_direction * 2 - 1);

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
            V2 tile_pos = collided_y_tile_pos - V2{(f32)game_object->looking_direction * 2 - 1, 0};
            if (!tile_map[pos_to_index(tile_pos)].solid &&
                ((game_object->pos.x + game_object->speed.x + game_object->hit_box.x / 2 <= tile_pos.x * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS / 2) && (ceilf(game_object->pos.x + game_object->hit_box.x / 2) >= tile_pos.x * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS / 2) ||
                 (game_object->pos.x + game_object->speed.x - game_object->hit_box.x / 2 >= tile_pos.x * TILE_SIZE_PIXELS - TILE_SIZE_PIXELS / 2) && (floorf(game_object->pos.x - game_object->hit_box.x / 2) <= tile_pos.x * TILE_SIZE_PIXELS - TILE_SIZE_PIXELS / 2)))
            {
                game_object->pos.x = tile_pos.x * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS / 2 * (game_object->looking_direction * 2 - 1) + game_object->hit_box.x / 2 * -(game_object->looking_direction * 2 - 1);
                supposed_cond = Condition_HANGING;
                game_object->looking_direction = (Direction)((-(game_object->looking_direction * 2 - 1) + 1) / 2);
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
        draw_bitmap(game_object->pos + V2{0, (game_object->sprite.size.y * 5 - game_object->hit_box.y) / 2}, V2{game_object->sprite.size.x * -(game_object->looking_direction * 2 - 1), game_object->sprite.size.y} * 5, 0, game_object->sprite, LAYER_GAME_OBJECT);
        draw_rect(game_object->pos, game_object->hit_box, 0, 0xFFFF0000, LAYER_FORGROUND);

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
                game_object->speed.x += (game_object->looking_direction * 2 - 1) * 18;
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
                V2 collided_y_tile_pos = index_to_pos(collisions.y.tile_index);

                Tile downleft_tile = tile_map[pos_to_index(collided_y_tile_pos + V2{-1, 0})];
                Tile downright_tile = tile_map[pos_to_index(collided_y_tile_pos + V2{1, 0})];
                Tile left_tile = tile_map[pos_to_index(collided_y_tile_pos + V2{-1, 1})];
                Tile right_tile = tile_map[pos_to_index(collided_y_tile_pos + V2{1, 1})];
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
            Tile tile = tile_map[pos_to_index(obj_tile_pos + V2{(f32)tile_index * (game_object->looking_direction * 2 - 1), 0})];
            if (tile.solid)
            {
                vision_length = fabs((obj_tile_pos.x + tile_index * (game_object->looking_direction * 2 - 1)) * TILE_SIZE_PIXELS - game_object->pos.x - TILE_SIZE_PIXELS / 2 * (game_object->looking_direction * 2 - 1));
            }
        }

        if (check_vision_box(game_object->pos, V2{vision_length / 2 * (f32)(game_object->looking_direction * 2 - 1), 0}, V2{vision_length, 30}, triggers, 1, true) && timers[game_object->can_jump] < 0 && game_object->condition != Condition_FALLING)
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
                    i32 index = pos_to_index(tile_pos);
                    tile_map[index].type = type;
                }
            }
        }
    }

    //налаживаем свойства тайлов
    for (i32 index = 0; index < tile_count; index++)
    {
        V2 tile_pos = index_to_pos(index);
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
            i32 left_tile = pos_to_index(tile_pos + V2{-1, 0});
            i32 right_tile = pos_to_index(tile_pos + V2{1, 0});
            i32 top_tile = pos_to_index(tile_pos + V2{0, 1});
            i32 bottom_tile = pos_to_index(tile_pos + V2{0, -1});
            i32 bottomright_tile = pos_to_index(tile_pos + V2{1, -1});
            i32 topright_tile = pos_to_index(tile_pos + V2{1, 1});
            i32 bottomleft_tile = pos_to_index(tile_pos + V2{-1, -1});
            i32 topleft_tile = pos_to_index(tile_pos + V2{-1, 1});
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
            // add_game_object(Game_Object_ZOMBIE, spawn_pos);
            camera.pos = spawn_pos;
            camera.target = spawn_pos;
            solid = false;
            sprite = img_Door;
            break;
        };
        case Tile_Type_EXIT:
        {
            solid = false;
            sprite = img_Door;
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
    V2 tilePos = index_to_pos(tile_index);

    if (tile->solid)
    {
        draw_rect(tilePos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS}, 0, 0xFFFFFF00, LAYER_TILE);
    }

    // // задник
    // if (tile->type == Tile_Type_NONE || tile->type == Tile_Type_PARAPET)
    // {
    //     draw_bitmap(tilePos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS}, 0, img_BackGround, LAYER_BACKGROUND);
    // }
    // else if (tile->type == Tile_Type_ENTER)
    // {
    //     draw_bitmap(tilePos * TILE_SIZE_PIXELS + V2{0, (f32)(tile_map[tile_index].sprite.size.y * 2.5 - TILE_SIZE_PIXELS / 2)}, tile_map[tile_index].sprite.size * 5, tile_map[tile_index].angle, tile_map[tile_index].sprite, LAYER_FBACKGROUND);
    // }
    // else if (tile->type == Tile_Type_EXIT)
    // {
    //     draw_bitmap(tilePos * TILE_SIZE_PIXELS + V2{0, (f32)(tile_map[tile_index].sprite.size.y * 2.5 - TILE_SIZE_PIXELS / 2)}, tile_map[tile_index].sprite.size * 5, tile_map[tile_index].angle, tile_map[tile_index].sprite, LAYER_FBACKGROUND);
    // }
    // else if (tile->type != Tile_Type_NONE)
    // {
    //     draw_bitmap(tilePos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS}, tile_map[tile_index].angle, tile_map[tile_index].sprite, LAYER_TILE);
    // }

    if (tile->type == Tile_Type_PARAPET)
    {
        if (tile_map[pos_to_index(tilePos + V2{0, -1})].type == Tile_Type_NONE)
        {
            tile->type = Tile_Type_NONE;
        }
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