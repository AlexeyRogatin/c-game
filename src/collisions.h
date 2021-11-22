bool test_side(f32 wall_x, f32 obj_speed_x, f32 obj_speed_y, f32 obj_rel_pos_x, f32 obj_rel_pos_y, f32 *min_time, f32 min_y, f32 max_y)
{
    bool hit = false;

    if (obj_speed_x != 0.0f)
    {
        f32 time = (wall_x - obj_rel_pos_x) / obj_speed_x;
        f32 y = obj_rel_pos_y + time * obj_speed_y;
        if ((time >= 0.0f) && (time < *min_time) && unit(V2{obj_speed_x, 0}).x != unit(V2{wall_x, 0}).x)
        {
            if (y >= min_y && y <= max_y)
            {
                *min_time = time;
                hit = true;
            }
        }
    }

    return hit;
}

Collisions check_collision(Game_memory *memory, Game_Object *game_object, bool ignores_up_collision)
{
    Collisions collisions = {0};

    V2 real_collision_box_pos = V2{game_object->collision_box_pos.x * game_object->looking_direction, game_object->collision_box_pos.y};
    V2 old_pos = game_object->pos + real_collision_box_pos;
    V2 new_pos = old_pos + game_object->speed;
    V2 start_tile = min(old_pos - game_object->collision_box * 0.5, new_pos - game_object->collision_box * 0.5);
    V2 finish_tile = max(old_pos + game_object->collision_box * 0.5, new_pos + game_object->collision_box * 0.5);

    start_tile = floor(start_tile / TILE_SIZE_PIXELS);
    finish_tile = ceil(finish_tile / TILE_SIZE_PIXELS);

    f32 remaining_time = 1.0f;
    V2 total_speed = game_object->speed;
    for (i32 iterations = 0; iterations < 4 && remaining_time > 0.0f; iterations++)
    {
        f32 min_time = 1.0f;
        V2 wall_normal = V2{};

        Collisions collision_iteration = {0};
        for (i32 tile_y = (i32)start_tile.y; tile_y <= finish_tile.y; tile_y++)
        {
            for (i32 tile_x = (i32)start_tile.x; tile_x <= finish_tile.x; tile_x++)
            {
                i32 index = get_index(V2{(f32)tile_x, (f32)tile_y});
                Tile tile = memory->tile_map[index];
                if (tile.solid)
                {
                    V2 tile_pos = V2{(f32)tile_x, (f32)tile_y} * TILE_SIZE_PIXELS;
                    V2 tile_min = (V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS} + game_object->collision_box) * (-0.5);
                    V2 tile_max = (V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS} + game_object->collision_box) * 0.5;
                    if (tile.solid == Solidness_Type_DOWN_SPIKES)
                    {
                        tile_max.y -= TILE_SIZE_PIXELS * 0.5;
                    }

                    V2 obj_rel_pos = game_object->pos + real_collision_box_pos - tile_pos;

                    if (tile.solid != Solidness_Type_UP)
                    {
                        if (test_side(tile_min.x, total_speed.x, total_speed.y, obj_rel_pos.x, obj_rel_pos.y, &min_time, tile_min.y, tile_max.y))
                        {
                            wall_normal = V2{-1, 0};
                            collision_iteration.x.happened = true;
                            collision_iteration.y.happened = false;
                            collision_iteration.x.tile_index = index;
                            collision_iteration.x.tile_side = Side_LEFT;
                        }
                        if (test_side(tile_max.x, total_speed.x, total_speed.y, obj_rel_pos.x, obj_rel_pos.y, &min_time, tile_min.y, tile_max.y))
                        {
                            wall_normal = V2{1, 0};
                            collision_iteration.x.happened = true;
                            collision_iteration.y.happened = false;
                            collision_iteration.x.tile_index = index;
                            collision_iteration.x.tile_side = Side_RIGHT;
                        }
                        if (test_side(tile_min.y, total_speed.y, total_speed.x, obj_rel_pos.y, obj_rel_pos.x, &min_time, tile_min.x, tile_max.x))
                        {
                            wall_normal = V2{0, -1};
                            collision_iteration.y.happened = true;
                            collision_iteration.x.happened = false;
                            collision_iteration.y.tile_index = index;
                            collision_iteration.y.tile_side = Side_BOTTOM;
                        }
                    }
                    if (!(ignores_up_collision && tile.solid == Solidness_Type_UP))
                    {
                        if (test_side(tile_max.y, total_speed.y, total_speed.x, obj_rel_pos.y, obj_rel_pos.x, &min_time, tile_min.x, tile_max.x))
                        {
                            wall_normal = V2{0, 1};
                            collision_iteration.y.happened = true;
                            collision_iteration.x.happened = false;
                            collision_iteration.y.tile_index = index;
                            collision_iteration.y.tile_side = Side_TOP;
                        }
                    }
                }
            }
        }

        if (collision_iteration.x.happened)
        {
            collisions.x = collision_iteration.x;
        }
        if (collision_iteration.y.happened)
        {
            collisions.y = collision_iteration.y;
        }

        f32 time_epsilon = 0.01f;
        if (min_time < 1.0f)
        {
            min_time = max(0.0f, min_time - time_epsilon);
        }

        //передвигаем персонажа до точки столкновения
        game_object->pos += min_time * total_speed;

        //убираем часть скорости, которую мы уже прошли
        total_speed *= 1.0f - min_time;

        //удаляем скорость столкновения
        game_object->speed -= dot(game_object->speed, wall_normal) * wall_normal * (game_object->bounce + 1.0f);
        total_speed -= dot(total_speed, wall_normal) * wall_normal;

        remaining_time -= min_time * remaining_time;
    }
    game_object->delta += game_object->pos - (old_pos - game_object->collision_box_pos);

    return collisions;
}

