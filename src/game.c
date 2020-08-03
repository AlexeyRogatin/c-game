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

#define BUTTON_COUNT 6

typedef union {
    struct
    {
        Button left;
        Button right;
        Button up;
        Button down;
        Button space;
        Button shift;
    };
    Button buttons[BUTTON_COUNT];
} Input;

//получение картинки
typedef struct
{
    u32 *pixels;
    V2 size;
} Bitmap;

Bitmap win32_read_bmp(char *file_name);

//вставляем картинки
Bitmap imgNone = win32_read_bmp("../data/none.bmp");

Bitmap imgPlayerRightIdle = win32_read_bmp("../data/lexaRightIdle.bmp");
Bitmap imgPlayerRightCrouchIdle = win32_read_bmp("../data/lexaRightCrouchIdle.bmp");
Bitmap imgPlayerRightHit = win32_read_bmp("../data/lexaRightHit.bmp");
Bitmap imgPlayerRightJump = win32_read_bmp("../data/lexaRightJump.bmp");
Bitmap imgPlayerRightHanging = win32_read_bmp("../data/lexaRightWall.bmp");
Bitmap imgPlayerRightLookingUp = win32_read_bmp("../data/lexaRightLookingUp.bmp");

Bitmap imgPlayerRightStep[8] = {
    win32_read_bmp("../data/lexaRightStep1.bmp"),
    win32_read_bmp("../data/lexaRightStep2.bmp"),
    win32_read_bmp("../data/lexaRightStep3.bmp"),
    win32_read_bmp("../data/lexaRightStep4.bmp"),
    win32_read_bmp("../data/lexaRightStep5.bmp"),
    win32_read_bmp("../data/lexaRightStep6.bmp"),
    win32_read_bmp("../data/lexaRightStep7.bmp"),
    win32_read_bmp("../data/lexaRightStep8.bmp"),
};

Bitmap imgPlayerRightCrouchStep[8] = {
    win32_read_bmp("../data/lexaRightCrouchStep1.bmp"),
    win32_read_bmp("../data/lexaRightCrouchStep2.bmp"),
    win32_read_bmp("../data/lexaRightCrouchStep3.bmp"),
    win32_read_bmp("../data/lexaRightCrouchStep4.bmp"),
    win32_read_bmp("../data/lexaRightCrouchStep5.bmp"),
    win32_read_bmp("../data/lexaRightCrouchStep6.bmp"),
    win32_read_bmp("../data/lexaRightCrouchStep7.bmp"),
    win32_read_bmp("../data/lexaRightCrouchStep8.bmp"),
};

Bitmap imgPlayerLeftIdle = win32_read_bmp("../data/lexaLeftIdle.bmp");
Bitmap imgPlayerLeftCrouchIdle = win32_read_bmp("../data/lexaLeftCrouchIdle.bmp");
Bitmap imgPlayerLeftHit = win32_read_bmp("../data/lexaLeftHit.bmp");
Bitmap imgPlayerLeftJump = win32_read_bmp("../data/lexaLeftJump.bmp");
Bitmap imgPlayerLeftHanging = win32_read_bmp("../data/lexaLeftWall.bmp");
Bitmap imgPlayerLeftLookingUp = win32_read_bmp("../data/lexaLeftLookingUp.bmp");

Bitmap imgPlayerLeftStep[8] = {
    win32_read_bmp("../data/lexaLeftStep1.bmp"),
    win32_read_bmp("../data/lexaLeftStep2.bmp"),
    win32_read_bmp("../data/lexaLeftStep3.bmp"),
    win32_read_bmp("../data/lexaLeftStep4.bmp"),
    win32_read_bmp("../data/lexaLeftStep5.bmp"),
    win32_read_bmp("../data/lexaLeftStep6.bmp"),
    win32_read_bmp("../data/lexaLeftStep7.bmp"),
    win32_read_bmp("../data/lexaLeftStep8.bmp"),
};

