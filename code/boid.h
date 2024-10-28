#pragma once
#include <vector>

#include "raylib.h"
#include "raymath.h"

class Boid
{
public:
    Vector3 m_Position;
    Vector3 m_Velocity;

    Vector3 Align(const std::vector<Boid>& boids) const;
    Vector3 Cohere(const std::vector<Boid>& boids) const;
    Vector3 Separate(const std::vector<Boid>& boids) const;
};

void DrawBoids(const std::vector<Boid>& boids);
void UpdateBoids(std::vector<Boid>& boids, const float worldSize);