#define DISTANT_HANGING_VALUE 7

Collision check_expanded_collision(Game_memory *memory, Game_Object *game_object, V2 expansion)
{
    Collision expanded_collision = {0};

    V2 total_speed = expansion;

    V2 real_collision_box_pos = V2{game_object->collision_box_pos.x * game_object->looking_direction, game_object->collision_box_pos.y};
    V2 old_pos = game_object->pos + real_collision_box_pos;
    V2 new_pos = old_pos + total_speed;
    V2 start_tile = min(old_pos - game_object->collision_box * 0.5, new_pos - game_object->collision_box * 0.5);
    V2 finish_tile = max(old_pos + game_object->collision_box * 0.5, new_pos + game_object->collision_box * 0.5);

    start_tile = floor(start_tile / TILE_SIZE_PIXELS);
    finish_tile = ceil(finish_tile / TILE_SIZE_PIXELS);

    for (i32 tile_y = (i32)start_tile.y; tile_y <= finish_tile.y; tile_y++)
    {
        for (i32 tile_x = (i32)start_tile.x; tile_x <= finish_tile.x; tile_x++)
        {
            f32 min_time = 1.0f;
            i32 index = get_index(V2{(f32)tile_x, (f32)tile_y});
            Tile tile = memory->tile_map[index];
            if (tile.solid == Solidness_Type_NORMAL)
            {
                V2 tile_pos = V2{(f32)tile_x, (f32)tile_y} * TILE_SIZE_PIXELS;
                V2 tile_min = (V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS} + game_object->collision_box) * (-0.5);
                V2 tile_max = (V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS} + game_object->collision_box) * 0.5;

                V2 obj_rel_pos = game_object->pos + real_collision_box_pos - tile_pos;

                if (test_side(tile_min.x, total_speed.x, total_speed.y, obj_rel_pos.x, obj_rel_pos.y, &min_time, tile_min.y, tile_max.y))
                {
                    expanded_collision.happened = true;
                    expanded_collision.tile_index = index;
                    expanded_collision.tile_side = Side_LEFT;
                }
                if (test_side(tile_max.x, total_speed.x, total_speed.y, obj_rel_pos.x, obj_rel_pos.y, &min_time, tile_min.y, tile_max.y))
                {
                    expanded_collision.happened = true;
                    expanded_collision.tile_index = index;
                    expanded_collision.tile_side = Side_RIGHT;
                }
            }
        }
    }

    return expanded_collision;
}

