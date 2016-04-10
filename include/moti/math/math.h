#pragma once

#include "math_types.h"
#include <math.h>

namespace moti {

    //////////////////////////////////////////////////////////////////////////
    // Vec4 

    inline Vec4& operator+=(Vec4& a, const Vec4& b) {
        a.un.fval[0] += b.un.fval[0];
        a.un.fval[1] += b.un.fval[1];
        a.un.fval[2] += b.un.fval[2];
        a.un.fval[3] += b.un.fval[3];
        return a;
    }

    inline Vec4& operator-=(Vec4& a, const Vec4& b) {
        a.un.fval[0] -= b.un.fval[0];
        a.un.fval[1] -= b.un.fval[1];
        a.un.fval[2] -= b.un.fval[2];
        a.un.fval[3] -= b.un.fval[3];
        return a;
    }

    inline Vec4& operator*=(Vec4& a, const Vec4& b) {
        a.un.fval[0] *= b.un.fval[0];
        a.un.fval[1] *= b.un.fval[1];
        a.un.fval[2] *= b.un.fval[2];
        a.un.fval[3] *= b.un.fval[3];
        return a;
    }

    inline Vec4& operator*=(Vec4& a, float s) {
        a.un.fval[0] *= s;
        a.un.fval[1] *= s;
        a.un.fval[2] *= s;
        a.un.fval[3] *= s;
        return a;
    }

    inline Vec4 operator+(Vec4 a, const Vec4& b) {
        return a += b;
    }

    inline Vec4 operator-(Vec4 a, const Vec4& b) {
        return a -= b;
    }

    inline Vec4 operator-(const Vec4& a) {
        return Vec4{
            -a.un.fval[0],
            -a.un.fval[1],
            -a.un.fval[2],
            -a.un.fval[3]
        };
    }

    inline Vec4 operator*(Vec4 a, const Vec4& b) {
        return a *= b;
    }

    inline Vec4 operator*(float s, Vec4 a) {
        return a *= s;
    }

    inline Vec4 operator*(Vec4 a, float s) {
        return a *= s;
    }

    inline float dot(const Vec4& a, const Vec4& b) {
        return a.un.fval[0] * b.un.fval[0]
            + a.un.fval[1] * b.un.fval[1]
            + a.un.fval[2] * b.un.fval[2]
            + a.un.fval[3] * b.un.fval[3];
    }

    inline float lengthSquared(const Vec4& a) {
        return dot(a, a);
    }

    inline float length(const Vec4& a) {
        return sqrtf(lengthSquared(a));
    }

    inline Vec4 normalize(Vec4& a) {
        float len = length(a);
        float inv = 1.f / len;
        a *= inv;
        return a;
    }

    inline float distance(const Vec4& a, const Vec4& b) {
        return length(b - a);
    }

    inline float angle(const Vec4& a, const Vec4& b) {
        return acos(dot(a, b) / (length(a) * length(b)));
    }

    inline float* toPointer(Vec4& a) {
        return &a.un.fval[0];
    }

    inline const float* toPointer(const Vec4& a) {
        return &a.un.fval[0];
    }

    //////////////////////////////////////////////////////////////////////////
    // Mat 4
}

