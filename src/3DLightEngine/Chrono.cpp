#include "Chrono.h"
#include <Window.h>

Chrono::Chrono() : m_lastTime(timeGetTime()), m_pauseTime(0)
{
}

Chrono::~Chrono()
{
}

void Chrono::Start()
{
    if (!m_isPaused) return;

    m_lastTime += timeGetTime() - m_pauseTime;
    m_isPaused = false;
}

void Chrono::Pause()
{
    if (m_isPaused) return;

    m_pauseTime = timeGetTime();
    m_isPaused = true;
}

float Chrono::GetElapsedTime()
{
    if (m_isPaused)
    {
        uint64_t elapsedtime = m_pauseTime - m_lastTime;
        return elapsedtime / 1000.0f;
    }

    uint64_t currentTime = timeGetTime();
    uint64_t elapsedTime = currentTime - m_lastTime;
    return elapsedTime / 1000.0f;
}

float Chrono::Reset()
{
    uint64_t currentTime = timeGetTime();

    if (m_isPaused)
    {
        uint64_t elapsedTime = m_pauseTime - m_lastTime;
        m_lastTime = currentTime;
        m_pauseTime = currentTime;
        return elapsedTime / 1000.0f;
    }

    uint64_t elapsedTime = currentTime - m_lastTime;
    m_lastTime = currentTime;
    return elapsedTime / 1000.0f;
}