bool deal_damage(Game_memory *memory, Game_Object *dealing_object, Game_Object *taking_object, f32 damage, bool ignores_invelnerability = false)
{
    bool result = false;
    if (memory->timers[taking_object->invulnerable_timer] <= 0 && memory->timers[dealing_object->cant_control_timer] <= 0)
    {
        taking_object->healthpoints -= damage;
        result = true;

        if (taking_object->healthpoints <= 0)
        {
            taking_object->exists = false;
            if (taking_object->weapon.index)
            {
                Game_Object *weapon = get_game_object(memory, taking_object->weapon);
                if (weapon)
                {
                    weapon->speed = taking_object->speed * 0.5;
                    taking_object->weapon.id = 0;
                }
            }
        }
    }
    return result;
}

typedef struct
{
    Side side;
    Game_Object *object;
} Object_Collision;

Object_Collision check_object_collision(Game_memory *memory, Game_Object *game_object, Game_Object_Type *triggering_objects, i32 triggering_object_count)
{
    V2 real_hit_box_pos = V2{game_object->hit_box_pos.x * game_object->looking_direction, game_object->hit_box_pos.y};
    V2 obj_pos = game_object->pos + real_hit_box_pos - game_object->delta;
    Object_Collision collision = {};
    collision.side = (Side)-3;

    f32 min_time = 1.0f;
    for (i32 game_object_index = 0; game_object_index < memory->game_object_count; game_object_index++)
    {
        for (i32 trigger_index = 0; trigger_index < triggering_object_count; trigger_index++)
        {
            Game_Object *trigger_object = &memory->game_objects[game_object_index];
            if (trigger_object->exists && trigger_object->type == triggering_objects[trigger_index])
            {

                V2 trigger_min = (trigger_object->hit_box + game_object->hit_box) * (-0.5f);
                V2 trigger_max = (trigger_object->hit_box + game_object->hit_box) * 0.5f;

                V2 trigger_real_hit_box_pos = V2{trigger_object->hit_box_pos.x * trigger_object->looking_direction, trigger_object->hit_box_pos.y};

                V2 obj_rel_pos = obj_pos - (trigger_object->pos - trigger_object->delta + trigger_real_hit_box_pos);

                if (obj_rel_pos.x > trigger_min.x && obj_rel_pos.x < trigger_max.x &&
                    obj_rel_pos.y > trigger_min.y && obj_rel_pos.y < trigger_max.y)
                {
                    collision.side = Side_INNER;
                    collision.object = trigger_object;
                }
                else
                {
                    if (test_side(trigger_min.x, game_object->delta.x - trigger_object->delta.x, game_object->delta.y - trigger_object->delta.y, obj_rel_pos.x, obj_rel_pos.y, &min_time, trigger_min.y, trigger_max.y))
                    {
                        collision.side = Side_LEFT;
                        collision.object = trigger_object;
                    }
                    if (test_side(trigger_max.x, game_object->delta.x - trigger_object->delta.x, game_object->delta.y - trigger_object->delta.y, obj_rel_pos.x, obj_rel_pos.y, &min_time, trigger_min.y, trigger_max.y))
                    {
                        collision.side = Side_RIGHT;
                        collision.object = trigger_object;
                    }
                    if (test_side(trigger_min.y, game_object->delta.y - trigger_object->delta.y, game_object->delta.x - trigger_object->delta.x, obj_rel_pos.y, obj_rel_pos.x, &min_time, trigger_min.x, trigger_max.x))
                    {
                        collision.side = Side_BOTTOM;
                        collision.object = trigger_object;
                    }
                    if (test_side(trigger_max.y, game_object->delta.y - trigger_object->delta.y, game_object->delta.x - trigger_object->delta.x, obj_rel_pos.y, obj_rel_pos.x, &min_time, trigger_min.x, trigger_max.x))
                    {
                        collision.side = Side_TOP;
                        collision.object = trigger_object;
                    }
                }
            }
        }
    }
    return collision;
}

