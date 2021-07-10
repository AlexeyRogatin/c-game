#include "stdlib.h"
#include "time.h"

typedef struct
{
    f32 x;
    f32 y;
} V2;

V2 operator+(V2 a, V2 b)
{
    V2 result = {a.x + b.x, a.y + b.y};
    return result;
}
V2 operator*(V2 a, f32 s)
{
    V2 result = {a.x * s, a.y * s};
    return result;
}
V2 operator*(f32 s, V2 a)
{
    V2 result = {a.x * s, a.y * s};
    return result;
}
V2 operator*(V2 a, V2 b)
{
    V2 result = {a.x * b.x, a.y * b.y};
    return result;
}
V2 operator-(V2 a, V2 b)
{
    V2 result = {a.x - b.x, a.y - b.y};
    return result;
}
V2 operator/(V2 a, f32 c)
{
    V2 result = {a.x / c, a.y / c};
    return result;
}
V2 operator/(f32 c, V2 a)
{
    V2 result = {c / a.x, c / a.y};
    return result;
}
V2 operator/(V2 a, V2 b)
{
    V2 result = {a.x / b.x, a.y / b.y};
    return result;
}
bool operator==(V2 a, V2 b)
{
    bool result = a.x == b.x && a.y == b.y;
    return result;
}
bool operator!=(V2 a, V2 b)
{
    bool result = !(a == b);
    return result;
}
V2 operator+=(V2 &a, V2 b)
{
    a = a + b;
    return a;
}
V2 operator-=(V2 &a, V2 b)
{
    a = a - b;
    return a;
}
V2 operator*=(V2 &a, f32 c)
{
    a = a * c;
    return a;
}
V2 operator*=(V2 &a, V2 b)
{
    a = a * b;
    return a;
}
V2 operator/=(V2 &a, f32 c)
{
    a = a / c;
    return a;
}
V2 operator/=(V2 &a, V2 b)
{
    a = a / b;
    return a;
}
V2 floor(V2 a)
{
    V2 result = V2{floorf(a.x), floorf(a.y)};
    return result;
}
V2 ceil(V2 a)
{
    V2 result = V2{ceilf(a.x), ceilf(a.y)};
    return result;
}
V2 round(V2 a)
{
    V2 result = V2{roundf(a.x), roundf(a.y)};
    return result;
}
f32 dot(V2 a, V2 b)
{
    f32 result = a.x * b.x + a.y * b.y;
    return result;
}
f32 length(V2 a)
{
    f32 length_sqr = a.x * a.x + a.y * a.y;
    f32 result = sqrtf(length_sqr);
    return result;
}
f32 length_sqr(V2 a)
{
    f32 result = a.x * a.x + a.y * a.y;
    return result;
}
V2 unit(V2 a)
{
    V2 result = {0, 0};
    f32 l = length(a);
    if (l != 0)
    {
        result = a / length(a);
    }
    return result;
};

V2 abs(V2 a)
{
    V2 result = {
        fabsf(a.x),
        fabsf(a.y),
    };
    return result;
}

u64 rol64(u64 x, int k)
{
    return (x << k) | (x >> (64 - k));
}

struct xoshiro256ss_state
{
    u64 s[4];
};

xoshiro256ss_state __global_random_state;
#define U64_MAX 0xffffffffffffffff

u64 xoshiro256ss(struct xoshiro256ss_state *state)
{
    u64 *s = state->s;
    u64 const result = rol64(s[1] * 5, 7) * 9;
    u64 const t = s[1] << 17;

    s[2] ^= s[0];
    s[3] ^= s[1];
    s[1] ^= s[2];
    s[0] ^= s[3];

    s[2] ^= t;
    s[3] = rol64(s[3], 45);

    return result;
}

struct splitmix64_state
{
    u64 s;
};

u64 splitmix64(struct splitmix64_state *state)
{
    u64 result = (state->s += 0x9E3779B97f4A7C15);
    result = (result ^ (result >> 30)) * 0xBF58476D1CE4E5B9;
    result = (result ^ (result >> 27)) * 0x94D049BB133111EB;
    return result ^ (result >> 31);
}

// as an example; one could do this same thing for any of the other generators
struct xoshiro256ss_state xorshift256_init(u64 seed)
{
    struct splitmix64_state smstate = {seed};
    struct xoshiro256ss_state result = {0};

    u64 tmp = splitmix64(&smstate);
    result.s[0] = tmp;
    tmp = splitmix64(&smstate);
    result.s[1] = tmp;
    tmp = splitmix64(&smstate);
    result.s[2] = tmp;
    tmp = splitmix64(&smstate);
    result.s[3] = tmp;

    return result;
}

