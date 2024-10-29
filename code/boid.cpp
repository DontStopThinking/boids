#include "boid.h"

#include "rlgl.h"

#include "mathutils.h"

namespace
{
    constexpr float viewRadius = 10.0f;
    constexpr float maxForce = 0.1f;
    constexpr float boundaryThreshold = 5.0f;

    Vector3 TurnBoidIfCloseToBoundary(const Boid& boid, const float worldLimit)
    {
        Vector3 steeringForce = {};

        const float limit = worldLimit - boundaryThreshold;
        const float bx = boid.m_Position.x;
        const float by = boid.m_Position.y;
        const float bz = boid.m_Position.z;

        if (bx > limit)
        {
            steeringForce.x = -maxForce * (1.0f - (worldLimit - bx) / boundaryThreshold);
        }
        else if (bx < -limit)
        {
            steeringForce.x = maxForce * (1.0f - (worldLimit + bx) / boundaryThreshold);
        }

        if (by > limit)
        {
            steeringForce.y = -maxForce * (1.0f - (worldLimit - by) / boundaryThreshold);
        }
        else if (by < -limit)
        {
            steeringForce.y = maxForce * (1.0f - (worldLimit + by) / boundaryThreshold);
        }

        if (bz > limit)
        {
            steeringForce.z = -maxForce * (1.0f - (worldLimit - bz) / boundaryThreshold);
        }
        else if (bz < -limit)
        {
            steeringForce.z = maxForce * (1.0f - (worldLimit + bz) / boundaryThreshold);
        }

        return steeringForce;
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

    void PrintBoid(const Boid* boid)
    {
        std::printf("Position: { .x = %f, .y = %f, .z = %f}, Velocity:  .x = %f, .y = %f, .z = %f}\n",
            boid->m_Position.x, boid->m_Position.y, boid->m_Position.z,
            boid->m_Velocity.x, boid->m_Velocity.y, boid->m_Velocity.z);
    }
}

Vector3 Boid::Align(const GameState* gameState) const
{
    const Boid* boids = gameState->m_Boids;
    const int numBoids = gameState->m_NumBoids;

    Vector3 steeringForce = {};

    int numNearbyBoids = 0;

    for (int i = 0; i < numBoids; i++)
    {
        const Boid boid = boids[i];

        const float distance = Vector3Distance(m_Position, boid.m_Position);
        const bool isOtherPosSameAsMe = (m_Position == boid.m_Position);

        if (!isOtherPosSameAsMe && distance < viewRadius)
        {
            steeringForce += boid.m_Velocity; // Sum up velocities of all nearby boids
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

Vector3 Boid::Cohere(const GameState* gameState) const
{
    const Boid* boids = gameState->m_Boids;
    const int numBoids = gameState->m_NumBoids;

    Vector3 steeringForce = {};

    int numNearbyBoids = 0;

    for (int i = 0; i < numBoids; i++)
    {
        const Boid boid = boids[i];

        const float distance = Vector3Distance(m_Position, boid.m_Position);
        const bool isOtherPosSameAsMe = (m_Position == boid.m_Position);

        if (!isOtherPosSameAsMe && distance < viewRadius)
        {
            steeringForce += boid.m_Position; // Sum up the position of all nearby boids
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

Vector3 Boid::Separate(const GameState* gameState) const
{
    const Boid* boids = gameState->m_Boids;
    const int numBoids = gameState->m_NumBoids;

    constexpr float separationDistance = 20.0f;

    Vector3 steeringForce = {};
    int numNearbyBoids = 0;

    for (int i = 0; i < numBoids; i++)
    {
        const Boid boid = boids[i];

        const float distance = Vector3Distance(m_Position, boid.m_Position);
        const bool isOtherPosSameAsMe = (m_Position == boid.m_Position);

        if (!isOtherPosSameAsMe && distance < separationDistance)
        {
            numNearbyBoids++;
            // Divide by distance so closer boids have a higher impact on separation force
            Vector3 diff = (m_Position - boid.m_Position) / distance;
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

void DrawBoids(const GameState* gameState)
{
    const Boid* boids = gameState->m_Boids;
    const int numBoids = gameState->m_NumBoids;

    for (int i = 0; i < numBoids; i++)
    {
        const Boid boid = boids[i];

        DrawBoid(boid, DARKBLUE, 3);
    }
}

void UpdateBoids(GameState* gameState)
{
    constexpr float maxSpeed = 3.0f;

    Boid* boids = gameState->m_Boids;
    const int numBoids = gameState->m_NumBoids;
    const float worldSize = gameState->m_WorldSize;

    for (int i = 0; i < numBoids; i++)
    {
        Boid& boid = boids[i];

        // Apply alignment, cohesion and separation.
        Vector3 acceleration = boid.Align(gameState) + boid.Cohere(gameState) + boid.Separate(gameState);

        acceleration += TurnBoidIfCloseToBoundary(boid, worldSize);

        // Update position
        boid.m_Velocity += acceleration;
        boid.m_Velocity = Vector3ClampValue(boid.m_Velocity, 0, maxSpeed);
        boid.m_Position += boid.m_Velocity;
    }
}