bool check_vision_box(Game_memory *memory, i32 *trigger_index, V2 vision_point, V2 vision_pos, V2 vision_size, Game_Object_Type *triggering_objects, i32 triggering_objects_count, bool goes_through_walls, bool draw)
{
    bool vision_triggered = false;
    for (i32 game_object_index = 0; game_object_index < memory->game_object_count; game_object_index++)
    {
        for (i32 triggers_index = 0; triggers_index < triggering_objects_count; triggers_index++)
        {
            Game_Object game_object = memory->game_objects[game_object_index];
            if (game_object.type == triggering_objects[triggers_index])
            {
                if (vision_pos.x - vision_size.x * 0.5 - game_object.hit_box.x * 0.5 < game_object.pos.x && vision_pos.x + vision_size.x * 0.5 + game_object.hit_box.x * 0.5 > game_object.pos.x &&
                    vision_pos.y - vision_size.y * 0.5 - game_object.hit_box.y * 0.5 < game_object.pos.y && vision_pos.y + vision_size.y * 0.5 + game_object.hit_box.y * 0.5 > game_object.pos.y)
                {
                    vision_triggered = true;
                    trigger_index = &game_object_index;
                    if (!goes_through_walls)
                    {
                        f32 k = (vision_point.y - game_object.pos.y) / (vision_point.x - game_object.pos.x);
                        f32 b = vision_point.y - k * vision_point.x;
                        //y=kx+b
                        V2 min_point = round(V2{min(vision_point.x, game_object.pos.x), min(vision_point.y, game_object.pos.y)} / TILE_SIZE_PIXELS);
                        V2 max_point = round(V2{max(vision_point.x, game_object.pos.x), max(vision_point.y, game_object.pos.y)} / TILE_SIZE_PIXELS);
                        for (i32 y = (i32)min_point.y; y <= max_point.y; y++)
                        {
                            for (i32 x = (i32)min_point.x; x <= max_point.x; x++)
                            {
                                V2 tile_pos = V2{(f32)x, (f32)y};
                                if (memory->tile_map[get_index(tile_pos)].solid)
                                {
                                    tile_pos *= TILE_SIZE_PIXELS;
                                    i32 collisions = 0;
                                    f32 x1 = (f32)(((tile_pos.y - TILE_SIZE_PIXELS * 0.5f) - b) / k);
                                    f32 x2 = (f32)(((tile_pos.y + TILE_SIZE_PIXELS * 0.5f) - b) / k);
                                    f32 y1 = (f32)(k * (tile_pos.x - TILE_SIZE_PIXELS * 0.5f) + b);
                                    f32 y2 = (f32)(k * (tile_pos.x + TILE_SIZE_PIXELS * 0.5f) + b);
                                    bool bx1 = x1 >= tile_pos.x - TILE_SIZE_PIXELS * 0.5f && x1 <= tile_pos.x + TILE_SIZE_PIXELS * 0.5f;
                                    bool bx2 = x2 >= tile_pos.x - TILE_SIZE_PIXELS * 0.5f && x2 <= tile_pos.x + TILE_SIZE_PIXELS * 0.5f;
                                    bool by1 = y1 >= tile_pos.y - TILE_SIZE_PIXELS * 0.5f && y1 <= tile_pos.y + TILE_SIZE_PIXELS * 0.5f;
                                    bool by2 = y2 >= tile_pos.y - TILE_SIZE_PIXELS * 0.5f && y2 <= tile_pos.y + TILE_SIZE_PIXELS * 0.5f;
                                    if (bx1 || bx2 || by1 || by2)
                                    {
                                        vision_triggered = false;
                                        goto Result;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
Result:
    if (draw)
    {
        add_rect_to_queue(memory, vision_pos, vision_size, 0, 0xFF00FF00, LAYER_FORGROUND);
    }
    return vision_triggered;
}

#define JUMP_ON_ENEMY_BOOST 12
#define KNOCKBACK 30

void check_hits(Game_memory *memory, Game_Object *game_object)
{
    if (game_object->type == Game_Object_RAT)
    {

        Game_Object_Type triggers[] = {
            Game_Object_ZOMBIE,
            Game_Object_PLAYER,
        };
        Object_Collision obj_collision = check_object_collision(memory, game_object, triggers, ARRAY_COUNT(triggers));

        if (obj_collision.side != -3)
        {
            if (obj_collision.side != Side_BOTTOM)
            {
                V2 save_speed = obj_collision.object->speed;
                V2 save_delta = obj_collision.object->delta;
                obj_collision.object->speed = V2{0, 0};
                obj_collision.object->condition = Condition_FALLING;
                if (fabs(obj_collision.side) == 1)
                {
                    obj_collision.object->speed.x = game_object->pos.x + (game_object->hit_box.x + obj_collision.object->hit_box.x + 0.002f) * 0.5f * (-obj_collision.side) - obj_collision.object->pos.x;
                    obj_collision.object->condition = Condition_FALLING;
                    save_speed.x = obj_collision.object->speed.x;
                }
                else if (obj_collision.side == Side_TOP)
                {
                    save_speed.y = obj_collision.object->speed.y;
                    obj_collision.object->speed.y = game_object->pos.y + (game_object->hit_box.y + obj_collision.object->hit_box.y + 0.002f) * 0.5f * (-1.0f) - obj_collision.object->pos.y;
                }
                Collisions other_collisions = check_collision(memory, obj_collision.object, false);

                obj_collision.object->delta = obj_collision.object->delta - save_delta;

                Game_Object_Type rat_trigger[] = {Game_Object_RAT};
                Object_Collision other_rat_collision = check_object_collision(memory, obj_collision.object, rat_trigger, ARRAY_COUNT(rat_trigger));

                if (other_collisions.x.happened || other_collisions.y.happened || (other_rat_collision.side != -3 && other_rat_collision.object != game_object))
                {
                    deal_damage(memory, game_object, obj_collision.object, obj_collision.object->healthpoints, true);
                }
                obj_collision.object->speed = save_speed;
                obj_collision.object->delta += save_delta;
            }
        }
    }

    if (game_object->type == Game_Object_PLAYER)
    {
        //столкновения и движение
        Game_Object_Type triggers[] = {
            Game_Object_ZOMBIE,
            Game_Object_RAT,
        };
        Object_Collision obj_collision = check_object_collision(memory, game_object, triggers, sizeof(triggers) / sizeof(Game_Object_Type));

        if (obj_collision.side != -3)
        {
            if (obj_collision.side == Side_TOP)
            {
                if (deal_damage(memory, game_object, obj_collision.object, game_object->damage))
                {
                    game_object->speed.y = obj_collision.object->pos.y + obj_collision.object->hit_box_pos.y + obj_collision.object->hit_box.y * 0.5f + 0.001f - game_object->hit_box_pos.y + game_object->hit_box.y * 0.5f - game_object->pos.y;
                    check_collision(memory, game_object, false);

                    game_object->speed.y = JUMP_ON_ENEMY_BOOST;
                    obj_collision.object->speed.y -= game_object->speed.y + JUMP_ON_ENEMY_BOOST;

                    memory->timers[game_object->hanging_animation_timer] = 0;
                    game_object->condition = Condition_FALLING;
                }
            }
            else
            {
                if (obj_collision.object->type != Game_Object_RAT)
                {
                    if (deal_damage(memory, obj_collision.object, game_object, obj_collision.object->damage))
                    {
                        if (obj_collision.object->condition != Condition_FALLING)
                        {
                            obj_collision.object->moving_direction = (Direction)(i32)unit(V2{game_object->pos.x - obj_collision.object->pos.x, 0}).x;
                        }
                        game_object->speed = unit(game_object->pos - obj_collision.object->pos) * KNOCKBACK;
                        game_object->speed.y *= 0.5f;
                        game_object->speed.y += 7;

                        memory->timers[game_object->invulnerable_timer] = 90;
                        memory->timers[game_object->cant_control_timer] = 20;
                        memory->timers[game_object->hanging_animation_timer] = 0;
                        memory->pause = 3;
                        game_object->condition = Condition_FALLING;
                    }
                }
            }
        }
    }
    if (game_object->type == Game_Object_TOY_GUN_BULLET)
    {
        //столкновения и движение
        Game_Object_Type triggers[] = {
            Game_Object_ZOMBIE,
            Game_Object_RAT,
        };
        Object_Collision obj_collision = check_object_collision(memory, game_object, triggers, sizeof(triggers) / sizeof(Game_Object_Type));

        if (obj_collision.side != -3)
        {
            if (deal_damage(memory, game_object, obj_collision.object, game_object->damage))
            {
                game_object->exists = false;
            }
        }
    }
    if (game_object->type == Game_Object_BOMB || game_object->type == Game_Object_TOY_GUN)
    {
        Game_Object_Type triggers[] = {
            Game_Object_ZOMBIE,
            Game_Object_RAT,
        };

        Object_Collision obj_collision = check_object_collision(memory, game_object, triggers, sizeof(triggers) / sizeof(Game_Object_Type));
        if (obj_collision.side != -3)
        {
            if (length(game_object->speed) > 18.0f * game_object->mass)
            {
                deal_damage(memory, game_object, obj_collision.object, game_object->damage);
            }
        }
    }
}