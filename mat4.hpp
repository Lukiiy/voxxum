#pragma once

#include "mathutils.hpp"
#include <cmath>

struct Mat4 {
    float m[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
};

inline Mat4 operator* (const Mat4& a, const Mat4& b) {
    Mat4 result;

    for (int column = 0; column < 4; ++column)
        for (int row = 0; row < 4; ++row) {
            float s = 0.0f;

            for (int k = 0; k < 4; ++k) s += a.m[k * 4 + row] * b.m[column * 4 + k];

            result.m[column * 4 + row] = s;
        }

    return result;
}

inline Mat4 translation(float x, float y, float z) {
    Mat4 result;

    result.m[12] = x;
    result.m[13] = y;
    result.m[14] = z;

    return result;
}

inline Mat4 uniformScale(float scale) {
    Mat4 result;

    result.m[0] = scale;
    result.m[5] = scale;
    result.m[10] = scale;

    return result;
}

inline Mat4 perspective(float fovYRadians, float aspect, float nearZ, float farZ) {
    const float fov = 1.0f / std::tan(fovYRadians * .5f);
    Mat4 result;

    result.m[0] = fov / aspect;
    result.m[5] = fov;
    result.m[10] = (farZ + nearZ) / (nearZ - farZ);
    result.m[11] = -1.0f;
    result.m[14] = 2.0f * farZ * nearZ / (nearZ - farZ);
    result.m[15] = 0.0f;

    return result;
}

inline float dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vec3 cross(const Vec3& a, const Vec3& b) {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

// forward must be unit length
inline Mat4 lookAlong(const Vec3& eye, const Vec3& forward, const Vec3& up = {0.0f, 1.0f, 0.0f}) {
    Vec3 s = cross(forward, up);
    const float len = std::sqrt(dot(s, s));

    s = s * (1.0f / len);

    const Vec3 u = cross(s, forward);
    Mat4 result;

    result.m[0] = s.x; result.m[4] = s.y; result.m[8] = s.z;
    result.m[1] = u.x; result.m[5] = u.y; result.m[9] = u.z;
    result.m[2] = -forward.x; result.m[6] = -forward.y; result.m[10] = -forward.z;
    result.m[12] = -dot(s, eye);
    result.m[13] = -dot(u, eye);
    result.m[14] = dot(forward, eye);

    return result;
}
