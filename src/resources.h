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
    memory->bitmaps[0] = win32_read_bmp("./bitmaps/none.bmp");
    memory->bitmaps[1] = win32_read_bmp("./bitmaps/test.bmp");
    memory->bitmaps[2] = win32_read_bmp("./bitmaps/jermaIdle.bmp");
    memory->bitmaps[3] = win32_read_bmp("./bitmaps/jermaJump.bmp");
    memory->bitmaps[4] = win32_read_bmp("./bitmaps/jermaCrouchIdle.bmp");
    memory->bitmaps[5] = win32_read_bmp("./bitmaps/jermaWall.bmp");
    memory->bitmaps[6] = win32_read_bmp("./bitmaps/jermaWall.bmp");
    memory->bitmaps[7] = win32_read_bmp("./bitmaps/jermaWall.bmp");
    memory->bitmaps[8] = win32_read_bmp("./bitmaps/jermaLookingUp.bmp");
    memory->bitmaps[9] = win32_read_bmp("./bitmaps/jermaStep1.bmp");
    memory->bitmaps[10] = win32_read_bmp("./bitmaps/jermaStep2.bmp");
    memory->bitmaps[11] = win32_read_bmp("./bitmaps/jermaStep3.bmp");
    memory->bitmaps[12] = win32_read_bmp("./bitmaps/jermaStep4.bmp");
    memory->bitmaps[13] = win32_read_bmp("./bitmaps/jermaStep5.bmp");
    memory->bitmaps[14] = win32_read_bmp("./bitmaps/jermaStep6.bmp");
    memory->bitmaps[15] = win32_read_bmp("./bitmaps/jermaCrouchIdle1.bmp");
    memory->bitmaps[16] = win32_read_bmp("./bitmaps/jermaCrouchIdle2.bmp");
    memory->bitmaps[17] = win32_read_bmp("./bitmaps/jermaCrouchIdle3.bmp");
    memory->bitmaps[18] = win32_read_bmp("./bitmaps/jermaCrouchIdle4.bmp");
    memory->bitmaps[19] = win32_read_bmp("./bitmaps/jermaCrouchStep1.bmp");
    memory->bitmaps[20] = win32_read_bmp("./bitmaps/jermaCrouchStep2.bmp");
    memory->bitmaps[21] = win32_read_bmp("./bitmaps/jermaCrouchStep3.bmp");
    memory->bitmaps[22] = win32_read_bmp("./bitmaps/jermaCrouchStep4.bmp");
    memory->bitmaps[23] = win32_read_bmp("./bitmaps/jermaCrouchStep5.bmp");
    memory->bitmaps[24] = win32_read_bmp("./bitmaps/jermaCrouchStep6.bmp");
    memory->bitmaps[25] = win32_read_bmp("./bitmaps/zombieIdle.bmp");
    memory->bitmaps[26] = win32_read_bmp("./bitmaps/zombieStep1.bmp");
    memory->bitmaps[27] = win32_read_bmp("./bitmaps/zombieStep2.bmp");
    memory->bitmaps[28] = win32_read_bmp("./bitmaps/zombieStep3.bmp");
    memory->bitmaps[29] = win32_read_bmp("./bitmaps/zombieStep4.bmp");
    memory->bitmaps[30] = win32_read_bmp("./bitmaps/zombieStep5.bmp");
    memory->bitmaps[31] = win32_read_bmp("./bitmaps/zombieStep6.bmp");
    memory->bitmaps[32] = win32_read_bmp("./bitmaps/backGround.bmp");
    memory->bitmaps[33] = win32_read_bmp("./bitmaps/border.bmp");
    memory->bitmaps[34] = win32_read_bmp("./bitmaps/borderWithTransition.bmp");
    memory->bitmaps[35] = turn_bitmap(memory->bitmaps[Bitmap_type_TRANSITION_BORDER_0], 0.5 * PI);
    memory->bitmaps[36] = turn_bitmap(memory->bitmaps[Bitmap_type_TRANSITION_BORDER_0], PI);
    memory->bitmaps[37] = turn_bitmap(memory->bitmaps[Bitmap_type_TRANSITION_BORDER_0], 1.5 * PI);
    memory->bitmaps[38] = win32_read_bmp("./bitmaps/cornerBorder.bmp");
    memory->bitmaps[39] = turn_bitmap(memory->bitmaps[Bitmap_type_CORNER_BORDER_0], 0.5 * PI);
    memory->bitmaps[40] = turn_bitmap(memory->bitmaps[Bitmap_type_CORNER_BORDER_0], PI);
    memory->bitmaps[41] = turn_bitmap(memory->bitmaps[Bitmap_type_CORNER_BORDER_0], 1.5 * PI);
    memory->bitmaps[42] = win32_read_bmp("./bitmaps/brick1.bmp");
    memory->bitmaps[43] = win32_read_bmp("./bitmaps/brick2.bmp");
    memory->bitmaps[44] = win32_read_bmp("./bitmaps/brick3.bmp");
    memory->bitmaps[45] = win32_read_bmp("./bitmaps/brick4.bmp");
    memory->bitmaps[46] = win32_read_bmp("./bitmaps/brick5.bmp");
    memory->bitmaps[47] = win32_read_bmp("./bitmaps/brick6.bmp");
    memory->bitmaps[48] = win32_read_bmp("./bitmaps/brick7.bmp");
    memory->bitmaps[49] = win32_read_bmp("./bitmaps/brick8.bmp");
    memory->bitmaps[50] = win32_read_bmp("./bitmaps/brick9.bmp");
    memory->bitmaps[51] = win32_read_bmp("./bitmaps/brick10.bmp");
    memory->bitmaps[52] = win32_read_bmp("./bitmaps/brick11.bmp");
    memory->bitmaps[53] = win32_read_bmp("./bitmaps/brick12.bmp");
    memory->bitmaps[54] = win32_read_bmp("./bitmaps/elegantBrick.bmp");
    memory->bitmaps[55] = win32_read_bmp("./bitmaps/stone.bmp");
    memory->bitmaps[56] = win32_read_bmp("./bitmaps/floor1.bmp");
    memory->bitmaps[57] = win32_read_bmp("./bitmaps/floor5.bmp");
    memory->bitmaps[58] = win32_read_bmp("./bitmaps/door1.bmp");
    memory->bitmaps[59] = win32_read_bmp("./bitmaps/door2.bmp");
    memory->bitmaps[60] = win32_read_bmp("./bitmaps/door3.bmp");
    memory->bitmaps[61] = win32_read_bmp("./bitmaps/door4.bmp");
    memory->bitmaps[62] = win32_read_bmp("./bitmaps/door5.bmp");
    memory->bitmaps[63] = win32_read_bmp("./bitmaps/door6.bmp");
    memory->bitmaps[64] = win32_read_bmp("./bitmaps/door7.bmp");
    memory->bitmaps[65] = win32_read_bmp("./bitmaps/doorBack.bmp");
    memory->bitmaps[66] = win32_read_bmp("./bitmaps/parapet.bmp");
    memory->bitmaps[67] = win32_read_bmp("./bitmaps/health_bar.bmp");
    memory->bitmaps[68] = win32_read_bmp("./bitmaps/health.bmp");
    memory->bitmaps[69] = win32_read_bmp("./bitmaps/spikes.bmp");
    memory->bitmaps[70] = win32_read_bmp("./bitmaps/toyGun.bmp");
    memory->bitmaps[71] = win32_read_bmp("./bitmaps/toyGunBullet.bmp");
    memory->bitmaps[72] = win32_read_bmp("./bitmaps/exitSign.bmp");
    memory->bitmaps[73] = win32_read_bmp("./bitmaps/exitSignOff.bmp");
    memory->bitmaps[74] = win32_read_bmp("./bitmaps/lamp.bmp");
    memory->bitmaps[75] = win32_read_bmp("./bitmaps/lampOff.bmp");
    memory->bitmaps[76] = win32_read_bmp("./bitmaps/bomb.bmp");
    memory->bitmaps[77] = win32_read_bmp("./bitmaps/plank.bmp");
    memory->bitmaps[78] = win32_read_bmp("./bitmaps/unbreakablePlank.bmp");
    memory->bitmaps[79] = win32_read_bmp("./bitmaps/ratIdle.bmp");
    memory->bitmaps[80] = win32_read_bmp("./bitmaps/ratStep1.bmp");
    memory->bitmaps[81] = win32_read_bmp("./bitmaps/ratStep2.bmp");
}

void load_letters(Game_memory *memory, READ_FONT(stbtt_read_font), char *file_name, f32 letter_height)
{
    for (i32 index = 0; index < 256; index++)
    {
        memory->letters[index] = stbtt_read_font(file_name, index, letter_height);
    }
}