V2 get_tile_pos(i32 index)
{
    //size_x = CHUNK_SIZE_X * CHUNK_COUNT_X + BORDER_SIZE * 2;
    //index = y * (CHUNK_SIZE_X * CHUNK_COUNT_X + BORDER_SIZE * 2) + x;
    f32 y = (f32)floor(index / (CHUNK_SIZE_X * CHUNK_COUNT_X + BORDER_SIZE * 2));
    f32 x = index - y * (CHUNK_SIZE_X * CHUNK_COUNT_X + BORDER_SIZE * 2);
    V2 result = {x, y};
    return result;
}

i32 get_index(V2 coords)
{
    i32 index = (i32)(coords.y * (CHUNK_SIZE_X * CHUNK_COUNT_X + BORDER_SIZE * 2) + coords.x);
    return index;
}

void spawn_enemies(Game_memory *memory, V2 enter_pos)
{
    f32 danger_points = 30;
    f32 points = 0;
    while (points < danger_points)
    {
        V2 random_tile_pos = V2{(f32)random_int(&memory->__global_random_state, BORDER_SIZE, BORDER_SIZE + CHUNK_SIZE_X * CHUNK_COUNT_X - 1),
                                (f32)random_int(&memory->__global_random_state, BORDER_SIZE, BORDER_SIZE + CHUNK_SIZE_Y * CHUNK_COUNT_Y - 1)};
        Tile *random_tile = &memory->tile_map[get_index(random_tile_pos)];
        Tile down_tile = memory->tile_map[get_index(random_tile_pos - V2{0, 1})];
        Tile *left_tile = &memory->tile_map[get_index(random_tile_pos - V2{1, 0})];
        Tile *right_tile = &memory->tile_map[get_index(random_tile_pos + V2{1, 0})];
        if (random_tile->sprite.pixels == memory->bitmaps[Bitmap_type_BACKGROUND].pixels && down_tile.solid)
        {
            if (!left_tile->solid || !right_tile->solid)
            {
                if (distance_between_points(enter_pos, random_tile_pos) > CHUNK_SIZE_X * 0.5)
                {
                    if (random_float(&memory->__global_random_state, 0, 1) < 0.75)
                    {
                        add_game_object(memory, Game_Object_ZOMBIE, random_tile_pos * TILE_SIZE_PIXELS + V2{0, 0.01f});
                        random_tile->type = Tile_Type_OCCUPIED;
                        points++;
                    }
                    else
                    {
                        Tile_type save_type = random_tile->type;
                        random_tile->type = Tile_Type_OCCUPIED;
                        points += 2;
                        V2 obj_pos = V2{0, 0};
                        if (!left_tile->solid && !right_tile->solid)
                        {
                            if (random_int(&memory->__global_random_state, 0, 1) == 0)
                            {
                                obj_pos = V2{-0.5f, 0};
                                left_tile->type = Tile_Type_OCCUPIED;
                            }
                            else
                            {
                                obj_pos = V2{0.5f, 0};
                                right_tile->type = Tile_Type_OCCUPIED;
                            }
                        }
                        else if (!left_tile->solid)
                        {
                            obj_pos = V2{-0.5f, 0};
                            left_tile->type = Tile_Type_OCCUPIED;
                        }
                        else if (!right_tile->solid)
                        {
                            obj_pos = V2{0.5f, 0};
                            right_tile->type = Tile_Type_OCCUPIED;
                        }
                        else
                        {
                            points -= 2;
                            random_tile->type = save_type;
                        }

                        if (random_tile->type == Tile_Type_OCCUPIED)
                        {
                            add_game_object(memory, Game_Object_RAT, (random_tile_pos + obj_pos) * TILE_SIZE_PIXELS + V2{0, 0.01f});
                        }
                    }
                }
            }
        }
    }

    V2 map_size = {CHUNK_COUNT_X * CHUNK_SIZE_X + BORDER_SIZE * 2, CHUNK_COUNT_Y * CHUNK_SIZE_Y + BORDER_SIZE * 2};
    i32 tile_count = i32(map_size.x * map_size.y);

    for (i32 tile_index = 0; tile_index < tile_count; tile_index++)
    {
        if (memory->tile_map[tile_index].type == Tile_Type_OCCUPIED)
        {
            memory->tile_map[tile_index].type = Tile_Type_NONE;
        }
    }
}

