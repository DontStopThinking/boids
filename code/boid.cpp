#include "boid.h"

#include <cstdio>

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
        const float bx = boid.position.x;
        const float by = boid.position.y;
        const float bz = boid.position.z;

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
        rlTranslatef(boid.position.x, boid.position.y, boid.position.z);

        const float rotateAngle = (-RAD2DEG) * std::atan2(boid.velocity.x, -boid.velocity.z);
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
            boid->position.x, boid->position.y, boid->position.z,
            boid->velocity.x, boid->velocity.y, boid->velocity.z);
    }
}

Vector3 Boid::Align(const GameState* gameState) const
{
    const Boid* boids = gameState->boids;
    const int numBoids = gameState->numBoids;

    Vector3 steeringForce = {};

    int numNearbyBoids = 0;

    for (int i = 0; i < numBoids; i++)
    {
        const Boid boid = boids[i];

        const float distance = Vector3Distance(position, boid.position);
        const bool isOtherPosSameAsMe = (position == boid.position);

        if (!isOtherPosSameAsMe && distance < viewRadius)
        {
            steeringForce += boid.velocity; // Sum up velocities of all nearby boids
            numNearbyBoids++;
        }
    }

    if (numNearbyBoids > 0)
    {
        steeringForce /= (float)numNearbyBoids; // Average the force
        steeringForce -= velocity;
        steeringForce = Vector3ClampValue(steeringForce, 0.0f, maxForce);
    }

    return steeringForce;
}

Vector3 Boid::Cohere(const GameState* gameState) const
{
    const Boid* boids = gameState->boids;
    const int numBoids = gameState->numBoids;

    Vector3 steeringForce = {};

    int numNearbyBoids = 0;

    for (int i = 0; i < numBoids; i++)
    {
        const Boid boid = boids[i];

        const float distance = Vector3Distance(position, boid.position);
        const bool isOtherPosSameAsMe = (position == boid.position);

        if (!isOtherPosSameAsMe && distance < viewRadius)
        {
            steeringForce += boid.position; // Sum up the position of all nearby boids
            numNearbyBoids++;
        }
    }

    if (numNearbyBoids > 0)
    {
        steeringForce /= (float)numNearbyBoids; // The average position
        steeringForce -= position;
        steeringForce = Vector3ClampValue(steeringForce, 0, maxForce);
    }

    return steeringForce;
}

Vector3 Boid::Separate(const GameState* gameState) const
{
    const Boid* boids = gameState->boids;
    const int numBoids = gameState->numBoids;

    constexpr float separationDistance = 20.0f;

    Vector3 steeringForce = {};
    int numNearbyBoids = 0;

    for (int i = 0; i < numBoids; i++)
    {
        const Boid boid = boids[i];

        const float distance = Vector3Distance(position, boid.position);
        const bool isOtherPosSameAsMe = (position == boid.position);

        if (!isOtherPosSameAsMe && distance < separationDistance)
        {
            numNearbyBoids++;
            // Divide by distance so closer boids have a higher impact on separation force
            Vector3 diff = (position - boid.position) / distance;
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
    const Boid* boids = gameState->boids;
    const int numBoids = gameState->numBoids;

    for (int i = 0; i < numBoids; i++)
    {
        const Boid boid = boids[i];

        DrawBoid(boid, DARKBLUE, 3);
    }
}

void UpdateBoids(GameState* gameState)
{
    constexpr float maxSpeed = 3.0f;

    Boid* boids = gameState->boids;
    const int numBoids = gameState->numBoids;
    const float worldSize = gameState->worldSize;

    for (int i = 0; i < numBoids; i++)
    {
        Boid& boid = boids[i];

        // Apply alignment, cohesion and separation.
        Vector3 acceleration = boid.Align(gameState) + boid.Cohere(gameState) + boid.Separate(gameState);

        acceleration += TurnBoidIfCloseToBoundary(boid, worldSize);

        // Update position
        boid.velocity += acceleration;
        boid.velocity = Vector3ClampValue(boid.velocity, 0, maxSpeed);
        boid.position += boid.velocity;
    }
}
