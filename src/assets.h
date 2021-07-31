bool win32_write_entire_file(char *file_name)
{
    bool result = false;

    HANDLE file = CreateFileA(
        file_name,
        GENERIC_WRITE,
        NULL,
        NULL,
        CREATE_ALWAYS,
        NULL,
        NULL);

    assert(file != INVALID_HANDLE_VALUE);

    DWORD file_size_high;
    DWORD file_size = GetFileSize(file, &file_size_high);

    byte *file_memory = (byte *)malloc(file_size);
    DWORD bytes_written;

    if (WriteFile(file, file_memory, file_size, &bytes_written, NULL))
    {
        result = true;
    }

    return result;
}

File_Buffer win32_read_entire_file(char *file_name)
{
    HANDLE file = CreateFileA(
        file_name,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    assert(file != INVALID_HANDLE_VALUE);

    DWORD file_size_high;
    DWORD file_size = GetFileSize(file, &file_size_high);

    byte *file_memory = (byte *)malloc(file_size);
    DWORD bytes_read = 0;

    ReadFile(file, file_memory, file_size, &bytes_read, NULL);

    File_Buffer result;
    result.data = file_memory;
    result.size = file_size;
    return result;
}

//pack убирает промежутки между данными в struct
#pragma pack(push, 1)

typedef struct
{
    u16 signature;
    u32 file_size;
    u32 unused;
    u32 data_offset;
} Bmp_Header;

typedef struct
{
    u32 header_size;
    u32 width;
    u32 height;
    u16 planes;
    u16 bits_per_pixel;
    u32 compression;
    u32 image_size;
    u32 x_pixels_per_meter;
    u32 y_pixels_per_meter;
    u32 colors_used;
    u32 important_colors;
} Bmp_Info;
#pragma pack(pop)

READ_BMP(win32_read_bmp)
{
    File_Buffer file = win32_read_entire_file(file_name);
    Bmp_Header *header = (Bmp_Header *)file.data;
    Bmp_Info *info = (Bmp_Info *)(file.data + sizeof(Bmp_Header));
    u32 *pixels = (u32 *)(file.data + header->data_offset);

    Bitmap result = create_empty_bitmap(V2{(f32)info->width, (f32)info->height});

    u32 bitmap_start_offset = result.pitch + 1;

    for (u32 y = 0; y < info->height; y++)
    {
        for (u32 x = 0; x < info->width; x++)
        {
            ARGB pixel;
            pixel.argb = pixels[y * info->width + x];
            f32 alpha = (f32)(pixel.a / 0xFF);
            pixel.r = (u8)roundf(pixel.r * alpha);
            pixel.g = (u8)roundf(pixel.g * alpha);
            pixel.b = (u8)roundf(pixel.b * alpha);

            result.pixels[y * result.pitch + x + bitmap_start_offset] = pixel.argb;
        }
    }

    free(file.data);

    return result;
}

#define STB_TRUETYPE_IMPLEMENTATION 1
#include "stb_truetype.h"

Letter stbtt_read_font(char *font_name, i32 letter_code, f32 letter_height)
{
    File_Buffer ttf_file = win32_read_entire_file(font_name);
    stbtt_fontinfo font;
    stbtt_InitFont(&font, ttf_file.data, stbtt_GetFontOffsetForIndex(ttf_file.data, 0));

    i32 width, height, x_offset, y_offset;
    f32 scale = stbtt_ScaleForPixelHeight(&font, letter_height);
    u8 *mono_bitmap = stbtt_GetCodepointBitmap(&font, 0, scale, letter_code, &width, &height, &x_offset, &y_offset);

    Letter result;
    result.bitmap = create_empty_bitmap(V2{(f32)width, (f32)height});
    result.offset = V2{(f32)x_offset, (f32)y_offset};
    result.offset.y = height + result.offset.y;

    //создаём битмап символа
    u8 *source = mono_bitmap;
    u8 *dest_row = (u8 *)result.bitmap.pixels;

    for (i32 y = height - 1; y >= 0; y--)
    {
        u32 *dest = (u32 *)dest_row + result.bitmap.pitch * y;
        for (i32 x = 0; x < width; x++)
        {
            u8 alpha = *source++;
            *dest++ = (alpha << 24) | (alpha << 16) | (alpha << 8) | alpha;
        }
    }

    stbtt_FreeBitmap(mono_bitmap, 0);

    //таблица кернинга
    i32 length = stbtt_GetKerningTableLength(&font);
    stbtt_kerningentry *table = (stbtt_kerningentry *)malloc(sizeof(stbtt_kerningentry) * length);
    stbtt_GetKerningTable(&font, table, length);
    i32 kernings[256] = {0};
    for (i32 code = 0; code < 256; code++)
    {
        for (i32 index = 0; index < length; index++)
        {
            stbtt_kerningentry entry = table[index];
            if (entry.glyph1 == stbtt_FindGlyphIndex(&font, letter_code) && entry.glyph2 == stbtt_FindGlyphIndex(&font, code))
            {
                kernings[code] = (i32)(entry.advance * scale);
                break;
            }
        }
    }

    // for (i32 code = 0; code < 256; code++)
    // {
    //     kernings[code] = (i32)(stbtt_GetCodepointKernAdvance(&font, letter_code, code));
    // }

    if (letter_code == 'V' || letter_code == 'A')
    {
        i32 foo = 0;
    }

    result.kernings = kernings;
    i32 left_side_bearing;
    stbtt_GetCodepointHMetrics(&font, letter_code, &result.advance, &left_side_bearing);
    result.advance = (i32)(result.advance * scale);

    return result;
}