void spawn_lamps(Game_memory *memory, V2 enter_pos)
{
#define LAMPS_PER_LVL 3
    i32 lamps = 0;
    V2 lamps_pos[LAMPS_PER_LVL] = {};
    while (lamps < LAMPS_PER_LVL)
    {
        V2 random_tile_pos = V2{(f32)random_int(&memory->__global_random_state, BORDER_SIZE, BORDER_SIZE + CHUNK_SIZE_X * CHUNK_COUNT_X - 1),
                                (f32)random_int(&memory->__global_random_state, BORDER_SIZE, BORDER_SIZE + CHUNK_SIZE_Y * CHUNK_COUNT_Y - 1)};
        Tile *random_tile = &memory->tile_map[get_index(random_tile_pos)];
        Tile down_tile = memory->tile_map[get_index(random_tile_pos - V2{0, 1})];
        Tile down_down_tile = memory->tile_map[get_index(random_tile_pos - V2{0, 2})];
        Tile down_down_down_tile = memory->tile_map[get_index(random_tile_pos - V2{0, 3})];
        Tile left_tile = memory->tile_map[get_index(random_tile_pos - V2{1, 0})];
        Tile right_tile = memory->tile_map[get_index(random_tile_pos + V2{1, 0})];
        if (random_tile->sprite.pixels == memory->bitmaps[Bitmap_type_BACKGROUND].pixels && !down_tile.solid && (down_down_tile.solid || down_down_down_tile.solid))
        {
            if (!left_tile.solid || !right_tile.solid)
            {
                if (distance_between_points(enter_pos, random_tile_pos) > CHUNK_SIZE_X)
                {
                    bool spawn = true;
                    for (i32 lamp_index = 0; lamp_index < lamps; lamp_index++)
                    {
                        V2 lamp = lamps_pos[lamp_index];
                        if (distance_between_points(random_tile_pos, lamp) < CHUNK_SIZE_X)
                        {
                            spawn = false;
                            break;
                        }
                    }
                    if (spawn)
                    {
                        random_tile->type = Tile_Type_LAMP;
                        lamps_pos[lamps] = random_tile_pos;
                        lamps++;
                    }
                }
            }
        }
    }
}

char *invert_chunk(char *chunk)
{
    char *result = (char *)malloc(sizeof(char) * CHUNK_SIZE_X * CHUNK_SIZE_Y);
    for (i32 line = 0; line < CHUNK_SIZE_Y; line++)
    {
        for (i32 row = 0; row < CHUNK_SIZE_X; row++)
        {
            result[line * CHUNK_SIZE_X + row] = chunk[line * CHUNK_SIZE_X + CHUNK_SIZE_X - 1 - row];
        }
    }
    return result;
}

void update_map_bitmap(Game_memory *memory, Rect updated_tiles)
{
    for (u32 tile_y = (u32)updated_tiles.min.y; tile_y < updated_tiles.max.y; tile_y++)
    {
        for (u32 tile_x = (u32)updated_tiles.min.x; tile_x < updated_tiles.max.x; tile_x++)
        {
            V2 tile_pos = V2{(f32)tile_x, (f32)tile_y};
            i32 tile_index = get_index(tile_pos);
            Tile tile = memory->tile_map[tile_index];

            V2 center_pixel_pos = (TILE_SIZE_PIXELS * tile_pos + V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS} * 0.5f) / SPRITE_SCALE - tile.sprite.size * 0.5f;

            for (u32 pixel_y = 0; pixel_y < tile.sprite.size.y; pixel_y++)
            {
                for (u32 pixel_x = 0; pixel_x < tile.sprite.size.x; pixel_x++)
                {
                    V2 chunk_pixel_pos = center_pixel_pos + V2{(f32)pixel_x, (f32)pixel_y};

                    memory->map_bitmap.pixels[(i32)(chunk_pixel_pos.y * memory->map_bitmap.pitch + chunk_pixel_pos.x)] = tile.sprite.pixels[(pixel_y + 1) * tile.sprite.pitch + pixel_x + 1];
                }
            }
        }
    }
}

