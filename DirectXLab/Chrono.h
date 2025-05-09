#pragma once

class Chrono
{
public:
    Chrono();
    ~Chrono() = default;

    float Reset();

private:
    unsigned long m_sysTime;
};
