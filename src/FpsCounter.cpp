#include "FpsCounter.h"
#include <iostream>

FpsCounter::FpsCounter()
    : m_timePassed(0.0f)
    , m_framesPassed(0)
{
}

void FpsCounter::update(float delta)
{
    ++m_framesPassed;
    m_timePassed += delta;

    if (m_timePassed > 1.0f) {
        double val = 1000.0 / m_framesPassed;
        if (m_text)
            m_text->setText("ms/f: " + std::to_string(val));

        m_framesPassed = 0;
        m_timePassed -= 1.0f;
    }
}
