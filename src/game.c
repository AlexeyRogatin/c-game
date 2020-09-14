#include <malloc.h>
#include "math.h"
#include "game_math.c"

#define PI 3.14159265

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
    i32 pitch;
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

Camera camera = {{0, 0},};

typedef union {
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
    f32 angle;
    u32 color;
    Bitmap bitmap;
    Layer layer;
} Drawing;

Drawing draw_queue[4096];
i32 draw_queue_size = 0;

void clear_screen(Bitmap screen) {
    for(i32 i = 0; i < screen.size.x * screen.size.y; i++) {
        screen.pixels[i] = 0xFF000000;
    }
}

typedef struct {
    ARGB a, b, c, d;
} Bilinear_Sample;

Bilinear_Sample get_bilinear_sample(Bitmap bitmap, i32 x, i32 y) {
    ARGB a = {bitmap.pixels[y * bitmap.pitch + x]};
    ARGB b = {bitmap.pixels[y * bitmap.pitch + x + 1]};
    ARGB c = {bitmap.pixels[(y + 1) * bitmap.pitch + x]};
    ARGB d = {bitmap.pixels[(y + 1) * bitmap.pitch + x + 1]};

    Bilinear_Sample result = {a,b,c,d};
    return result;
}

ARGB lerp(ARGB a, ARGB b, f32 f) {
    ARGB result; 
    result.r = a.r * (1 - f) + b.r * f;
    result.g = a.g * (1 - f) + b.g * f;
    result.b = a.b * (1 - f) + b.b * f;
    result.a = a.a * (1 - f) + b.a * f;
    return result;
}

ARGB bilinear_blend(Bilinear_Sample sample, f32 x_f, f32 y_f) {
    ARGB ab = lerp(sample.a,sample.b, x_f);
    ARGB cd = lerp(sample.c,sample.d, x_f);
    ARGB abcd = lerp(ab,cd, y_f);
    return abcd;
}

void drawItem(Bitmap screen, Drawing drawing)
{
    if (drawing.type == DRAWING_TYPE_RECT)
    {
        V2 x_axis = rotateVector({drawing.size.x,0}, drawing.angle);
        V2 y_axis = rotateVector({0,drawing.size.y}, drawing.angle);

        V2 origin = drawing.pos - x_axis/2 - y_axis/2;

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

        for (i32 vertex_index = 0; vertex_index < 4; vertex_index++) {
            V2 vertex = vertices[vertex_index];
            if (vertex.x < min.x) min.x = vertex.x;
            if (vertex.y < min.y) min.y = vertex.y;
            if (vertex.x > max.x) max.x = vertex.x;
            if (vertex.y > max.y) max.y = vertex.y;
        }

        if(min.x < 0) min.x = 0;
        if(min.y < 0) min.y = 0;
        if(max.x > screen.size.x) max.x = screen.size.x;
        if(max.y > screen.size.y) max.y = screen.size.y;

        for (i32 y = floor(min.y); y < ceil(max.y); y++) {
            for (i32 x = floor(min.x); x < ceil(max.x); x++) {
                V2 pixel_coords = {(f32)x,(f32)y};
                V2 d = pixel_coords - origin;

                f32 dot_x = dot(d, x_axis) / x_axis_length;
                f32 dot_y = dot(d, y_axis) / y_axis_length;

                if (dot_x >= 0 && dot_x < x_axis_length && dot_y >= 0 && dot_y < y_axis_length) {
                    screen.pixels[y * (i32)screen.size.x + x] = drawing.color;
                }
            }
        }
    }

    if (drawing.type == DRAWING_TYPE_BITMAP)
    {
        V2 x_axis = rotateVector({drawing.size.x,0}, drawing.angle);
        V2 y_axis = rotateVector({0,drawing.size.y}, drawing.angle);

        V2 origin = drawing.pos - x_axis/2 - y_axis/2;

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

        for (i32 vertex_index = 0; vertex_index < 4; vertex_index++) {
            V2 vertex = vertices[vertex_index];
            if (vertex.x < min.x) min.x = vertex.x;
            if (vertex.y < min.y) min.y = vertex.y;
            if (vertex.x > max.x) max.x = vertex.x;
            if (vertex.y > max.y) max.y = vertex.y;
        }

        if(min.x < 0) min.x = 0;
        if(min.y < 0) min.y = 0;
        if(max.x > screen.size.x) max.x = screen.size.x;
        if(max.y > screen.size.y) max.y = screen.size.y;

        for (i32 y = floor(min.y); y < ceil(max.y); y++) {
            for (i32 x = floor(min.x); x < ceil(max.x); x++) {
                V2 pixel_coords = {(f32)x,(f32)y};
                V2 d = pixel_coords - origin;

                f32 dot_x = dot(d, x_axis) / x_axis_length;
                f32 dot_y = dot(d, y_axis) / y_axis_length;

                if (dot_x >= 0 && dot_x < x_axis_length && dot_y >= 0 && dot_y < y_axis_length) {
                    f32 u = dot_x / x_axis_length;
                    f32 v = dot_y / y_axis_length;
                    
                    f32 tex_x = u * drawing.bitmap.size.x;
                    f32 tex_y = v * drawing.bitmap.size.y;

                    i32 x_int = (i32)tex_x;
                    i32 y_int = (i32)tex_y;

                    f32 x_f = tex_x - x_int;
                    f32 y_f = tex_y - y_int;

                    ARGB pixel = {screen.pixels[y*(i32)screen.size.x + x]};
                    Bilinear_Sample sample = get_bilinear_sample(drawing.bitmap, x_int, y_int);
                    ARGB blended_sample = bilinear_blend(sample, x_f, y_f);
                    ARGB result;

                    f32 alpha = (0xFF - blended_sample.a) / 255.0f;
                    
                    result.r = blended_sample.r + pixel.r * alpha;
                    result.g = blended_sample.g + pixel.g * alpha;
                    result.b = blended_sample.b + pixel.b * alpha;
                    result.a = 0xFF;

                    screen.pixels[y * (i32)screen.size.x + x] = result.argb;
                }
            }
        }
    }
}

