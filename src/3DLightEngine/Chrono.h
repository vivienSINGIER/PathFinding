#pragma once

#include <cstdint> // Ajoutez ceci en haut du fichier pour uint64_t

class Chrono
{
public:
    Chrono();
    ~Chrono();

    void Start();
    void Pause();

    float GetElapsedTime();
    float Reset();

private:
    bool m_isPaused = false;
    uint64_t m_lastTime;
    uint64_t m_pauseTime;
};