f32 random_float(f32 start, f32 end)
{
    f32 r01 = (f32)xoshiro256ss(&__global_random_state) / (f32)U64_MAX;
    f32 result = r01 * (end - start) + start;
    return result;
};
i32 random_int(f32 start, f32 end)
{
    i32 result = (i32)round(random_float(start, end));
    return result;
};

V2 rotate_vector(V2 vector, f32 angle)
{
    V2 result = {
        cosf(angle) * vector.x - sinf(angle) * vector.y,
        sinf(angle) * vector.x + cosf(angle) * vector.y,
    };

    return result;
};

f32 angle_between_points(V2 a, V2 b)
{
    f32 tg = (b.y - a.y) / (b.x - a.x);
    f32 result = (f32)atan(tg);
    return result;
};

f32 distance_between_points(V2 a, V2 b)
{
    f32 result = sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
    return result;
}

f32 min(f32 a, f32 b)
{
    f32 result;
    if (a <= b)
    {
        result = a;
    }
    else
    {
        result = b;
    }
    return result;
}

f32 max(f32 a, f32 b)
{
    f32 result;
    if (a >= b)
    {
        result = a;
    }
    else
    {
        result = b;
    }
    return result;
}

V2 min(V2 a, V2 b)
{
    V2 result;
    result = {min(a.x, b.x), min(a.y, b.y)};
    return result;
}

V2 max(V2 a, V2 b)
{
    V2 result;
    result = {max(a.x, b.x), max(a.y, b.y)};
    return result;
}

V2 fract(V2 a)
{
    V2 result = {a.x - floorf(a.x), a.y - floorf(a.y)};
    return result;
}

V2 clamp01(V2 a)
{
    V2 result = {
        min(1.0f, max(0.0f, a.x)),
        min(1.0f, max(0.0f, a.y)),
    };
    return result;
}

V2 clamp(V2 a, V2 bord1, V2 bord2)
{
    V2 result = {
        min(bord2.x, max(bord1.x, a.x)),
        min(bord2.y, max(bord1.y, a.y)),
    };
    //  V2 result = clamp01((a - bord1) / bord2) * bord2 + bord1;
    return result;
}

typedef union
{
    u32 argb;
    struct
    {
        u8 b;
        u8 g;
        u8 r;
        u8 a;
    };
} ARGB;

union V4
{
    struct
    {
        f32 x, y, z, w;
    };
    struct
    {
        f32 r, g, b, a;
    };
};

V4 argb_to_v4(ARGB pixel)
{
    V4 result;
    result.r = (f32)pixel.r / 255.0f;
    result.g = (f32)pixel.g / 255.0f;
    result.b = (f32)pixel.b / 255.0f;
    result.a = (f32)pixel.a / 255.0f;
    return result;
}

V4 lerp(V4 a, V4 b, f32 f)
{
    V4 result;
    result.r = a.r * (1 - f) + b.r * f;
    result.g = a.g * (1 - f) + b.g * f;
    result.b = a.b * (1 - f) + b.b * f;
    result.a = a.a * (1 - f) + b.a * f;
    return result;
}

V4 operator*(V4 a, f32 s)
{
    V4 result;
    result.x = a.x * s;
    result.y = a.y * s;
    result.z = a.z * s;
    result.w = a.w * s;
    return result;
}
V4 operator+(V4 a, V4 b)
{
    V4 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    result.w = a.w + b.w;
    return result;
}

V4 operator*(f32 s, V4 a)
{
    V4 result;
    result.x = a.x * s;
    result.y = a.y * s;
    result.z = a.z * s;
    result.w = a.w * s;
    return result;
}

ARGB v4_to_argb(V4 color)
{
    ARGB result;
    result.r = (u8)(color.r * 255);
    result.g = (u8)(color.g * 255);
    result.b = (u8)(color.b * 255);
    result.a = (u8)(color.a * 255);
    return result;
}

#include <immintrin.h>
#include <intrin.h>

struct f32_8x
{
    __m256 v;

    f32 &operator[](int i)
    {
        return ((f32 *)&v)[i];
    }
};

f32_8x set1_f32(f32 value)
{
    f32_8x result = {_mm256_set1_ps(value)};
    return result;
}

f32_8x set8_f32(f32 a, f32 b, f32 c, f32 d, f32 e, f32 f, f32 g, f32 h)
{
    f32_8x result = {_mm256_setr_ps(a, b, c, d, e, f, g, h)};
    return result;
}

f32_8x operator*(f32_8x a, f32_8x b)
{
    f32_8x result = {_mm256_mul_ps(a.v, b.v)};
    return result;
}

