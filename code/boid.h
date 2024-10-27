#pragma once
#include <vector>
#include <memory>

#include "raylib.h"
#include "raymath.h"

class Boid
{
public:
    Vector3 m_Position;
    Vector3 m_Velocity;

    Vector3 Align(const std::vector<std::unique_ptr<Boid>>& boids) const;
    Vector3 Cohere(const std::vector<std::unique_ptr<Boid>>& boids) const;
    Vector3 Separate(const std::vector<std::unique_ptr<Boid>>& boids) const;
};

void DrawBoids(const std::vector<std::unique_ptr<Boid>>& boids);
void UpdateBoids(std::vector<std::unique_ptr<Boid>>& boids);