void generate_new_map(Game_memory *memory, Bitmap screen)
{
    char *normal_chunks[] = {
        "###  #####"
        "#     ####"
        "TTT   ####"
        "=== T  ###"
        "##      ##"
        "##    TT##"
        "#    ===##"
        "#   ######",

        "######## #"
        "#       ##"
        "##      ##"
        "#      ###"
        "          "
        "#       ##"
        "###    ###"
        "###TTTT###",

        "          "
        "          "
        "  P       "
        " TT TTTT  "
        "    PPP P "
        "TT  TTTTT "
        "#==       "
        "###======#",
        invert_chunk(normal_chunks[0]),
        invert_chunk(normal_chunks[1]),
        invert_chunk(normal_chunks[2]),
    };

    char *down_passage_chunks[] = {
        "          "
        "          "
        "          "
        "##     TT="
        "   #      "
        "     TTT#T"
        "T=TT   ==="
        "##### ####",

        "##    ## #"
        "        ##"
        "##      ##"
        "       ## "
        "          "
        "  TTT ###T"
        "===      ="
        "####   ###",

        "          "
        " =        "
        "          "
        "          "
        "   =_  =  "
        "# #=   =# "
        "###=  _=# "
        "###=_  =##",
        invert_chunk(down_passage_chunks[0]),
        invert_chunk(down_passage_chunks[1]),
        invert_chunk(down_passage_chunks[2]),
    };

    char *enter_down_passage_chunks[] = {
        "#      #  "
        "#        #"
        "       #  "
        "     N  # "
        "     T    "
        " ^     TTT"
        "T#TT   ==="
        "####  _  #",

        "          "
        "  N       "
        "TTTTC     "
        "       ## "
        "    =     "
        "TTTT=  TT#"
        "=====  ==="
        "####=_ =##",

        "       #  "
        "TT      ##"
        "          "
        "    N     "
        "    TT    "
        "#TT==  ###"
        "###=    ##"
        "#####  ###",
        invert_chunk(enter_down_passage_chunks[0]),
        invert_chunk(enter_down_passage_chunks[1]),
        invert_chunk(enter_down_passage_chunks[2]),
    };

    char *enter_chunks[] = {

        "          "
        "          "
        "    N     "
        "   TTTT   "
        "   =====  "
        "  =###### "
        "TT########"
        "##########",

        "          "
        "  N       "
        " TTT      "
        "       ## "
        "  ^ #     "
        "####   ###"
        "####  ####"
        "#####  ###",

        "       #  "
        " ##     # "
        "          "
        "    N     "
        "    ##    "
        "#TT##  ## "
        "####   ###"
        "#####  ###",
        invert_chunk(enter_chunks[0]),
        invert_chunk(enter_chunks[1]),
        invert_chunk(enter_chunks[2]),
    };

    char *passage_chunks[] = {

        " PPPPPPP  "
        " TTTTTTT  "
        "          "
        "         _"
        "          "
        "   ## #   "
        "TTTTTTTTTT"
        "#======###",

        "  ##   ## "
        "#         "
        "        # "
        "       ## "
        "  #    ###"
        "TTTT  ####"
        "##########"
        "##########",

        "##     #  "
        " #      ##"
        "          "
        "          "
        "          "
        "=TTT  == #"
        "#========#"
        "##########",
        invert_chunk(passage_chunks[0]),
        invert_chunk(passage_chunks[1]),
        invert_chunk(passage_chunks[2]),
    };

    char *exit_chunks[] = {

        "          "
        " #        "
        "#         "
        "      T  #"
        "          "
        " #   X    "
        "  TTTTTT  "
        " ######## ",
        "          "
        "#         "
        "#         "
        "         #"
        "  X       "
        " ====     "
        "##  #TTT  "
        "##########",
        "##      # "
        "         #"
        "          "
        "   X      "
        " #####    "
        "          "
        "  TTTTTT  "
        " ======== ",
        invert_chunk(exit_chunks[0]),
        invert_chunk(exit_chunks[1]),
        invert_chunk(exit_chunks[2]),
    };

    //удаляем объекты
    memory->game_object_count = 0;

    //удаляем таймеры
    memory->timers_count = 1;

    memory->lamp_count = 0;

    //генерация
    f32 down_chunk_chance = 0.25;

    //карта тайлов
    V2 map_size = {CHUNK_COUNT_X * CHUNK_SIZE_X + BORDER_SIZE * 2, CHUNK_COUNT_Y * CHUNK_SIZE_Y + BORDER_SIZE * 2};
    i32 tile_count = i32(map_size.x * map_size.y);

    memory->map_bitmap = create_empty_bitmap(map_size * TILE_SIZE_PIXELS / SPRITE_SCALE);

    for (i32 index = 0; index < tile_count; index++)
    {
        memory->tile_map[index].type = Tile_Type_BORDER;
        memory->tile_map[index].solid = (Solidness_Type)Solidness_Type_NORMAL;
        memory->tile_map[index].timer = -1;
        memory->tile_map[index].sprite = memory->bitmaps[Bitmap_type_NONE];
    }

    //создание чанков
    char *chunks_strings[CHUNK_COUNT_X * CHUNK_COUNT_Y];

    //путь через тайлы
    V2 enter_chunk_pos = {(f32)random_int(&memory->__global_random_state, 0, CHUNK_COUNT_X - 1), CHUNK_COUNT_Y - 1};
    V2 chunk_pos = enter_chunk_pos;
    V2 end_chunk_pos = {(f32)random_int(&memory->__global_random_state, 0, CHUNK_COUNT_X - 1), 0};

    //заполняем чанки
    for (i32 index = 0; index < CHUNK_COUNT_X * CHUNK_COUNT_Y; index++)
    {
        chunks_strings[index] = normal_chunks[random_int(&memory->__global_random_state, 1, ARRAY_COUNT(normal_chunks)) - 1];
    }

    i8 direction = (i8)random_int(&memory->__global_random_state, Direction_LEFT, Direction_RIGHT);
    if ((direction == Direction_RIGHT && chunk_pos.x == CHUNK_COUNT_X - 1) || (direction == Direction_LEFT && chunk_pos.x == 0))
    {
        direction = -direction;
    }

    while (chunk_pos != end_chunk_pos)
    {
        if ((random_float(&memory->__global_random_state, 0, 1) <= down_chunk_chance || direction == Direction_VERTICAL) && chunk_pos.y > 0)
        {
            if (chunk_pos == enter_chunk_pos)
            {
                //чанк-вход с проходом вниз
                chunks_strings[(i32)(chunk_pos.y * CHUNK_COUNT_X + chunk_pos.x)] = enter_down_passage_chunks[random_int(&memory->__global_random_state, 1, ARRAY_COUNT(enter_down_passage_chunks)) - 1];
            }
            else
            {
                //чанк-проход вниз
                chunks_strings[(i32)(chunk_pos.y * CHUNK_COUNT_X + chunk_pos.x)] = down_passage_chunks[random_int(&memory->__global_random_state, 1, ARRAY_COUNT(down_passage_chunks)) - 1];
            }
            chunk_pos.y--;

            direction = (i8)random_int(&memory->__global_random_state, 0, 1) * 2 - 1;
        }
        else
        {
            if ((direction == Direction_RIGHT && chunk_pos.x == CHUNK_COUNT_X - 1) || (direction == Direction_LEFT && chunk_pos.x == 0))
            {
                direction = Direction_VERTICAL;
            }

            if (chunk_pos.y == 0)
            {
                direction = (i8)unit(V2{end_chunk_pos.x - chunk_pos.x, 0}).x;
            }

            if (direction != Direction_VERTICAL)
            {
                if (chunk_pos == enter_chunk_pos)
                {
                    //чанк-вход
                    chunks_strings[(i32)(chunk_pos.y * CHUNK_COUNT_X + chunk_pos.x)] = enter_chunks[random_int(&memory->__global_random_state, 1, ARRAY_COUNT(enter_chunks)) - 1];
                }
                else
                {
                    //проходной чанк
                    chunks_strings[(i32)(chunk_pos.y * CHUNK_COUNT_X + chunk_pos.x)] = passage_chunks[random_int(&memory->__global_random_state, 1, ARRAY_COUNT(passage_chunks)) - 1];
                }
                chunk_pos.x += direction;
            }
        }
        //чанк-выход
        if (chunk_pos == end_chunk_pos)
        {
            chunks_strings[(i32)(chunk_pos.y * CHUNK_COUNT_X + chunk_pos.x)] = exit_chunks[random_int(&memory->__global_random_state, 1, ARRAY_COUNT(exit_chunks)) - 1];
        }
    }

    V2 enter_pos = {0, 0};
    char *chunk_string;
    //заполняем чанки
    for (i32 chunk_index_y = 0; chunk_index_y < CHUNK_COUNT_Y; chunk_index_y++)
    {
        for (i32 chunk_index_x = 0; chunk_index_x < CHUNK_COUNT_X; chunk_index_x++)
        {
            i32 chunk_index = chunk_index_y * CHUNK_COUNT_X + chunk_index_x;

            chunk_string = chunks_strings[chunk_index];
            for (i32 y = 0; y < CHUNK_SIZE_Y; y++)
            {
                for (i32 x = 0; x < CHUNK_SIZE_X; x++)
                {
                    char tile_char = chunk_string[(CHUNK_SIZE_Y - y - 1) * CHUNK_SIZE_X + x];
                    V2 tile_pos = {(f32)(x + chunk_index_x * CHUNK_SIZE_X + BORDER_SIZE), (f32)(y + chunk_index_y * CHUNK_SIZE_Y + BORDER_SIZE)};
                    i32 index = get_index(tile_pos);

                    Tile_type type = Tile_Type_NONE;
                    Solidness_Type solid = Solidness_Type_NONE;
                    i32 timer = -1;
                    Bitmap sprite = memory->bitmaps[Bitmap_type_BACKGROUND];

                    switch (tile_char)
                    {
                    case '#':
                    {
                        type = Tile_Type_NORMAL;
                        solid = Solidness_Type_NORMAL;

                        f32 chance = random_float(&memory->__global_random_state, 0, 1);
                        if (chance < 0.95)
                        {
                            if (chance > 0.15)
                            {
                                sprite = memory->bitmaps[Bitmap_type_BRICKS + random_int(&memory->__global_random_state, 0, 6)];
                            }
                            else
                            {
                                sprite = memory->bitmaps[Bitmap_type_STONE];
                            }
                        }
                        else
                        {
                            sprite = memory->bitmaps[Bitmap_type_BRICKS + random_int(&memory->__global_random_state, 7, 11)];
                        }
                    }
                    break;
                    case '=':
                    {
                        type = Tile_Type_NORMAL;
                        solid = Solidness_Type_NORMAL;
                        sprite = memory->bitmaps[Bitmap_type_ELEGANT_BRICK];
                    }
                    break;
                    case '-':
                    {
                        type = Tile_Type_BORDER;
                        solid = Solidness_Type_NORMAL;

                        sprite = memory->bitmaps[Bitmap_type_BORDER];
                    }
                    break;
                    case 'N':
                    {
                        type = Tile_Type_ENTER;
                        sprite = memory->bitmaps[Bitmap_type_NONE];

                        //addPlayer
                        enter_pos = tile_pos;
                        V2 spawn_pos = tile_pos * TILE_SIZE_PIXELS + V2{0.0f, 0.001f};
                        Game_Object *player = add_game_object(memory, Game_Object_PLAYER, spawn_pos);
                        memory->player = {player->id, memory->game_object_count - 1};
                        Game_Object *weapon = add_game_object(memory, Game_Object_TOY_GUN, spawn_pos);
                        player->weapon = {weapon->id, memory->game_object_count - 1};

                        memory->camera.target = spawn_pos;
                        border_camera(memory, screen);
                        memory->camera.pos = memory->camera.target;
                    }
                    break;
                    case 'X':
                    {
                        type = Tile_Type_EXIT;
                        timer = add_timer(memory, -1);
                        sprite = memory->bitmaps[Bitmap_type_NONE];
                    }
                    break;
                    case 'T':
                    {
                        type = Tile_Type_FLOOR;
                        solid = Solidness_Type_NORMAL;
                    }
                    break;
                    case 'P':
                    {
                        type = Tile_Type_PARAPET;
                    }
                    break;
                    case '^':
                    {
                        type = Tile_Type_SPIKES;
                    }
                    break;
                    case '_':
                    {
                        type = Tile_Type_PLANK;
                        solid = Solidness_Type_UP;
                    }
                    break;
                    }

                    memory->tile_map[index].type = type;
                    memory->tile_map[index].solid = solid;
                    memory->tile_map[index].timer = timer;
                    memory->tile_map[index].sprite = sprite;
                }
            }
        }
    }

    for (int i = 3; i <= 5; i++)
    {
        free(normal_chunks[i]);
        free(down_passage_chunks[i]);
        free(enter_down_passage_chunks[i]);
        free(enter_chunks[i]);
        free(passage_chunks[i]);
        free(exit_chunks[i]);
    }

    //налаживаем свойства тайлов после полной расстановки
    for (i32 index = 0; index < tile_count; index++)
    {
        V2 tile_pos = get_tile_pos(index);
        Tile_type type = memory->tile_map[index].type;
        Bitmap sprite = memory->tile_map[index].sprite;

        if (type == Tile_Type_FLOOR)
        {
            Bitmap left_tile = memory->tile_map[get_index(tile_pos + V2{-1, 0})].sprite;
            Bitmap bottom_tile = memory->tile_map[get_index(tile_pos + V2{0, -1})].sprite;
            Bitmap bottomleft_tile = memory->tile_map[get_index(tile_pos + V2{-1, -1})].sprite;
            Bitmap bottomright_tile = memory->tile_map[get_index(tile_pos + V2{1, -1})].sprite;

            if (left_tile.pixels == memory->bitmaps[Bitmap_type_TILED_FLOOR].pixels ||
                bottom_tile.pixels == memory->bitmaps[Bitmap_type_TILED_FLOOR].pixels ||
                bottomleft_tile.pixels == memory->bitmaps[Bitmap_type_TILED_FLOOR].pixels ||
                bottomright_tile.pixels == memory->bitmaps[Bitmap_type_TILED_FLOOR].pixels)
            {
                sprite = memory->bitmaps[Bitmap_type_TILED_FLOOR];
            }
            else if (left_tile.pixels == memory->bitmaps[Bitmap_type_MARBLE_FLOOR].pixels ||
                     bottom_tile.pixels == memory->bitmaps[Bitmap_type_MARBLE_FLOOR].pixels ||
                     bottomleft_tile.pixels == memory->bitmaps[Bitmap_type_MARBLE_FLOOR].pixels ||
                     bottomright_tile.pixels == memory->bitmaps[Bitmap_type_MARBLE_FLOOR].pixels)
            {
                sprite = memory->bitmaps[Bitmap_type_MARBLE_FLOOR];
            }
            else
            {
                if (random_int(&memory->__global_random_state, 0, 1) == 0)
                {
                    sprite = memory->bitmaps[Bitmap_type_TILED_FLOOR];
                }
                else
                {
                    sprite = memory->bitmaps[Bitmap_type_MARBLE_FLOOR];
                }
            }
        }

        if (type == Tile_Type_BORDER)
        {
            sprite = memory->bitmaps[Bitmap_type_BORDER];
            if (tile_pos.x != 0 && tile_pos.y != 0 && tile_pos.x != (map_size.x - 1) && tile_pos.y != (map_size.y - 1))
            {
                Tile_type left_tile = memory->tile_map[get_index(tile_pos + V2{-1, 0})].type;
                Tile_type right_tile = memory->tile_map[get_index(tile_pos + V2{1, 0})].type;
                Tile_type top_tile = memory->tile_map[get_index(tile_pos + V2{0, 1})].type;
                Tile_type bottom_tile = memory->tile_map[get_index(tile_pos + V2{0, -1})].type;
                Tile_type bottomright_tile = memory->tile_map[get_index(tile_pos + V2{1, -1})].type;
                Tile_type topright_tile = memory->tile_map[get_index(tile_pos + V2{1, 1})].type;
                Tile_type bottomleft_tile = memory->tile_map[get_index(tile_pos + V2{-1, -1})].type;
                Tile_type topleft_tile = memory->tile_map[get_index(tile_pos + V2{-1, 1})].type;

                if (bottom_tile != Tile_Type_BORDER)
                {
                    sprite = memory->bitmaps[Bitmap_type_TRANSITION_BORDER_0];
                }
                else
                {
                    if (right_tile != Tile_Type_BORDER)
                    {
                        sprite = memory->bitmaps[Bitmap_type_TRANSITION_BORDER_05];
                    }
                    else if (bottomleft_tile != Tile_Type_BORDER)
                    {
                        sprite = memory->bitmaps[Bitmap_type_CORNER_BORDER_15];
                    }

                    if (left_tile != Tile_Type_BORDER)
                    {
                        sprite = memory->bitmaps[Bitmap_type_TRANSITION_BORDER_15];
                    }
                    else if (bottomright_tile != Tile_Type_BORDER)
                    {
                        sprite = memory->bitmaps[Bitmap_type_CORNER_BORDER_0];
                    }
                }

                if (top_tile != Tile_Type_BORDER)
                {
                    sprite = memory->bitmaps[Bitmap_type_TRANSITION_BORDER_1];
                }
                else
                {
                    if (right_tile != Tile_Type_BORDER)
                    {
                        sprite = memory->bitmaps[Bitmap_type_TRANSITION_BORDER_05];
                    }
                    else if (topright_tile != Tile_Type_BORDER)
                    {
                        sprite = memory->bitmaps[Bitmap_type_CORNER_BORDER_05];
                    }

                    if (left_tile != Tile_Type_BORDER)
                    {
                        sprite = memory->bitmaps[Bitmap_type_TRANSITION_BORDER_15];
                    }
                    else if (topleft_tile != Tile_Type_BORDER)
                    {
                        sprite = memory->bitmaps[Bitmap_type_CORNER_BORDER_1];
                    }
                }
            }
        }

        if (type == Tile_Type_EXIT)
        {
            Tile *up_tile = &memory->tile_map[get_index(tile_pos + V2{0, 1})];
            up_tile->type = Tile_Type_NONE;
            up_tile->solid = (Solidness_Type)Solidness_Type_NONE;
            up_tile->timer = add_timer(memory, -(i32)INFINITY);
        }

        memory->tile_map[index].sprite = sprite;
    }

    for (u32 chunk_y = 0; chunk_y < CHUNK_COUNT_Y; chunk_y++)
    {
        for (u32 chunk_x = 0; chunk_x < CHUNK_COUNT_X; chunk_x++)
        {
            update_map_bitmap(memory, Rect{V2{0, 0}, V2{map_size.x, map_size.y}});
        }
    }

    spawn_enemies(memory, enter_pos);
    spawn_lamps(memory, enter_pos);
}

