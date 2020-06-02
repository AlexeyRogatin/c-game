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

//camera
typedef struct
{
    V2 pos;
} Camera;

Camera camera = {{0, 0}};

//drawing
void draw_rect(Bitmap screen, Camera camera, i32 x, i32 y, i32 width, i32 height, i32 color)
{
    x += screen.width / 2 - camera.pos.x;
    y += screen.height / 2 - camera.pos.y;
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

void draw_bitmap(Bitmap screen, Camera camera, i32 pos_x, i32 pos_y, Bitmap bitmap)
{
    pos_x += screen.width / 2 - camera.pos.x;
    pos_x += screen.height / 2 - camera.pos.y;
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

bool initialized = false;

void moveGameObject(Tile *tiles, Tile_Map tileMap, Game_Object *gameObject, Input input)
{
    //стороны объекта
    i32 objLeft = gameObject->pos.x - (gameObject->size.x / 2);
    i32 objRight = gameObject->pos.x + (gameObject->size.x / 2);
    i32 objBottom = gameObject->pos.y - (gameObject->size.y / 2);
    i32 objTop = gameObject->pos.y + (gameObject->size.y / 2);

    //проверка столкновений
    for (i32 tileIndex = 0; tileIndex < tileMap.width * tileMap.height; tileIndex++)
    {
        Tile tile = tiles[tileIndex];
        if (tile.type == Tile_Type_WALL)
        {
            i32 tileLeft = tile.pos.x * TILE_SIZE_PIXELS;
            i32 tileRight = tile.pos.x * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS;
            i32 tileBottom = tile.pos.y * TILE_SIZE_PIXELS;
            i32 tileTop = tile.pos.y * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS;

            if (gameObject->speed.x != 0)
            {
                i32 objSide;
                i32 tileSide;
                if (gameObject->speed.x > 0)
                {
                    objSide = objRight;
                    tileSide = tileLeft;
                }
                else
                {
                    objSide = objLeft;
                    tileSide = tileRight;
                }

                if (
                    !(objRight + gameObject->speed.x <= tileLeft ||
                      objLeft + gameObject->speed.x >= tileRight ||
                      objTop <= tileBottom ||
                      objBottom >= tileTop))
                {
                    gameObject->speed.x = 0;
                    gameObject->pos.x -= objSide - tileSide;
                }
            }

            if (gameObject->speed.y != 0)
            {
                i32 objSide;
                i32 tileSide;
                if (gameObject->speed.y > 0)
                {
                    objSide = objTop;
                    tileSide = tileBottom;
                }
                else
                {
                    objSide = objBottom;
                    tileSide = tileTop;
                }

                if (
                    !(objTop + gameObject->speed.y <= tileBottom ||
                      objBottom + gameObject->speed.y >= tileTop ||
                      objRight <= tileLeft ||
                      objLeft >= tileRight))
                {
                    gameObject->speed.y = 0;
                    gameObject->pos.y -= objSide - tileSide;
                    if (input.space.is_down && tileSide == tileTop)
                    {
                        gameObject->speed.y += 21;
                    }
                }
            }
        }
    }
    gameObject->pos += gameObject->speed;
}

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
            " #     #"
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
        player.size = {32, 32};
    }

    //clearRect
    draw_rect(screen, camera, screen.width / 2, screen.height / 2, screen.width, screen.height, 0xFF000000);

    //accel
    f32 accelConst = 0.5;
    f32 frictionConst = 0.95;
    f32 gravity = -0.7;

    player.speed += {(input.right.is_down - input.left.is_down) * accelConst,
                     0};

    //friction
    player.speed *= frictionConst;

    //gravity
    player.speed.y += gravity;

    moveGameObject(tile_map, tileMap, &player, input);

    camera.pos = player.pos;

    //drawTile
    for (i32 tileIndex = 0; tileIndex < tileMap.width * tileMap.height; tileIndex++)
    {
        u32 color = 0xFF000000;
        Tile tile = tile_map[tileIndex];
        if (tile.type == Tile_Type_WALL)
        {
            color = 0xFF00FFFF;
        }
        draw_rect(screen, camera, tile.pos.x * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS / 2, tile.pos.y * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS / 2, TILE_SIZE_PIXELS, TILE_SIZE_PIXELS, color);
    }

    //drawPlayer
    draw_rect(screen, camera, player.pos.x, player.pos.y, player.size.x, player.size.y, 0xFFFF0000);
}