#pragma once

struct Vec3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vec3 operator+ (const Vec3& o) const {
        return {
            x + o.x,
            y + o.y,
            z + o.z
        };
    }

    Vec3 operator- (const Vec3& o) const {
        return {
            x - o.x,
            y - o.y,
            z - o.z
        };
    }

    Vec3 operator* (float s) const {
        return {
            x * s,
            y * s,
            z * s
        };
    }
};

struct Vec3i {
    int x = 0;
    int y = 0;
    int z = 0;

    bool operator== (const Vec3i& o) const {
        return x == o.x && y == o.y && z == o.z;
    }
};

struct AABB {
    Vec3 min;
    Vec3 max;

    bool intersects(const AABB& o) const {
        return (min.x < o.max.x && max.x > o.min.x) && (min.y < o.max.y && max.y > o.min.y) && (min.z < o.max.z && max.z > o.min.z);
    }
};