void draw_tile_map(Game_memory *memory)
{
    V2 map_size = V2{CHUNK_COUNT_X * CHUNK_SIZE_X + BORDER_SIZE * 2, CHUNK_COUNT_Y * CHUNK_SIZE_Y + BORDER_SIZE * 2} * TILE_SIZE_PIXELS;
    V2 map_center = map_size * 0.5f - V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS} * 0.5f + V2{SPRITE_SCALE, SPRITE_SCALE};

    add_bitmap_to_queue(memory, map_center, map_size, 0, memory->map_bitmap, 1, 0x00000000, LAYER_TILE);
}

void update_tile(Game_memory *memory, i32 tile_index)
{
    Tile *tile = &memory->tile_map[tile_index];
    V2 tile_pos = get_tile_pos(tile_index);

    //обновление тайлов
    switch (tile->type)
    {
    case Tile_Type_PARAPET:
    {
        if (memory->tile_map[get_index(tile_pos + V2{0, -1})].type == Tile_Type_NONE)
        {
            tile->type = Tile_Type_NONE;
            tile->sprite = memory->bitmaps[Bitmap_type_BACKGROUND];
        }
        else
        {
            add_bitmap_to_queue(memory, tile_pos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS}, 0, memory->bitmaps[Bitmap_type_PARAPET], 1, 0x00000000, LAYER_FORGROUND);
        }
    }
    break;
    case Tile_Type_ENTER:
    {
        add_bitmap_to_queue(memory, tile_pos * TILE_SIZE_PIXELS + V2{0, (memory->bitmaps[Bitmap_type_DOOR].size.y * SPRITE_SCALE - TILE_SIZE_PIXELS) * 0.5f}, memory->bitmaps[Bitmap_type_DOOR].size * SPRITE_SCALE, 0, memory->bitmaps[Bitmap_type_DOOR], 1, 0x00000000, LAYER_ON_ON_BACKGROUND);

        V2 down_tile_pos = tile_pos - V2{0, 1};
        i32 down_tile_index = get_index(down_tile_pos);
        Tile *down_tile = &memory->tile_map[down_tile_index];
        if (!down_tile->solid)
        {
            down_tile->type = Tile_Type_UNBREAKABLE_PLANK;
            down_tile->solid = Solidness_Type_UP;
        }
    }
    break;
    case Tile_Type_EXIT:
    {

        V2 down_tile_pos = tile_pos - V2{0, 1};
        i32 down_tile_index = get_index(down_tile_pos);
        Tile *down_tile = &memory->tile_map[down_tile_index];
        if (!down_tile->solid)
        {
            down_tile->type = Tile_Type_UNBREAKABLE_PLANK;
            down_tile->solid = Solidness_Type_UP;
        }

        Bitmap door_sprite = memory->bitmaps[Bitmap_type_DOOR + 5];

        if (memory->timers[tile->timer] >= 0)
        {
            i8 step = (i8)floor(memory->timers[tile->timer] / 60.0f * 6.0f);
            door_sprite = memory->bitmaps[Bitmap_type_DOOR + step];
        }
        if (memory->timers[tile->timer] == 0)
        {
            memory->transition = -0.001f;
        }

        add_bitmap_to_queue(memory, tile_pos * TILE_SIZE_PIXELS + V2{0, (door_sprite.size.y * SPRITE_SCALE - TILE_SIZE_PIXELS) * 0.5f}, memory->bitmaps[Bitmap_type_DOOR_BACK].size * SPRITE_SCALE, 0, memory->bitmaps[Bitmap_type_DOOR_BACK], 1, 0x00000000, LAYER_ON_BACKGROUND);
        add_bitmap_to_queue(memory, tile_pos * TILE_SIZE_PIXELS + V2{0, (door_sprite.size.y * SPRITE_SCALE - TILE_SIZE_PIXELS) * 0.5f}, door_sprite.size * SPRITE_SCALE, 0, door_sprite, 1, 0x00000000, LAYER_ON_ON_BACKGROUND);

        //лампа
        V2 up_tile_pos = tile_pos + V2{0, 1};
        i32 up_tile_index = get_index(up_tile_pos);
        Tile *up_tile = &memory->tile_map[up_tile_index];
        Bitmap sprite = memory->bitmaps[Bitmap_type_EXIT_SIGN_OFF];
        if (memory->timers[up_tile->timer] < -60)
        {
            memory->timers[up_tile->timer] = 60;
        }
        else if (memory->timers[up_tile->timer] > 0)
        {
            sprite = memory->bitmaps[Bitmap_type_EXIT_SIGN];
            add_light_to_queue(memory, up_tile_pos * TILE_SIZE_PIXELS, 100, 0x3300FF00);
        }
        add_bitmap_to_queue(memory, up_tile_pos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS}, 0, sprite, 1, 0x00000000, LAYER_ON_BACKGROUND);
    }
    break;
    case Tile_Type_LAMP:
    {
        if (distance_between_points(tile_pos * TILE_SIZE_PIXELS, memory->game_objects[0].pos) < TILE_SIZE_PIXELS * 1.5 && tile->timer == -1)
        {
            tile->timer = add_timer(memory, 6);
            memory->lamp_count++;
            if (memory->lamp_count == 3)
            {
                memory->game_objects[0].healthpoints++;
            }
        }

        Bitmap sprite = memory->bitmaps[Bitmap_type_LAMP];
        if (tile->timer == -1)
        {
            sprite = memory->bitmaps[Bitmap_type_LAMP_OFF];
        }
        else
        {
            f32 transition_lvl = (1.0f - max((f32)memory->timers[tile->timer], 0.0f) / 6.0f) + random_float(&memory->__global_random_state, -0.005f, 0.005f);
            if (memory->timers[tile->timer] == 0)
            {
                memory->timers[tile->timer] = random_int(&memory->__global_random_state, -999, -600);
            }
            if (memory->timers[tile->timer] == -1000)
            {
                transition_lvl = 0;
                sprite = memory->bitmaps[Bitmap_type_LAMP_OFF];
                if (random_float(&memory->__global_random_state, 0.0f, 1.0f) < 0.7)
                {
                    memory->timers[tile->timer] = random_int(&memory->__global_random_state, -999, -950);
                }
                else
                {
                    memory->timers[tile->timer] = random_int(&memory->__global_random_state, -999, -1);
                }
            }
            add_light_to_queue(memory, tile_pos * TILE_SIZE_PIXELS, 150 * transition_lvl, 0x55FFAA00);
        }
        add_bitmap_to_queue(memory, tile_pos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS}, 0, sprite, 1, 0x00000000, LAYER_ON_BACKGROUND);
    }
    break;
    case Tile_Type_SPIKES:
    {
        if (memory->tile_map[get_index(tile_pos + V2{0, -1})].type == Tile_Type_NONE)
        {
            tile->type = Tile_Type_NONE;
            tile->sprite = memory->bitmaps[Bitmap_type_BACKGROUND];
        }
        add_bitmap_to_queue(memory, tile_pos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS}, 0, memory->bitmaps[Bitmap_type_SPIKES], 1, 0x00000000, LAYER_ON_BACKGROUND);
    }
    break;
    case Tile_Type_PLANK:
    {
        add_bitmap_to_queue(memory, tile_pos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS}, 0, memory->bitmaps[Bitmap_type_PLANK], 1, 0x00000000, LAYER_ON_BACKGROUND);
    }
    break;
    case Tile_Type_UNBREAKABLE_PLANK:
    {
        add_bitmap_to_queue(memory, tile_pos * TILE_SIZE_PIXELS, V2{TILE_SIZE_PIXELS, TILE_SIZE_PIXELS}, 0, memory->bitmaps[Bitmap_type_UNBREAKABLE_PLANK], 1, 0x00000000, LAYER_ON_BACKGROUND);
    }
    break;
    }
}