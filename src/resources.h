//получение повёрнутых картинок
Bitmap turn_bitmap(Bitmap bitmap, f64 angle)
{
    Bitmap result;
    result.pitch = bitmap.pitch;
    result.size = bitmap.size;

    u64 alignment = (u32)(8 * sizeof(u32));
    u64 screen_buffer_size = (u32)(4 * (bitmap.size.x + 2) * (bitmap.size.y + 2));
    screen_buffer_size += alignment - (screen_buffer_size % alignment);
    u32 *pixels = (u32 *)_aligned_malloc(screen_buffer_size, alignment);
    memset(pixels, 0, screen_buffer_size);

    result.pixels = pixels;

    for (i32 y = 0; y < bitmap.size.y + 2; y++)
    {
        for (i32 x = 0; x < bitmap.size.x + 2; x++)
        {
            i32 pixel = bitmap.pixels[y * bitmap.pitch + x];
            if (angle == 0)
            {
                result.pixels[y * result.pitch + x] = pixel;
            }
            if (angle == PI * 0.5)
            {
                result.pixels[x * ((i32)result.size.x + 2) + ((i32)result.size.y + 1 - y)] = pixel;
            }
            if (angle == PI)
            {
                result.pixels[((i32)result.size.y + 1 - y) * result.pitch + ((i32)result.size.x + 1 - x)] = pixel;
            }
            if (angle == PI * 1.5)
            {
                result.pixels[((i32)result.size.x + 1 - x) * ((i32)result.size.x + 2) + y] = pixel;
            }
        }
    }
    return result;
}