f32_8x operator/(f32_8x a, f32_8x b)
{
    f32_8x result = {_mm256_div_ps(a.v, b.v)};
    return result;
}

f32_8x operator+(f32_8x a, f32_8x b)
{
    f32_8x result = {_mm256_add_ps(a.v, b.v)};
    return result;
}

f32_8x operator-(f32_8x a, f32_8x b)
{
    f32_8x result = {_mm256_sub_ps(a.v, b.v)};
    return result;
}

f32_8x operator+=(f32_8x &a, f32_8x b)
{
    a = a + b;
    return a;
}

f32_8x floor(f32_8x v)
{
    f32_8x result = {_mm256_floor_ps(v.v)};
    return result;
}

struct i32_8x
{
    __m256i v;

    i32 &operator[](int i)
    {
        return ((i32 *)&v)[i];
    }
};

i32_8x to_i32_8x(f32_8x v)
{
    i32_8x result = {_mm256_cvtps_epi32(v.v)};
    return result;
}

f32_8x to_f32_8x(i32_8x v)
{
    f32_8x result = {_mm256_cvtepi32_ps(v.v)};
    return result;
}

i32_8x gather(const int *address, i32_8x offsets, i32_8x mask)
{
    __m256i default = {};
    i32_8x result = {_mm256_mask_i32gather_epi32(default, address, offsets.v, mask.v, 4)};
    return result;
}

i32_8x set1_i32(i32 a)
{
    i32_8x result = {_mm256_set1_epi32(a)};
    return result;
}

i32_8x operator>>(i32_8x a, i32 b)
{
    i32_8x result = {_mm256_srli_epi32(a.v, b)};
    return result;
}

i32_8x operator<<(i32_8x a, i32 b)
{
    i32_8x result = {_mm256_slli_epi32(a.v, b)};
    return result;
}

i32_8x operator|(i32_8x a, i32_8x b)
{
    i32_8x result = {_mm256_or_si256(a.v, b.v)};
    return result;
}

i32_8x operator&(i32_8x a, i32_8x b)
{
    i32_8x result = {_mm256_and_si256(a.v, b.v)};
    return result;
}

i32_8x load(void *address)
{
    i32_8x result = {_mm256_load_si256((const __m256i *)address)};
    return result;
}

i32_8x operator>=(f32_8x a, f32_8x b)
{
    __m256 r = _mm256_cmp_ps(b.v, a.v, _CMP_LT_OS);
    i32_8x result = {*(__m256i *)&r};
    return result;
}
i32_8x operator<(f32_8x a, f32_8x b)
{
    __m256 r = _mm256_cmp_ps(a.v, b.v, _CMP_LT_OS);
    i32_8x result = {*(__m256i *)&r};
    return result;
}

void store(void *address, i32_8x data, i32_8x mask)
{
    _mm256_maskstore_epi32((int *)address, mask.v, data.v);
}

struct V2_8x
{
    f32_8x x, y;

    V2 operator[](int i)
    {
        return V2{x[i], y[i]};
    }
};

V2_8x set1(V2 v)
{
    V2_8x result = {
        set1_f32(v.x),
        set1_f32(v.y),
    };
    return result;
}

f32_8x dot(V2_8x a, V2_8x b)
{
    f32_8x result = (a.x * b.x) + (a.y * b.y);
    return result;
}

V2_8x operator*(V2_8x a, V2_8x b)
{
    V2_8x result = {
        a.x * b.x,
        a.y * b.y,
    };
    return result;
}

V2_8x operator+(V2_8x a, V2_8x b)
{
    V2_8x result = {
        a.x + b.x,
        a.y + b.y,
    };
    return result;
}

V2_8x operator-(V2_8x a, V2_8x b)
{
    V2_8x result = {
        a.x - b.x,
        a.y - b.y,
    };
    return result;
}

V2_8x operator+=(V2_8x &a, V2_8x b)
{
    a = a + b;
    return a;
}

V2_8x floor(V2_8x v)
{
    V2_8x result = {
        floor(v.x),
        floor(v.y),
    };
    return result;
}

V2_8x fract(V2_8x v)
{
    V2_8x result = v - floor(v);
    return result;
}

f32_8x min(f32_8x a, f32_8x b)
{
    f32_8x result = {_mm256_min_ps(a.v, b.v)};
    return result;
}

f32_8x max(f32_8x a, f32_8x b)
{
    f32_8x result = {_mm256_max_ps(a.v, b.v)};
    return result;
}

V2_8x clamp01(V2_8x v)
{
    V2_8x result = V2_8x{
        min(set1_f32(1), max(v.x, set1_f32(0))),
        min(set1_f32(1), max(v.y, set1_f32(0))),
    };
    return result;
}

