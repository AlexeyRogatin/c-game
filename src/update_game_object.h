void apply_deflection(Game_Object *game_object, V2 recent_speed)
{
    if (recent_speed.y != 0 && game_object->speed.y == 0)
    {
        game_object->target_deflection = V2{-recent_speed.y, recent_speed.y};
    }
    if (game_object->speed.y != 0)
    {
        game_object->target_deflection = V2{game_object->speed.y, -game_object->speed.y};
    }

    game_object->deflection += (game_object->target_deflection - game_object->deflection) * 0.3f;

    if (length(game_object->target_deflection - game_object->deflection) < 0.1f)
    {
        game_object->target_deflection = V2{0, 0};
    }
}

typedef struct
{
    bool wall_rotation;
    bool abyss_rotation;
} Walking_AI_product;

Walking_AI_product walking_AI(Game_memory *memory, Game_Object *game_object)
{
    Walking_AI_product result = {0};

    V2 obj_pos = game_object->pos + game_object->collision_box_pos + V2{game_object->collision_box.x * 0.5f * game_object->looking_direction, 0};
    V2 obj_tile_pos = round(obj_pos / TILE_SIZE_PIXELS);

    Tile front_tile = memory->tile_map[get_index(obj_tile_pos + V2{1.0f * game_object->mooving_direction, 0})];
    Tile down_front_tile = memory->tile_map[get_index(obj_tile_pos + V2{1.0f * game_object->mooving_direction, -1})];

    //если доходит до края тайла, то разворачивается
    if ((!down_front_tile.solid || front_tile.solid == Solidness_Type_NORMAL) &&
        obj_pos.x * game_object->mooving_direction >
            (obj_tile_pos.x + 0.4f * game_object->mooving_direction) * TILE_SIZE_PIXELS * game_object->mooving_direction)
    {
        if (front_tile.solid == Solidness_Type_NORMAL)
        {
            result.wall_rotation = true;
        }
        else
        {
            result.abyss_rotation = true;
        }
        game_object->mooving_direction = (Direction)(-game_object->mooving_direction);
    }

    return result;
}

bool standing_on_one_tile_AI(Game_memory *memory, Collisions collisions, Game_Object *game_object)
{
    bool result = false;

    V2 obj_collision_pos = game_object->pos + game_object->collision_box_pos;

    V2 obj_right_tile_pos = round((obj_collision_pos + V2{game_object->collision_box.x * 0.5f, 0}) / TILE_SIZE_PIXELS);
    f32 tile_obj_width = floorf(game_object->hit_box.x / TILE_SIZE_PIXELS);
    V2 obj_left_tile_pos = obj_right_tile_pos - V2{tile_obj_width, 0};
    V2 right_tile_pos = obj_right_tile_pos + V2{1, 0};
    V2 left_tile_pos = obj_left_tile_pos + V2{-1, 0};
    V2 down_right_tile_pos = obj_right_tile_pos + V2{1, -1};
    V2 down_left_tile_pos = obj_left_tile_pos + V2{-1, -1};
    Tile right_tile = memory->tile_map[get_index(right_tile_pos)];
    Tile left_tile = memory->tile_map[get_index(left_tile_pos)];
    Tile down_right_tile = memory->tile_map[get_index(down_right_tile_pos)];
    Tile down_left_tile = memory->tile_map[get_index(down_left_tile_pos)];

    if ((right_tile.solid == Solidness_Type_NORMAL || !down_right_tile.solid) && (left_tile.solid == Solidness_Type_NORMAL || !down_left_tile.solid))
    {
        f32 destination = (obj_right_tile_pos.x + obj_left_tile_pos.x) * TILE_SIZE_PIXELS * 0.5f;
        if (fabs(destination - game_object->pos.x) >= 5)
        {
            game_object->mooving_direction = (Direction)(i32)unit(V2{destination - game_object->pos.x, 0}).x;
        }
        else
        {
            game_object->mooving_direction = Direction_NONE;
            if (!(right_tile.solid == Solidness_Type_NORMAL && left_tile.solid == Solidness_Type_NORMAL))
            {
                if (right_tile.solid == Solidness_Type_NORMAL)
                {
                    game_object->looking_direction = Direction_LEFT;
                }
                else if (left_tile.solid == Solidness_Type_NORMAL)
                {
                    game_object->looking_direction = Direction_RIGHT;
                }
            }
        }
        result = true;
    }

    return result;
}

