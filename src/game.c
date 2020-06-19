#include <malloc.h>
#include "math.h"
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
    Tile_Type_BORDER,
    Tile_Type_ENTER,
    Tile_Type_EXIT,
} Tile_Type;

typedef struct
{
    V2 pos;
    Tile_Type type;
} Tile;

#define TILE_SIZE_PIXELS 64
#define CHUNK_SIZE_X 10
#define CHUNK_SIZE_Y 8
#define CHUNK_COUNT_X 4
#define CHUNK_COUNT_Y 4

Tile *tile_map = NULL;

typedef struct
{
    V2 pos;
    V2 size;
    V2 speed;
} Game_Object;

Game_Object player;

bool initialized = false;

void moveGameObject(Tile *tiles, Game_Object *gameObject, Input input)
{
    if (gameObject->speed.y > 0)
    {
        i32 foo = 0;
    }

    V2 fullSpeed = gameObject->speed;

    V2 speedLeft = fullSpeed;

    V2 speedChange = unit(gameObject->speed);

    if (length(fullSpeed) > length(speedChange))
    {
        gameObject->speed = {0, 0};
    }

    bool collisionXHappened = false;
    bool collisionYHappened = false;

    do
    {
        if (!collisionXHappened)
        {
            if (fullSpeed.x > speedChange.x && fullSpeed.x > 0)
            {
                if (speedLeft.x < speedChange.x)
                {
                    gameObject->speed.x = speedLeft.x;
                    speedLeft.x = 0;
                }
                else
                {
                    gameObject->speed.x = speedChange.x;
                    speedLeft.x -= speedChange.x;
                }
            }
            if (fullSpeed.x < speedChange.x && fullSpeed.x < 0)
            {
                if (speedLeft.x > speedChange.x)
                {
                    gameObject->speed.x = speedLeft.x;
                    speedLeft.x = 0;
                }
                else
                {
                    gameObject->speed.x = speedChange.x;
                    speedLeft.x -= speedChange.x;
                }
            }
        }
        if (!collisionYHappened)
        {
            if (fullSpeed.y > speedChange.y && fullSpeed.y > 0)
            {
                if (speedLeft.y < speedChange.y)
                {
                    gameObject->speed.y = speedLeft.y;
                    speedLeft.y = 0;
                }
                else
                {
                    gameObject->speed.y = speedChange.y;
                    speedLeft.y -= speedChange.y;
                }
            }
            if (fullSpeed.y < speedChange.y && fullSpeed.y < 0)
            {
                if (speedLeft.y > speedChange.y)
                {
                    gameObject->speed.y = speedLeft.y;
                    speedLeft.y = 0;
                }
                else
                {
                    gameObject->speed.y = speedChange.y;
                    speedLeft.y -= speedChange.y;
                }
            }
        }

        //стороны объекта
        i32 objLeft = gameObject->pos.x - (gameObject->size.x / 2);
        i32 objRight = gameObject->pos.x + (gameObject->size.x / 2);
        i32 objBottom = gameObject->pos.y - (gameObject->size.y / 2);
        i32 objTop = gameObject->pos.y + (gameObject->size.y / 2);

        V2 objTilePos = {roundf((gameObject->pos.x + gameObject->speed.x) / TILE_SIZE_PIXELS), roundf((gameObject->pos.y + gameObject->speed.y) / TILE_SIZE_PIXELS)};

        //проверка столкновений с тайлами
        for (i32 x = (i32)objTilePos.x - 2; x < (i32)objTilePos.x + 2; x++)
        {
            for (i32 y = (i32)objTilePos.y - 2; y < (i32)objTilePos.y + 2; y++)
            {
                i32 tileIndex = y * CHUNK_SIZE_X * (CHUNK_COUNT_X + 2) + x;
                Tile tile = tiles[tileIndex];
                if (tile.type == Tile_Type_WALL || tile.type == Tile_Type_BORDER)
                {
                    i32 tileLeft = tile.pos.x * TILE_SIZE_PIXELS - TILE_SIZE_PIXELS / 2;
                    i32 tileRight = tile.pos.x * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS / 2;
                    i32 tileBottom = tile.pos.y * TILE_SIZE_PIXELS - TILE_SIZE_PIXELS / 2;
                    i32 tileTop = tile.pos.y * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS / 2;

                    if (gameObject->speed.x != 0 && !collisionXHappened)
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
                            collisionXHappened = true;
                            gameObject->pos.x -= objSide - tileSide;
                        }
                    }

                    if (gameObject->speed.y != 0 && !collisionYHappened)
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
                            collisionYHappened = true;
                            gameObject->pos.y -= objSide - tileSide;
                            if (input.space.went_down && tileSide == tileTop)
                            {
                                //21
                                gameObject->speed.y += 21 * 60;
                                collisionYHappened = false;
                            }
                        }
                    }
                }
            }
        }
        gameObject->pos += gameObject->speed;
    } while (length(gameObject->speed) != 0 && length(fullSpeed) > length(speedChange));
}

