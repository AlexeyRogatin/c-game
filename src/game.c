#include <malloc.h>
#include "game_math.c"

//input
typedef struct
{
    bool is_down;
    bool went_down;
    bool went_up;
} Button;

#define BUTTON_COUNT 5

typedef union {
    struct
    {
        Button left;
        Button right;
        Button up;
        Button down;
        Button space;
    };
    Button buttons[BUTTON_COUNT];
} Input;

//getBitmap
typedef struct
{
    u32 *pixels;
    i32 width;
    i32 height;
} Bitmap;

Bitmap win32_read_bmp(char *file_name);

//drawing
void draw_rect(Bitmap screen, i32 x, i32 y, i32 width, i32 height, i32 color)
{
    i32 left = x - width / 2;
    i32 right = x + width / 2;
    i32 bottom = y - height / 2;
    i32 top = y + height / 2;
    if (left < 0)
        left = 0;
    if (right > screen.width)
        right = screen.width;
    if (bottom < 0)
        bottom = 0;
    if (top > screen.height)
        top = screen.height;

    for (i32 y = bottom; y < top; y++)
    {
        for (i32 x = left; x < right; x++)
        {
            *(screen.pixels + y * screen.width + x) = color;
        }
    }
}

void draw_bitmap(Bitmap screen, i32 pos_x, i32 pos_y, Bitmap bitmap)
{
    i32 left = pos_x - bitmap.width / 2;
    i32 right = pos_x + bitmap.width / 2;
    i32 bottom = pos_y - bitmap.height / 2;
    i32 top = pos_y + bitmap.height / 2;

    if (left < 0)
        left = 0;
    if (right > screen.width)
        right = screen.width;
    if (bottom < 0)
        bottom = 0;
    if (top > screen.height)
        top = screen.height;

    i32 width = right - left;
    i32 height = top - bottom;

    for (i32 y = 0; y < top - bottom; y++)
    {
        for (i32 x = 0; x < right - left; x++)
        {
            i32 screen_x = left + x;
            i32 screen_y = bottom + y;

            i32 texture_x = x + (left - pos_x);
            i32 texure_y = y + (bottom - pos_y);

            u32 texel = bitmap.pixels[bitmap.width * texure_y + texture_x];
            screen.pixels[screen_y * screen.width + screen_x] = texel;
        }
    }
}

typedef enum
{
    Tile_Type_NONE,
    Tile_Type_WALL,
} Tile_Type;

typedef struct
{
    V2 pos;
    Tile_Type type;
} Tile;

#define TILE_SIZE_PIXELS 64

Tile *tile_map = NULL;

typedef struct
{
    V2 pos;
    V2 size;
    V2 speed;
} Game_Object;

typedef struct
{
    i32 width;
    i32 height;
} Tile_Map;

Game_Object player;

f32 gravity = 0;

bool initialized = false;

void game_update(Bitmap screen, Input input)
{
    Tile_Map tileMap = {
        8,
        4,
    };

    //only one time
    if (!initialized)
    {
        initialized = true;

        tile_map = (Tile *)malloc(sizeof(Tile) * tileMap.width * tileMap.height);

        char tile_string[] =
            "       #"
            "#  #    "
            "#      #"
            "#####  #";

        //start at last row

        for (i32 y = 0; y < tileMap.height; y++)
        {
            for (i32 x = 0; x < tileMap.width; x++)
            {
                i32 index = y * tileMap.width + x;
                Tile_Type type = Tile_Type_NONE;
                if (tile_string[(tileMap.height - y - 1) * tileMap.width + x] == '#')
                {
                    type = Tile_Type_WALL;
                }
                tile_map[index].pos = {(f32)x, (f32)y};
                tile_map[index].type = type;
            }
        }

        //addPlayer
        player = {0};
        player.pos = {100, 100};
        player.size = {64, 64};
    }

    //clearRect
    draw_rect(screen, screen.width / 2, screen.height / 2, screen.width, screen.height, 0xFF000000);

    for (i32 tileIndex = 0; tileIndex < tileMap.width * tileMap.height; tileIndex++)
    {
        u32 color = 0xFF000000;
        Tile tile = tile_map[tileIndex];
        if (tile.type == Tile_Type_WALL)
        {
            color = 0xFF00FFFF;
        }
        draw_rect(screen, tile.pos.x * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS / 2, tile.pos.y * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS / 2, TILE_SIZE_PIXELS, TILE_SIZE_PIXELS, color);
    }

    // for (i32 y = 0; y < tile_map_height; y++)
    // {
    //     for (i32 x = 0; x < tile_map_width; x++)
    //     {
    //         u32 color = 0xFF000000;
    //         Tile_Type type = tile_map[y * tile_map_width + x].type;
    //         if (type == Tile_Type_WALL)
    //         {
    //             color = 0xFF00FFFF;
    //         }
    //         draw_rect(screen, x * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS / 2, y * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS / 2, TILE_SIZE_PIXELS, TILE_SIZE_PIXELS, color);
    //     }
    // }

    //accel
    f32 accelConst = 5;
    f32 frictionConst = 0.95;

    player.speed = {(input.right.is_down - input.left.is_down) * accelConst,
                    (input.up.is_down - input.down.is_down) * accelConst};

    //friction
    player.speed *= frictionConst;

    //gravity
    player.speed.y += gravity;

    player.pos += player.speed;

    //drawPlayer
    draw_rect(screen, player.pos.x, player.pos.y, player.size.x, player.size.y, 0xFFFF0000);
}