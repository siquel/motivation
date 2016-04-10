#pragma once

#include <stdint.h>
#include <string.h> // memset

namespace moti {

    struct Vec4 {
        union {
            float fval[4];
            int32_t ival[4];
            uint32_t uval[4];
        } un;

        void setIdentity() {
            memset(un.fval, 0, sizeof(un.fval));
            un.fval[3] = 1.f;
        }

        float& operator[](uint32_t index) {
            return un.fval[index];
        }

        const float& operator[](uint32_t index) const {
            return un.fval[index];
        }
    };

    struct Mat4 {
        union {
            float val[16];
            Vec4 col[4];
        } un;

        void setIdentity() {
            memset(un.val, 0, sizeof(un.val));
            un.val[0] = un.val[5] = un.val[10] = un.val[15] = 1.0f;
        }

        Vec4& operator[](uint32_t index) {
            return un.col[index];
        }

        const Vec4& operator[](uint32_t index) const {
            return un.col[index];
        }
    };


}