//drawing
void draw_rect(V2 pos, V2 size, f32 angle, u32 color, Layer layer)
{
    Drawing drawing;
    drawing.type = DRAWING_TYPE_RECT;
    drawing.pos = pos;
    drawing.size = size;
    drawing.angle = angle;
    drawing.color = color;
    drawing.bitmap = imgNone;
    drawing.layer = layer;
    assert(draw_queue_size < 4096);
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

    i32 lookingKeyHoldTimer;
} Game_Object;

i32 gameObjectCount = 0;
Game_Object gameObjects[512];

typedef enum {
    Side_RIGHT,
    Side_LEFT,
    Side_BOTTOM,
    Side_TOP,
} Side;

typedef struct {
    bool happened;
    i32 tileIndex;
    i32 tileType;
    Side tileSide;
} Collision;

typedef struct {
    Collision X;
    Collision Y;
} Collisions;

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
        addTimer(0),
        addTimer(0),
        addTimer(0),

        NULL,
        0,
        1,
        IDLE,

        addTimer(0),
    };

    if (type == PLAYER)
    {
        gameObject.hitBox = {44, 60};
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
Collisions checkCollision(Tile_Type *tiles, Game_Object *gameObject)
{
    Game_Object *ourObject = gameObject;

    f32 speedLength = length(ourObject->speed);

    bool collisionXHappened = false;
    bool collisionYHappened = false;

    Collisions collisions;

    //y
    collisions.X.happened = false;
    //x
    collisions.Y.happened = false;

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
        f32 objLeft = ourObject->pos.x - (ourObject->hitBox.x / 2);
        f32 objRight = ourObject->pos.x + (ourObject->hitBox.x / 2);
        f32 objBottom = ourObject->pos.y - (ourObject->hitBox.y / 2);
        f32 objTop = ourObject->pos.y + (ourObject->hitBox.y / 2);

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

                    f32 objSide;
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
                            !(objTop + speedPart.y <= tileBottom ||
                              objBottom + speedPart.y >= tileTop ||
                              objRight <= tileLeft ||
                              objLeft >= tileRight))
                        {
                            ourObject->speed.y = 0;
                            speedUnit.y = 0;
                            speedPart.y = 0;

                            ourObject->pos.y -= objSide - tileSide;

                            collisionYHappened = true;

                            Collision thisCollision;
                            thisCollision.happened = true;
                            thisCollision.tileIndex = tileIndex;
                            if(tileSide == tileBottom) {
                                thisCollision.tileSide = Side_BOTTOM;
                            } else {
                                thisCollision.tileSide = Side_TOP;
                            }

                            collisions.Y = thisCollision;
                        }
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
                            !(objRight + speedPart.x <= tileLeft ||
                              objLeft + speedPart.x >= tileRight ||
                              objTop <= tileBottom ||
                              objBottom >= tileTop))
                        {

                            ourObject->speed.x = 0;
                            speedUnit.x = 0;
                            speedPart.x = 0;
                            
                            ourObject->pos.x -= objSide - tileSide;
                            collisionXHappened = true;

                            Collision thisCollision;
                            thisCollision.happened = true;
                            thisCollision.tileIndex = tileIndex;
                            if(tileSide == tileLeft) {
                                thisCollision.tileSide = Side_LEFT;
                            } else {
                                thisCollision.tileSide = Side_RIGHT;
                            }

                            collisions.X = thisCollision;
                        }
                    }
                }
            }
        }
        ratio += ratioChange;
    }
    gameObject->pos = ourObject->pos;
    gameObject->pos += ourObject->speed;

    return collisions;
}