void update_game_object(Game_memory *memory, i32 index, Input input, Bitmap screen)
{
    Game_Object *game_object = &memory->game_objects[index];

    game_object->delta = V2{0, 0};

#define RUNNING_ACCEL 3.8
#define NORMAL_ACCEL 1.9
#define CROUCHING_ACCEL 0.85
#define JUMP_BUTTON_DURATION 15
#define ADDITIONAL_JUMP_FRAMES 6
#define HANGING_ANIMATION_TIME 16
#define LOOKING_KEY_HOLDING 40
#define CROUCHING_ANIMATION_TIME 8
#define LOOKING_DISTANCE 80
#define COLLISION_EXPANCION 8

    if (game_object->type == Game_Object_PLAYER)
    {
        V2 recent_speed = game_object->speed;

        //хитбокс
        game_object->hit_box_pos = V2{0, -5};
        game_object->hit_box = V2{40, 70};

        //движение игрока
        if (memory->timers[game_object->cant_control_timer] < 0)
        {
            if (input.z.went_down)
            {
                memory->timers[game_object->jump] = JUMP_BUTTON_DURATION;
            }

            game_object->mooving_direction = (Direction)(input.right.is_down - input.left.is_down);
        }
        else
        {
            game_object->mooving_direction = Direction_NONE;
            memory->timers[game_object->jump] = 0;
        }

        //константы ускорения
        if (input.shift.is_down)
        {
            game_object->accel = (f32)NORMAL_ACCEL;
        }
        else
        {
            game_object->accel = (f32)RUNNING_ACCEL;
        }
        if (game_object->condition == Condition_CROUCHING_IDLE || game_object->condition == Condition_CROUCHING_MOOVING)
        {
            game_object->accel = (f32)CROUCHING_ACCEL;
        }

        f32 gravity = -2 * game_object->jump_height / (game_object->jump_duration * game_object->jump_duration);

        //скорость по x
        game_object->speed += {game_object->mooving_direction * game_object->accel, 0};

        //во время прыжка
        if (memory->timers[game_object->jump_timer] > 0)
        {
            if (!input.z.is_down)
            {
                memory->timers[game_object->jump_timer] = 0;
            }
        }

        if (game_object->speed.y > 0 && memory->timers[game_object->jump_timer] <= 0)
        {
            gravity *= 2;
        }

        //гравитация
        game_object->speed.y += gravity;

        if (game_object->speed.y > TILE_SIZE_PIXELS * 0.5)
        {
            game_object->speed.y = TILE_SIZE_PIXELS * 0.5;
        }

        f32 sliding_speed = -((game_object->hit_box.y + TILE_SIZE_PIXELS) * 0.5f) / (f32)(HANGING_ANIMATION_TIME);

        if (game_object->condition >= Condition_HANGING && game_object->condition <= Condition_HANGING_LOOKING_DOWN)
        {
            game_object->speed.x = 0;
            if (memory->timers[game_object->hanging_animation_timer] <= 0)
            {
                game_object->speed.y = 0;
            }
            else
            {
                game_object->speed.y = sliding_speed;
            }
        }

        //трение
        game_object->speed.x *= game_object->friction;

        //направление
        if (!(game_object->condition >= Condition_HANGING && game_object->condition <= Condition_HANGING_LOOKING_DOWN))
        {
            if (game_object->mooving_direction != Direction_NONE)
            {
                game_object->looking_direction = game_object->mooving_direction;
            }
        }

        //столкновения
        Collisions collisions = check_collision(memory, game_object, memory->timers[game_object->walks_throught_planks_timer] > 0);

        Game_Object old_object = *game_object;
        old_object.pos -= game_object->delta;

        //функции движения, связанные с коллизией
        //состояние падает
        Condition supposed_cond = Condition_FALLING;

        //состояние стоит
        if (collisions.y.happened && collisions.y.tile_side == Side_TOP)
        {
            supposed_cond = Condition_IDLE;
            memory->timers[game_object->can_jump] = ADDITIONAL_JUMP_FRAMES;

            //состояние ползком и смотрит вверх
            if (input.down.is_down && !input.up.is_down)
            {
                supposed_cond = Condition_CROUCHING_IDLE;
            }
            else if (input.up.is_down && !input.down.is_down && fabs(game_object->speed.x) <= 1 && memory->timers[game_object->crouching_animation_timer] <= 0)
            {
                supposed_cond = Condition_LOOKING_UP;
            }

            //состояния движения
            if (fabs(game_object->speed.x) > 1 && (supposed_cond == Condition_IDLE || supposed_cond == Condition_CROUCHING_IDLE))
            {

                game_object->moved_through_pixels += (i32)round(game_object->speed.x);

                if (supposed_cond == Condition_IDLE)
                {
                    supposed_cond = Condition_MOOVING;
                }
                else
                {
                    supposed_cond = Condition_CROUCHING_MOOVING;
                }
            }
        }

        if (input.left.went_down || input.right.went_down)
        {
            game_object->moved_through_pixels = 0;
        }

        Collision expanded_collision = {0};
        if (collisions.x.happened)
        {
            expanded_collision = collisions.x;
        }
        else
        {
            expanded_collision = check_expanded_collision(memory, game_object, V2{(f32)game_object->looking_direction * COLLISION_EXPANCION, 0});
        }

        //состояние весит
        if (expanded_collision.happened)
        {
            V2 collided_x_tile_pos = get_tile_pos(expanded_collision.tile_index);
            i8 tile_direction = (i8)unit(V2{(collided_x_tile_pos.x * TILE_SIZE_PIXELS - game_object->pos.x), 0}).x;
            i32 collision_up_tile_index = get_index(V2{collided_x_tile_pos.x, collided_x_tile_pos.y + 1});
            i32 collision_frontup_tile_index = get_index(V2{collided_x_tile_pos.x - game_object->looking_direction, collided_x_tile_pos.y + 1});

            if (memory->tile_map[collision_up_tile_index].solid != Solidness_Type_NORMAL && memory->tile_map[collision_frontup_tile_index].solid != Solidness_Type_NORMAL && tile_direction == game_object->looking_direction)
            {
                if (game_object->delta.y < 0 &&
                    old_object.pos.y + game_object->collision_box_pos.y > collided_x_tile_pos.y * TILE_SIZE_PIXELS - 2 &&
                    old_object.pos.y + game_object->collision_box_pos.y + game_object->delta.y <= collided_x_tile_pos.y * TILE_SIZE_PIXELS - 2)
                {
                    supposed_cond = Condition_HANGING;

                    game_object->hanging_index = expanded_collision.tile_index;

                    game_object->speed = {0, 0};

                    game_object->pos.y = collided_x_tile_pos.y * TILE_SIZE_PIXELS - game_object->collision_box_pos.y - 2;

                    game_object->pos.x = collided_x_tile_pos.x * (f32)TILE_SIZE_PIXELS - game_object->looking_direction * (TILE_SIZE_PIXELS * 0.5f + game_object->collision_box.x * 0.5f + 0.001f) + game_object->collision_box_pos.x;
                }
            }
        }

        //переход на стенку из состояния ползком
        if (collisions.y.happened && memory->tile_map[collisions.y.tile_index].solid == Solidness_Type_NORMAL)
        {
            if (supposed_cond == Condition_CROUCHING_IDLE || supposed_cond == Condition_CROUCHING_MOOVING)
            {
                V2 collided_y_tile_pos = get_tile_pos(collisions.y.tile_index);
                V2 collisionY_front_tile_pos = collided_y_tile_pos + V2{(f32)game_object->looking_direction, 0};
                i32 collisionY_front_tile_pos_index = get_index(collisionY_front_tile_pos);

                if (!memory->tile_map[collisionY_front_tile_pos_index].solid &&
                    ((old_object.pos.x + game_object->delta.x + game_object->collision_box.x * 0.5f + game_object->collision_box_pos.x <= collisionY_front_tile_pos.x * (f32)TILE_SIZE_PIXELS + TILE_SIZE_PIXELS * 0.5f) && (ceilf(old_object.pos.x + game_object->collision_box_pos.x + game_object->collision_box.x * 0.5f) >= collisionY_front_tile_pos.x * (f32)TILE_SIZE_PIXELS + TILE_SIZE_PIXELS * 0.5f) ||
                     (old_object.pos.x + game_object->delta.x - game_object->collision_box.x * 0.5f + game_object->collision_box_pos.x >= collisionY_front_tile_pos.x * (f32)TILE_SIZE_PIXELS - TILE_SIZE_PIXELS * 0.5f) && (floorf(old_object.pos.x + game_object->collision_box_pos.x - game_object->collision_box.x * 0.5f) <= collisionY_front_tile_pos.x * (f32)TILE_SIZE_PIXELS - TILE_SIZE_PIXELS * 0.5f)))
                {
                    game_object->pos.x = collisionY_front_tile_pos.x * (f32)TILE_SIZE_PIXELS - ((TILE_SIZE_PIXELS - game_object->collision_box.x) * 0.5f - 0.001f) * game_object->looking_direction;

                    supposed_cond = Condition_HANGING;

                    game_object->hanging_index = collisions.y.tile_index;

                    game_object->looking_direction = (Direction)(-game_object->looking_direction);

                    game_object->speed.x = 0;
                    game_object->speed.y = sliding_speed;

                    game_object->condition = Condition_IDLE;
                    memory->timers[game_object->hanging_animation_timer] = HANGING_ANIMATION_TIME;
                }
            }
        }

        //состояние смежные с состоянием весит
        if (game_object->condition >= Condition_HANGING && game_object->condition <= Condition_HANGING_LOOKING_DOWN)
        {
            if (input.up.is_down && !input.down.is_down)
            {
                supposed_cond = Condition_HANGING_LOOKING_UP;
            }
            else if (input.down.is_down && !input.up.is_down)
            {
                supposed_cond = Condition_HANGING_LOOKING_DOWN;
            }
            else
            {
                supposed_cond = Condition_HANGING;
            }
        }

        //работа с предполагаемым состоянием (TODO возможно нужно убрать)
        if ((game_object->condition != Condition_HANGING && game_object->condition != Condition_HANGING_LOOKING_DOWN && game_object->condition != Condition_HANGING_LOOKING_UP) ||
            (supposed_cond == Condition_HANGING || supposed_cond == Condition_HANGING_LOOKING_DOWN || supposed_cond == Condition_HANGING_LOOKING_UP))
        {
            //начинаем смотреть вверх и вниз
            if ((game_object->condition != Condition_CROUCHING_IDLE && supposed_cond == Condition_CROUCHING_IDLE) ||
                (game_object->condition != Condition_LOOKING_UP && supposed_cond == Condition_LOOKING_UP) ||
                (game_object->condition != Condition_HANGING_LOOKING_UP && supposed_cond == Condition_HANGING_LOOKING_UP) ||
                (game_object->condition != Condition_HANGING_LOOKING_DOWN && supposed_cond == Condition_HANGING_LOOKING_DOWN))
            {
                memory->timers[game_object->looking_key_held_timer] = LOOKING_KEY_HOLDING;
            }

            //поднимаемся и встаём
            if ((game_object->condition != Condition_CROUCHING_IDLE && game_object->condition != Condition_CROUCHING_MOOVING) && (supposed_cond == Condition_CROUCHING_IDLE || supposed_cond == Condition_CROUCHING_MOOVING))
            {
                if (memory->timers[game_object->crouching_animation_timer] < 0)
                {
                    memory->timers[game_object->crouching_animation_timer] = 0;
                }
                memory->timers[game_object->crouching_animation_timer] = (i32)(CROUCHING_ANIMATION_TIME - memory->timers[game_object->crouching_animation_timer] * 0.5f) * 2;
            }
            if ((game_object->condition == Condition_CROUCHING_IDLE || game_object->condition == Condition_CROUCHING_MOOVING) && (supposed_cond != Condition_CROUCHING_IDLE && supposed_cond != Condition_CROUCHING_MOOVING))
            {
                if (memory->timers[game_object->crouching_animation_timer] < 0)
                {
                    memory->timers[game_object->crouching_animation_timer] = 0;
                }
                memory->timers[game_object->crouching_animation_timer] = (i32)(CROUCHING_ANIMATION_TIME - memory->timers[game_object->crouching_animation_timer] * 0.5f);
            }
            game_object->condition = supposed_cond;
        }

        //что делают состояния
        if (game_object->condition == Condition_IDLE || game_object->condition == Condition_MOOVING)
        {
            if (memory->timers[game_object->crouching_animation_timer] > 0)
            {
                i32 step = (i32)ceil(memory->timers[game_object->crouching_animation_timer] * 0.5);
                game_object->sprite = memory->bitmaps[Bitmap_type_PLAYER_CROUCH_START + step];
            }
            else if (game_object->condition == Condition_IDLE)
            {
                game_object->speed.x *= game_object->friction;
                game_object->sprite = memory->bitmaps[Bitmap_type_PLAYER_IDLE];
            }
            else if (game_object->condition == Condition_MOOVING)
            {
                f32 step_length = 35;
                if (length(game_object->sprite.size) > 600)
                {
                    step_length = 50;
                }
                i8 step = (i8)floor(fabsf((f32)game_object->moved_through_pixels) / step_length);
                while (step > 5)
                {
                    step -= 6;
                }

                game_object->sprite = memory->bitmaps[Bitmap_type_PLAYER_STEP + step];
            }
        }

        if (game_object->condition == Condition_CROUCHING_MOOVING || game_object->condition == Condition_CROUCHING_IDLE)
        {
            if (memory->timers[game_object->crouching_animation_timer] > 0)
            {
                const i8 step = 4 - (i8)ceil(memory->timers[game_object->crouching_animation_timer] * 0.25);
                game_object->sprite = memory->bitmaps[Bitmap_type_PLAYER_CROUCH_START + step];
            }
            else
            {
                if (game_object->condition == Condition_CROUCHING_IDLE)
                {
                    game_object->sprite = memory->bitmaps[Bitmap_type_PLAYER_CROUCH_IDLE];
                }
                if (game_object->condition == Condition_CROUCHING_MOOVING)
                {
                    i8 step = (i8)floor(fabsf((f32)game_object->moved_through_pixels) * 0.1);
                    while (step > 5)
                    {
                        step -= 6;
                    }
                    game_object->sprite = memory->bitmaps[Bitmap_type_PLAYER_CROUCH_STEP + step];
                }
            }

            game_object->hit_box_pos = V2{0, -22.5f};
            game_object->hit_box = V2{40, 35};
        }

        if (game_object->condition == Condition_LOOKING_UP)
        {
            game_object->sprite = memory->bitmaps[Bitmap_type_PLAYER_LOOKING_UP];
        }

        if (game_object->condition == Condition_FALLING)
        {
            game_object->sprite = memory->bitmaps[Bitmap_type_PLAYER_JUMP];
        }

        if (game_object->condition >= Condition_HANGING && game_object->condition <= Condition_HANGING_LOOKING_DOWN)
        {
            if (game_object->condition == Condition_HANGING)
            {
                game_object->sprite = memory->bitmaps[Bitmap_type_PLAYER_HANGING];
            }
            else if (game_object->condition == Condition_HANGING_LOOKING_DOWN)
            {
                game_object->sprite = memory->bitmaps[Bitmap_type_PLAYER_HANGING_DOWN];
            }
            else
            {
                game_object->sprite = memory->bitmaps[Bitmap_type_PLAYER_HANGING_UP];
            }

            // if ((memory->timers[game_object->hanging_animation_timer] <= 1 && (length(game_object->speed) > 0)) ||
            //     (memory->timers[game_object->hanging_animation_timer] > 1 && game_object->speed.y != sliding_speed))
            // {
            //     game_object->condition = Condition_FALLING;
            //     memory->timers[game_object->hanging_animation_timer] = 0;
            // }

            memory->timers[game_object->can_jump] = 2;

            if (game_object->hanging_index)
            {
                Tile tile = memory->tile_map[game_object->hanging_index];
                if (!tile.solid)
                {
                    game_object->condition = Condition_FALLING;
                }
            }
        }
        else
        {
            game_object->hanging_index = 0;
        }

        //камера
        memory->camera.target = game_object->pos;

        //смотрим вниз и вверх
        if ((game_object->condition == Condition_CROUCHING_IDLE || game_object->condition == Condition_HANGING_LOOKING_DOWN) && memory->timers[game_object->looking_key_held_timer] <= 0)
        {
            memory->camera.target.y = game_object->pos.y - screen.size.y / memory->camera.scale.y * 0.5f + f32(LOOKING_DISTANCE);
        }

        if ((game_object->condition == Condition_LOOKING_UP || game_object->condition == Condition_HANGING_LOOKING_UP) && memory->timers[game_object->looking_key_held_timer] <= 0)
        {
            memory->camera.target.y = game_object->pos.y + screen.size.y / memory->camera.scale.y * 0.5f - f32(LOOKING_DISTANCE);
        }

        //искривление
        apply_deflection(game_object, recent_speed);

        game_object->deflection += (game_object->target_deflection - game_object->deflection) * 0.3f;

        if (length(game_object->target_deflection - game_object->deflection) < 0.1f)
        {
            game_object->target_deflection = V2{0, 0};
        }

        //камера
        memory->camera.pos += (memory->camera.target - memory->camera.pos) * 0.25f;

        //границы для камеры
        border_camera(memory, screen);

        //интерфейс

#define BAR_HEIGHT 75.0f * (screen.size.y / 1080) / memory->camera.scale.y
#define BAR_WIDTH 225.0f * (screen.size.x / 1920) / memory->camera.scale.x
#define INTERVAL 25.0f * ((screen.size / V2{1080, 1920}) / memory->camera.scale)

        // V2 UI_shake = V2{0, 0};

        // if (game_object->healthpoints / game_object->max_healthpoints <= 0.5)
        // {
        //     UI_shake = {random_float(&memory->__global_random_state, -10 * (0.5f - game_object->healthpoints / game_object->max_healthpoints), 10 * (0.5f - game_object->healthpoints / game_object->max_healthpoints)), random_float(&memory->__global_random_state, -5 * (0.5f - game_object->healthpoints / game_object->max_healthpoints), 5 * (0.5f - game_object->healthpoints / game_object->max_healthpoints))};
        // }

        add_bitmap_to_queue(memory,
                            memory->camera.pos + V2{-screen.size.x, screen.size.y} / memory->camera.scale * 0.5f + V2{BAR_WIDTH / 2 + INTERVAL.x, -(BAR_HEIGHT / 2 + INTERVAL.y)} - V2{(1 - game_object->healthpoints / game_object->max_healthpoints) * BAR_WIDTH, 0} / 2,
                            V2{game_object->healthpoints / game_object->max_healthpoints * BAR_WIDTH, BAR_HEIGHT},
                            0, memory->bitmaps[Bitmap_type_HEALTH], 1, 0x00000000, LAYER_UI);

        add_bitmap_to_queue(memory,
                            memory->camera.pos + V2{-screen.size.x, screen.size.y} / memory->camera.scale * 0.5f + V2{BAR_WIDTH / 2 + INTERVAL.x, -(BAR_HEIGHT / 2 + INTERVAL.y)},
                            V2{BAR_WIDTH, BAR_HEIGHT}, 0, memory->bitmaps[Bitmap_type_HEALTH_BAR], 1, 0x00000000, LAYER_UI);

        //взаимодействие с тайлами

        if (input.space.went_down)
        {
            i32 tile_index = check_for_interactive_tiles(memory, game_object);
            if (tile_index != -1)
            {
                Tile *tile = &memory->tile_map[tile_index];
                V2 tile_pos = get_tile_pos(tile_index);
                if (tile->type == Tile_Type_EXIT)
                {
                    if (game_object->condition != Condition_FALLING && memory->timers[tile->timer] <= 0)
                    {
                        game_object->pos.x = tile_pos.x * TILE_SIZE_PIXELS;
                        game_object->speed = V2{0, 0};

                        memory->timers[game_object->cant_control_timer] = 60;
                        memory->timers[game_object->invulnerable_timer] = 60;
                        memory->timers[tile->timer] = 60;

                        game_object->layer = LAYER_ON_ON_BACKGROUND;
                    }
                }
            }
        }

        //прыжок
        if (memory->timers[game_object->jump] > 0 && memory->timers[game_object->can_jump] > 0)
        {
            memory->timers[game_object->jump] = 0;
            memory->timers[game_object->can_jump] = 0;
            if (game_object->condition >= Condition_HANGING && game_object->condition <= Condition_HANGING_LOOKING_DOWN)
            {
                if (memory->timers[game_object->hanging_animation_timer] > 0)
                {
                    memory->timers[game_object->jump] = memory->timers[game_object->hanging_animation_timer] + 1;
                }
                else
                {
                    if (!input.down.is_down)
                    {
                        game_object->speed.y = 2 * game_object->jump_height / game_object->jump_duration;
                        memory->timers[game_object->jump_timer] = (i32)ceilf(game_object->jump_duration);
                    }
                    game_object->condition = Condition_FALLING;
                }
            }
            else
            {
                //спрыгиваем с планок
                if (collisions.y.happened)
                {
                    if (memory->tile_map[collisions.y.tile_index].solid == Solidness_Type_UP && input.down.is_down)
                    {

                        memory->timers[game_object->walks_throught_planks_timer] = 2;
                        memory->timers[game_object->can_jump] = 0;
                    }
                    else
                    {
                        game_object->speed.y = 2 * game_object->jump_height / game_object->jump_duration;
                        memory->timers[game_object->jump_timer] = (i32)ceilf(game_object->jump_duration + 1.0f);
                    }
                }
            }
        }

        if (input.s.went_down)
        {
            V2 bomb_pos = game_object->pos + game_object->collision_box_pos - V2{0, 0.25f} * SPRITE_SCALE + V2{game_object->collision_box.x * 0.5f - 6.0f * SPRITE_SCALE * 0.5f, 0.0f} * (f32)game_object->looking_direction;
            Game_Object *bomb = add_game_object(memory, Game_Object_BOMB, bomb_pos);
            if (game_object->condition != Condition_CROUCHING_IDLE && game_object->condition != Condition_CROUCHING_MOOVING)
            {
                if (input.up.is_down)
                {
                    V2 rotated_vector = rotate_vector(V2{THROWING_SPEED, 0}, UP_ANGLE);
                    bomb->speed = (V2{rotated_vector.x * game_object->looking_direction, rotated_vector.y} + game_object->speed) * bomb->mass;
                }
                else if (game_object->condition == Condition_FALLING && input.down.is_down)
                {
                    V2 rotated_vector = rotate_vector(V2{THROWING_SPEED, 0}, DOWN_ANGLE);
                    bomb->speed = (V2{rotated_vector.x * game_object->looking_direction, rotated_vector.y} + game_object->speed) * bomb->mass;
                }
                else
                {
                    V2 rotated_vector = rotate_vector(V2{THROWING_SPEED, 0}, NORMAL_ANGLE);
                    bomb->speed = (V2{rotated_vector.x * game_object->looking_direction, rotated_vector.y} + game_object->speed) * bomb->mass;
                }
            }
            else
            {
                V2 rotated_vector = rotate_vector(V2{TOSSING_SPEED, 0}, NORMAL_ANGLE);
                bomb->speed = (V2{rotated_vector.x * game_object->looking_direction, rotated_vector.y} + game_object->speed) * bomb->mass;
            }
        }

        //оружие
        if (input.x.went_down)
        {
            if ((game_object->condition == Condition_CROUCHING_IDLE || game_object->condition == Condition_CROUCHING_MOOVING || game_object->condition == Condition_HANGING_LOOKING_DOWN))
            {
                //забираем
                if (!game_object->weapon.index)
                {
                    for (i32 obj_index = memory->game_object_count - 1; obj_index >= 0; obj_index--)
                    {
                        Game_Object *weapon = &memory->game_objects[obj_index];
                        if (weapon->exists)
                        {
                            if (weapon->type >= Game_Object_TOY_GUN && weapon->type <= Game_Object_BOMB)
                            {
                                Rect object_rect = Rect{weapon->pos + weapon->collision_box_pos - weapon->collision_box * 0.5 - game_object->collision_box * 0.5, weapon->pos + weapon->collision_box_pos + weapon->collision_box * 0.5 + game_object->collision_box * 0.5};
                                V2 collision_point = V2{game_object->pos.x, game_object->pos.y + game_object->collision_box_pos.y - game_object->collision_box.y * 0.75f};
                                if (collision_point.x >= object_rect.min.x && collision_point.x <= object_rect.max.x &&
                                    collision_point.y >= object_rect.min.y && collision_point.y <= object_rect.max.y)
                                {
                                    game_object->weapon = {weapon->id, obj_index};
                                }
                            }
                        }
                    }
                }
                else
                {
                    //отпускаем
                    Game_Object *weapon = get_game_object(memory, game_object->weapon);
                    game_object->weapon = {0, 0};
                    weapon->speed = (rotate_vector(V2{TOSSING_SPEED * weapon->looking_direction, 0}, NORMAL_ANGLE) + game_object->speed) * weapon->mass;
                }
            }
        }

        if (game_object->weapon.index)
        {
            //держим оружие
            Game_Object *weapon = get_game_object(memory, game_object->weapon);
            if (weapon)
            {
                weapon->pos = game_object->pos + game_object->collision_box_pos + (V2{game_object->collision_box.x - weapon->collision_box.x, 0} * 0.5f - weapon->collision_box_pos) * (f32)game_object->looking_direction;
                weapon->speed = V2{40.0f * game_object->looking_direction, 0};
                if (game_object->layer < LAYER_GAME_OBJECT)
                {
                    weapon->speed = V2{(TILE_SIZE_PIXELS - weapon->collision_box.x) * 0.5f * game_object->looking_direction, 0};
                }
                weapon->looking_direction = game_object->looking_direction;
            }
        }

        //прорисовка игрока

        //хитбокс
        // draw_rect(memory, game_object->pos + game_object->hit_box_pos, game_object->hit_box, 0, 0xFFFF0000, LAYER_FORGROUND);

        f32 alpha = 1.0f;
        if (memory->timers[game_object->invulnerable_timer] > 0 && game_object->layer >= LAYER_GAME_OBJECT)
        {
            if (memory->timers[game_object->invulnerable_timer] % 20 <= 5)
            {

                alpha = (memory->timers[game_object->invulnerable_timer] % 10 + 1) / 11.0f;
            }
            else
            {
                alpha = 1.0f - (memory->timers[game_object->invulnerable_timer] % 10 + 1) / 11.0f;
            }
        }

        V2 sprite_size = V2{16, 16} * SPRITE_SCALE;
        if (game_object->sprite.size.x > game_object->sprite.size.y)
        {
            sprite_size.x = game_object->sprite.size.x / game_object->sprite.size.y * sprite_size.x;
        }
        else if (game_object->sprite.size.x < game_object->sprite.size.y)
        {
            sprite_size.y = game_object->sprite.size.y / game_object->sprite.size.x * sprite_size.y;
        }

        // add_rect_to_queue(memory, game_object->pos + game_object->hit_box_pos, game_object->hit_box, 0, 0xFFFF0000, LAYER_UI);

        add_bitmap_to_queue(memory, game_object->pos + V2{0, game_object->deflection.y * 0.5f},
                            V2{(sprite_size.x + game_object->deflection.x) * game_object->looking_direction,
                               sprite_size.y + game_object->deflection.y},
                            0, game_object->sprite, alpha, 0x00000000, game_object->layer);

        add_light_to_queue(memory, game_object->pos, 300, 0xFF000000);
    }

#define ZOMBIE_ACCEL 6
#define JUMP_LATENCY 25
#define ZOMBIE_VISION_BOX (V2{5.0f, 2.5f} * TILE_SIZE_PIXELS)

    if (game_object->type == Game_Object_ZOMBIE)
    {

        V2 recent_speed = game_object->speed;

        //движение
        game_object->speed.x += game_object->mooving_direction * game_object->accel;

        //прыжок
        if (game_object->condition != Condition_FALLING)
        {
            if (memory->timers[game_object->jump] == 0)
            {
                game_object->speed.y = f32(2 * game_object->jump_height / game_object->jump_duration * random_float(&memory->__global_random_state, 0.4f, 1));
                game_object->accel = ZOMBIE_ACCEL;
            }
        }

        //гравитация
        f32 gravity = -2 * game_object->jump_height / (game_object->jump_duration * game_object->jump_duration);
        game_object->speed.y += gravity;

        //трение
        game_object->speed.x *= game_object->friction;

        //направление
        if (game_object->mooving_direction != Direction_NONE)
        {
            game_object->looking_direction = game_object->mooving_direction;
        }

        //поведение, связанное с тайлами
        Collisions collisions = {};
        collisions = check_collision(memory, game_object, false);

        //если стоит
        if (collisions.y.happened && collisions.y.tile_side == Side_TOP)
        {
            game_object->accel = 0.75f;
            //состояния стоит и движение
            if (fabs(game_object->speed.x) > 1)
            {
                if (game_object->moved_through_pixels > 0 && game_object->speed.x < 0 || game_object->moved_through_pixels < 0 && game_object->speed.x > 0)
                {
                    game_object->moved_through_pixels = 0;
                }
                game_object->moved_through_pixels += (i32)roundf(game_object->speed.x);
                game_object->condition = Condition_MOOVING;
            }
            else
            {
                game_object->condition = Condition_IDLE;
            }

            //если не заряжает прыжок
            if (memory->timers[game_object->jump] < 0)
            {
                if (!standing_on_one_tile_AI(memory, collisions, game_object))
                {
                    walking_AI(memory, game_object);
                }
            }
            else
            {
                game_object->mooving_direction = Direction_NONE;
            }
        }
        else
        {
            game_object->condition = Condition_FALLING;
            memory->timers[game_object->jump] = -1;
        }

        //эффекты состояний
        if (game_object->condition == Condition_IDLE)
        {
            game_object->speed.x *= game_object->friction;
            game_object->sprite = memory->bitmaps[Bitmap_type_ZOMBIE_IDLE];
        }

        if (game_object->condition == Condition_MOOVING)
        {
            i8 step = (i8)floor(fabsf((f32)game_object->moved_through_pixels) * 0.05);
            while (step > 5)
            {
                step -= 6;
            }

            game_object->sprite = memory->bitmaps[Bitmap_type_ZOMBIE_STEP + step];
        }

        if (game_object->condition == Condition_FALLING)
        {
            game_object->mooving_direction = game_object->looking_direction;
            game_object->sprite = memory->bitmaps[Bitmap_type_ZOMBIE_IDLE];
        }

        //обзор
        Game_Object_Type triggers[1];
        triggers[0] = Game_Object_PLAYER;

        i32 trigger_index = -1;

        if (check_vision_box(memory, &trigger_index, game_object->pos, game_object->pos + V2{(ZOMBIE_VISION_BOX.x + game_object->hit_box.x) * (f32)(game_object->looking_direction), 0} * 0.5, ZOMBIE_VISION_BOX, triggers, 1, false, false) &&
            memory->timers[game_object->jump] < 0)
        {
            memory->timers[game_object->jump] = JUMP_LATENCY;
        }

        //искривление
        apply_deflection(game_object, recent_speed);

        // draw_rect(game_object->pos, game_object->collision_box, 0, 0xFF00FF00, LAYER_FORGROUND);
        // add_rect_to_queue(memory, game_object->pos + game_object->hit_box_pos, game_object->hit_box, 0, 0xFFFF0000, LAYER_FORGROUND);
        add_bitmap_to_queue(memory, game_object->pos + V2{0, game_object->deflection.y * 0.5f}, V2{(game_object->sprite.size.x * SPRITE_SCALE + game_object->deflection.x) * game_object->looking_direction, (game_object->sprite.size.y * SPRITE_SCALE + game_object->deflection.y)}, 0, game_object->sprite, 1, 0x00000000, game_object->layer);
    }

#define RAT_VISION_BOX (V2{5.0f, 0.8f} * TILE_SIZE_PIXELS)

    if (game_object->type == Game_Object_RAT)
    {

        V2 recent_speed = game_object->speed;

        if (memory->timers[game_object->jump] >= 0)
        {
            game_object->accel = 2.75f;
        }
        else
        {
            game_object->accel = 0.75f;
        }
        //движение
        game_object->speed.x += game_object->mooving_direction * game_object->accel;

        //гравитация
        f32 gravity = -2 * game_object->jump_height / (game_object->jump_duration * game_object->jump_duration);
        game_object->speed.y += gravity;

        //трение
        game_object->speed.x *= game_object->friction;

        //направление
        if (game_object->mooving_direction != Direction_NONE)
        {
            game_object->looking_direction = game_object->mooving_direction;
        }

        //поведение, связанное с тайлами
        Collisions collisions = {};
        collisions = check_collision(memory, game_object, false);

        //если стоит
        if (collisions.y.happened && collisions.y.tile_side == Side_TOP)
        {
            game_object->accel = 0.75f;
            //состояния стоит и движение
            if (fabs(game_object->speed.x) > 1)
            {
                if (game_object->moved_through_pixels > 0 && game_object->speed.x < 0 || game_object->moved_through_pixels < 0 && game_object->speed.x > 0)
                {
                    game_object->moved_through_pixels = 0;
                }
                game_object->moved_through_pixels += (i32)roundf(game_object->speed.x);
                game_object->condition = Condition_MOOVING;
            }
            else
            {
                game_object->condition = Condition_IDLE;
            }

            Walking_AI_product product = walking_AI(memory, game_object);
            if (memory->timers[game_object->jump] < 0)
            {
                standing_on_one_tile_AI(memory, collisions, game_object);
            }
            else
            {
                if (product.abyss_rotation)
                {
                    game_object->mooving_direction = (Direction)-game_object->mooving_direction;
                }

                if (product.wall_rotation)
                {
                    memory->timers[game_object->jump] = -1;
                }
            }
        }
        else
        {
            game_object->condition = Condition_FALLING;
        }

        //эффекты состояний
        if (game_object->condition == Condition_IDLE)
        {
            game_object->speed.x *= game_object->friction;
            game_object->sprite = memory->bitmaps[Bitmap_type_RAT_IDLE];
        }

        if (game_object->condition == Condition_MOOVING)
        {
            i8 step = (i8)floor(fabsf((f32)game_object->moved_through_pixels) * 0.05);
            while (step > 1)
            {
                step -= 2;
            }

            game_object->sprite = memory->bitmaps[Bitmap_type_RAT_STEP + step];
        }

        if (game_object->condition == Condition_FALLING)
        {
            game_object->mooving_direction = game_object->looking_direction;
            game_object->sprite = memory->bitmaps[Bitmap_type_RAT_IDLE];
        }

        //обзор
        Game_Object_Type triggers[1];
        triggers[0] = Game_Object_PLAYER;

        i32 trigger_index = -1;

        if (check_vision_box(memory, &trigger_index, game_object->pos, game_object->pos + V2{(RAT_VISION_BOX.x + game_object->hit_box.x) * (f32)(game_object->looking_direction), 0} * 0.5f, RAT_VISION_BOX, triggers, 1, false, false))
        {
            memory->timers[game_object->jump] = 180;
            game_object->mooving_direction = game_object->looking_direction;
        }

        //искривление
        apply_deflection(game_object, recent_speed);

        // draw_rect(game_object->pos, game_object->collision_box, 0, 0xFF00FF00, LAYER_FORGROUND);
        // add_rect_to_queue(memory, game_object->pos + game_object->hit_box_pos, game_object->hit_box, 0, 0xFFFF0000, LAYER_FORGROUND);
        add_bitmap_to_queue(memory, game_object->pos + V2{0, game_object->deflection.y * 0.5f}, V2{(game_object->sprite.size.x * SPRITE_SCALE + game_object->deflection.x) * game_object->looking_direction, (game_object->sprite.size.y * SPRITE_SCALE + game_object->deflection.y)}, 0, game_object->sprite, 1, 0x00000000, game_object->layer);
    }

    if (game_object->type >= Game_Object_TOY_GUN_BULLET && game_object->type <= Game_Object_TOY_GUN_BULLET)
    {
        switch (game_object->type)
        {
        case Game_Object_TOY_GUN_BULLET:
        {
            game_object->speed.y += game_object->gravity;
            game_object->speed *= game_object->friction;

            if (memory->timers[game_object->cant_control_timer] <= 0)
            {
                f32 tan = game_object->speed.y / game_object->speed.x;

                game_object->angle = (f32)atan(tan);
            }
            else
            {
                game_object->angle = game_object->speed.x * 0.5f;
            }
        }
        break;
        }

        Collisions collisions = check_collision(memory, game_object, false);
        if (memory->timers[game_object->cant_control_timer] == -1)
        {
            memory->timers[game_object->cant_control_timer] = 600;
        }
        if ((collisions.x.happened || collisions.y.happened) && memory->timers[game_object->cant_control_timer] <= 0)
        {
            memory->timers[game_object->cant_control_timer] = 0;
        }

        f32 alpha = 1;
        if (memory->timers[game_object->cant_control_timer] > 0)
        {
            alpha = memory->timers[game_object->cant_control_timer] / 600.0f;
        }

        if (memory->timers[game_object->cant_control_timer] == 1)
        {
            game_object->exists = false;
        }

        add_bitmap_to_queue(memory, game_object->pos, V2{(f32)TILE_SIZE_PIXELS * game_object->looking_direction, TILE_SIZE_PIXELS}, game_object->angle, memory->bitmaps[Bitmap_type_TOY_GUN_BULLET], alpha, 0x00000000, LAYER_GAME_OBJECT);
    }

    if (game_object->type >= Game_Object_TOY_GUN && game_object->type <= Game_Object_TOY_GUN)
    {
        Game_Object *player = get_game_object(memory, memory->player);

        f32 gravity = game_object->gravity;

        if (player && player->weapon.index == index)
        {
            gravity = 0.0f;
            game_object->layer = player->layer;
        }

        game_object->speed.y += gravity;

        game_object->speed.x *= game_object->friction;

        check_collision(memory, game_object, false);

        if (input.x.went_down && player)
        {
            if (!(player->condition == Condition_CROUCHING_IDLE || player->condition == Condition_CROUCHING_MOOVING || player->condition == Condition_HANGING_LOOKING_DOWN))
            {
                if (player->weapon.index == index)
                {
                    if (memory->timers[player->cant_control_timer] < 0 && memory->timers[game_object->cant_control_timer] < 0)
                    {
                        Game_Object *bullet = add_game_object(memory, Game_Object_TOY_GUN_BULLET, game_object->pos + game_object->collision_box_pos + V2{0, 3});
                        bullet->speed = V2{45.0f * game_object->looking_direction * random_float(&memory->__global_random_state, 0.75f, 1.0f), random_float(&memory->__global_random_state, -1, 5)};
                        bullet->looking_direction = game_object->looking_direction;

                        memory->timers[game_object->cant_control_timer] = 10;
                    }
                }
            }
        }

        add_bitmap_to_queue(memory, game_object->pos, V2{(f32)TILE_SIZE_PIXELS * game_object->looking_direction, TILE_SIZE_PIXELS}, 0, game_object->sprite, 1, 0x00000000, game_object->layer);
    }

    if (game_object->type == Game_Object_BOMB)
    {
        Game_Object *player = get_game_object(memory, memory->player);

        f32 gravity = game_object->gravity;

        game_object->speed.y += gravity;

        game_object->speed.x *= game_object->friction;

        check_collision(memory, game_object, false);

        game_object->angle -= game_object->speed.x * 0.05f;

        if (player && player->weapon.index == index)
        {
            gravity = 0.0f;
            game_object->speed = V2{0, 0};
            game_object->angle = 0.0f;
            game_object->layer = player->layer;
        }

        if (input.x.went_down && player)
        {
            if (!(player->condition == Condition_CROUCHING_IDLE || player->condition == Condition_CROUCHING_MOOVING || player->condition == Condition_HANGING_LOOKING_DOWN))
            {
                if (player->weapon.index == index)
                {

                    player->weapon = {0, 0};
                    if (input.up.is_down)
                    {
                        V2 rotated_speed = rotate_vector(V2{THROWING_SPEED, 0}, UP_ANGLE);
                        game_object->speed = (V2{rotated_speed.x * game_object->looking_direction, rotated_speed.y} + player->speed) * game_object->mass;
                    }
                    else if (player->condition == Condition_FALLING && input.down.is_down)
                    {
                        V2 rotated_speed = rotate_vector(V2{THROWING_SPEED, 0}, DOWN_ANGLE);
                        game_object->speed = (V2{rotated_speed.x * game_object->looking_direction, rotated_speed.y} + player->speed) * game_object->mass;
                    }
                    else
                    {
                        V2 rotated_speed = rotate_vector(V2{THROWING_SPEED, 0}, NORMAL_ANGLE);
                        game_object->speed = (V2{rotated_speed.x * game_object->looking_direction, rotated_speed.y} + player->speed) * game_object->mass;
                    }
                }
            }
        }

        if (memory->timers[game_object->invulnerable_timer] == 0)
        {
            game_object->exists = false;
            f32 explosion_radius = TILE_SIZE_PIXELS * 2.5f;
            V2 tile_min = floor((game_object->pos - V2{explosion_radius, explosion_radius}) / TILE_SIZE_PIXELS);
            V2 tile_max = ceil((game_object->pos + V2{explosion_radius, explosion_radius}) / TILE_SIZE_PIXELS);
            Rect explosion_rect = intersect(Rect{tile_min, tile_max}, Rect{V2{0, 0}, V2{CHUNK_COUNT_X * CHUNK_SIZE_X + BORDER_SIZE * 2, CHUNK_COUNT_Y * CHUNK_SIZE_Y + BORDER_SIZE * 2}});
            for (i32 y = (i32)explosion_rect.min.y; y <= (i32)explosion_rect.max.y; y++)
            {
                for (i32 x = (i32)explosion_rect.min.x; x <= (i32)explosion_rect.max.x; x++)
                {
                    V2 tile_pos = V2{(f32)x, (f32)y};
                    Tile *tile = &memory->tile_map[get_index(tile_pos)];
                    if (tile->solid && tile->type != Tile_Type_BORDER && tile->type != Tile_Type_UNBREAKABLE_PLANK)
                    {
                        if (distance_between_points(game_object->pos, tile_pos * TILE_SIZE_PIXELS) < explosion_radius)
                        {
                            tile->type = Tile_Type_NONE;
                            tile->sprite = memory->bitmaps[Bitmap_type_BACKGROUND];
                            tile->timer = -1;
                            tile->solid = (Solidness_Type)0;
                        }
                    }
                }
            }

            update_map_bitmap(memory, explosion_rect);

#define EXPLOSION_BOOST 40

            for (i32 obj_index = 0; obj_index < memory->game_object_count; obj_index++)
            {
                Game_Object *other_object = &memory->game_objects[obj_index];
                f32 distance = distance_between_points(game_object->pos, other_object->pos);
                if (distance <= explosion_radius)
                {
                    if (other_object->max_healthpoints)
                    {
                        deal_damage(memory, game_object, other_object, 10.0f);
                    }
                    else
                    {
                        other_object->speed = unit(other_object->pos - game_object->pos) * EXPLOSION_BOOST * (1.0f - distance / explosion_radius) * other_object->mass;
                    }
                    if (other_object->type == Game_Object_BOMB)
                    {
                        memory->timers[other_object->invulnerable_timer] = random_int(&memory->__global_random_state, 1, 15);
                    }
                }
            }

            add_screen_shake(memory, 10, 4.0f);
        }

        f32 transition_lvl = min((memory->timers[game_object->invulnerable_timer] / 30.0f), 1.0f);
        ARGB color = {0xFFFF0000};
        color.a = (u8)(color.a * min(1.0f - ((memory->timers[game_object->invulnerable_timer] - 30.0f) / 210.0f), 1.0f));

        if (transition_lvl == 1.0f)
        {
            V2 fuze_pos = rotate_vector(V2{0.0f, 3.5f} / 16.0f * TILE_SIZE_PIXELS, game_object->angle);
            add_light_to_queue(memory, game_object->pos + fuze_pos + V2{random_float(&memory->__global_random_state, -1.0f, 1.0f), random_float(&memory->__global_random_state, -1.0f, 1.0f)}, 15 + random_float(&memory->__global_random_state, -2.0f, 2.0f), 0xFFFF3300);
        }

        // draw_rect(memory, game_object->pos + game_object->collision_box_pos, game_object->collision_box, 0, 0xFF00FF00, LAYER_UI);
        add_bitmap_to_queue(memory,
                            game_object->pos + V2{random_float(&memory->__global_random_state, -4.0f, 4.0f), random_float(&memory->__global_random_state, -4.0f, 4.0f)} * (1.0f - transition_lvl),
                            V2{(f32)TILE_SIZE_PIXELS * game_object->looking_direction, TILE_SIZE_PIXELS} + V2{TILE_SIZE_PIXELS * (f32)game_object->looking_direction, TILE_SIZE_PIXELS} * (1.0f - transition_lvl), game_object->angle, game_object->sprite, 1, color.argb, game_object->layer);
    }
}