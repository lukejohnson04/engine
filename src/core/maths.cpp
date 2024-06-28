#pragma once

#define MAX(a,b) ((a>=b)?(a):(b))
#define MIN(a,b) ((a<=b)?(a):(b))

//#include <cmath>

#define PI 3.1415926f

v2 v2::normalize() {
    float len = sqrt((x * x) + (y * y));
    return v2(x / len, y / len);
}

v2 v2::mult(float factor) {
    return v2(x * factor, y * factor);
}

v2 v2::rotate(float rad) {
    v2 res;
    float cs = cos(rad);
    float sn = sin(rad);

    res.x = x * cs - y * sn; 
    res.y = x * sn + y * cs;
    return res;
}

float v2::length() {
    return sqrt((x * x) + (y * y));
}

internal
float V2DotProduct(v2 a, v2 b) {
    return a.x * b.x + a.y * b.y;
}

internal
v2 V2Add(v2 a, v2 b) {
    return v2(a.x + b.x, a.y + b.y);
}

internal
v2 V2Sub(v2 a, v2 b) {
    return v2(a.x - b.x, a.y - b.y);
}

internal
float angle_to(v2 a, v2 b) {
    float angle = atan2(a.y - b.y, a.x - b.x);
    return angle;
}

internal
v2 angle_to_vec(float angle) {
    v2 p = {-cos(angle), -sin(angle)};
    return p;
}

// Function to determine difference between angles, accounting for wrapping (i.e. comparing 720 and 30)
internal
float angle_diff(float a, float b) {
    return PI - abs(abs(a-b) - PI);
}

internal
v2 vec_to(v2 a, v2 b) {
    v2 diff = {b.x - a.x, b.y - a.y};
    return diff.normalize();
}

internal
float vec_to_angle(v2 a) {
    return angle_to({0,0},a);
}

internal
int lerp(int a, int b, float t) {
    return int(a * (1.0 - t) + (b * t));
}

internal
float lerp(float a, float b, float t) {
    return a * (1.0 - t) + (b * t);
}

internal
float deg_2_rad(float d) {
    return d * (PI/180.f);
}

internal
float rad_2_deg(float d) {
    return d / (PI/180.f);
}

internal
bool rect_contains_point(iRect rect, v2i point) {
    return point.x > rect.x && point.x < rect.x + rect.w && point.y > rect.y && point.y < rect.y + rect.h;
}

internal
float distance_between(v2 a, v2 b) {
    float x_sqr = (a.x-b.x)*(a.x-b.x);
    float y_sqr = (a.y-b.y)*(a.y-b.y);
    return sqrt(x_sqr + y_sqr);
}

internal
float lerp_rotation(float rot, float dest, float t) {
    if (dest - rot > PI) {
        dest -= PI*2;
    }
    float res = lerp(rot,dest,t);
    if (res < 0) {
        res += PI*2;
    }
    return res;
}

internal
inline float wrap_rotation(float rot) {
    while (rot >= PI*2) {
        rot -= PI*2;
    }
    while (rot < 0) {
        rot += PI*2;
    }
    return rot;
}
