#include "mathutils.h"

float RandomFloat()
{
    return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
}

float RandomFloat(const float min, const float max)
{
    const float random = RandomFloat();
    const float range = max - min;
    return (random * range) + min;
}

Vector3 CreateRandomVector3()
{
    Vector3 result = {};

    const float angle = RandomFloat() * 2.0f * PI;
    const float vz = RandomFloat() * 2.0f - 1.0f;
    const float vzBase = std::sqrtf(1 - vz * vz);
    const float vx = vzBase * std::cosf(angle);
    const float vy = vzBase * std::sinf(angle);

    result.x = vx;
    result.y = vy;
    result.z = vz;

    return result;
}

Vector3 operator*(const Vector3 v, const float val)
{
    return Vector3Scale(v, val);
}

Vector3 operator+(const Vector3 v1, const Vector3 v2)
{
    return Vector3Add(v1, v2);
}

Vector3& operator+=(Vector3& v1, const Vector3 v2)
{
    v1 = v1 + v2;
    return v1;
}

Vector3 operator-(const Vector3 v)
{
    return Vector3Negate(v);
}

Vector3 operator-(const Vector3 v1, const Vector3 v2)
{
    return Vector3Subtract(v1, v2);
}

Vector3& operator-=(Vector3& v1, const Vector3 v2)
{
    v1 = v1 - v2;
    return v1;
}

Vector3 operator/(const Vector3 v1, const Vector3 v2)
{
    return Vector3Divide(v1, v2);
}

Vector3& operator/=(Vector3& v1, const Vector3 v2)
{
    v1 = v1 / v2;
    return v1;
}

bool operator==(const Vector3 v1, const Vector3 v2)
{
    return Vector3Equals(v1, v2);
}

bool operator>(const Vector3 v, const int n)
{
    return Vector3Length(v) > n;
}
