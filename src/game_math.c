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