#include <cstdio>
#include <vector>
#include <cmath>
#include <memory>
#include <ctime>
#include <cstdlib>

#include "raylib.h"
#include "raymath.h"

#include "boid.h"
#include "mathutils.h"

int main()
{
    constexpr int screenWidth = 1024;
    constexpr int screenHeight = 800;

    std::srand((unsigned int)std::time(nullptr));

    InitWindow(screenWidth, screenHeight, "Boids");

    SetTargetFPS(60);

    Camera camera =
    {
        .position = { .x = 10.0f, .y = 10.0f, .z = 10.0f },
        .target = { .x = 0.0f, .y = 0.0f, .z = 0.0f },
        .up = { .x = 0.0f, .y = 1.0f, .z = 0.0f },
        .fovy = 45.0f,
        .projection = CAMERA_PERSPECTIVE
    };

    DisableCursor();

    constexpr int numBoids = 200;

    std::vector<std::unique_ptr<Boid>> boids;
    boids.reserve(numBoids);

    for (int i = 0; i < numBoids; i++)
    {
        const float rx = RandomFloat(-10.0f, 10.0f);
        const float ry = RandomFloat(0.0f, 10.0f);
        const float rz = RandomFloat(-10.0f, 10.0f);

        auto boid = std::make_unique<Boid>();

        boid->m_Position =
        {
            .x = rx,
            .y = ry,
            .z = rz
        };
        boid->m_Velocity = CreateRandomVector3() * 0.05f;

        boids.push_back(std::move(boid));
    }

    while (!WindowShouldClose())
    {
        /**** BEGIN UPDATE ****/
        UpdateCamera(&camera, CAMERA_THIRD_PERSON);

        UpdateBoids(boids);
        /**** END UPDATE ****/

        /**** BEGIN DRAW ****/
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);

                DrawGrid(100, 1.0f);

                DrawBoids(boids);

            EndMode3D();

            DrawFPS(5, 5);

        EndDrawing();
        /**** END DRAW ****/
    }

    CloseWindow();

    return 0;
}