Bitmap imgPlayerLeftCrouchStep[8] = {
    win32_read_bmp("../data/lexaLeftCrouchStep1.bmp"),
    win32_read_bmp("../data/lexaLeftCrouchStep2.bmp"),
    win32_read_bmp("../data/lexaLeftCrouchStep3.bmp"),
    win32_read_bmp("../data/lexaLeftCrouchStep4.bmp"),
    win32_read_bmp("../data/lexaLeftCrouchStep5.bmp"),
    win32_read_bmp("../data/lexaLeftCrouchStep6.bmp"),
    win32_read_bmp("../data/lexaLeftCrouchStep7.bmp"),
    win32_read_bmp("../data/lexaLeftCrouchStep8.bmp"),
};

Bitmap imgTilePlate = win32_read_bmp("../data/tilePlate.bmp");

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

typedef enum
{
    DRAWING_TYPE_RECT,
    DRAWING_TYPE_BITMAP,
} Drawing_type;

typedef enum
{
    LAYER_CLEAR,
    LAYER_BACKGROUND,
    LAYER_PLAYER,
    LAYER_NORMAL,
    LAYER_FORGROUND,
} Layer;

typedef struct
{
    Drawing_type type;
    V2 pos;
    V2 size;
    u32 color;
    Bitmap bitmap;
    Layer layer;
} Drawing;

Drawing draw_queue[4096];
i32 draw_queue_size = 0;

void drawItem(Bitmap screen, Drawing drawing)
{
    V2 pos = drawing.pos;
    V2 size = drawing.size;

    if (drawing.type == DRAWING_TYPE_RECT)
    {
        u32 color = drawing.color;

        pos.x += screen.size.x / 2 - camera.pos.x;
        pos.y += screen.size.y / 2 - camera.pos.y;
        i32 left = pos.x - size.x / 2;
        i32 right = pos.x + size.x / 2;
        i32 bottom = pos.y - size.y / 2;
        i32 top = pos.y + size.y / 2;

        ARGB newColor;
        newColor.argb = color;
        f32 alpha = (f32)newColor.a / 0xFF;

        if (left < 0)
            left = 0;
        if (right > screen.size.x)
            right = screen.size.x;
        if (bottom < 0)
            bottom = 0;
        if (top > screen.size.y)
            top = screen.size.y;

        for (i32 y = bottom; y < top; y++)
        {
            for (i32 x = left; x < right; x++)
            {
                ARGB color;
                color.argb = screen.pixels[y * (i32)screen.size.x + x];

                color.r = newColor.r * alpha + color.r * (1 - alpha);
                color.g = newColor.g * alpha + color.g * (1 - alpha);
                color.b = newColor.b * alpha + color.b * (1 - alpha);
                color.a = 0xFF;

                screen.pixels[y * (i32)screen.size.x + x] = color.argb;
            }
        }
    }

    if (drawing.type == DRAWING_TYPE_BITMAP)
    {
        Bitmap bitmap = drawing.bitmap;

        pos.x += screen.size.x / 2 - camera.pos.x;
        pos.y += screen.size.y / 2 - camera.pos.y;

        i32 leftOnScreen = pos.x - size.x / 2;
        i32 rightOnScreen = pos.x + size.x / 2;
        i32 bottomOnScreen = pos.y - size.y / 2;
        i32 topOnScreen = pos.y + size.y / 2;

        if (!(rightOnScreen < 0 || leftOnScreen > screen.size.x || topOnScreen < 0 || bottomOnScreen > screen.size.y))
        {

            if (leftOnScreen < 0)
                leftOnScreen = 0;
            if (rightOnScreen > screen.size.x)
                rightOnScreen = screen.size.x;
            if (bottomOnScreen < 0)
                bottomOnScreen = 0;
            if (topOnScreen > screen.size.y)
                topOnScreen = screen.size.y;

            i32 widthOnScreen = rightOnScreen - leftOnScreen;
            i32 heightOnScreen = topOnScreen - bottomOnScreen;

            f32 screenPixelOnBitmapWidth = (f32)(bitmap.size.x) / (f32)size.x;
            f32 screenPixelOnBitmapHeight = (f32)(bitmap.size.y) / (f32)size.y;

            for (i32 y = 1; y < heightOnScreen - 1; y++)
            {
                for (i32 x = 1; x < widthOnScreen - 1; x++)
                {
                    i32 screen_x = leftOnScreen + x;
                    i32 screen_y = bottomOnScreen + y;

                    f32 texture_x = (x + (leftOnScreen - (pos.x - size.x / 2))) * screenPixelOnBitmapWidth;
                    f32 texture_y = (y + (bottomOnScreen - (pos.y - size.y / 2))) * screenPixelOnBitmapHeight;

                    f32 f_x = texture_x - (i32)texture_x;
                    f32 f_y = texture_y - (i32)texture_y;

                    ARGB texel_a, texel_b, texel_c, texel_d, texel_ab, texel_cd, newColor;

                    texel_a.argb = bitmap.pixels[(i32)bitmap.size.x * (i32)texture_y + (i32)texture_x];
                    texel_b.argb = bitmap.pixels[(i32)bitmap.size.x * (i32)texture_y + (i32)texture_x + 1];
                    texel_c.argb = bitmap.pixels[(i32)bitmap.size.x * ((i32)texture_y + 1) + (i32)texture_x];
                    texel_d.argb = bitmap.pixels[(i32)bitmap.size.x * ((i32)texture_y + 1) + ((i32)texture_x + 1)];

                    texel_ab.a = texel_a.a * (1 - f_x) + texel_b.a * f_x;
                    texel_ab.r = texel_a.r * (1 - f_x) + texel_b.r * f_x;
                    texel_ab.g = texel_a.g * (1 - f_x) + texel_b.g * f_x;
                    texel_ab.b = texel_a.b * (1 - f_x) + texel_b.b * f_x;

                    texel_cd.a = texel_c.a * (1 - f_x) + texel_d.a * f_x;
                    texel_cd.r = texel_c.r * (1 - f_x) + texel_d.r * f_x;
                    texel_cd.g = texel_c.g * (1 - f_x) + texel_d.g * f_x;
                    texel_cd.b = texel_c.b * (1 - f_x) + texel_d.b * f_x;

                    newColor.a = texel_ab.a * (1 - f_y) + texel_cd.a * f_y;
                    newColor.r = texel_ab.r * (1 - f_y) + texel_cd.r * f_y;
                    newColor.g = texel_ab.g * (1 - f_y) + texel_cd.g * f_y;
                    newColor.b = texel_ab.b * (1 - f_y) + texel_cd.b * f_y;

                    //transparency
                    f32 alpha = (f32)newColor.a / 0xFF;

                    ARGB color;
                    color.argb = screen.pixels[screen_y * (i32)screen.size.x + screen_x];

                    color.r = newColor.r * alpha + color.r * (1 - alpha);
                    color.g = newColor.g * alpha + color.g * (1 - alpha);
                    color.b = newColor.b * alpha + color.b * (1 - alpha);
                    color.a = 0xFF;

                    screen.pixels[screen_y * (i32)screen.size.x + screen_x] = color.argb;
                }
            }
        }
    }
}

