#include "boid.h"

#include "mathutils.h"

namespace
{
    constexpr float viewRadius = 10.0f;
    constexpr float maxForce = 0.1f;

    void WrapBoidIfGreaterThan(Boid& boid, const float limit)
    {
        if (boid.m_Position.x > limit) boid.m_Position.x = -limit;
        if (boid.m_Position.x < -limit) boid.m_Position.x = limit;
        if (boid.m_Position.y > limit) boid.m_Position.y = -limit;
        if (boid.m_Position.y < -limit) boid.m_Position.y = limit;
        if (boid.m_Position.z > limit) boid.m_Position.z = -limit;
        if (boid.m_Position.z < -limit) boid.m_Position.z = limit;
    }

    void ReverseBoidIfGreaterThan(Boid& boid, const int limit)
    {
        if (boid.m_Position > limit)
        {
            boid.m_Velocity = -boid.m_Velocity;
        }
    }
}

Vector3 Boid::Align(const std::vector<std::unique_ptr<Boid>>& boids) const
{
    Vector3 steeringForce = {};

    int numNearbyBoids = 0;

    for (const auto& boid : boids)
    {
        const float distance = Vector3Distance(m_Position, boid->m_Position);

        const bool isOtherPosSameAsMe = (m_Position == boid->m_Position);

        if (!isOtherPosSameAsMe && distance < viewRadius)
        {
            steeringForce += boid->m_Velocity;
            numNearbyBoids++;
        }
    }

    if (numNearbyBoids > 0)
    {
        steeringForce /= (float)numNearbyBoids; // Average the force
        steeringForce -= m_Velocity;
        steeringForce = Vector3ClampValue(steeringForce, 0.0f, maxForce);
    }

    return steeringForce;
}

Vector3 Boid::Cohere(const std::vector<std::unique_ptr<Boid>>& boids) const
{
    Vector3 steeringForce = {};

    int numNearbyBoids = 0;

    for (const auto& boid : boids)
    {
        float distance = Vector3Distance(m_Position, boid->m_Position);
        const bool isOtherPosSameAsMe = (m_Position == boid->m_Position);

        if (!isOtherPosSameAsMe && distance < viewRadius)
        {
            steeringForce += boid->m_Position;
            numNearbyBoids++;
        }
    }

    if (numNearbyBoids > 0)
    {
        steeringForce /= (float)numNearbyBoids;
        steeringForce -= m_Position;
        steeringForce = Vector3ClampValue(steeringForce, 0, maxForce);
    }

    return steeringForce;
}

Vector3 Boid::Separate(const std::vector<std::unique_ptr<Boid>>& boids) const
{
    constexpr float separationDistance = 15.0f;

    Vector3 steeringForce = {};
    int numNearbyBoids = 0;

    for (const auto& boid : boids)
    {
        float distance = Vector3Distance(m_Position, boid->m_Position);
        const bool isOtherPosSameAsMe = (m_Position == boid->m_Position);

        if (!isOtherPosSameAsMe && distance < separationDistance)
        {
            numNearbyBoids++;
            Vector3 diff = m_Position - boid->m_Position;
            diff /= distance;
            steeringForce += diff;
        }
    }

    if (numNearbyBoids > 0)
    {
        steeringForce /= (float)numNearbyBoids;
        steeringForce = Vector3ClampValue(steeringForce, 0, maxForce);
    }

    return steeringForce;
}

void DrawBoids(const std::vector<std::unique_ptr<Boid>>& boids)
{
    constexpr float radius = 1.5f;

    for (const auto& boid : boids)
    {
         DrawSphere(boid->m_Position, radius, BLUE);
    }
}

void UpdateBoids(std::vector<std::unique_ptr<Boid>>& boids)
{
    constexpr float maxSpeed = 3.0f;

    for (std::unique_ptr<Boid>& boid : boids)
    {
        // Apply alignment, cohesion and separation.
        Vector3 acceleration = boid->Align(boids) + boid->Cohere(boids) + boid->Separate(boids);

        // Update position
        boid->m_Velocity += acceleration;
        boid->m_Velocity = Vector3ClampValue(boid->m_Velocity, 0, maxSpeed);
        boid->m_Position += boid->m_Velocity;

        WrapBoidIfGreaterThan(*boid, 100.0f);
    }
}