V2_8x clamp(V2_8x v, V2_8x bord1, V2_8x bord2)
{
    V2_8x result = V2_8x{
        min(bord2.x, max(v.x, bord1.x)),
        min(bord2.y, max(v.y, bord1.y)),
    };
    return result;
}

union V4_8x
{
    struct
    {
        f32_8x x, y, z, w;
    };
    struct
    {
        f32_8x r, g, b, a;
    };

    V4 operator[](int i)
    {
        return V4{x[i], y[i], z[i], w[i]};
    }
};

// 00 00 00 FF (mask)

// AA RR GG BB (c)

// 00 00 00 AA
// c >> 24

// 00 00 00 RR
// (c >> 16) | mask

// 00 00 00 GG
// (c >> 8) | mask

// 00 00 00 BB
// c | mask

V4_8x argb_to_v4_8x(i32_8x pixel)
{
    V4_8x result;
    i32_8x mask = set1_i32(0xFF);
    f32_8x pixel_a = to_f32_8x(pixel >> 24);
    f32_8x pixel_r = to_f32_8x((pixel >> 16) & mask);
    f32_8x pixel_g = to_f32_8x((pixel >> 8) & mask);
    f32_8x pixel_b = to_f32_8x(pixel & mask);

    f32_8x one_over_255 = set1_f32(1 / 255.0f);

    result.r = pixel_r * one_over_255;
    result.g = pixel_g * one_over_255;
    result.b = pixel_b * one_over_255;
    result.a = pixel_a * one_over_255;
    return result;
}

V4_8x lerp(V4_8x a, V4_8x b, f32_8x f)
{
    V4_8x result;
    f32_8x one_8x = set1_f32(1);
    result.r = a.r * (one_8x - f) + b.r * f;
    result.g = a.g * (one_8x - f) + b.g * f;
    result.b = a.b * (one_8x - f) + b.b * f;
    result.a = a.a * (one_8x - f) + b.a * f;
    return result;
}

V4_8x operator*(V4_8x a, f32_8x s)
{
    V4_8x result;
    result.x = a.x * s;
    result.y = a.y * s;
    result.z = a.z * s;
    result.w = a.w * s;
    return result;
}
V4_8x operator+(V4_8x a, V4_8x b)
{
    V4_8x result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    result.w = a.w + b.w;
    return result;
}

V4_8x operator*(f32_8x s, V4_8x a)
{
    V4_8x result;
    result.x = a.x * s;
    result.y = a.y * s;
    result.z = a.z * s;
    result.w = a.w * s;
    return result;
}

V4_8x operator*(V4_8x a, V4_8x b)
{
    V4_8x result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    result.z = a.z * b.z;
    result.w = a.w * b.w;
    return result;
}

i32_8x v4_to_argb_8x(V4_8x color)
{
    f32_8x one255 = set1_f32(255);
    i32_8x result;
    i32_8x a = to_i32_8x(color.a * one255) << 24;
    i32_8x r = to_i32_8x(color.r * one255) << 16;
    i32_8x g = to_i32_8x(color.g * one255) << 8;
    i32_8x b = to_i32_8x(color.b * one255);
    result = a | r | g | b;
    return result;
}

V4_8x set1(V4 v)
{
    V4_8x result = {
        set1_f32(v.x),
        set1_f32(v.y),
        set1_f32(v.z),
        set1_f32(v.w),
    };
    return result;
}

V4_8x clamp01(V4_8x v)
{
    V4_8x result = V4_8x{
        min(set1_f32(1), max(v.x, set1_f32(0))),
        min(set1_f32(1), max(v.y, set1_f32(0))),
        min(set1_f32(1), max(v.z, set1_f32(0))),
        min(set1_f32(1), max(v.w, set1_f32(0))),
    };
    return result;
}

typedef struct
{
    V2 min;
    V2 max;
} Rect;

V2 get_size(Rect rect)
{
    V2 size = {rect.max.x - rect.min.x, rect.max.y - rect.min.y};
    return size;
}

f32 get_area(Rect r)
{
    V2 size = get_size(r);
    f32 result = size.x * size.y;
    return result;
}

V2 get_center(Rect rect)
{
    V2 result = rect.min + get_size(rect) * 0.5;
    return result;
}

Rect intersect(Rect rect1, Rect rect2)
{
    Rect result = {
        {max(rect1.min.x, rect2.min.x), max(rect1.min.y, rect2.min.y)},
        {min(rect1.max.x, rect2.max.x), min(rect1.max.y, rect2.max.y)}};
    return result;
}

bool has_area(Rect rect)
{
    V2 size = get_size(rect);
    bool result = size.x > 0 && size.y > 0;
    return result;
}
