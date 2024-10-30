#pragma once

#include "raylib.h"
#include "raymath.h"

#include "game.h"

class Boid
{
public:
    Vector3 position;
    Vector3 velocity;

    Vector3 Align(const GameState* gameState) const;
    Vector3 Cohere(const GameState* gameState) const;
    Vector3 Separate(const GameState* gameState) const;
};

void DrawBoids(const GameState* gameState);
void UpdateBoids(GameState* gameState);
