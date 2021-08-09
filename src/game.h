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

//вызывает ошибку при условии
#define assert(cond) \
    if (!(cond))     \
        *(int *)NULL = 12;

#include "math.h"
#include "game_math.cpp"
#include <string.h>

//fps
#define TARGET_TIME_PER_FRAME (1.0f / 60.0f)
#define DT ((f32)TARGET_TIME_PER_FRAME)

#define PI 3.14159265359
#define INT_INFINITY 2147483647

#define TILE_SIZE_PIXELS 80
#define CHUNK_SIZE_X 10
#define CHUNK_SIZE_Y 8
#define CHUNK_COUNT_X 4
#define CHUNK_COUNT_Y 4
#define BORDER_SIZE 2

#define SPRITE_SCALE 5

#define ARRAY_COUNT(arr) (sizeof(arr) / sizeof(arr[0]))

typedef struct
{
    byte *data;
    u32 size;
} File_Buffer;

//битмап
typedef struct
{
    u32 *pixels;
    V2 size;
    i32 pitch;
} Bitmap;

Bitmap create_empty_bitmap(V2 size)
{
    u64 alignment = 8 * sizeof(u32);
    u64 screen_buffer_size = 4 * ((i32)size.x + 2) * ((i32)size.y + 2);
    screen_buffer_size += alignment - (screen_buffer_size % alignment);

    Bitmap result;
    result.pitch = (i32)size.x + 2;
    result.size = size;
    result.pixels = (u32 *)_aligned_malloc(screen_buffer_size, alignment);
    memset(result.pixels, 0, screen_buffer_size);
    return result;
}

//ввод
typedef struct
{
    bool is_down;
    bool went_down;
    bool went_up;
} Button;

#define BUTTON_COUNT 13

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
        Button l;
        Button p;
        Button t;
        Button s;
        Button shift;
        Button space;
    };
    Button buttons[BUTTON_COUNT];
} Input;

