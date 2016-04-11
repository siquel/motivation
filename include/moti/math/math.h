#pragma once

#include "math_types.h"
#include <math.h>

namespace moti {

    inline float radians(float degrees) {
        return degrees * static_cast<float>(0.01745329251994329576923690768489);
    }

    //////////////////////////////////////////////////////////////////////////
    // Vec3

    inline Vec3& operator+=(Vec3& a, const Vec3& b) {
        a[0] += b[0];
        a[1] += b[1];
        a[2] += b[2];
        return a;
    }

    inline Vec3& operator-=(Vec3& a, const Vec3& b) {
        a[0] -= b[0];
        a[1] -= b[1];
        a[2] -= b[2];
        return a;
    }

    inline Vec3& operator*=(Vec3& a, float k) {
        a[0] *= k;
        a[1] *= k;
        a[2] *= k;
        return a;
    }

    inline Vec3 operator-(const Vec3& a) {
        Vec3 v;
        v[0] = -a[0];
        v[1] = -a[1];
        v[2] = -a[2];
        return v;
    }

    inline Vec3 operator+(Vec3 a, const Vec3& b) {
        a += b;
        return a;
    }

    inline Vec3 operator-(Vec3 a, const Vec3& b) {
        a -= b;
        return a;
    }

    inline Vec3 operator*(Vec3 a, float k) {
        a *= k;
        return a;
    }

    inline Vec3 operator*(float k, Vec3 a) {
        a *= k;
        return a;
    }

    inline float dot(const Vec3& a, const Vec3& b) {
        return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
    }

    inline Vec3 cross(const Vec3& x, const Vec3& y) {
        return Vec3{
            x.y * y.z - y.y * x.z,
            x.z * y.x - y.z * x.x,
            x.x * y.y - y.x * x.y
        };
    }

    inline float lengthSquared(const Vec3& a) {
        return dot(a, a);
    }

    inline float length(const Vec3& a) {
        return sqrtf(lengthSquared(a));
    }

    inline Vec3 normalize(const Vec3& a) {
        const float len = length(a);
        const float inv_len = 1.0f / len;
        
        return a * inv_len;
    }

    //////////////////////////////////////////////////////////////////////////
    // Vec4 

    inline Vec4& operator+=(Vec4& a, const Vec4& b) {
        a[0] += b[0];
        a[1] += b[1];
        a[2] += b[2];
        a[3] += b[3];
        return a;
    }

    inline Vec4& operator-=(Vec4& a, const Vec4& b) {
        a[0] -= b[0];
        a[1] -= b[1];
        a[2] -= b[2];
        a[3] -= b[3];
        return a;
    }

    inline Vec4& operator*=(Vec4& a, const Vec4& b) {
        a[0] *= b[0];
        a[1] *= b[1];
        a[2] *= b[2];
        a[3] *= b[3];
        return a;
    }

    inline Vec4& operator*=(Vec4& a, float s) {
        a[0] *= s;
        a[1] *= s;
        a[2] *= s;
        a[3] *= s;
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
            -a[0],
            -a[1],
            -a[2],
            -a[3]
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
        return a[0] * b[0]
            + a[1] * b[1]
            + a[2] * b[2]
            + a[3] * b[3];
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
        return &a[0];
    }

    inline const float* toPointer(const Vec4& a) {
        return &a[0];
    }

    //////////////////////////////////////////////////////////////////////////
    // Mat 4

    inline Mat4& operator+=(Mat4& a, const Mat4& b) {
        a.col[0] += b.col[0];
        a.col[1] += b.col[1];
        a.col[2] += b.col[2];
        a.col[3] += b.col[3];
        return a;
    }

    inline Mat4& operator-=(Mat4& a, const Mat4& b) {
        a.col[0] -= b.col[0];
        a.col[1] -= b.col[1];
        a.col[2] -= b.col[2];
        a.col[3] -= b.col[3];
        return a;
    }

    inline Mat4& operator*=(Mat4& a, float k) {
        a.col[0] *= k;
        a.col[1] *= k;
        a.col[2] *= k;
        a.col[3] *= k;
        return a;
    }

