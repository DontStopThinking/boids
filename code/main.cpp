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

    constexpr int numBoids = 300;
    constexpr float worldSize = 200.0f;
    constexpr float worldSizeHalf = worldSize / 2;

    GameMemory gameMemory = {};
    gameMemory.permanentStorageSize = sizeof(GameState) + (sizeof(Boid) * numBoids);
    gameMemory.permanentStorage = VirtualAlloc(
        nullptr,
        gameMemory.permanentStorageSize,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE);

    if (!gameMemory.permanentStorage)
    {
        std::puts("ERROR: Failed to allocate memory for the game. Exiting.");
        return -1;
    }

    GameState* gameState = (GameState*)gameMemory.permanentStorage;
    gameState->numBoids = numBoids;
    gameState->worldSize = worldSizeHalf;

    // We assign the boids array pointer to point to just after the gameState object. So after this our memory
    // layout will basically be:
    // --------------------------------------------------------------
    // | gameState object | array that gameState->boids points to |
    // --------------------------------------------------------------
    gameState->boids = (Boid*)((uint8_t*)gameMemory.permanentStorage + sizeof(GameState));

    for (int i = 0; i < numBoids; i++)
    {
        const float rx = RandomFloat(-worldSizeHalf, worldSizeHalf);
        const float ry = RandomFloat(-worldSizeHalf, worldSizeHalf);
        const float rz = RandomFloat(-worldSizeHalf, worldSizeHalf);

        Boid boid = {};

        boid.position =
        {
            .x = rx,
            .y = ry,
            .z = rz
        };
        boid.velocity = CreateRandomVector3() * 0.3f;

        gameState->boids[i] = boid;
    }

    constexpr float moveSpeed = 2.0f;
    constexpr float mouseSensitivity = 0.05f;

    bool paused = false;

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

        if (IsKeyPressed(KEY_P))
        {
            paused = !paused;
        }

        UpdateCameraPro(&camera, cameraMovement, cameraRotation, 0.0f);

        if (!paused)
        {
            UpdateBoids(gameState);
        }
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

            if (paused)
            {
                DrawText("Paused", screenWidth - 100, 10, 25, ORANGE);
            }

            DrawFPS(5, 5);


        EndDrawing();
        /**** END DRAW ****/
    }

    CloseWindow();

    return 0;
}
