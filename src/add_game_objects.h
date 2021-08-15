typedef enum
{
    Side_LEFT,
    Side_RIGHT,
    Side_BOTTOM,
    Side_TOP,
    Side_INNER,
} Side;

typedef struct
{
    bool happened;
    i32 tile_index;
    Side tile_side;
} Collision;

typedef struct
{
    Collision x;
    Collision y;
} Collisions;

Game_Object *get_game_object(Game_memory *memory, Game_Object_Handle handle)
{
    Game_Object *result = &memory->game_objects[handle.index];
    if (handle.index > memory->game_object_count || !handle.id || result->id != handle.id || !result->exists)
    {
        result = NULL;
    }
    return result;
}

Game_Object *add_game_object(Game_memory *memory, Game_Object_Type type, V2 pos)
{
    Game_Object game_object;

    //по умолчанию
    game_object.id = memory->id_count;

    game_object.type = type;

    game_object.exists = true;

    game_object.healthpoints = 0;
    game_object.max_healthpoints = 0;

    game_object.damage = 1.0f;

    game_object.pos = pos;
    game_object.collision_box_pos = V2{0, -12};
    game_object.collision_box = V2{35, 56};
    game_object.hit_box_pos = V2{0, -3};
    game_object.hit_box = V2{40, 75};
    game_object.speed = V2{0, 0};
    game_object.delta = V2{0, 0};
    game_object.deflection = V2{0, 0};
    game_object.target_deflection = V2{0, 0};
    game_object.angle = 0.0f;

    game_object.weapon = Game_Object_Handle{0, 0};

    game_object.mooving_direction = Direction_NONE;
    game_object.jump = NULL;

    game_object.accel = 0.75f;
    game_object.friction = 0.75;
    game_object.gravity = 0.0f;
    game_object.jump_height = TILE_SIZE_PIXELS * 2;
    game_object.jump_duration = 19;
    game_object.bounce = 0.0f;
    game_object.mass = 1.0f;

    game_object.moved_through_pixels = 0;
    game_object.looking_direction = (Direction)(random_int(&memory->__global_random_state, 0, 1) * 2 - 1);
    game_object.condition = Condition_IDLE;
    game_object.sprite = memory->bitmaps[Bitmap_type_NONE];

    game_object.jump_timer = NULL;
    game_object.looking_key_held_timer = NULL;
    game_object.invulnerable_timer = NULL;
    game_object.hanging_animation_timer = NULL;
    game_object.hanging_index = 0;
    game_object.crouching_animation_timer = NULL;
    game_object.cant_control_timer = NULL;
    game_object.walks_throught_planks_timer = NULL;

    game_object.layer = LAYER_GAME_OBJECT;

    if (type == Game_Object_PLAYER)
    {
        game_object.healthpoints = 3;
        game_object.max_healthpoints = 3;

        game_object.jump_timer = add_timer(memory, 0);
        game_object.jump = add_timer(memory, 0);
        game_object.can_jump = add_timer(memory, 0);
        game_object.looking_key_held_timer = add_timer(memory, 0);
        game_object.crouching_animation_timer = add_timer(memory, 0);
        game_object.hanging_animation_timer = add_timer(memory, 0);
        game_object.cant_control_timer = add_timer(memory, 0);
        game_object.invulnerable_timer = add_timer(memory, 0);
        game_object.walks_throught_planks_timer = add_timer(memory, 0);

        game_object.jump_height = (f32)(TILE_SIZE_PIXELS * 2.2) - game_object.collision_box.y;
    }

    if (type == Game_Object_ZOMBIE)
    {
        game_object.healthpoints = 1;
        game_object.max_healthpoints = 1;

        game_object.jump = add_timer(memory, -1);
        game_object.can_jump = add_timer(memory, 0);

        game_object.jump_duration = 15;

        game_object.mooving_direction = (Direction)(random_int(&memory->__global_random_state, 0, 1) * 2 - 1);
    }

    if (type == Game_Object_RAT)
    {
        game_object.collision_box_pos = V2{0, -12};
        game_object.collision_box = V2{150, 56};
        game_object.hit_box_pos = V2{0, -3};
        game_object.hit_box = V2{170, 75};

        game_object.healthpoints = 3;
        game_object.max_healthpoints = 3;

        game_object.jump = add_timer(memory, -1);

        game_object.mooving_direction = (Direction)(random_int(&memory->__global_random_state, 0, 1) * 2 - 1);
    }

    if (type == Game_Object_TOY_GUN)
    {
        game_object.collision_box = V2{7, 7} / 16 * TILE_SIZE_PIXELS;
        game_object.collision_box_pos = V2{-2.5f, 2.5f};
        game_object.collision_box = V2{7, 7} / 16 * TILE_SIZE_PIXELS;
        game_object.collision_box_pos = V2{-2.5f, 2.5f};

        game_object.cant_control_timer = add_timer(memory, -1);

        game_object.friction = 0.90f;
        game_object.gravity = -0.5f;
        game_object.damage = 0.0f;

        game_object.sprite = memory->bitmaps[Bitmap_type_TOY_GUN];
    }

    if (type == Game_Object_TOY_GUN_BULLET)
    {
        game_object.collision_box = V2{1, 1} / 16 * TILE_SIZE_PIXELS;
        game_object.collision_box_pos = V2{0, 0.5} / 16 * TILE_SIZE_PIXELS;
        game_object.hit_box = V2{1, 1} / 16 * TILE_SIZE_PIXELS;
        game_object.hit_box_pos = V2{0, 0.5} / 16 * TILE_SIZE_PIXELS;
        game_object.cant_control_timer = add_timer(memory, -2);
        game_object.bounce = 0.75f;
        game_object.gravity = -2.0f;
        game_object.friction = 0.9f;
    }

    if (type == Game_Object_BOMB)
    {
        game_object.collision_box = V2{5, 5} * SPRITE_SCALE;
        game_object.collision_box_pos = V2{0, 0.0f} * SPRITE_SCALE;
        game_object.hit_box = V2{5, 5} * SPRITE_SCALE;
        game_object.hit_box_pos = V2{0, 0.0f} * SPRITE_SCALE;

        game_object.gravity = -0.66f;
        game_object.friction = 0.96f;

        game_object.sprite = memory->bitmaps[Bitmap_type_BOMB];
        game_object.bounce = 0.3f;
        game_object.mass = 0.75f;

        game_object.invulnerable_timer = add_timer(memory, 240);
    }

    i32 slot_index = memory->game_object_count;

    for (i32 objectIndex = 0; objectIndex < memory->game_object_count; objectIndex++)
    {
        if (memory->game_objects[objectIndex].exists == false)
        {
            slot_index = objectIndex;
            break;
        }
    }

    if (slot_index == memory->game_object_count)
    {
        memory->game_object_count++;
    }

    memory->game_objects[slot_index] = game_object;
    memory->id_count++;

    return &memory->game_objects[slot_index];
}