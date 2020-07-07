#include <malloc.h>
#include "math.h"
#include "game_math.c"

//fps
f64 target_time_per_frame = 1.0f / 60.0f;
f32 dt = (f32)target_time_per_frame;

//ввод
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

//получение картинки
typedef struct
{
    u32 *pixels;
    i32 width;
    i32 height;
} Bitmap;

Bitmap win32_read_bmp(char *file_name);

//камера
typedef struct
{
    V2 pos;
} Camera;

Camera camera = {{0, 0}};

typedef union {
    struct
    {
        u8 b;
        u8 g;
        u8 r;
        u8 a;
    };
    u32 argb;
} ARGB;

//drawing
void draw_rect(Bitmap screen, i32 x, i32 y, i32 width, i32 height, u32 color)
{
    x += screen.width / 2 - camera.pos.x;
    y += screen.height / 2 - camera.pos.y;
    i32 left = x - width / 2;
    i32 right = x + width / 2;
    i32 bottom = y - height / 2;
    i32 top = y + height / 2;

    ARGB newColor;
    newColor.argb = color;
    f32 alpha = (f32)newColor.a / 0xFF;

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
                ARGB color;
                color.argb = *(screen.pixels + y * screen.width + x);

                color.r = newColor.r * alpha + color.r * (1 - alpha);
                color.g = newColor.g * alpha + color.g * (1 - alpha);
                color.b = newColor.b * alpha + color.b * (1 - alpha);
                color.a = 0xFF;

                *(screen.pixels + y * screen.width + x) = color.argb;
            }
        }
    }
}

void draw_bitmap(Bitmap screen, i32 pos_x, i32 pos_y, Bitmap bitmap)
{
    pos_x += screen.width / 2 - camera.pos.x;
    pos_y += screen.height / 2 - camera.pos.y;
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

    for (i32 y = 0; y < height; y++)
    {
        for (i32 x = 0; x < width; x++)
        {
            i32 screen_x = left + x;
            i32 screen_y = bottom + y;

            i32 texture_x = x + (left - (pos_x - bitmap.width / 2));
            i32 texure_y = y + (bottom - (pos_y - bitmap.height / 2));

            ARGB newColor;
            newColor.argb = bitmap.pixels[bitmap.width * texure_y + texture_x];
            f32 alpha = (f32)newColor.a / 0xFF;

            ARGB color;
            color.argb = screen.pixels[screen_y * screen.width + screen_x];

            color.r = newColor.r * alpha + color.r * (1 - alpha);
            color.g = newColor.g * alpha + color.g * (1 - alpha);
            color.b = newColor.b * alpha + color.b * (1 - alpha);
            color.a = 0xFF;

            screen.pixels[screen_y * screen.width + screen_x] = color.argb;
        }
    }
}

//тайлы
typedef enum
{
    Tile_Type_NONE,
    Tile_Type_WALL,
    Tile_Type_BORDER,
    Tile_Type_ENTER,
    Tile_Type_EXIT,
} Tile_Type;

#define TILE_SIZE_PIXELS 64
#define CHUNK_SIZE_X 10
#define CHUNK_SIZE_Y 8
#define CHUNK_COUNT_X 4
#define CHUNK_COUNT_Y 4

Tile_Type *tile_map = NULL;

V2 getTilePos(i32 index)
{
    //index = y * (CHUNK_SIZE_X * (CHUNK_COUNT_X + 2)) + x;
    f32 y = floor(index / (CHUNK_SIZE_X * (CHUNK_COUNT_X + 2)));
    f32 x = index - y * (CHUNK_SIZE_X * (CHUNK_COUNT_X + 2));
    V2 result = {x, y};
    return result;
}

//сущности
typedef enum
{
    PLAYER,
} Game_Object_Type;

typedef enum
{
    RIGHT,
    LEFT,
} Direction;

typedef enum
{
    IDLE,
    MOOVING,
    FALLING,
    GOT_HIT,
    HANGING,
} Condition;