//загрузка ресурсов
void load_bitmaps(Game_memory *memory, READ_BMP(win32_read_bmp))
{
    memory->bitmaps[Bitmap_type_NONE] = win32_read_bmp("./bitmaps/none.bmp");
    memory->bitmaps[Bitmap_type_TEST] = win32_read_bmp("./bitmaps/test.bmp");
    memory->bitmaps[Bitmap_type_PLAYER_IDLE] = win32_read_bmp("./bitmaps/lexaIdle.bmp");
    memory->bitmaps[Bitmap_type_PLAYER_JUMP] = win32_read_bmp("./bitmaps/lexaJump.bmp");
    memory->bitmaps[Bitmap_type_PLAYER_CROUCH_IDLE] = win32_read_bmp("./bitmaps/lexaCrouchIdle.bmp");
    memory->bitmaps[Bitmap_type_PLAYER_HANGING] = win32_read_bmp("./bitmaps/lexaWall.bmp");
    memory->bitmaps[Bitmap_type_PLAYER_HANGING_UP] = win32_read_bmp("./bitmaps/lexaWallUp.bmp");
    memory->bitmaps[Bitmap_type_PLAYER_HANGING_DOWN] = win32_read_bmp("./bitmaps/lexaWallDown.bmp");
    memory->bitmaps[Bitmap_type_PLAYER_LOOKING_UP] = win32_read_bmp("./bitmaps/lexaLookingUp.bmp");
    memory->bitmaps[Bitmap_type_PLAYER_STEP] = win32_read_bmp("./bitmaps/lexaStep1.bmp");
    memory->bitmaps[Bitmap_type_PLAYER_STEP + 1] = win32_read_bmp("./bitmaps/lexaStep2.bmp");
    memory->bitmaps[Bitmap_type_PLAYER_STEP + 2] = win32_read_bmp("./bitmaps/lexaStep3.bmp");
    memory->bitmaps[Bitmap_type_PLAYER_STEP + 3] = win32_read_bmp("./bitmaps/lexaStep4.bmp");
    memory->bitmaps[Bitmap_type_PLAYER_STEP + 4] = win32_read_bmp("./bitmaps/lexaStep5.bmp");
    memory->bitmaps[Bitmap_type_PLAYER_STEP + 5] = win32_read_bmp("./bitmaps/lexaStep6.bmp");
    memory->bitmaps[Bitmap_type_PLAYER_CROUCH_START] = win32_read_bmp("./bitmaps/lexaCrouchIdle1.bmp");
    memory->bitmaps[Bitmap_type_PLAYER_CROUCH_START + 1] = win32_read_bmp("./bitmaps/lexaCrouchIdle2.bmp");
    memory->bitmaps[Bitmap_type_PLAYER_CROUCH_START + 2] = win32_read_bmp("./bitmaps/lexaCrouchIdle3.bmp");
    memory->bitmaps[Bitmap_type_PLAYER_CROUCH_START + 3] = win32_read_bmp("./bitmaps/lexaCrouchIdle4.bmp");
    memory->bitmaps[Bitmap_type_PLAYER_CROUCH_STEP] = win32_read_bmp("./bitmaps/lexaCrouchStep1.bmp");
    memory->bitmaps[Bitmap_type_PLAYER_CROUCH_STEP + 1] = win32_read_bmp("./bitmaps/lexaCrouchStep2.bmp");
    memory->bitmaps[Bitmap_type_PLAYER_CROUCH_STEP + 2] = win32_read_bmp("./bitmaps/lexaCrouchStep3.bmp");
    memory->bitmaps[Bitmap_type_PLAYER_CROUCH_STEP + 3] = win32_read_bmp("./bitmaps/lexaCrouchStep4.bmp");
    memory->bitmaps[Bitmap_type_PLAYER_CROUCH_STEP + 4] = win32_read_bmp("./bitmaps/lexaCrouchStep5.bmp");
    memory->bitmaps[Bitmap_type_PLAYER_CROUCH_STEP + 5] = win32_read_bmp("./bitmaps/lexaCrouchStep6.bmp");
    memory->bitmaps[Bitmap_type_PLAYER_CLIMB_STEP] = win32_read_bmp("./bitmaps/lexaClimbStep1.bmp");
    memory->bitmaps[Bitmap_type_PLAYER_CLIMB_STEP + 1] = win32_read_bmp("./bitmaps/lexaClimbStep2.bmp");
    memory->bitmaps[Bitmap_type_PLAYER_CLIMB_STEP + 2] = win32_read_bmp("./bitmaps/lexaClimbStep3.bmp");
    memory->bitmaps[Bitmap_type_PLAYER_CLIMB_STEP + 3] = win32_read_bmp("./bitmaps/lexaClimbStep2.bmp");
    memory->bitmaps[Bitmap_type_ZOMBIE_IDLE] = win32_read_bmp("./bitmaps/zombieIdle.bmp");
    memory->bitmaps[Bitmap_type_ZOMBIE_STEP] = win32_read_bmp("./bitmaps/zombieStep1.bmp");
    memory->bitmaps[Bitmap_type_ZOMBIE_STEP + 1] = win32_read_bmp("./bitmaps/zombieStep2.bmp");
    memory->bitmaps[Bitmap_type_ZOMBIE_STEP + 2] = win32_read_bmp("./bitmaps/zombieStep3.bmp");
    memory->bitmaps[Bitmap_type_ZOMBIE_STEP + 3] = win32_read_bmp("./bitmaps/zombieStep4.bmp");
    memory->bitmaps[Bitmap_type_ZOMBIE_STEP + 4] = win32_read_bmp("./bitmaps/zombieStep5.bmp");
    memory->bitmaps[Bitmap_type_ZOMBIE_STEP + 5] = win32_read_bmp("./bitmaps/zombieStep6.bmp");
    memory->bitmaps[Bitmap_type_BACKGROUND] = win32_read_bmp("./bitmaps/backGround.bmp");
    memory->bitmaps[Bitmap_type_BORDER] = win32_read_bmp("./bitmaps/border.bmp");
    memory->bitmaps[Bitmap_type_TRANSITION_BORDER_0] = win32_read_bmp("./bitmaps/borderWithTransition.bmp");
    memory->bitmaps[Bitmap_type_TRANSITION_BORDER_05] = turn_bitmap(memory->bitmaps[Bitmap_type_TRANSITION_BORDER_0], 0.5 * PI);
    memory->bitmaps[Bitmap_type_TRANSITION_BORDER_1] = turn_bitmap(memory->bitmaps[Bitmap_type_TRANSITION_BORDER_0], PI);
    memory->bitmaps[Bitmap_type_TRANSITION_BORDER_15] = turn_bitmap(memory->bitmaps[Bitmap_type_TRANSITION_BORDER_0], 1.5 * PI);
    memory->bitmaps[Bitmap_type_CORNER_BORDER_0] = win32_read_bmp("./bitmaps/cornerBorder.bmp");
    memory->bitmaps[Bitmap_type_CORNER_BORDER_05] = turn_bitmap(memory->bitmaps[Bitmap_type_CORNER_BORDER_0], 0.5 * PI);
    memory->bitmaps[Bitmap_type_CORNER_BORDER_1] = turn_bitmap(memory->bitmaps[Bitmap_type_CORNER_BORDER_0], PI);
    memory->bitmaps[Bitmap_type_CORNER_BORDER_15] = turn_bitmap(memory->bitmaps[Bitmap_type_CORNER_BORDER_0], 1.5 * PI);
    memory->bitmaps[Bitmap_type_BRICKS] = win32_read_bmp("./bitmaps/brick1.bmp");
    memory->bitmaps[Bitmap_type_BRICKS + 1] = win32_read_bmp("./bitmaps/brick2.bmp");
    memory->bitmaps[Bitmap_type_BRICKS + 2] = win32_read_bmp("./bitmaps/brick3.bmp");
    memory->bitmaps[Bitmap_type_BRICKS + 3] = win32_read_bmp("./bitmaps/brick4.bmp");
    memory->bitmaps[Bitmap_type_BRICKS + 4] = win32_read_bmp("./bitmaps/brick5.bmp");
    memory->bitmaps[Bitmap_type_BRICKS + 5] = win32_read_bmp("./bitmaps/brick6.bmp");
    memory->bitmaps[Bitmap_type_BRICKS + 6] = win32_read_bmp("./bitmaps/brick7.bmp");
    memory->bitmaps[Bitmap_type_BRICKS + 7] = win32_read_bmp("./bitmaps/brick8.bmp");
    memory->bitmaps[Bitmap_type_BRICKS + 8] = win32_read_bmp("./bitmaps/brick9.bmp");
    memory->bitmaps[Bitmap_type_BRICKS + 9] = win32_read_bmp("./bitmaps/brick10.bmp");
    memory->bitmaps[Bitmap_type_BRICKS + 10] = win32_read_bmp("./bitmaps/brick11.bmp");
    memory->bitmaps[Bitmap_type_BRICKS + 11] = win32_read_bmp("./bitmaps/brick12.bmp");
    memory->bitmaps[Bitmap_type_ELEGANT_BRICK] = win32_read_bmp("./bitmaps/elegantBrick.bmp");
    memory->bitmaps[Bitmap_type_STONE] = win32_read_bmp("./bitmaps/stone.bmp");
    memory->bitmaps[Bitmap_type_TILED_FLOOR] = win32_read_bmp("./bitmaps/floor1.bmp");
    memory->bitmaps[Bitmap_type_MARBLE_FLOOR] = win32_read_bmp("./bitmaps/floor5.bmp");
    memory->bitmaps[Bitmap_type_DOOR] = win32_read_bmp("./bitmaps/door1.bmp");
    memory->bitmaps[Bitmap_type_DOOR + 1] = win32_read_bmp("./bitmaps/door2.bmp");
    memory->bitmaps[Bitmap_type_DOOR + 2] = win32_read_bmp("./bitmaps/door3.bmp");
    memory->bitmaps[Bitmap_type_DOOR + 3] = win32_read_bmp("./bitmaps/door4.bmp");
    memory->bitmaps[Bitmap_type_DOOR + 4] = win32_read_bmp("./bitmaps/door5.bmp");
    memory->bitmaps[Bitmap_type_DOOR + 5] = win32_read_bmp("./bitmaps/door6.bmp");
    memory->bitmaps[Bitmap_type_DOOR + 6] = win32_read_bmp("./bitmaps/door7.bmp");
    memory->bitmaps[Bitmap_type_DOOR_BACK] = win32_read_bmp("./bitmaps/doorBack.bmp");
    memory->bitmaps[Bitmap_type_PARAPET] = win32_read_bmp("./bitmaps/parapet.bmp");
    memory->bitmaps[Bitmap_type_HEALTH_BAR] = win32_read_bmp("./bitmaps/health_bar.bmp");
    memory->bitmaps[Bitmap_type_HEALTH] = win32_read_bmp("./bitmaps/health.bmp");
    memory->bitmaps[Bitmap_type_SPIKES] = win32_read_bmp("./bitmaps/spikes.bmp");
    memory->bitmaps[Bitmap_type_TOY_GUN] = win32_read_bmp("./bitmaps/toyGun.bmp");
    memory->bitmaps[Bitmap_type_TOY_GUN_BULLET] = win32_read_bmp("./bitmaps/toyGunBullet.bmp");
    memory->bitmaps[Bitmap_type_EXIT_SIGN] = win32_read_bmp("./bitmaps/exitSign.bmp");
    memory->bitmaps[Bitmap_type_EXIT_SIGN_OFF] = win32_read_bmp("./bitmaps/exitSignOff.bmp");
    memory->bitmaps[Bitmap_type_LAMP] = win32_read_bmp("./bitmaps/lamp.bmp");
    memory->bitmaps[Bitmap_type_LAMP_OFF] = win32_read_bmp("./bitmaps/lampOff.bmp");
    memory->bitmaps[Bitmap_type_BOMB] = win32_read_bmp("./bitmaps/bomb.bmp");
    memory->bitmaps[Bitmap_type_PLANK] = win32_read_bmp("./bitmaps/plank.bmp");
    memory->bitmaps[Bitmap_type_UNBREAKABLE_PLANK] = win32_read_bmp("./bitmaps/unbreakablePlank.bmp");
    memory->bitmaps[Bitmap_type_RAT_IDLE] = win32_read_bmp("./bitmaps/ratIdle.bmp");
    memory->bitmaps[Bitmap_type_RAT_STEP] = win32_read_bmp("./bitmaps/ratStep1.bmp");
    memory->bitmaps[Bitmap_type_RAT_STEP + 1] = win32_read_bmp("./bitmaps/ratStep2.bmp");
    memory->bitmaps[Bitmap_type_RAT_STEP + 2] = win32_read_bmp("./bitmaps/hook.bmp");
    memory->bitmaps[Bitmap_type_RAT_STEP + 3] = win32_read_bmp("./bitmaps/rope.bmp");
}

void load_letters(Game_memory *memory, READ_FONT(stbtt_read_font), char *file_name, f32 letter_height)
{
    for (i32 index = 0; index < 256; index++)
    {
        memory->letters[index] = stbtt_read_font(file_name, index, letter_height);
    }
}