//changePos принимает три значения: -1, 0, 1. 
//-1 хитбокс перемещается к низу
//0 xитбокс не перемещается
//1 хитбокс перемещается к верху
void changeHitBox(Game_Object *gameObject, V2 hitBox, i8 changePos) {
    V2 objTilePos = {roundf(gameObject->pos.x / TILE_SIZE_PIXELS),
                         roundf(gameObject->pos.y / TILE_SIZE_PIXELS)};
    
    i32 tileCountX = ceilf((hitBox.x - gameObject->hitBox.x)/ TILE_SIZE_PIXELS);
    i32 tileCountY = ceilf((hitBox.y - gameObject->hitBox.y) / TILE_SIZE_PIXELS);

    if(changePos) {
        if(changePos == -1) {
            gameObject->pos.y += -gameObject->hitBox.y / 2 + hitBox.y / 2;
        }
        if(changePos == 1) {
            gameObject->pos.y += gameObject->hitBox.y / 2 - hitBox.y / 2;
        }
    }

    for(i32 y = objTilePos.y - tileCountY; y <= objTilePos.y + tileCountY;y ++) {
        for(i32 x = objTilePos.x - tileCountX; x <= objTilePos.x + tileCountX;x ++) {
            i32 tileIndex = y*(CHUNK_COUNT_X + 2) * CHUNK_SIZE_X + x;
            Tile_Type tile = tile_map[tileIndex];

            if(tile == Tile_Type_WALL || tile == Tile_Type_BORDER) {

                V2 tilePos = getTilePos(tileIndex);

                f32 recentObjTop = gameObject->pos.y + gameObject->hitBox.y/2;
                f32 recentObjBottom = gameObject->pos.y - gameObject->hitBox.y/2;
                f32 recentObjRight = gameObject->pos.x + gameObject->hitBox.x/2;
                f32 recentObjLeft = gameObject->pos.x - gameObject->hitBox.x/2;

                f32 objTop = gameObject->pos.y + hitBox.y/2;
                f32 objBottom = gameObject->pos.y - hitBox.y/2;
                f32 objRight = gameObject->pos.x + hitBox.x/2;
                f32 objLeft = gameObject->pos.x - hitBox.x/2;

                f32 tileTop = tilePos.y * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS/2;
                f32 tileBottom = tilePos.y * TILE_SIZE_PIXELS - TILE_SIZE_PIXELS/2;
                f32 tileRight = tilePos.x * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS/2;
                f32 tileLeft = tilePos.x * TILE_SIZE_PIXELS - TILE_SIZE_PIXELS/2;

                if(recentObjLeft >= tileRight && objLeft <= tileRight) {
                    gameObject->pos.x -= objLeft - tileRight;
                }

                if(recentObjRight <= tileLeft && objRight >= tileLeft) {
                    gameObject->pos.x -= objRight - tileLeft;
                }

                if(recentObjBottom >= tileTop && objBottom <= tileTop) {
                    gameObject->pos.y -= objBottom - tileTop;
                }

                if(recentObjTop <= tileBottom && objTop >= tileBottom) {
                    gameObject->pos.y -= objTop - tileBottom;
                }
            }
        }
    }
    gameObject->hitBox = hitBox;
}

