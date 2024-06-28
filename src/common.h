#pragma once
#include <inttypes.h>

#define FPS_MAX 60.0f
#define FRAME_TIME_MS (1000.f / FPS_MAX)
#define FRAME_TIME (1.0f / FPS_MAX)

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef int32_t bool32;
typedef bool32 b32;

typedef float real32;
typedef double real64;

typedef real32 f32; // float
typedef real64 d64; // double

typedef u16 entity_id;

#define local_persist static
#define global_variable static
#define internal static

#define Kilobytes(size) (1024LL*size)
#define Megabytes(size) (1024LL*Kilobytes(size))
#define Gigabytes(size) (1024LL*Megabytes(size))

struct GameMemory {
    bool is_initialized;
    u64 size;
    void *permanentStorage;
};

#define ID_DONT_EXIST 999999999

// collision layers
enum {
    LAYER_NONE=0,
    LAYER_PHYSICS=1
};

struct Color {
    u8 r, g, b, a;
    Color(u8 nr=0, u8 ng=0, u8 nb=0, u8 na=0) : r(nr), g(ng), b(nb), a(na) {}
    Color(u32 hex) : r((hex >> 24) & u8(~(1<<8))), g((hex >> 16) & u8(~(1<<8))), b((hex >> 8) & u8(~(1<<8))), a(hex & u8(~(1<<8))) {}

    u32 hex() {
        return (r << 24) | (g << 16) | (b << 8) | a;
    }
};

inline bool operator==(Color left, Color right) {
    return left.r == right.r && left.g == right.g && left.b == right.b && left.a == right.a;
}

struct v2 {
    v2(float nx, float ny) : x(nx), y(ny) {}
    v2() {}
    float x = 0;
    float y = 0;
    v2 normalize();
    v2 mult(float factor);
    v2 rotate(float rad);
    float length();
};

inline v2 operator*(v2 left, float mult) {
    return {left.x*mult,left.y*mult};
}

inline v2 &operator*=(v2 &left, float mult) {
    left.x *= mult;
    left.y *= mult;
    return left;
}

inline v2 operator+(v2 left, v2 right) {
    return {left.x+right.x,left.y+right.y};
}

inline v2 operator-(v2 left, v2 right) {
    return {left.x-right.x,left.y-right.y};
}

inline v2 operator-(v2 right) {
    return {-right.x,-right.y};
}

inline v2 &operator+=(v2 &left, v2 right) {
    left.x += right.x;
    left.y += right.y;
    return left;
}

inline v2 &operator-=(v2 &left, v2 right) {
    left.x -= right.x;
    left.y -= right.y;
    return left;
}



struct v2i {
    int x = 0;
    int y = 0;
};


struct Transform {
    v2 pos={0,0};
    v2 scale={1,1};
    v2 origin={0,0};
    // radians
    float rotation=0.f;
    // pull flip out of this struct
    bool flip=false;
    // have room for one more float alignment?
    // apply parent transform
    Transform GetInverse() {
        Transform cp = *this;
        cp.pos.x *= -1;
        cp.pos.y *= -1;
        // flip scale over 1
        cp.scale.x = 1.f/cp.scale.x;
        cp.scale.y = 1.f/cp.scale.y;
        cp.origin.x *= -1;
        cp.origin.y *= -1;
        return cp;
    }

    Transform Apply(Transform transform) {
        Transform cp = *this;
        cp.pos.x += transform.pos.x;
        cp.pos.y += transform.pos.y;
        cp.scale.x *= transform.scale.x;
        cp.scale.y *= transform.scale.y;
        return cp;
    }
};

struct fRect {
    fRect() {}
    fRect(float nx, float ny, float nw, float nh) : x(nx),y(ny),w(nw),h(nh){}
    bool contains(int px, int py);
    float x=0, y=0, w=0, h=0;
};

struct iRect {
    iRect() {}
    iRect(int nx, int ny, int nw, int nh) : x(nx),y(ny),w(nw),h(nh){}
    bool contains(int px, int py);
    int x=0, y=0, w=0, h=0;
};

bool fRect::contains(int px, int py) {
    if (!w || !h) {
        return false;
    }
    return (px >= x && px <= x + w && py >= y && py <= y + h);
}

bool iRect::contains(int px, int py) {
    if (!w || !h) {
        return false;
    }
    return (px >= x && px <= x + w && py >= y && py <= y + h);
}
