#include <cstdio>
#include <cmath>
#include <ctime>
#include <cstdlib>

#include "raylib.h"
#include "raymath.h"

#include "raylibwindows.h"
#include "game.h"
#include "boid.h"
#include "mathutils.h"

int main()
{
    constexpr int screenWidth = 1024;
    constexpr int screenHeight = 800;

    std::srand((unsigned int)std::time(nullptr));

    InitWindow(screenWidth, screenHeight, "Boids");

    constexpr int fps = 60;
    SetTargetFPS(fps);

    Camera camera =
    {
        .position = { .x = 0.0f, .y = 100.0f, .z = 300.0f },
        .target = { .x = 0.0f, .y = 0.0f, .z = 0.0f },
        .up = { .x = 0.0f, .y = 1.0f, .z = 0.0f },
        .fovy = 45.0f,
        .projection = CAMERA_PERSPECTIVE
    };

    DisableCursor();

    constexpr int numBoids = 200;
    constexpr float worldSize = 200.0f;
    constexpr float worldSizeHalf = worldSize / 2;

    GameMemory gameMemory = {};
    gameMemory.m_PermanentStorageSize = sizeof(GameState) + (sizeof(Boid) * numBoids);
    gameMemory.m_PermanentStorage = VirtualAlloc(
        nullptr,
        gameMemory.m_PermanentStorageSize,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE);

    if (!gameMemory.m_PermanentStorage)
    {
        std::puts("ERROR: Failed to allocate memory for the game. Exiting.");
        return -1;
    }

    GameState* gameState = (GameState*)gameMemory.m_PermanentStorage;
    gameState->m_NumBoids = numBoids;
    gameState->m_WorldSize = worldSizeHalf;

    // We assign the boids array pointer to point to just after the gameState object. So after this our memory
    // layout will basically be:
    // -----------------------------------------------
    // | gameState object | gameState->m_Boids array |
    // ---------------------------------------------- -
    gameState->m_Boids = (Boid*)((uint8_t*)gameMemory.m_PermanentStorage + sizeof(GameState));

    for (int i = 0; i < numBoids; i++)
    {
        const float rx = RandomFloat(-worldSizeHalf, worldSizeHalf);
        const float ry = RandomFloat(-worldSizeHalf, worldSizeHalf);
        const float rz = RandomFloat(-worldSizeHalf, worldSizeHalf);

        Boid boid = {};

        boid.m_Position =
        {
            .x = rx,
            .y = ry,
            .z = rz
        };
        boid.m_Velocity = CreateRandomVector3() * 0.3f;

        gameState->m_Boids[i] = boid;
    }

    constexpr float moveSpeed = 2.0f;
    constexpr float mouseSensitivity = 0.05f;

    while (!WindowShouldClose())
    {
        /**** BEGIN UPDATE ****/
        const Vector3 cameraMovement =
        {
            .x = (IsKeyDown(KEY_W) - IsKeyDown(KEY_S)) * moveSpeed,
            .y = (IsKeyDown(KEY_D) - IsKeyDown(KEY_A)) * moveSpeed,
            .z = (IsKeyDown(KEY_E) - IsKeyDown(KEY_Q)) * moveSpeed
        };
        const Vector3 cameraRotation =
        {
            .x = GetMouseDelta().x * mouseSensitivity,
            .y = GetMouseDelta().y * mouseSensitivity,
            .z = 0.0f
        };

        UpdateCameraPro(&camera, cameraMovement, cameraRotation, 0.0f);

        UpdateBoids(gameState);
        /**** END UPDATE ****/

        /**** BEGIN DRAW ****/
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);

                DrawBoids(gameState);

                DrawCube(
                    Vector3{ .x = 0.0f, .y = 0.0f, .z = 0.0f },
                    worldSize, worldSize, worldSize, Fade(GRAY, 0.4f));

                DrawCubeWires(
                    Vector3{ .x = 0.0f, .y = 0.0f, .z = 0.0f },
                    worldSize, worldSize, worldSize, BLACK);

                DrawGrid(100, 10.0f);

            EndMode3D();

            DrawFPS(5, 5);


        EndDrawing();
        /**** END DRAW ****/
    }

    CloseWindow();

    return 0;
}
