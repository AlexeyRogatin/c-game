#include "game.h"
#include <malloc.h>
#include "resources.h"

V2 world_to_screen(Bitmap screen, Camera camera, V2 p)
{
    V2 result = (p - camera.pos) * camera.scale + screen.size * 0.5f;
    return result;
}

V2 screen_to_world(Bitmap screen, Camera camera, V2 p)
{
    V2 result = (p - screen.size * 0.5f) / camera.scale + camera.pos;
    return result;
}

#include "drawing.h"

//таймеры
i32 add_timer(Game_memory *memory, i32 time)
{
    memory->timers[memory->timers_count] = time;
    memory->timers_count++;
    return memory->timers_count - 1;
}

void update_timers(Game_memory *memory)
{
    for (i32 i = 0; i < memory->timers_count; i++)
    {
        memory->timers[i]--;
    }
}

#include "add_game_objects.h"

//границы камеры
void border_camera(Game_memory *memory, Bitmap screen)
{
    if (!(memory->camera.pos.x - screen.size.x / memory->camera.scale.x * 0.5 - SPRITE_SCALE > -TILE_SIZE_PIXELS * 0.5))
    {
        memory->camera.pos.x = f32(-TILE_SIZE_PIXELS * 0.5 + screen.size.x / memory->camera.scale.x * 0.5 + SPRITE_SCALE);
    }

    if (!(memory->camera.pos.x + screen.size.x / memory->camera.scale.x * 0.5 < -TILE_SIZE_PIXELS * 0.5 + TILE_SIZE_PIXELS * CHUNK_COUNT_X * CHUNK_SIZE_X + 2 * BORDER_SIZE * TILE_SIZE_PIXELS))
    {
        memory->camera.pos.x = f32(-TILE_SIZE_PIXELS * 0.5 + TILE_SIZE_PIXELS * CHUNK_COUNT_X * CHUNK_SIZE_X + 2 * BORDER_SIZE * TILE_SIZE_PIXELS - screen.size.x / memory->camera.scale.x * 0.5);
    }

    if (!(memory->camera.pos.y - screen.size.y / memory->camera.scale.y * 0.5 - SPRITE_SCALE > -TILE_SIZE_PIXELS * 0.5))
    {
        memory->camera.pos.y = f32(-TILE_SIZE_PIXELS * 0.5 + screen.size.y / memory->camera.scale.y * 0.5 + SPRITE_SCALE);
    }

    if (!(memory->camera.pos.y + screen.size.y / memory->camera.scale.y * 0.5 < -TILE_SIZE_PIXELS * 0.5 + TILE_SIZE_PIXELS * CHUNK_COUNT_Y * CHUNK_SIZE_Y + 2 * BORDER_SIZE * TILE_SIZE_PIXELS))
    {
        memory->camera.pos.y = f32(-TILE_SIZE_PIXELS * 0.5 + TILE_SIZE_PIXELS * CHUNK_COUNT_Y * CHUNK_SIZE_Y + 2 * BORDER_SIZE * TILE_SIZE_PIXELS - screen.size.y / memory->camera.scale.y * 0.5);
    }
}

#include "tile_map.h"
#include "collisions.h"

#define THROWING_SPEED 30.0f
#define TOSSING_SPEED 6.0f
#define NORMAL_ANGLE (f32)(PI / 6)
#define UP_ANGLE (f32)(PI / 3)
#define DOWN_ANGLE (f32)(-PI / 3)

//функция заставляет персонажа поднимать/бросать предметы
bool pick_item_check(Game_Object *player, Game_Object *game_object, i32 index)
{
    bool result = false;
    if ((player->condition == Condition_CROUCHING_IDLE || player->condition == Condition_CROUCHING_MOOVING || player->condition == Condition_HANGING_LOOKING_DOWN))
    {
        if (!player->weapon.index)
        {
            if (player->condition != Condition_HANGING_LOOKING_DOWN)
            {
                Rect object_rect = Rect{game_object->pos + game_object->collision_box_pos - game_object->collision_box * 0.5 - player->collision_box * 0.5, game_object->pos + game_object->collision_box_pos + game_object->collision_box * 0.5 + player->collision_box * 0.5};
                V2 collision_point = V2{player->pos.x, player->pos.y + player->collision_box_pos.y - player->collision_box.y * 0.75f};
                if (collision_point.x >= object_rect.min.x && collision_point.x <= object_rect.max.x &&
                    collision_point.y >= object_rect.min.y && collision_point.y <= object_rect.max.y)
                {
                    player->weapon = {game_object->id, index};
                }
            }
        }
        else if (player->weapon.index == index)
        {
            player->weapon = {0, 0};
            game_object->speed = (rotate_vector(V2{TOSSING_SPEED * game_object->looking_direction, 0}, NORMAL_ANGLE) + player->speed) * game_object->mass;
        }
        result = true;
    }
    return result;
}

void add_screen_shake(Game_memory *memory, i32 time, f32 power)
{
    memory->timers[memory->screen_shake_timer] = time;
    memory->screen_shake_power += power;
}

i32 check_for_interactive_tiles(Game_memory *memory, Game_Object *game_object)
{
    i32 result = -1;
    V2 obj_tile_pos = V2{roundf(game_object->pos.x / TILE_SIZE_PIXELS), roundf(game_object->pos.y / TILE_SIZE_PIXELS)};

    i32 index = get_index(obj_tile_pos);
    Tile tile = memory->tile_map[index];
    if (tile.timer >= 0)
    {
        result = index;
    }
    return result;
}

