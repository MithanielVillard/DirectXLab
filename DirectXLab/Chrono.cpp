#include "pch.h"
#include "Chrono.h"
#include <Windows.h>

Chrono::Chrono()
{
    m_sysTime = timeGetTime();
}

float Chrono::Reset()
{
    DWORD newSysTime = timeGetTime();
    DWORD elapsedTime = newSysTime - m_sysTime;
    
    m_sysTime = newSysTime;
    
    return elapsedTime/1000.0f;
}

