#pragma once
#include <cmath>

#include "raylib.h"

float RandomFloat();
float RandomFloat(const float min, const float max);
Vector3 CreateRandomVector3();

Vector3 operator*(const Vector3 v, const float val);
Vector3 operator+(const Vector3 v1, const Vector3 v2);
Vector3 operator+(const Vector3 v, const float val);
Vector3& operator+=(Vector3& v1, const Vector3 v2);
Vector3& operator+=(Vector3& v, const float val);
Vector3 operator-(const Vector3 v);
Vector3 operator-(const Vector3 v1, const Vector3 v2);
Vector3 operator-(const Vector3 v, const float val);
Vector3& operator-=(Vector3& v1, const Vector3 v2);
Vector3& operator-=(Vector3& v, const float val);
Vector3 operator/(const Vector3 v1, const Vector3 v2);
Vector3& operator/=(Vector3& v1, const Vector3 v2);
Vector3 operator/(const Vector3 v, const float value);
Vector3& operator/=(Vector3& v, const float value);
bool operator==(const Vector3 v1, const Vector3 v2);
bool operator>(const Vector3 v, const int n);