typedef struct
{
    Game_Object_Type type;

    bool exists;

    V2 pos;
    V2 size;
    V2 speed;

    bool goLeft;
    bool goRight;
    bool jump;

    Bitmap sprite;
    i32 movedThroughPixels;
    Direction lookingDirection;
    Condition condition;
} Game_Object;

i32 gameObjectCount = 0;
Game_Object gameObjects[512];

Game_Object *addGameObject(Game_Object_Type type, V2 pos)
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

        NULL,
        0,
        1,
        IDLE,
    };

    if (type == PLAYER)
    {
        gameObject.size = {34, 52};
    }

    i32 slotIndex = gameObjectCount;

    for (i32 objectIndex = 0; objectIndex < gameObjectCount; objectIndex++)
    {
        if (gameObjects[objectIndex].exists == false)
        {
            slotIndex = objectIndex;
            break;
        }
    }

    if (slotIndex == gameObjectCount)
    {
        gameObjectCount++;
    }

    gameObjects[slotIndex] = gameObject;

    return &gameObjects[slotIndex];
}

//вставляем картинки
Bitmap imgPlayerRightIdle;
Bitmap imgPlayerRightHit;
Bitmap imgPlayerRightJump;
Bitmap imgPlayerRightHanging;
Bitmap imgPlayerRightStep[8];
Bitmap imgPlayerLeftIdle;
Bitmap imgPlayerLeftHit;
Bitmap imgPlayerLeftJump;
Bitmap imgPlayerLeftStep[8];
Bitmap imgPlayerLeftHanging;

bool initialized = false;