#include "update_game_object.h"

extern "C" GAME_UPDATE(game_update)
{
    memory->camera.scale = 0.55f * V2{1, 1};
    //выполняется один раз
    if (!memory->initialized)
    {
        memory->initialized = true;

        load_bitmaps(memory, memory->win32_read_bmp);
        // load_letters(memory, memory->stbtt_read_font, "./fonts/comic.ttf", 128.0f);

        memory->camera.pos = V2{0, 0};
        memory->camera.scale = V2{1, 1} * 0.55f;
        memory->camera.target = V2{0, 0};

        V2 darkness_size = ceil(screen.size / memory->camera.scale / SPRITE_SCALE);
        memory->darkness = create_empty_bitmap(darkness_size);

        memory->transition = 0;

        memory->draw_queue_size = 0;
        memory->game_object_count = 0;
        memory->timers_count = 0;

        memory->pause = 0;

        memory->id_count = 1;

        memory->lamp_count = 1;

        memory->player = {0, 0};

        memory->screen_shake_timer = add_timer(memory, 0);

        generate_new_map(memory, screen);
    }

    //очистка экрана
    clear_screen(memory, screen, memory->darkness);

    if (input.t.went_down)
    {
        if (memory->pause == INT_INFINITY)
        {
            memory->pause = 0;
        }
        else if (memory->pause < 0)
        {
            memory->pause = INT_INFINITY;
        }
    }

    //плавный переход
    if (memory->transition < 0)
    {
        memory->pause = INT_INFINITY;
        memory->transition += (-1.0f - memory->transition) * 0.08f;
        if (memory->transition < -0.999f)
        {
            memory->transition = 0.999f;
            generate_new_map(memory, screen);
            memory->pause = 0;
        }
    }
    else if (memory->transition > 0)
    {
        memory->pause = INT_INFINITY;
        memory->transition += (0.0f - memory->transition) * 0.08f;
        if (memory->transition < 0.001f)
        {
            memory->transition = 0.0f;
            memory->pause = 0;
        }
    }

    if (input.r.went_down)
    {
        generate_new_map(memory, screen);
    }

    if (memory->pause <= 0)
    {
        memory->draw_queue_size = 0;
        memory->light_queue_size = 0;

        V2 map_size = {CHUNK_COUNT_X * CHUNK_SIZE_X + BORDER_SIZE * 2, CHUNK_COUNT_Y * CHUNK_SIZE_Y + BORDER_SIZE * 2};
        i32 tile_count = i32(map_size.x * map_size.y);
        i32 interval = SPRITE_SCALE;

        //обновление сущностей
        for (i32 object_index = 0; object_index < memory->game_object_count; object_index++)
        {
            if (memory->game_objects[object_index].exists)
            {
                update_game_object(memory, object_index, input, screen);
            }
        }
        for (i32 object_index = 0; object_index < memory->game_object_count; object_index++)
        {
            if (memory->game_objects[object_index].exists)
            {
                check_hits(memory, &memory->game_objects[object_index]);
            }
        }

        //обновление тайлов
        for (i32 tile_index = 0; tile_index < tile_count; tile_index++)
        {
            update_tile(memory, tile_index);
        }
        draw_tile_map(memory);

        if (memory->timers[memory->screen_shake_timer] > 0)
        {
            memory->camera.pos += V2{random_float(&memory->__global_random_state, -memory->screen_shake_power, memory->screen_shake_power),
                                     random_float(&memory->__global_random_state, -memory->screen_shake_power, memory->screen_shake_power)};

            border_camera(memory, screen);
        }
        else
        {
            memory->screen_shake_power = 0.0f;
        }

        //прорисовка темноты
        add_bitmap_to_queue(memory, memory->camera.pos, memory->darkness.size * SPRITE_SCALE, 0, memory->darkness, 1, 0x00000000, LAYER_DARKNESS);

        // draw_text(memory, "aiAWAWAWAWVA", memory->camera.pos - V2{screen.size.x / memory->camera.scale.x * 0.45f, 0.0f}, V2{0, 0}, 0, 0.0f, 0, 0.0f, LAYER_UI);

        update_timers(memory);
    }

    if (memory->pause != INT_INFINITY)
    {
        memory->pause--;
    }

    //сортируем qrawQueue

    //to do переделать под insertion sort
    i32 mistakes;
    do
    {
        mistakes = 0;
        for (i32 drawing_index = 1; drawing_index < memory->draw_queue_size; drawing_index++)
        {
            if (memory->draw_queue[drawing_index].layer < memory->draw_queue[drawing_index - 1].layer)
            {
                Drawing drawing = memory->draw_queue[drawing_index];

                memory->draw_queue[drawing_index] = memory->draw_queue[drawing_index - 1];
                memory->draw_queue[drawing_index - 1] = drawing;

                mistakes++;
            }
        }
    } while (mistakes != 0);

    clip_rect_items_render(memory->high_priority_queue, memory, screen, memory->darkness, memory->light_queue, memory->light_queue_size);
    clip_rect_items_render(memory->high_priority_queue, memory, screen, screen, memory->draw_queue, memory->draw_queue_size);
}