#include "boid.h"

#include "rlgl.h"

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

    void DrawBoid(const Boid& boid, const Color color, const float scale)
    {
        constexpr int numIndices = 18;
        constexpr int indices[numIndices] = {
            2,1,0,
            1,3,0,
            4,3,1,
            2,4,1,
            0,4,2,
            0,3,4,
        };

        constexpr int numVerts = 5;
        constexpr float verts[numVerts * 3] = {
            0.0f,  0.0f,  0.0f, // 0
            0.0f,  0.0f, -1.0f, // 1
            0.8f,  0.3f,  0.4f, // 2
            -0.8f,  0.3f,  0.4f, // 3
            0.0f, -0.1f,  0.0f, // 4
        };

        rlPushMatrix();
        rlTranslatef(boid.m_Position.x, boid.m_Position.y, boid.m_Position.z);

        const float rotateAngle = (-RAD2DEG) * std::atan2(boid.m_Velocity.x, -boid.m_Velocity.z);
        rlRotatef(rotateAngle, 0.0f, 1.0f, 0.0f);

        rlScalef(scale, scale, scale);

        rlColor4ub(color.r, color.g, color.b, color.a);

        rlBegin(RL_TRIANGLES);
        for (int i = 0; i < numIndices; i++)
        {
            rlVertex3f(
                verts[indices[i] * 3],
                verts[indices[i] * 3 + 1],
                verts[indices[i] * 3 + 2]);
        }
        rlEnd();
        rlPopMatrix();
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
            steeringForce += boid->m_Velocity; // Sum up velocities of all nearby boids
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
        const float distance = Vector3Distance(m_Position, boid->m_Position);
        const bool isOtherPosSameAsMe = (m_Position == boid->m_Position);

        if (!isOtherPosSameAsMe && distance < viewRadius)
        {
            steeringForce += boid->m_Position; // Sum up the position of all nearby boids
            numNearbyBoids++;
        }
    }

    if (numNearbyBoids > 0)
    {
        steeringForce /= (float)numNearbyBoids; // The average position
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
        const float distance = Vector3Distance(m_Position, boid->m_Position);
        const bool isOtherPosSameAsMe = (m_Position == boid->m_Position);

        if (!isOtherPosSameAsMe && distance < separationDistance)
        {
            numNearbyBoids++;
            // Divide by distance so closer boids have a higher impact on separation force
            Vector3 diff = (m_Position - boid->m_Position) / distance;
            steeringForce += diff;
        }
    }

    if (numNearbyBoids > 0)
    {
        steeringForce /= (float)numNearbyBoids; // Get average separation force
        steeringForce = Vector3ClampValue(steeringForce, 0, maxForce);
    }

    return steeringForce;
}

void DrawBoids(const std::vector<std::unique_ptr<Boid>>& boids)
{
    constexpr float radius = 1.5f;

    for (const auto& boid : boids)
    {
        DrawBoid(*boid, DARKBLUE, 3);
    }
}

void UpdateBoids(std::vector<std::unique_ptr<Boid>>& boids, const float worldSize)
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

        WrapBoidIfGreaterThan(*boid, worldSize);
    }
}
