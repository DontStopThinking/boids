#pragma once

//#include "boid.h"

class Boid;

struct GameState
{
    int m_NumBoids;
    float m_WorldSize;
    Boid* m_Boids;
};

struct GameMemory
{
    size_t m_PermanentStorageSize;
    void* m_PermanentStorage;
};