//движение сущностей
void moveGameObject(Tile_Type *tiles, Game_Object *gameObject)
{
    Game_Object *ourObject = gameObject;

    f32 speedLength = length(ourObject->speed);

    if (speedLength)
    {
        bool collisionXHappened = false;
        bool collisionYHappened = false;

        i32 ratioIndex = 1;
        while (speedLength / ratioIndex > TILE_SIZE_PIXELS)
        {
            ratioIndex++;
        }

        f32 ratioChange = (speedLength / ratioIndex) / speedLength;

        f32 ratio = ratioChange;
        V2 speedUnit = unit(ourObject->speed);

        while (ratio <= 1)
        {
            V2 speedPart = speedUnit * ratio * speedLength;

            //стороны объекта
            i32 objLeft = ourObject->pos.x - (ourObject->size.x / 2);
            i32 objRight = ourObject->pos.x + (ourObject->size.x / 2);
            i32 objBottom = ourObject->pos.y - (ourObject->size.y / 2);
            i32 objTop = ourObject->pos.y + (ourObject->size.y / 2);

            V2 objTilePos = {roundf((ourObject->pos.x + speedPart.x) / TILE_SIZE_PIXELS),
                             roundf((ourObject->pos.y + speedPart.y) / TILE_SIZE_PIXELS)};

            //проверка столкновений с тайлами
            for (i32 x = (i32)objTilePos.x - 2; x < (i32)objTilePos.x + 2; x++)
            {
                for (i32 y = (i32)objTilePos.y - 2; y < (i32)objTilePos.y + 2; y++)
                {
                    i32 tileIndex = y * CHUNK_SIZE_X * (CHUNK_COUNT_X + 2) + x;
                    Tile_Type tile = tiles[tileIndex];
                    V2 tilePos = getTilePos(tileIndex);
                    if (tile && (tile == Tile_Type_WALL || tile == Tile_Type_BORDER))
                    {
                        i32 tileLeft = tilePos.x * TILE_SIZE_PIXELS - TILE_SIZE_PIXELS / 2;
                        i32 tileRight = tilePos.x * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS / 2;
                        i32 tileBottom = tilePos.y * TILE_SIZE_PIXELS - TILE_SIZE_PIXELS / 2;
                        i32 tileTop = tilePos.y * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS / 2;

                        i32 objSide;
                        i32 tileSide;

                        if (speedPart.y != 0)
                        {
                            if (speedPart.y > 0)
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
                                !(objTop + speedPart.y <= tileBottom ||
                                  objBottom + speedPart.y >= tileTop ||
                                  objRight <= tileLeft ||
                                  objLeft >= tileRight))
                            {
                                ourObject->speed.y = 0;
                                speedPart.y = 0;
                                speedUnit.y = 0;

                                ourObject->pos.y -= objSide - tileSide;

                                collisionYHappened = true;
                            }
                        }

                        if (speedPart.x != 0)
                        {
                            if (speedPart.x > 0)
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
                                !(objRight + speedPart.x < tileLeft ||
                                  objLeft + speedPart.x > tileRight ||
                                  objTop <= tileBottom ||
                                  objBottom >= tileTop))
                            {

                                ourObject->speed.x = 0;
                                speedPart.x = 0;
                                speedUnit.x = 0;
                                ourObject->pos.x -= objSide - tileSide;
                                collisionXHappened = true;

                                if (ourObject->type == PLAYER && !collisionYHappened && ourObject->speed.y < 0 && ourObject->pos.y >= tilePos.y * TILE_SIZE_PIXELS && ourObject->pos.y + ourObject->speed.y <= tilePos.y * TILE_SIZE_PIXELS)
                                {
                                    i32 upTileIndex = (y + 1) * CHUNK_SIZE_X * (CHUNK_COUNT_X + 2) + x;
                                    if (tile_map[upTileIndex] == Tile_Type_NONE)
                                    {
                                        ourObject->condition = HANGING;
                                        ourObject->speed.y = 0;
                                        ourObject->pos.y = tilePos.y * TILE_SIZE_PIXELS;
                                        collisionYHappened = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            ratio += ratioChange;
        }
        gameObject->pos = ourObject->pos;
        gameObject->pos += ourObject->speed;
        gameObject->condition = ourObject->condition;

        if (!collisionYHappened && gameObject->condition != HANGING)
        {
            gameObject->condition = FALLING;
            gameObject->size.y = 49;
        }
    }
}

void updateGameObject(Game_Object *gameObject, Input input, Bitmap screen)
{
    if (gameObject->type == PLAYER)
    {
        gameObject->size.y = 52;

        //движение игрока
        gameObject->goLeft = input.left.is_down;
        gameObject->goRight = input.right.is_down;
        gameObject->jump = input.space.went_down;

        //константы скорости
        //0.75
        f32 accelConst = 1.85;
        //0.95
        f32 frictionConst = 0.90;
        //-0.75
        f32 gravity = -1.80;

        gameObject->speed += {(gameObject->goRight - gameObject->goLeft) * accelConst, 0};

        //трение
        gameObject->speed *= frictionConst;

        //гравитация
        if (gameObject->condition != HANGING)
        {
            gameObject->speed.y += gravity;
        }

        //спрайты персонажа

        //направление
        if (gameObject->speed.x > 0 && !(gameObject->condition == HANGING && gameObject->lookingDirection == RIGHT))
        {
            gameObject->lookingDirection = RIGHT;
        }
        if (gameObject->speed.x < 0 && !(gameObject->condition == HANGING && gameObject->lookingDirection == LEFT))
        {
            gameObject->lookingDirection = LEFT;
        }

        if (gameObject->condition == IDLE)
        {
            if (gameObject->lookingDirection == RIGHT)
            {
                gameObject->sprite = imgPlayerRightIdle;
            }
            if (gameObject->lookingDirection == LEFT)
            {
                gameObject->sprite = imgPlayerLeftIdle;
            }
        }

        if (gameObject->condition == MOOVING)
        {
            if (gameObject->lookingDirection == RIGHT)
            {
                i8 step = (i32)floor(gameObject->movedThroughPixels / 15);
                while (step > 7)
                {
                    step -= 8;
                }
                while (step < 0)
                {
                    step += 8;
                }
                gameObject->sprite = imgPlayerRightStep[step];
            }

            if (gameObject->lookingDirection == LEFT)
            {
                i8 step = (i32)floor(gameObject->movedThroughPixels / 15);
                while (step > 7)
                {
                    step -= 8;
                }
                while (step < 0)
                {
                    step += 8;
                }
                gameObject->sprite = imgPlayerLeftStep[7 - step];
            }
        }

        //столкновения и движение
        moveGameObject(tile_map, gameObject);

        //состояние нормальное
        if (gameObject->condition != HANGING)
        {
            gameObject->condition = IDLE;
        }

        //состояние движится
        if (gameObject->speed.x > 1 || gameObject->speed.x < -1)
        {

            gameObject->movedThroughPixels += gameObject->speed.x;

            gameObject->condition = MOOVING;
        }
        else
        {
            gameObject->movedThroughPixels = 0;
        }

        if (gameObject->condition == GOT_HIT)
        {
            if (gameObject->lookingDirection == RIGHT)
            {
                gameObject->sprite = imgPlayerRightHit;
            }
            if (gameObject->lookingDirection == LEFT)
            {
                gameObject->sprite = imgPlayerLeftHit;
            }
        }

        if (gameObject->condition == FALLING)
        {
            if (gameObject->lookingDirection == RIGHT)
            {
                gameObject->sprite = imgPlayerRightJump;
            }
            if (gameObject->lookingDirection == LEFT)
            {
                gameObject->sprite = imgPlayerLeftJump;
            }
        }

        if (gameObject->condition == HANGING)
        {
            if (gameObject->lookingDirection == RIGHT)
            {
                gameObject->sprite = imgPlayerRightHanging;
            }
            if (gameObject->lookingDirection == LEFT)
            {
                gameObject->sprite = imgPlayerLeftHanging;
            }
        }

        if (input.space.went_down && gameObject->condition != FALLING)
        {
            gameObject->speed.y += 27;
            gameObject->condition = FALLING;
        }

        //камера
        camera.pos = gameObject->pos;

        if (!(gameObject->pos.x - screen.width / 2 > -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * CHUNK_SIZE_X - 300))
        {
            camera.pos.x = -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * CHUNK_SIZE_X - 300 + screen.width / 2;
        }

        if (!(gameObject->pos.x + screen.width / 2 < -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * (CHUNK_COUNT_X + 1) * CHUNK_SIZE_X + 300))
        {
            camera.pos.x = -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * (CHUNK_COUNT_X + 1) * CHUNK_SIZE_X + 300 - screen.width / 2;
        }

        if (!(gameObject->pos.y - screen.height / 2 > -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * CHUNK_SIZE_Y - 240))
        {
            camera.pos.y = -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * CHUNK_SIZE_Y - 240 + screen.height / 2;
        }

        if (!(gameObject->pos.y + screen.height / 2 < -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * (CHUNK_COUNT_Y + 1) * CHUNK_SIZE_Y + 240))
        {
            camera.pos.y = -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * (CHUNK_COUNT_Y + 1) * CHUNK_SIZE_Y + 240 - screen.height / 2;
        }

        //drawPlayer
        draw_bitmap(screen, gameObject->pos.x, gameObject->pos.y + 6, gameObject->sprite);
    }
}

void game_update(Bitmap screen, Input input)
{
    V2 mapSize = {CHUNK_SIZE_X * (CHUNK_COUNT_X + 2), CHUNK_SIZE_Y * (CHUNK_COUNT_Y + 2)};

    //выполняется один раз
    if (!initialized)
    {
        initialized = true;

        //загружаем картинки
        imgPlayerRightIdle = win32_read_bmp("../data/lexaRightIdle.bmp");
        imgPlayerRightHit = win32_read_bmp("../data/lexaRightHit.bmp");
        imgPlayerRightJump = win32_read_bmp("../data/lexaRightJump.bmp");
        imgPlayerRightHanging = win32_read_bmp("../data/lexaRightWall.bmp");
        imgPlayerRightStep[0] = win32_read_bmp("../data/lexaRightStep1.bmp");
        imgPlayerRightStep[1] = win32_read_bmp("../data/lexaRightStep2.bmp");
        imgPlayerRightStep[2] = win32_read_bmp("../data/lexaRightStep3.bmp");
        imgPlayerRightStep[3] = win32_read_bmp("../data/lexaRightStep4.bmp");
        imgPlayerRightStep[4] = win32_read_bmp("../data/lexaRightStep5.bmp");
        imgPlayerRightStep[5] = win32_read_bmp("../data/lexaRightStep6.bmp");
        imgPlayerRightStep[6] = win32_read_bmp("../data/lexaRightStep7.bmp");
        imgPlayerRightStep[7] = win32_read_bmp("../data/lexaRightStep8.bmp");
        imgPlayerLeftIdle = win32_read_bmp("../data/lexaLeftIdle.bmp");
        imgPlayerLeftHit = win32_read_bmp("../data/lexaLeftHit.bmp");
        imgPlayerLeftJump = win32_read_bmp("../data/lexaLeftJump.bmp");
        imgPlayerLeftHanging = win32_read_bmp("../data/lexaLeftWall.bmp");
        imgPlayerLeftStep[0] = win32_read_bmp("../data/lexaLeftStep1.bmp");
        imgPlayerLeftStep[1] = win32_read_bmp("../data/lexaLeftStep2.bmp");
        imgPlayerLeftStep[2] = win32_read_bmp("../data/lexaLeftStep3.bmp");
        imgPlayerLeftStep[3] = win32_read_bmp("../data/lexaLeftStep4.bmp");
        imgPlayerLeftStep[4] = win32_read_bmp("../data/lexaLeftStep5.bmp");
        imgPlayerLeftStep[5] = win32_read_bmp("../data/lexaLeftStep6.bmp");
        imgPlayerLeftStep[6] = win32_read_bmp("../data/lexaLeftStep7.bmp");
        imgPlayerLeftStep[7] = win32_read_bmp("../data/lexaLeftStep8.bmp");

        tile_map = (Tile_Type *)malloc(sizeof(Tile_Type) * mapSize.x * mapSize.y);

        //making chunks
        char *chunkStrings[(CHUNK_COUNT_X + 2) * (CHUNK_COUNT_Y + 2)];

        //путь через тайлы
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

        //заполняем чанки
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
                            addGameObject(PLAYER, {(f32)tileX * TILE_SIZE_PIXELS, (f32)tileY * TILE_SIZE_PIXELS});
                        }
                        if (chunk_string[(CHUNK_SIZE_Y - y - 1) * CHUNK_SIZE_X + x] == 'X')
                        {
                            type = Tile_Type_EXIT;
                        }
                        if (chunk_string[(CHUNK_SIZE_Y - y - 1) * CHUNK_SIZE_X + x] == '=')
                        {
                            type = Tile_Type_BORDER;
                        }
                        tile_map[index] = type;
                    }
                }
            }
        }
    }

    //очистка экрана
    draw_rect(screen, camera.pos.x, camera.pos.y, screen.width + 5, screen.height + 5, 0xFFFF8800);

    //обновление сущностей
    for (i32 objectIndex = 0; objectIndex < gameObjectCount; objectIndex++)
    {
        if (gameObjects[objectIndex].exists == true)
        {
            updateGameObject(&gameObjects[objectIndex], input, screen);
        }
    }

    //drawTile
    for (i32 tileIndex = 0; tileIndex < mapSize.x * mapSize.y; tileIndex++)
    {
        u32 color = 0x00000000;
        Tile_Type tile = tile_map[tileIndex];
        V2 tilePos = getTilePos(tileIndex);
        if (tile == Tile_Type_WALL)
        {
            color = 0xFFFFFF00;
        }
        if (tile == Tile_Type_ENTER || tile == Tile_Type_EXIT)
        {
            color = 0xFFFF0000;
        }
        if (tile == Tile_Type_BORDER)
        {
            color = 0xFF0000FF;
        }
        draw_rect(screen, tilePos.x * TILE_SIZE_PIXELS, tilePos.y * TILE_SIZE_PIXELS, TILE_SIZE_PIXELS, TILE_SIZE_PIXELS, color);
    }
}