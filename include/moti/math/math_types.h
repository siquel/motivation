#pragma once

#include <stdint.h>
#include <string.h> // memset

namespace moti {

    struct Vec3 {
        float x, y, z;

        void setIdentity() {
            memset(&x, 0, sizeof(float) * 3);
            z = 1.f;
        }

        float& operator[](uint32_t index) {
            return *(&x + index);
        }

        const float& operator[](uint32_t index) const {
            return *(&x + index);
        }
    };

    struct Vec4 {
        float x, y, z, w;

        void setIdentity() {
            memset(&x, 0, 4 * sizeof(float));
            w = 1.f;
        }

        float& operator[](uint32_t index) {
            return *(&x + index);
        }

        const float& operator[](uint32_t index) const {
            return *(&x + index);
        }
    };

    struct Mat4 {
        Vec4 col[4];

        void setIdentity() {
            memset(col, 0, sizeof(col));
            col[0][0] = 1.f;
            col[1][1] = 1.f;
            col[2][2] = 1.f;
            col[3][3] = 1.f;
        }

        Vec4& operator[](uint32_t index) {
            return col[index];
        }

        const Vec4& operator[](uint32_t index) const {
            return col[index];
        }
    };


}