//drawing
void draw_rect(V2 pos, V2 size, u32 color, Layer layer)
{
    Drawing drawing;
    drawing.type = DRAWING_TYPE_RECT;
    drawing.pos = pos;
    drawing.size = size;
    drawing.color = color;
    drawing.bitmap = imgNone;
    drawing.layer = layer;
    assert(draw_queue_size < 4096);
    draw_queue[draw_queue_size] = drawing;
    draw_queue_size++;
}

void draw_bitmap(V2 pos, V2 size, Bitmap bitmap, Layer layer)
{
    Drawing drawing;
    drawing.type = DRAWING_TYPE_BITMAP;
    drawing.pos = pos;
    drawing.size = size;
    drawing.color = NULL;
    drawing.bitmap = bitmap;
    drawing.layer = layer;
    assert(draw_queue_size < 4096);
    draw_queue[draw_queue_size] = drawing;
    draw_queue_size++;
}

//таймеры

i32 timers[512];
i32 timersCount = 0;

i32 addTimer(i32 time)
{
    timers[timersCount] = time;
    timersCount++;
    return timersCount - 1;
}

void updateTimers()
{
    for (i32 i = 0; i < timersCount; i++)
    {
        timers[i]--;
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

#define TILE_SIZE_PIXELS 76
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
    CROUCHING_IDLE,
    CROUCHING_MOOVING,
    FALLING,
    GOT_HIT,
    HANGING,
} Condition;

typedef struct
{
    Game_Object_Type type;

    bool exists;

    V2 pos;
    V2 hitBox;
    V2 recentHitBox;
    V2 speed;

    bool goLeft;
    bool goRight;
    i32 jump;
    i32 canIncreaseJump;
    i32 canJump;

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
        {0, 0},

        false,
        false,
        addTimer(0),
        addTimer(0),
        addTimer(0),

        NULL,
        0,
        1,
        IDLE,
    };

    if (type == PLAYER)
    {
        gameObject.hitBox = {44, 60};
        gameObject.recentHitBox = {44, 60};
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

bool initialized = false;

//движение сущностей
void moveGameObject(Tile_Type *tiles, Game_Object *gameObject)
{
    Game_Object *ourObject = gameObject;

    f32 speedLength = length(ourObject->speed);

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
        i32 objLeft = ourObject->pos.x - (ourObject->hitBox.x / 2);
        i32 objRight = ourObject->pos.x + (ourObject->hitBox.x / 2);
        i32 objBottom = ourObject->pos.y - (ourObject->hitBox.y / 2);
        i32 objTop = ourObject->pos.y + (ourObject->hitBox.y / 2);

        i32 recentObjLeft = ourObject->pos.x - (ourObject->recentHitBox.x / 2);
        i32 recentObjRight = ourObject->pos.x + (ourObject->recentHitBox.x / 2);
        i32 recentObjBottom = ourObject->pos.y - (ourObject->recentHitBox.y / 2);
        i32 recentObjTop = ourObject->pos.y + (ourObject->recentHitBox.y / 2);

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

                    if (speedUnit.y != 0)
                    {
                        if (speedUnit.y > 0)
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
                            !(recentObjTop + speedPart.y <= tileBottom ||
                              recentObjBottom + speedPart.y >= tileTop ||
                              recentObjRight <= tileLeft ||
                              recentObjLeft >= tileRight))
                        {
                            gameObject->speed.y = unit(gameObject->speed).y;
                            ourObject->speed.y = 0;
                            speedUnit.y = 0;

                            ourObject->pos.y -= objSide - tileSide;

                            collisionYHappened = true;

                            if (tileSide == tileBottom)
                            {
                                timers[ourObject->canIncreaseJump] = 0;
                            }
                        }
                    }

                    if (collisionYHappened && tileSide == tileTop && ourObject->condition != HANGING)
                    {
                        ourObject->condition = IDLE;
                        timers[ourObject->canJump] = 3;
                    }

                    if (speedUnit.x != 0)
                    {
                        if (speedUnit.x > 0)
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
                            !(recentObjRight + speedPart.x <= tileLeft ||
                              recentObjLeft + speedPart.x >= tileRight ||
                              recentObjTop <= tileBottom ||
                              recentObjBottom >= tileTop))
                        {

                            gameObject->speed.x = unit(gameObject->speed).x;
                            ourObject->speed.x = 0;
                            speedUnit.x = 0;
                            ourObject->pos.x -= objSide - tileSide;
                            collisionXHappened = true;

                            i32 upTileIndex = (y + 1) * CHUNK_SIZE_X * (CHUNK_COUNT_X + 2) + x;

                            if (tile_map[upTileIndex] == Tile_Type_NONE)
                            {
                                //весит
                                if (ourObject->type == PLAYER && ourObject->speed.y < 0 && ourObject->pos.y >= tilePos.y * TILE_SIZE_PIXELS && ourObject->pos.y + ourObject->speed.y <= tilePos.y * TILE_SIZE_PIXELS)
                                {
                                    ourObject->condition = HANGING;

                                    gameObject->speed.y = unit(gameObject->speed).y;
                                    ourObject->speed.y = 0;
                                    speedUnit.y = 0;

                                    ourObject->pos.y = tilePos.y * TILE_SIZE_PIXELS;
                                    collisionYHappened = true;
                                }

                                //помощь в карабкании
                                if (objBottom < tileTop && objBottom + 12 > tileTop)
                                {
                                    ourObject->speed.y += 5;
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
    gameObject->canJump = ourObject->canJump;
}

void updateGameObject(Game_Object *gameObject, Input input, Bitmap screen)
{
    if (gameObject->type == PLAYER)
    {
        //движение игрока
        gameObject->goLeft = input.left.is_down;
        gameObject->goRight = input.right.is_down;

        if (input.space.went_down)
        {
            timers[gameObject->jump] = 6;
        }

        //константы скорости
        f32 accelConst;
        if (input.shift.is_down)
        {
            //0.75
            accelConst = 3;
        }
        else
        {
            accelConst = 1.5;
        }
        if (gameObject->condition == CROUCHING_IDLE || gameObject->condition == CROUCHING_MOOVING)
        {
            accelConst = 0.5;
        }
        //0.95
        f32 frictionConst = 0.80;
        //-0.75
        f32 gravity = -4;

        //прыжок
        if (timers[gameObject->jump] > 0 && timers[gameObject->canJump] > 0)
        {
            if (!(gameObject->condition == HANGING && input.down.is_down))
            {
                timers[gameObject->canIncreaseJump] = 13;
            }

            timers[gameObject->jump] = 0;
            gameObject->condition = FALLING;
        }

        if (gameObject->speed.y < 0)
        {
            timers[gameObject->canIncreaseJump] = 0;
        }

        if (timers[gameObject->canIncreaseJump] > 0 && input.space.is_down)
        {
            gameObject->speed.y += timers[gameObject->canIncreaseJump] * 0.92;
        }
        else
        {
            timers[gameObject->canIncreaseJump] = 0;
        }

        if (gameObject->condition != HANGING)
        {
            //скорость по x
            gameObject->speed += {(gameObject->goRight - gameObject->goLeft) * accelConst, 0};

            //гравитация
            gameObject->speed.y += gravity;
        }
        else
        {
            if (gameObject->lookingDirection == RIGHT)
            {
                gameObject->speed.x = 1;
            }
            if (gameObject->lookingDirection == LEFT)
            {
                gameObject->speed.x = -1;
            }
        }

        //трение
        gameObject->speed *= frictionConst;

        //направление
        if (gameObject->speed.x > 0)
        {
            gameObject->lookingDirection = RIGHT;
        }
        if (gameObject->speed.x < 0)
        {
            gameObject->lookingDirection = LEFT;
        }

        //состояние падает
        if (gameObject->condition != HANGING)
        {
            gameObject->condition = FALLING;
        }

        //столкновения и движение
        moveGameObject(tile_map, gameObject);

        //состояние ползком
        if (gameObject->condition == IDLE && input.down.is_down)
        {
            gameObject->condition = CROUCHING_IDLE;
        }

        //состояние движится
        if ((gameObject->speed.x > 1 || gameObject->speed.x < -1) && (gameObject->condition == IDLE || gameObject->condition == CROUCHING_IDLE))
        {

            gameObject->movedThroughPixels += gameObject->speed.x;

            if (gameObject->condition == IDLE)
            {
                gameObject->condition = MOOVING;
            }
            else
            {
                gameObject->condition = CROUCHING_MOOVING;
            }
        }
        else
        {
            gameObject->movedThroughPixels = 0;
        }

        gameObject->recentHitBox = gameObject->hitBox;

        //состояния
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
            gameObject->hitBox = {44, 60};
        }

        if (gameObject->condition == CROUCHING_IDLE)
        {
            if (gameObject->lookingDirection == RIGHT)
            {
                gameObject->sprite = imgPlayerRightCrouchIdle;
            }
            if (gameObject->lookingDirection == LEFT)
            {
                gameObject->sprite = imgPlayerLeftCrouchIdle;
            }
            gameObject->hitBox = {58, 30};
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
            gameObject->hitBox = {44, 60};
        }

        if (gameObject->condition == CROUCHING_MOOVING)
        {
            if (gameObject->lookingDirection == RIGHT)
            {
                i8 step = (i32)floor(gameObject->movedThroughPixels / 7);
                while (step > 7)
                {
                    step -= 8;
                }
                while (step < 0)
                {
                    step += 8;
                }
                gameObject->sprite = imgPlayerRightCrouchStep[step];
            }

            if (gameObject->lookingDirection == LEFT)
            {
                i8 step = (i32)floor(gameObject->movedThroughPixels / 7);
                while (step > 7)
                {
                    step -= 8;
                }
                while (step < 0)
                {
                    step += 8;
                }
                gameObject->sprite = imgPlayerLeftCrouchStep[7 - step];
            }
            gameObject->hitBox = {58, 30};
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
            gameObject->hitBox = {44, 56};
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
            timers[gameObject->canJump] = 5;
            gameObject->hitBox = {44, 60};
        }

        //камера
        camera.pos = gameObject->pos;

        if (!(gameObject->pos.x - screen.size.x / 2 > -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * CHUNK_SIZE_X - 150))
        {
            camera.pos.x = -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * CHUNK_SIZE_X - 150 + screen.size.x / 2;
        }

        if (!(gameObject->pos.x + screen.size.x / 2 < -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * (CHUNK_COUNT_X + 1) * CHUNK_SIZE_X + 150))
        {
            camera.pos.x = -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * (CHUNK_COUNT_X + 1) * CHUNK_SIZE_X + 150 - screen.size.x / 2;
        }

        if (!(gameObject->pos.y - screen.size.y / 2 > -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * CHUNK_SIZE_Y - 120))
        {
            camera.pos.y = -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * CHUNK_SIZE_Y - 120 + screen.size.y / 2;
        }

        if (!(gameObject->pos.y + screen.size.y / 2 < -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * (CHUNK_COUNT_Y + 1) * CHUNK_SIZE_Y + 120))
        {
            camera.pos.y = -TILE_SIZE_PIXELS / 2 + TILE_SIZE_PIXELS * (CHUNK_COUNT_Y + 1) * CHUNK_SIZE_Y + 120 - screen.size.y / 2;
        }

        // static f32 sizeX = 1;
        // sizeX += 0.01f;

        //drawPlayer
        draw_bitmap(gameObject->pos + V2{0, (gameObject->sprite.size.y - gameObject->hitBox.y) / 2 - 2}, gameObject->sprite.size, gameObject->sprite, LAYER_PLAYER);

        // //drawPlayer
        // draw_bitmap(gameObject->pos + V2{0, 7} - V2{100, 0}, gameObject->sprite.size * 2, gameObject->sprite, LAYER_PLAYER);
        // //drawPlayer
        // draw_bitmap(gameObject->pos + V2{0, 7} + V2{100, 0}, gameObject->sprite.size / 2, gameObject->sprite, LAYER_PLAYER);
    }
}

void game_update(Bitmap screen, Input input)
{
    V2 mapSize = {CHUNK_SIZE_X * (CHUNK_COUNT_X + 2), CHUNK_SIZE_Y * (CHUNK_COUNT_Y + 2)};

    //выполняется один раз
    if (!initialized)
    {
        initialized = true;

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
    draw_rect(camera.pos, screen.size * 1.1, 0xFFFFAA00, LAYER_CLEAR);

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
        Tile_Type tile = tile_map[tileIndex];
        V2 tilePos = getTilePos(tileIndex);
        if (tile == Tile_Type_WALL)
        {
            draw_rect(tilePos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS}, 0xFFFFFF00, LAYER_NORMAL);
        }
        if (tile == Tile_Type_ENTER || tile == Tile_Type_EXIT)
        {
            draw_rect(tilePos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS}, 0xFFFF0000, LAYER_BACKGROUND);
        }
        if (tile == Tile_Type_BORDER)
        {
            draw_rect(tilePos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS}, 0xFF0000FF, LAYER_NORMAL);
        }
        if (((i32)tilePos.x + 1) % 3 == 0 && ((i32)tilePos.y + 1) % 3 == 0)
        {
            draw_bitmap(tilePos * TILE_SIZE_PIXELS, V2{(TILE_SIZE_PIXELS + 2) * 3, (TILE_SIZE_PIXELS + 2) * 3}, imgTilePlate, LAYER_BACKGROUND);
        }
    }

    //сортируем qrawQueue
    Drawing new_draw_queue[4096];
    i32 new_draw_queue_size = 0;
    Layer layer = (Layer)0;

    while (new_draw_queue_size != draw_queue_size)
    {
        for (i32 drawingIndex = 0; drawingIndex < draw_queue_size; drawingIndex++)
        {
            if (draw_queue[drawingIndex].layer == layer)
            {
                new_draw_queue[new_draw_queue_size] = draw_queue[drawingIndex];
                new_draw_queue_size++;
            }
        }
        layer = (Layer)((i32)layer + 1);
    }

    draw_queue_size = 0;

    for (i32 i = 0; i < new_draw_queue_size; i++)
    {
        drawItem(screen, new_draw_queue[i]);
    }

    updateTimers();
}