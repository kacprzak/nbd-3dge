#ifndef FPSCOUNTER_H
#define FPSCOUNTER_H

#include "Text.h"

class FpsCounter
{
public:
    FpsCounter();

    void update(float delta);
    void setText(std::shared_ptr<Text> text)
    {
        m_text = text;
    }

private:
    float m_timePassed;
    unsigned int m_framesPassed;
    std::shared_ptr<Text> m_text;
};

#endif // FPSCOUNTER_H