void game_update(Bitmap screen, Input input)
{
    V2 mapSize = {CHUNK_SIZE_X * (CHUNK_COUNT_X + 2), CHUNK_SIZE_Y * (CHUNK_COUNT_Y + 2)};

    //only one time
    if (!initialized)
    {
        initialized = true;

        tile_map = (Tile *)malloc(sizeof(Tile) * mapSize.x * mapSize.y);

        //making chunks
        char *chunkStrings[(CHUNK_COUNT_X + 2) * (CHUNK_COUNT_Y + 2)];

        V2 enterChunkPos = {randomInt(1, CHUNK_COUNT_X), 1};
        V2 chunkPos = enterChunkPos;
        V2 endChunkPos = {randomInt(1, CHUNK_COUNT_X), CHUNK_COUNT_Y};

        //заполняем чанки
        for (i32 y = 0; y < CHUNK_COUNT_Y + 2; y++)
        {
            for (i32 x = 0; x < CHUNK_COUNT_X + 2; x++)
            {
                if (y == 0 || y == CHUNK_COUNT_Y + 1 || x == 0 || x == CHUNK_COUNT_Y + 1)
                {
                    //чанк-граница
                    chunkStrings[y * (CHUNK_COUNT_X + 2) + x] =
                        "=========="
                        "=========="
                        "=========="
                        "=========="
                        "=========="
                        "=========="
                        "=========="
                        "==========";
                }
                else
                {
                    //чанк-наполнитель
                    chunkStrings[y * (CHUNK_COUNT_X + 2) + x] =
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

        i8 direction = randomInt(-1, 1);
        if ((direction == 1 && chunkPos.x == CHUNK_COUNT_X) || (direction == -1 && chunkPos.x == 1))
        {
            direction = -direction;
        }

        while (chunkPos != endChunkPos)
        {
            if ((randomFloat(0, 1) < 0.3 || direction == 0) && chunkPos.y < CHUNK_COUNT_Y)
            {
                if (chunkPos == enterChunkPos)
                {
                    //чанк-вход с проходом вниз
                    chunkStrings[(i32)(chunkPos.y * (CHUNK_COUNT_X + 2) + chunkPos.x)] =
                        "#      #  "
                        "#         "
                        "          "
                        "          "
                        "     N    "
                        "    ###   "
                        " #        "
                        "##### ####";
                }
                else
                {
                    //чанк-проход вниз
                    chunkStrings[(i32)(chunkPos.y * (CHUNK_COUNT_X + 2) + chunkPos.x)] =
                        "          "
                        "          "
                        "      ####"
                        "##        "
                        "   #      "
                        "   #######"
                        " #     ###"
                        "##### ####";
                }
                chunkPos.y++;

                i32 chance = randomInt(0, 1);
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
                if ((direction == 1 && chunkPos.x == CHUNK_COUNT_X) || (direction == -1 && chunkPos.x == 1))
                {
                    direction = 0;
                }

                if (chunkPos.y == CHUNK_COUNT_Y)
                {
                    direction = endChunkPos.x - chunkPos.x;
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
                    if (chunkPos == enterChunkPos)
                    {
                        //чанк-вход
                        chunkStrings[(i32)(chunkPos.y * (CHUNK_COUNT_X + 2) + chunkPos.x)] =
                            "          "
                            "          "
                            "    N     "
                            "   ####   "
                            "   #####  "
                            "  ####### "
                            "   ###### "
                            "##########";
                    }
                    else
                    {
                        //обычный чанк
                        chunkStrings[(i32)(chunkPos.y * (CHUNK_COUNT_X + 2) + chunkPos.x)] =
                            "          "
                            " #######  "
                            "          "
                            "         #"
                            "          "
                            "   ###    "
                            "#      ###"
                            "##########";
                    }
                    chunkPos.x += direction;
                }
            }
            //чанк-выход
            if (chunkPos == endChunkPos)
            {
                chunkStrings[(i32)(chunkPos.y * (CHUNK_COUNT_X + 2) + chunkPos.x)] =
                    "          "
                    " #        "
                    "#         "
                    "         #"
                    "          "
                    "     X    "
                    "  ######  "
                    " ######## ";
            }
        }

        char *chunk_string;

        //start at last row
        for (i32 chunkIndexY = 0; chunkIndexY < CHUNK_COUNT_Y + 2; chunkIndexY++)
        {
            for (i32 chunkIndexX = 0; chunkIndexX < CHUNK_COUNT_X + 2; chunkIndexX++)
            {
                chunk_string = chunkStrings[(CHUNK_COUNT_Y + 2 - chunkIndexY - 1) * (CHUNK_COUNT_X + 2) + chunkIndexX];
                for (i32 y = 0; y < CHUNK_SIZE_Y; y++)
                {
                    for (i32 x = 0; x < CHUNK_SIZE_X; x++)
                    {
                        i32 tileX = x + chunkIndexX * CHUNK_SIZE_X;
                        i32 tileY = y + chunkIndexY * CHUNK_SIZE_Y;
                        i32 index = tileY * mapSize.x + tileX;
                        Tile_Type type = Tile_Type_NONE;
                        if (chunk_string[(CHUNK_SIZE_Y - y - 1) * CHUNK_SIZE_X + x] == '#')
                        {
                            type = Tile_Type_WALL;
                        }
                        if (chunk_string[(CHUNK_SIZE_Y - y - 1) * CHUNK_SIZE_X + x] == 'N')
                        {
                            type = Tile_Type_ENTER;
                            //addPlayer
                            player = {0};
                            player.pos = {(f32)tileX * TILE_SIZE_PIXELS,
                                          (f32)tileY * TILE_SIZE_PIXELS};
                            player.size = {42, 42};
                        }
                        if (chunk_string[(CHUNK_SIZE_Y - y - 1) * CHUNK_SIZE_X + x] == 'X')
                        {
                            type = Tile_Type_EXIT;
                        }
                        if (chunk_string[(CHUNK_SIZE_Y - y - 1) * CHUNK_SIZE_X + x] == '=')
                        {
                            type = Tile_Type_BORDER;
                        }
                        tile_map[index].pos = {(f32)tileX, (f32)tileY};
                        tile_map[index].type = type;
                    }
                }
            }
        }
    }

    //clearRect
    draw_rect(screen, camera, camera.pos.x, camera.pos.y, screen.width + 5, screen.height + 5, 0xFF000000);

    //accel
    //0.95
    f32 accelConst = 2;
    f32 frictionConst = 0.95;
    f32 gravity = -0.75;
    //-0.75

    player.speed += {(input.right.is_down - input.left.is_down) * accelConst, (input.up.is_down - input.down.is_down) * accelConst};

    //friction
    player.speed *= frictionConst;

    //gravity
    player.speed.y += gravity;

    moveGameObject(tile_map, &player, input);

    camera.pos = player.pos;

    if (!(player.pos.x - screen.width / 2 > -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * CHUNK_SIZE_X - 300))
    {
        camera.pos.x = -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * CHUNK_SIZE_X - 300 + screen.width / 2;
    }

    if (!(player.pos.x + screen.width / 2 < -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * (CHUNK_COUNT_X + 1) * CHUNK_SIZE_X + 300))
    {
        camera.pos.x = -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * (CHUNK_COUNT_X + 1) * CHUNK_SIZE_X + 300 - screen.width / 2;
    }

    if (!(player.pos.y - screen.height / 2 > -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * CHUNK_SIZE_Y - 240))
    {
        camera.pos.y = -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * CHUNK_SIZE_Y - 240 + screen.height / 2;
    }

    if (!(player.pos.y + screen.height / 2 < -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * (CHUNK_COUNT_Y + 1) * CHUNK_SIZE_Y + 240))
    {
        camera.pos.y = -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * (CHUNK_COUNT_Y + 1) * CHUNK_SIZE_Y + 240 - screen.height / 2;
    }

    //drawTile
    for (i32 tileIndex = 0; tileIndex < mapSize.x * mapSize.y; tileIndex++)
    {
        u32 color = 0xFF000000;
        Tile tile = tile_map[tileIndex];
        if (tile.type == Tile_Type_WALL)
        {
            color = 0xFFFFFF00;
        }
        if (tile.type == Tile_Type_ENTER || tile.type == Tile_Type_EXIT)
        {
            color = 0xFFFF0000;
        }
        if (tile.type == Tile_Type_BORDER)
        {
            color = 0xFF0000FF;
        }
        draw_rect(screen, camera, tile.pos.x * TILE_SIZE_PIXELS, tile.pos.y * TILE_SIZE_PIXELS, TILE_SIZE_PIXELS, TILE_SIZE_PIXELS, color);
    }

    //drawPlayer
    draw_rect(screen, camera, player.pos.x, player.pos.y, player.size.x, player.size.y, 0xFFFF0000);
}