void updateGameObject(Game_Object *gameObject, Input input, Bitmap screen) {
    if (gameObject->type == PLAYER)
    {
        //предпологаемое состояние
        Condition condition;

        //движение игрока
        gameObject->goLeft = input.left.is_down;
        gameObject->goRight = input.right.is_down;

        if (input.space.went_down)
        {
            timers[gameObject->jump] = 5;
        }

        //константы скорости
        f32 accelConst;
        if (input.shift.is_down)
        {
            accelConst = 2.8;
        }
        else
        {
            accelConst = 1.5;
        }
        if (gameObject->condition == CROUCHING_IDLE || gameObject->condition == CROUCHING_MOOVING)
        {
            accelConst = 0.5;
        }
        f32 frictionConst = 0.80;
        f32 gravity = -4;

        //прыжок
        if (timers[gameObject->jump] > 0 && timers[gameObject->canJump] > 0)
        {
            if (!(gameObject->condition == HANGING && input.down.is_down))
            {
                timers[gameObject->canIncreaseJump] = 13;
            }

            gameObject->speed.y = 0;
            timers[gameObject->jump] = 0;
            timers[gameObject->canJump] = 0;
            changeHitBox(gameObject,{44,60},-1);
            gameObject->condition = FALLING;
        }

        if (timers[gameObject->canIncreaseJump] > 0 && input.space.is_down)
        {
            gameObject->speed.y += timers[gameObject->canIncreaseJump] * 0.92;
        }
        else
        {
            timers[gameObject->canIncreaseJump] = 0;
        }

        if (gameObject->speed.y < 0)
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
        condition = FALLING;

        //столкновения и движение
        Collisions collisions = checkCollision(tile_map, gameObject);

        //нельзя прыгать выше и состояние стоит
        if (collisions.Y.happened)
        {
            if(collisions.Y.tileSide == Side_BOTTOM) {
                timers[gameObject->canIncreaseJump] = 0;
            } else {
                condition = IDLE;
                timers[gameObject->canJump] = 4;
            }
        }

        V2 collidedXTilePos = getTilePos(collisions.X.tileIndex);
        V2 collidedYTilePos = getTilePos(collisions.Y.tileIndex);

        i32 upTileIndex = (collidedXTilePos.y + 1) * CHUNK_SIZE_X * (CHUNK_COUNT_X + 2) + collidedXTilePos.x;

        if (collisions.X.happened && tile_map[upTileIndex] == Tile_Type_NONE)
        {
            //весит
            if (gameObject->speed.y < 0 && gameObject->pos.y > collidedXTilePos.y * TILE_SIZE_PIXELS && gameObject->pos.y + gameObject->speed.y <= collidedXTilePos.y * TILE_SIZE_PIXELS)
            {
                gameObject->condition = HANGING;

                gameObject->speed = {0,0};

                gameObject->pos.y = collidedXTilePos.y * TILE_SIZE_PIXELS;
            }

            //помощь в карабкании
            if (gameObject->speed.y <=0 && gameObject->pos.y - gameObject->hitBox.y / 2 > collidedXTilePos.y * TILE_SIZE_PIXELS + TILE_SIZE_PIXELS/2 - 15)
            {
                gameObject->speed.y = 15;
            }
        }

        //состояние ползком
        if (condition == IDLE && input.down.is_down)
        {
            condition = CROUCHING_IDLE;
        }

        //состояние движится
        if ((gameObject->speed.x > 1 || gameObject->speed.x < -1) && (condition == IDLE || condition == CROUCHING_IDLE))
        {

            gameObject->movedThroughPixels += gameObject->speed.x;

            if (condition == IDLE)
            {
                condition = MOOVING;
            }
            else
            {
                condition = CROUCHING_MOOVING;
            }
        }
        else
        {
            gameObject->movedThroughPixels = 0;
        }

        //что делают состояния
        if(gameObject->condition != HANGING) {
            gameObject->condition = condition;
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
            changeHitBox(gameObject, V2{44,60}, -1);
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

            changeHitBox(gameObject, V2{44,30}, -1);
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
            changeHitBox(gameObject, V2{44,60}, -1);
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
            changeHitBox(gameObject, V2{44,30}, -1);
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
            changeHitBox(gameObject, V2{44,55}, 1);
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
            changeHitBox(gameObject, V2{44,60}, 0);
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

        //drawPlayer

        //хитбокс
        // draw_rect(gameObject->pos,gameObject->hitBox,0xFFFFFFFF,LAYER_FORGROUND);

        draw_bitmap(gameObject->pos + V2{0, (gameObject->sprite.size.y - gameObject->hitBox.y) / 2 - 2}, gameObject->sprite.size, 0, gameObject->sprite, LAYER_PLAYER);

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
                            V2 spawnPos = {(f32)tileX * TILE_SIZE_PIXELS, (f32)tileY * TILE_SIZE_PIXELS};
                            addGameObject(PLAYER, spawnPos);
                            camera.pos = spawnPos; 
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
    clear_screen(screen);

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
            draw_rect(tilePos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS},0, 0xFFFFFF00, LAYER_NORMAL);
        }
        if (tile == Tile_Type_ENTER || tile == Tile_Type_EXIT)
        {
            draw_rect(tilePos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS},0, 0xFFFF0000, LAYER_BACKGROUND);
        }
        if (tile == Tile_Type_BORDER)
        {
            draw_rect(tilePos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS},0, 0xFF0000FF, LAYER_NORMAL);
        }
        if (((i32)tilePos.x + 1) % 3 == 0 && ((i32)tilePos.y + 1) % 3 == 0)
        {
            draw_bitmap(tilePos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS * 3, TILE_SIZE_PIXELS * 3},0, imgTilePlate, LAYER_CLEAR);
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
                draw_queue[drawingIndex].pos.x += screen.size.x / 2 - camera.pos.x;
                draw_queue[drawingIndex].pos.y += screen.size.y / 2 - camera.pos.y;
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