//прорисовка
typedef enum
{
    DRAWING_TYPE_RECT,
    DRAWING_TYPE_BITMAP,
    DRAWING_TYPE_OLD_BITMAP,
    DRAWING_TYPE_LIGHT,
    DRAWING_TYPE_OLD_LIGHT,
    DRAWING_TYPE_TEXT,
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

//получение картинки
typedef struct
{
    Drawing_Type type;
    V2 pos;
    V2 size;
    f32 angle;
    f32 alpha;
    u32 color;
    Bitmap bitmap;
    char *string;
    Layer layer;
} Drawing;

//сущности
typedef enum
{
    Game_Object_PLAYER,
    Game_Object_ZOMBIE,
    Game_Object_TOY_GUN,
    Game_Object_BOMB,
    Game_Object_TOY_GUN_BULLET,
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

//камера
typedef struct
{
    V2 pos;
    V2 target;
    V2 scale;
} Camera;

typedef struct
{
    i32 id;
    i32 index;
} Game_Object_Handle;

typedef struct
{
    i32 id;

    Game_Object_Type type;

    bool exists;

    f32 healthpoints;
    f32 max_healthpoints;

    f32 damage;

    V2 pos;
    V2 collision_box_pos;
    V2 collision_box;
    V2 hit_box_pos;
    V2 hit_box;
    V2 speed;
    V2 delta;
    V2 deflection;
    V2 target_deflection;
    f32 angle;

    Game_Object_Handle weapon;

    bool go_left;
    bool go_right;
    i32 jump;

    f32 accel;
    f32 friction;
    f32 gravity;
    f32 jump_height;
    f32 jump_duration;
    f32 bounce;
    f32 mass;

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
    i32 hanging_index;
    i32 cant_control_timer;
    i32 invulnerable_timer;
    i32 walks_throught_planks_timer;

    Layer layer;
} Game_Object;

typedef enum
{
    Bitmap_type_NONE,
    Bitmap_type_TEST,
    Bitmap_type_PLAYER_IDLE,
    Bitmap_type_PLAYER_JUMP,
    Bitmap_type_PLAYER_CROUCH_IDLE,
    Bitmap_type_PLAYER_HANGING,
    Bitmap_type_PLAYER_HANGING_UP,
    Bitmap_type_PLAYER_HANGING_DOWN,
    Bitmap_type_PLAYER_LOOKING_UP,
    Bitmap_type_PLAYER_STEP,
    Bitmap_type_PLAYER_STEP_END = Bitmap_type_PLAYER_STEP + 5,
    Bitmap_type_PLAYER_CROUCH_START,
    Bitmap_type_PLAYER_CROUCH_START_END = Bitmap_type_PLAYER_CROUCH_START + 3,
    Bitmap_type_PLAYER_CROUCH_STEP,
    Bitmap_type_PLAYER_CROUCH_STEP_END = Bitmap_type_PLAYER_CROUCH_STEP + 5,
    Bitmap_type_ZOMBIE_IDLE,
    Bitmap_type_ZOMBIE_STEP,
    Bitmap_type_ZOMBIE_STEP_END = Bitmap_type_ZOMBIE_STEP + 5,
    Bitmap_type_BACKGROUND,
    Bitmap_type_BORDER,
    Bitmap_type_TRANSITION_BORDER_0,
    Bitmap_type_TRANSITION_BORDER_05,
    Bitmap_type_TRANSITION_BORDER_1,
    Bitmap_type_TRANSITION_BORDER_15,
    Bitmap_type_CORNER_BORDER_0,
    Bitmap_type_CORNER_BORDER_05,
    Bitmap_type_CORNER_BORDER_1,
    Bitmap_type_CORNER_BORDER_15,
    Bitmap_type_BRICKS,
    Bitmap_type_BRICK_END = Bitmap_type_BRICKS + 11,
    Bitmap_type_ELEGANT_BRICK,
    Bitmap_type_STONE,
    Bitmap_type_MARBLE_FLOOR,
    Bitmap_type_TILED_FLOOR,
    Bitmap_type_DOOR,
    Bitmap_type_DOOR_END = Bitmap_type_DOOR + 6,
    Bitmap_type_DOOR_BACK,
    Bitmap_type_PARAPET,
    Bitmap_type_HEALTH_BAR,
    Bitmap_type_HEALTH,
    Bitmap_type_SPIKES,
    Bitmap_type_TOY_GUN,
    Bitmap_type_TOY_GUN_BULLET,
    Bitmap_type_EXIT_SIGN,
    Bitmap_type_EXIT_SIGN_OFF,
    Bitmap_type_LAMP,
    Bitmap_type_LAMP_OFF,
    Bitmap_type_BOMB,
    Bitmap_type_PLANK,
    Bitmap_type_UNBREAKABLE_PLANK,
    Bitmap_type_COUNT,
} Bitmap_type;

//буква
typedef struct
{
    Bitmap bitmap;
    V2 offset;
    i32 *kernings;
    i32 advance;
} Letter;

//тайлы
typedef enum
{
    Tile_Type_NONE,
    Tile_Type_OCCUPIED,
    Tile_Type_NORMAL,
    Tile_Type_FLOOR,
    Tile_Type_BORDER,
    Tile_Type_ENTER,
    Tile_Type_EXIT,
    Tile_Type_PARAPET,
    Tile_Type_SPIKES,
    Tile_Type_LAMP,
    Tile_Type_PLANK,
    Tile_Type_UNBREAKABLE_PLANK,
} Tile_type;

typedef enum
{
    Solidness_Type_NONE = 0,
    Solidness_Type_NORMAL = 1,
    Solidness_Type_UP = 2,
} Solidness_Type;

typedef struct
{
    Tile_type type;
    Bitmap sprite;
    Solidness_Type solid;
    i32 timer;
} Tile;

#define READ_BMP(name) Bitmap name(char *file_name)
typedef READ_BMP(Read_BMP);

#define READ_FONT(name) Letter name(char *file_name, i32 letter_code, f32 letter_height)
typedef READ_FONT(Read_Font);

//состояние игры
typedef struct
{
    //таймеры
    i32 timers[1024];
    i32 timers_count;

    Drawing draw_queue[1024 * 8];
    i32 draw_queue_size;

    Tile tile_map[(CHUNK_COUNT_X * CHUNK_SIZE_X + BORDER_SIZE * 2) * (CHUNK_COUNT_Y * CHUNK_SIZE_Y + BORDER_SIZE * 2)];

    f32 transition;
    Bitmap darkness;

    i32 game_object_count;
    Game_Object game_objects[512];

    bool initialized;

    Camera camera;

    Bitmap bitmaps[Bitmap_type_COUNT];
    Letter letters[256];

    Read_BMP *win32_read_bmp;
    Read_Font *stbtt_read_font;

    xoshiro256ss_state __global_random_state;

    i32 pause;

    i32 id_count;

    i32 lamp_count;

    Game_Object_Handle player;

    i32 screen_shake_timer;
    f32 screen_shake_power;
} Game_memory;

#define GAME_UPDATE(name) void name(Bitmap screen, Game_memory *memory, Input input)
typedef GAME_UPDATE(Game_Update);
GAME_UPDATE(Game_Update_Stub)
{
}

i32 string_length(char *string)
{
    i32 count = 0;
    while (*string++)
    {
        ++count;
    }
    return count;
}