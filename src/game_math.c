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
V2 operator/=(V2 &a, f32 c)
{
    a = a / c;
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

f32 random_float(f32 start, f32 end)
{
    f32 result = rand() / (f32)RAND_MAX * (end - start) + start;
    return result;
};
i32 random_int(f32 start, f32 end)
{
    i32 result = round(random_float(start, end));
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
    f32 result = atan(tg);
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

union V4 {
    struct {
        f32 x, y, z, w;
    };
    struct {
        f32 r, g, b, a;
    };
};

V4 argb_to_v4(ARGB pixel) {
    V4 result;
    result.r = (f32)pixel.r/255.0f;
    result.g = (f32)pixel.g/255.0f;
    result.b = (f32)pixel.b/255.0f;
    result.a = (f32)pixel.a/255.0f;
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

V4 operator*(V4 a, f32 s) {
    V4 result;
    result.x = a.x*s;
    result.y = a.y*s;
    result.z = a.z*s;
    result.w = a.w*s;
    return result;
}
V4 operator+(V4 a, V4 b) {
    V4 result;
    result.x = a.x+b.x;
    result.y = a.y+b.y;
    result.z = a.z+b.z;
    result.w = a.w+b.w;
    return result;
}

V4 operator*(f32 s, V4 a) {
    V4 result;
    result.x = a.x*s;
    result.y = a.y*s;
    result.z = a.z*s;
    result.w = a.w*s;
    return result;
}

ARGB v4_to_argb(V4 color) {
    ARGB result;
    result.r = (u8)(color.r*255);
    result.g = (u8)(color.g*255);
    result.b = (u8)(color.b*255);
    result.a = (u8)(color.a*255);
    return result;
}