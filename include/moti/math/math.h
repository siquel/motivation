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

    inline Mat4& operator+=(Mat4& a, const Mat4& b) {
        a.un.col[0] += b.un.col[0];
        a.un.col[1] += b.un.col[1];
        a.un.col[2] += b.un.col[2];
        a.un.col[3] += b.un.col[3];
        return a;
    }

    inline Mat4& operator-=(Mat4& a, const Mat4& b) {
        a.un.col[0] -= b.un.col[0];
        a.un.col[1] -= b.un.col[1];
        a.un.col[2] -= b.un.col[2];
        a.un.col[3] -= b.un.col[3];
        return a;
    }

    inline Mat4& operator*=(Mat4& a, float k) {
        a.un.col[0] *= k;
        a.un.col[1] *= k;
        a.un.col[2] *= k;
        a.un.col[3] *= k;
        return a;
    }

    inline Mat4& operator*=(Mat4& a, const Mat4& b) {
        Mat4 tmp;
        
        tmp[0][0] = a[0][0] * b[0][0] + a[0][1] * b[1][0] + a[0][2] * b[2][0] + a[0][3] * b[3][0];
        tmp[0][1] = a[0][0] * b[0][1] + a[0][1] * b[1][1] + a[0][2] * b[2][1] + a[0][3] * b[3][1];
        tmp[0][2] = a[0][0] * b[0][2] + a[0][1] * b[1][2] + a[0][2] * b[2][2] + a[0][3] * b[3][2];
        tmp[0][3] = a[0][0] * b[0][3] + a[0][1] * b[1][3] + a[0][2] * b[2][3] + a[0][3] * b[3][3];

        tmp[1][0] = a[1][0] * b[0][0] + a[1][1] * b[1][0] + a[1][2] * b[2][0] + a[1][3] * b[3][0];
        tmp[1][1] = a[1][0] * b[0][1] + a[1][1] * b[1][1] + a[1][2] * b[2][1] + a[1][3] * b[3][1];
        tmp[1][2] = a[1][0] * b[0][2] + a[1][1] * b[1][2] + a[1][2] * b[2][2] + a[1][3] * b[3][2];
        tmp[1][3] = a[1][0] * b[0][3] + a[1][1] * b[1][3] + a[1][2] * b[2][3] + a[1][3] * b[3][3];

        tmp[2][0] = a[2][0] * b[0][0] + a[2][1] * b[1][0] + a[2][2] * b[2][0] + a[2][3] * b[3][0];
        tmp[2][1] = a[2][0] * b[0][1] + a[2][1] * b[1][1] + a[2][2] * b[2][1] + a[2][3] * b[3][1];
        tmp[2][2] = a[2][0] * b[0][2] + a[2][1] * b[1][2] + a[2][2] * b[2][2] + a[2][3] * b[3][2];
        tmp[2][3] = a[2][0] * b[0][3] + a[2][1] * b[1][3] + a[2][2] * b[2][3] + a[2][3] * b[3][3];

        tmp[3][0] = a[3][0] * b[0][0] + a[3][1] * b[1][0] + a[3][2] * b[2][0] + a[3][3] * b[3][0];
        tmp[3][1] = a[3][0] * b[0][1] + a[3][1] * b[1][1] + a[3][2] * b[2][1] + a[3][3] * b[3][1];
        tmp[3][2] = a[3][0] * b[0][2] + a[3][1] * b[1][2] + a[3][2] * b[2][2] + a[3][3] * b[3][2];
        tmp[3][3] = a[3][0] * b[0][3] + a[3][1] * b[1][3] + a[3][2] * b[2][3] + a[3][3] * b[3][3];

        a = tmp;
        return a;
    }

    
    inline Mat4 operator+(Mat4 a, const Mat4& b) {
        a += b;
        return a;
    }

    inline Mat4 operator-(Mat4 a, const Mat4& b) {
        a -= b;
        return a;
    }

    inline Mat4 operator*(Mat4 a, float k) {
        a *= k;
        return a;
    }

    inline Mat4 operator*(float k, Mat4 a) {
        a *= k;
        return a;
    }

    inline Vec4 operator*(const Vec4& v, const Mat4& a) {
        Vec4 r;
        r[0] = v[0] * a[0][0] + v[1] * a[1][0] + v[2] * a[2][0] + v[3] * a[3][0];
        r[1] = v[0] * a[0][1] + v[1] * a[1][1] + v[2] * a[2][1] + v[3] * a[3][1];
        r[2] = v[0] * a[0][2] + v[1] * a[1][2] + v[2] * a[2][2] + v[3] * a[3][2];
        r[3] = v[0] * a[0][3] + v[1] * a[1][3] + v[2] * a[2][3] + v[3] * a[3][3];
        return r;
    }

    inline Mat4 operator*(Mat4 a, const Mat4& b) {
        a *= b;
        return a;
    }
}

