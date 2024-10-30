#pragma once

class Boid;

struct GameState
{
    int numBoids;
    float worldSize;
    Boid* boids;
};

struct GameMemory
{
    size_t permanentStorageSize;
    void* permanentStorage;
};
