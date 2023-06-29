#pragma once
#include "runtime/core/math/math.h"
#include "runtime/core/math/vector/vector3.h"
#include "runtime/core/meta/reflection/reflection.h"

#include <cassert>

namespace ArchViz
{
    REFLECTION_TYPE(Vector4)
    CLASS(Vector4, Fields)
    {
        REFLECTION_BODY(Vector4);

    public:
        float x {0.f}, y {0.f}, z {0.f}, w {0.f};

    public:
        Vector4() = default;
        Vector4(float x_, float y_, float z_, float w_) : x {x_}, y {y_}, z {z_}, w {w_} {}
        Vector4(const Vector3& v3, float w_) : x {v3.x}, y {v3.y}, z {v3.z}, w {w_} {}

        explicit Vector4(float coords[4]) : x {coords[0]}, y {coords[1]}, z {coords[2]}, w {coords[3]} {}

        float operator[](size_t i) const
        {
            assert(i < 4);
            return *(&x + i);
        }

        float& operator[](size_t i)
        {
            assert(i < 4);
            return *(&x + i);
        }

        /// Pointer accessor for direct copying
        float* ptr() { return &x; }
        /// Pointer accessor for direct copying
        const float* ptr() const { return &x; }

        void zero()
        {
            x = 0;
            y = 0;
            z = 0;
            w = 0;
        }

        Vector4& operator=(float scalar)
        {
            x = scalar;
            y = scalar;
            z = scalar;
            w = scalar;
            return *this;
        }

        bool operator==(const Vector4& rhs) const { return (x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w); }

        bool operator!=(const Vector4& rhs) const { return !(rhs == *this); }

        Vector4 operator+(const Vector4& rhs) const { return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w); }
        Vector4 operator-(const Vector4& rhs) const { return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w); }
        Vector4 operator*(float scalar) const { return Vector4(x * scalar, y * scalar, z * scalar, w * scalar); }
        Vector4 operator*(const Vector4& rhs) const { return Vector4(rhs.x * x, rhs.y * y, rhs.z * z, rhs.w * w); }
        Vector4 operator/(float scalar) const
        {
            assert(scalar != 0.0);
            return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
        }
        Vector4 operator/(const Vector4& rhs) const
        {
            assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0 && rhs.w != 0);
            return Vector4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w);
        }

        const Vector4& operator+() const { return *this; }

        Vector4 operator-() const { return Vector4(-x, -y, -z, -w); }

        friend Vector4 operator*(float scalar, const Vector4& rhs) { return Vector4(scalar * rhs.x, scalar * rhs.y, scalar * rhs.z, scalar * rhs.w); }

        friend Vector4 operator/(float scalar, const Vector4& rhs)
        {
            assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0 && rhs.w != 0);
            return Vector4(scalar / rhs.x, scalar / rhs.y, scalar / rhs.z, scalar / rhs.w);
        }

        friend Vector4 operator+(const Vector4& lhs, float rhs) { return Vector4(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs, lhs.w + rhs); }

        friend Vector4 operator+(float lhs, const Vector4& rhs) { return Vector4(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z, lhs + rhs.w); }

        friend Vector4 operator-(const Vector4& lhs, float rhs) { return Vector4(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs, lhs.w - rhs); }

        friend Vector4 operator-(float lhs, const Vector4& rhs) { return Vector4(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z, lhs - rhs.w); }

        // arithmetic updates
        Vector4& operator+=(const Vector4& rhs)
        {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            w += rhs.w;
            return *this;
        }

        Vector4& operator-=(const Vector4& rhs)
        {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;
            w -= rhs.w;
            return *this;
        }

        Vector4& operator*=(float scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            w *= scalar;
            return *this;
        }

        Vector4& operator+=(float scalar)
        {
            x += scalar;
            y += scalar;
            z += scalar;
            w += scalar;
            return *this;
        }

        Vector4& operator-=(float scalar)
        {
            x -= scalar;
            y -= scalar;
            z -= scalar;
            w -= scalar;
            return *this;
        }

        Vector4& operator*=(const Vector4& rhs)
        {
            x *= rhs.x;
            y *= rhs.y;
            z *= rhs.z;
            w *= rhs.w;
            return *this;
        }

        Vector4& operator/=(float scalar)
        {
            assert(scalar != 0.0);

            x /= scalar;
            y /= scalar;
            z /= scalar;
            w /= scalar;
            return *this;
        }

        Vector4& operator/=(const Vector4& rhs)
        {
            assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0);
            x /= rhs.x;
            y /= rhs.y;
            z /= rhs.z;
            w /= rhs.w;
            return *this;
        }

        float length() const { return std::sqrtf(x * x + y * y + z * z + w * w); }

        float squaredLength() const { return x * x + y * y + z * z + w * w; }

        float distance(const Vector4& rhs) const { return (*this - rhs).length(); }

        float squaredDistance(const Vector4& rhs) const { return (*this - rhs).squaredLength(); }

        void normalise()
        {
            float length = this->length();
            if (length == 0.f)
                return;

            float inv_lengh = 1.0f / length;
            x *= inv_lengh;
            y *= inv_lengh;
            z *= inv_lengh;
            w *= inv_lengh;
        }

        Vector4 normalisedCopy(void) const
        {
            Vector4 ret = *this;
            ret.normalise();
            return ret;
        }

        /** Calculates the dot (scalar) product of this vector with another.
        @param
        vec Vector with which to calculate the dot product (together
        with this one).
        @returns
        A float representing the dot product value.
        */
        float dotProduct(const Vector4& vec) const { return x * vec.x + y * vec.y + z * vec.z + w * vec.w; }

        /// Check whether this vector contains valid values
        bool isNaN() const { return Math::isNan(x) || Math::isNan(y) || Math::isNan(z) || Math::isNan(w); }

        bool isValid() const
        {
            if (x * 0.0f != x * 0.0f)
                return false;

            if (y * 0.0f != y * 0.0f)
                return false;

            if (z * 0.0f != z * 0.0f)
                return false;

            if (w * 0.0f != w * 0.0f)
                return false;

            return true;
        }

        // special
        static const Vector4 ZERO;
        static const Vector4 UNIT_SCALE;
    };

} // namespace ArchViz
