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

//fps
#define TARGET_TIME_PER_FRAME (1.0f / 60.0f)
#define DT ((f32)TARGET_TIME_PER_FRAME)

#define PI 3.14159265359

#define TILE_SIZE_PIXELS 80
#define CHUNK_SIZE_X 10
#define CHUNK_SIZE_Y 8
#define CHUNK_COUNT_X 4
#define CHUNK_COUNT_Y 4
#define BORDER_SIZE 2

#define SPRITE_SCALE 5

#define ARRAY_COUNT(arr) (sizeof(arr) / sizeof(arr[0]))

//битмап
typedef struct
{
    u32 *pixels;
    V2 size;
    i32 pitch;
} Bitmap;

//ввод
typedef struct
{
    bool is_down;
    bool went_down;
    bool went_up;
} Button;

#define BUTTON_COUNT 10

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
    Layer layer;
} Drawing;

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

//камера
typedef struct
{
    V2 pos;
    V2 target;
    V2 scale;
} Camera;

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
    Bitmap_type_COUNT,
} Bitmap_type;

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

#define READ_BMP(name) Bitmap name(char *file_name)
typedef READ_BMP(Read_BMP);

//состояние игры
typedef struct
{
    //таймеры
    i32 timers[512];
    i32 timers_count;

    Drawing draw_queue[1024 * 8];
    i32 draw_queue_size;

    bool transition_is_on;
    f32 darkness_lvl;

    Tile tile_map[(CHUNK_COUNT_X * CHUNK_SIZE_X + BORDER_SIZE * 2) * (CHUNK_COUNT_Y * CHUNK_SIZE_Y + BORDER_SIZE * 2)];

    Bitmap darkness;
    f32 light_lvl;

    i32 game_object_count;
    Game_Object game_objects[512];
    f32 UI_alpha;

    bool draw_darkness;

    bool initialized;

    Camera camera;

    Bitmap bitmaps[Bitmap_type_COUNT];

    Read_BMP *win32_read_bmp;
} Game_memory;

#define GAME_UPDATE(name) void name(Bitmap screen, Game_memory *memory, Input input)
typedef GAME_UPDATE(Game_Update);
GAME_UPDATE(Game_Update_Stub)
{
}

typedef struct
{
    void *recording_handle;
    void *play_back_handle;
    i32 input_recording_index;
    i32 input_playing_index;
    Game_memory *memory;
} win32_State;