    inline Mat4& operator*=(Mat4& m, const Mat4& b) {
        const Vec4& srcA0 = m[0];
        const Vec4& srcA1 = m[1];
        const Vec4& srcA2 = m[2];
        const Vec4& srcA3 = m[3];

        const Vec4& srcB0 = b[0];
        const Vec4& srcB1 = b[1];
        const Vec4& srcB2 = b[2];
        const Vec4& srcB3 = b[3];

        Mat4 tmp;
        
        tmp[0] = srcA0 * srcB0[0] + srcA1 * srcB0[1] + srcA2 * srcB0[2] + srcA3 * srcB0[3];
        tmp[1] = srcA0 * srcB1[0] + srcA1 * srcB1[1] + srcA2 * srcB1[2] + srcA3 * srcB1[3];
        tmp[2] = srcA0 * srcB2[0] + srcA1 * srcB2[1] + srcA2 * srcB2[2] + srcA3 * srcB2[3];
        tmp[3] = srcA0 * srcB3[0] + srcA1 * srcB3[1] + srcA2 * srcB3[2] + srcA3 * srcB3[3];
        
        m = tmp;
        return m;
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

    inline void translate(Mat4& m, const Vec3& v) {
        m[3][0] = v[0];
        m[3][1] = v[1];
        m[3][2] = v[2];
    }

    inline Mat4 scale(const Mat4& m, const Vec3& v) {
        Mat4 r;
        r[0] = m[0] * v[0];
        r[1] = m[1] * v[1];
        r[2] = m[2] * v[2];
        r[3] = m[3];
        return r;
    }
    // From GLM
    inline void look(Mat4& m, const Vec3& pos, const Vec3& target, const Vec3& up) {
        m.setIdentity();
        Vec3 const f(normalize(target - pos));
        Vec3 const s(normalize(cross(f, up)));
        Vec3 const u(cross(s, f));

        m[0][0] = s[0];
        m[1][0] = s[1];
        m[2][0] = s[2];
        m[0][1] = u[0];
        m[1][1] = u[1];
        m[2][1] = u[2];
        m[0][2] = -f[0];
        m[1][2] = -f[1];
        m[2][2] = -f[2];
        m[3][0] = -dot(s, pos);
        m[3][1] = -dot(u, pos);
        m[3][2] = dot(f, pos);

    }

    // From GLM
    inline void rotate(Mat4& m, float radians, const Vec3& v) {
        float const a = radians;
        float const c = cos(a);
        float const s = sin(a);

        Vec3 axis(normalize(v));
        Vec3 temp((1.f - c) * axis);

        Mat4 Rotate;
        Rotate[0][0] = c + temp[0] * axis[0];
        Rotate[0][1] = 0 + temp[0] * axis[1] + s * axis[2];
        Rotate[0][2] = 0 + temp[0] * axis[2] - s * axis[1];

        Rotate[1][0] = 0 + temp[1] * axis[0] - s * axis[2];
        Rotate[1][1] = c + temp[1] * axis[1];
        Rotate[1][2] = 0 + temp[1] * axis[2] + s * axis[0];

        Rotate[2][0] = 0 + temp[2] * axis[0] + s * axis[1];
        Rotate[2][1] = 0 + temp[2] * axis[1] - s * axis[0];
        Rotate[2][2] = c + temp[2] * axis[2];

        Mat4 Result;
        Result[0] = m[0] * Rotate[0][0] + m[1] * Rotate[0][1] + m[2] * Rotate[0][2];
        Result[1] = m[0] * Rotate[1][0] + m[1] * Rotate[1][1] + m[2] * Rotate[1][2];
        Result[2] = m[0] * Rotate[2][0] + m[1] * Rotate[2][1] + m[2] * Rotate[2][2];
        Result[3] = m[3];
        m = Result;
    }

    // From GLM
    inline void perspective(Mat4& m, float fovy, float aspect, float nearz, float farz) {
        memset(&m, 0, sizeof(Mat4));
        float const tanHalfFovy = tan(fovy / 2.f);

        m[0][0] = 1.f / (aspect * tanHalfFovy);
        m[1][1] = 1.f / (tanHalfFovy);
        m[2][2] = -(farz + nearz) / (farz - nearz);
        m[2][3] = -1.f;
        m[3][2] = -(2 * farz * nearz) / (farz - nearz);
    }

    inline float* toPointer(Mat4& m) {
        return &m[0][0];
    }

    inline const float* toPointer(const Mat4& m){
        return &m[0][0];
    }
}

