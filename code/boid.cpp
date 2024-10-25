#include "boid.h"

#include "mathutils.h"

namespace
{
    void ReverseBoidIfGreaterThan(Boid* boid, const int limit)
    {
        if (boid->m_Position > limit)
        {
            boid->m_Velocity = -boid->m_Velocity;
        }
    }
}

Vector3 Boid::Align(const std::vector<std::unique_ptr<Boid>>& boids) const
{
    const float perception = 0.8f;
    int total = 0;

    Vector3 steeringForce = {};
    for (const auto& boid : boids)
    {
        const float distance = Vector3Distance(m_Position, boid->m_Position);

        const bool isOtherPosSameAsMe = (m_Position == boid->m_Position);

        if (distance < perception && !isOtherPosSameAsMe)
        {
            steeringForce += boid->m_Velocity;
            total++;
        }
    }

    const float size = (float)boids.size();

    Vector3 boidsSizeVec =
    {
        .x = size,
        .y = size,
        .z = size
    };

    if (total > 0)
    {
        steeringForce /= boidsSizeVec;
        steeringForce -= m_Velocity;
    }

    return steeringForce;
}

void DrawBoids(const std::vector<std::unique_ptr<Boid>>& boids)
{
    constexpr float radius = 0.2f;

    for (const auto& boid : boids)
    {
        DrawSphere(boid->m_Position, radius, BLUE);
    }
}

void UpdateBoids(std::vector<std::unique_ptr<Boid>>& boids)
{
    for (const auto& boid : boids)
    {
        // Update alignment
        // Set acceleration to the alignment force. If our boids had any mass then we'd be multiplying the mass
        // here.
        boid->m_Acceleration = boid->Align(boids);

        // Update position
        boid->m_Position += boid->m_Velocity;
        boid->m_Velocity += boid->m_Acceleration;

        ReverseBoidIfGreaterThan(boid.get(), 20);
    }
}
