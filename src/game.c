#include <malloc.h>
#include "game_math.c"

f64 target_time_per_frame = 1.0f / 60.0f;
f32 dt = (f32)target_time_per_frame;

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

    if (!(bottom > screen.height || top < 0 || right < 0 || left > screen.width))
    {
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
}

void draw_bitmap(Bitmap screen, Camera camera, i32 pos_x, i32 pos_y, Bitmap bitmap)
{
    pos_x += screen.width / 2 - camera.pos.x;
    pos_x += screen.height / 2 - camera.pos.y;
    i32 left = pos_x - bitmap.width / 2;
    i32 right = pos_x + bitmap.width / 2;
    i32 bottom = pos_y - bitmap.height / 2;
    i32 top = pos_y + bitmap.height / 2;

    if (!(bottom > screen.height || top < 0 || right < 0 || left > screen.width))
    {
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

#define TILE_SIZE_PIXELS 42

Tile *tile_map = NULL;

typedef struct
{
    V2 chunkSize;
    V2 chunkCount;
} Tile_Map;

typedef struct
{
    V2 pos;
    V2 size;
    V2 speed;
} Game_Object;

Game_Object player;

bool initialized = false;

void moveGameObject(Tile *tiles, Tile_Map tileMap, Game_Object *gameObject, Input input)
{
    //стороны объекта
    i32 objLeft = gameObject->pos.x - (gameObject->size.x / 2);
    i32 objRight = gameObject->pos.x + (gameObject->size.x / 2);
    i32 objBottom = gameObject->pos.y - (gameObject->size.y / 2);
    i32 objTop = gameObject->pos.y + (gameObject->size.y / 2);

    V2 objTilePos = {roundf(gameObject->pos.x / TILE_SIZE_PIXELS),
                     roundf(gameObject->pos.y / TILE_SIZE_PIXELS)};

    //проверка столкновений
    for (i32 x = (i32)objTilePos.x - 2; x < (i32)objTilePos.x + 2; x++)
    {
        for (i32 y = (i32)objTilePos.y - 2; y < (i32)objTilePos.y + 2; y++)
        {
            i32 tileIndex = y * tileMap.chunkSize.x * tileMap.chunkCount.x + x;
            Tile tile = tiles[tileIndex];
            if (tile.type && tile.type == Tile_Type_WALL)
            {
                i32 tileLeft = tile.pos.x * TILE_SIZE_PIXELS - TILE_SIZE_PIXELS / 2;
                i32 tileRight = tile.pos.x * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS / 2;
                i32 tileBottom = tile.pos.y * TILE_SIZE_PIXELS - TILE_SIZE_PIXELS / 2;
                i32 tileTop = tile.pos.y * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS / 2;

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
                        !(objRight + gameObject->speed.x * dt <= tileLeft ||
                          objLeft + gameObject->speed.x * dt >= tileRight ||
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
                        !(objTop + gameObject->speed.y * dt <= tileBottom ||
                          objBottom + gameObject->speed.y * dt >= tileTop ||
                          objRight <= tileLeft ||
                          objLeft >= tileRight))
                    {
                        gameObject->speed.y = 0;
                        gameObject->pos.y -= objSide - tileSide;
                        if (input.space.is_down && tileSide == tileTop)
                        {
                            gameObject->speed.y += 21 * 60 * dt * 60;
                        }
                    }
                }
            }
        }
    }
    gameObject->pos += gameObject->speed * dt;
}

void game_update(Bitmap screen, Input input)
{
    Tile_Map tileMap = {
        {20, 16},
        {4, 4}};

    V2 mapSize = {tileMap.chunkSize.x * tileMap.chunkCount.x, tileMap.chunkSize.y * tileMap.chunkCount.y};

    //only one time
    if (!initialized)
    {
        initialized = true;

        tile_map = (Tile *)malloc(sizeof(Tile) * mapSize.x * mapSize.y);

        char tile_string[] =
            "#                   "
            "  #                 "
            "      #             "
            "                    "
            "                    "
            "                    "
            "        #           "
            "                    "
            "                    "
            "         #    #     "
            "                    "
            "            #       "
            "                    "
            "               #    "
            "                    "
            "### ### # ## ######";

        //start at last row
        for (i32 chunkIndexY = 0; chunkIndexY < tileMap.chunkCount.y; chunkIndexY++)
        {
            for (i32 chunkIndexX = 0; chunkIndexX < tileMap.chunkCount.x; chunkIndexX++)
            {
                for (i32 y = 0; y < tileMap.chunkSize.y; y++)
                {
                    for (i32 x = 0; x < tileMap.chunkSize.x; x++)
                    {
                        i32 tileX = x + chunkIndexX * tileMap.chunkSize.x;
                        i32 tileY = y + chunkIndexY * tileMap.chunkSize.y;
                        i32 index = tileY * mapSize.x + tileX;
                        Tile_Type type = Tile_Type_NONE;
                        if (tile_string[((i32)tileMap.chunkSize.y - y - 1) * (i32)tileMap.chunkSize.x + x] == '#')
                        {
                            type = Tile_Type_WALL;
                        }
                        tile_map[index].pos = {(f32)tileX, (f32)tileY};
                        tile_map[index].type = type;
                    }
                }
            }
        }

        //addPlayer
        player = {0};
        player.pos = {100, 100};
        player.size = {32, 32};
    }

    //clearRect
    draw_rect(screen, camera, camera.pos.x, camera.pos.y, screen.width + 5, screen.height + 5, 0xFF000000);

    //accel
    f32 accelConst = 0.5 * 60 * 60;
    f32 frictionConst = 0.95;
    f32 gravity = -0.7 * 60 * 60;

    player.speed += {(input.right.is_down - input.left.is_down) * accelConst * dt, 0};

    //friction
    player.speed *= frictionConst;

    //gravity
    player.speed.y += gravity * dt;

    moveGameObject(tile_map, tileMap, &player, input);

    camera.pos = player.pos;

    //drawTile
    for (i32 tileIndex = 0; tileIndex < mapSize.x * mapSize.y; tileIndex++)
    {
        u32 color = 0xFF000000;
        Tile tile = tile_map[tileIndex];
        if (tile.type == Tile_Type_WALL)
        {
            color = 0xFF00FFFF;
        }
        draw_rect(screen, camera, tile.pos.x * TILE_SIZE_PIXELS, tile.pos.y * TILE_SIZE_PIXELS, TILE_SIZE_PIXELS, TILE_SIZE_PIXELS, color);
    }

    //drawPlayer
    draw_rect(screen, camera, player.pos.x, player.pos.y, player.size.x, player.size.y, 0xFFFF0000);
}