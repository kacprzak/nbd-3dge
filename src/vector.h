#ifndef VECTOR_H
#define VECTOR_H

#include <cassert>
#include <cmath>

template <typename T>
struct Vector {
    T x, y, z;

    Vector() : x(0), y(0), z(0) {}

    Vector(T ax, T ay = 0, T az = 0) : x(ax), y(ay), z(az) {}

    float length() const
    {
        return std::sqrt(x*x + y*y + z*z);
    }

    const T& operator[](unsigned int i) const
    {
        assert(i < 3);
        switch (i) {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        default:
            assert(0);
        }
    }

    T& operator[](unsigned int i)
    {
        assert(i < 3);
        switch (i) {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        default:
            assert(0);
        }
    }

    Vector& operator=(const Vector& rhs)
    {
        if (this == &rhs)
            return *this;

        this->x = rhs.x;
        this->y = rhs.y;
        this->z = rhs.z;
        return *this;
    }

    const Vector operator*(T rhs) const
    {
        Vector v;
        v.x = this->x * rhs;
        v.y = this->y * rhs;
        v.z = this->z * rhs;
        return v;
    }

    Vector& operator*=(T rhs)
    {
        this->x *= rhs;
        this->y *= rhs;
        this->z *= rhs;
        return *this;
    }

    const Vector operator+(const Vector& rhs) const
    {
        Vector v;
        v.x = this->x + rhs.x;
        v.y = this->y + rhs.y;
        v.z = this->z + rhs.z;
        return v;
    }

    Vector& operator+=(const Vector& rhs)
    {
        this->x += rhs.x;
        this->y += rhs.y;
        this->z += rhs.z;
        return *this;
    }

    const Vector operator-(const Vector& rhs) const
    {
        Vector v;
        v.x = this->x - rhs.x;
        v.y = this->y - rhs.y;
        v.z = this->z - rhs.z;
        return v;
    }

    Vector& operator-=(const Vector& rhs)
    {
        this->x -= rhs.x;
        this->y -= rhs.y;
        this->z -= rhs.z;
        return *this;
    }

    const T dot(const Vector& rhs) const
    {
        return x*rhs.x + y*rhs.y + z*rhs.z;
    }

    const Vector cross(const Vector& rhs) const
    {
        Vector v;
        v.x = this->y*rhs.z - this->z*rhs.y;
        v.y = this->z*rhs.x - this->x*rhs.z;
        v.z = this->x*rhs.y - this->y*rhs.x;
        return v;
    }

    const Vector normal() const
    {
        Vector n;
        float len = length();
        if (len == 0.0f)
            return *this;
        n.x = this->x/len;
        n.y = this->y/len;
        n.z = this->z/len;
        return n;
    }

    void normalize()
    {
        Vector n = normal();
        this->x = n.x;
        this->y = n.y;
        this->z = n.z;
    }
};

#endif // VECTOR_H
