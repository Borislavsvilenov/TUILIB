#ifndef VEC2_H
#define VEC2_H

#include <math.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// Data Structures
// ============================================================================

typedef struct {
    float x;
    float y;
} Vec2;

typedef struct {
    int x;
    int y;
} Vec2i;

// Constant zero vectors
#define VEC2_ZERO  ((Vec2){0.0f, 0.0f})
#define VEC2I_ZERO ((Vec2i){0, 0})

// ============================================================================
// Float Vector (Vec2) Operations
// ============================================================================

static inline Vec2 vec2_add(Vec2 a, Vec2 b) { return (Vec2){a.x + b.x, a.y + b.y}; }
static inline Vec2 vec2_sub(Vec2 a, Vec2 b) { return (Vec2){a.x - b.x, a.y - b.y}; }
static inline Vec2 vec2_scale(Vec2 a, float s) { return (Vec2){a.x * s, a.y * s}; }
static inline Vec2 vec2_div(Vec2 a, float s) { return (Vec2){a.x / s, a.y / s}; }
static inline Vec2 vec2_neg(Vec2 a) { return (Vec2){-a.x, -a.y}; }

static inline float vec2_dot(Vec2 a, Vec2 b) { return a.x * b.x + a.y * b.y; }
static inline float vec2_cross(Vec2 a, Vec2 b) { return a.x * b.y - a.y * b.x; }

static inline float vec2_mag_sq(Vec2 a) { return a.x * a.x + a.y * a.y; }
static inline float vec2_mag(Vec2 a) { return sqrt(vec2_mag_sq(a)); }

static inline float vec2_dist_sq(Vec2 a, Vec2 b) { return vec2_mag_sq(vec2_sub(b, a)); }
static inline float vec2_dist(Vec2 a, Vec2 b) { return sqrt(vec2_dist_sq(a, b)); }

static inline Vec2 vec2_norm(Vec2 a) {
    float m = vec2_mag(a);
    if (m < 1e-6f) return VEC2_ZERO;
    return vec2_scale(a, 1.0f / m);
}

static inline Vec2 vec2_lerp(Vec2 a, Vec2 b, float t) {
    return (Vec2){a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t};
}

static inline Vec2 vec2_rotate(Vec2 v, float angle_rad) {
    float c = cos(angle_rad);
    float s = sin(angle_rad);
    return (Vec2){v.x * c - v.y * s, v.x * s + v.y * c};
}

static inline Vec2 vec2_reflect(Vec2 v, Vec2 n) {
    float d = vec2_dot(v, n);
    return vec2_sub(v, vec2_scale(n, 2.0f * d));
}

static inline bool vec2_equals(Vec2 a, Vec2 b, float epsilon) {
    return fabs(a.x - b.x) <= epsilon && fabs(a.y - b.y) <= epsilon;
}

// ============================================================================
// Integer Vector (Vec2i) Operations
// ============================================================================

static inline Vec2i vec2i_add(Vec2i a, Vec2i b) { return (Vec2i){a.x + b.x, a.y + b.y}; }
static inline Vec2i vec2i_sub(Vec2i a, Vec2i b) { return (Vec2i){a.x - b.x, a.y - b.y}; }
static inline Vec2i vec2i_neg(Vec2i a) { return (Vec2i){-a.x, -a.y}; }

static inline float vec2i_mag_sq(Vec2i a) { return (float)(a.x * a.x + a.y * a.y); }
static inline float vec2i_mag(Vec2i a) { return sqrt(vec2i_mag_sq(a)); }

static inline float vec2i_dist_sq(Vec2i a, Vec2i b) { return vec2i_mag_sq(vec2i_sub(b, a)); }
static inline float vec2i_dist(Vec2i a, Vec2i b) { return sqrt(vec2i_dist_sq(a, b)); }

#ifdef __cplusplus
}
#endif

#endif // VEC2_H
