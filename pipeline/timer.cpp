#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include "timer.h"

cg_timer* get_global_timer()
{
    static cg_timer timer;
    return &timer;
}

cg_timer::cg_timer()
{
    m_bTimerStopped = true;
    m_bUsingQPF = false;
    m_llQPFTicksPerSec = 0;
    m_llStopTime = 0;
    m_llLastElapsedTime = 0;
    m_llBaseTime = 0;

    LARGE_INTEGER qwTicksPerSec;
    m_bUsingQPF = (bool)(QueryPerformanceFrequency(&qwTicksPerSec) != 0);
    m_llQPFTicksPerSec = qwTicksPerSec.QuadPart;
}

void cg_timer::reset()
{
    if (!m_bUsingQPF)
        return;

    LARGE_INTEGER qwTime;
    if (m_llStopTime != 0)
        qwTime.QuadPart = m_llStopTime;
    else
        QueryPerformanceCounter(&qwTime);

    m_llBaseTime = qwTime.QuadPart;
    m_llLastElapsedTime = qwTime.QuadPart;
    m_llStopTime = 0;
    m_bTimerStopped = FALSE;
}

void cg_timer::start()
{
    if (!m_bUsingQPF)
        return;

    LARGE_INTEGER qwTime;
    QueryPerformanceCounter(&qwTime);

    if (m_bTimerStopped)
        m_llBaseTime += qwTime.QuadPart - m_llStopTime;
    m_llStopTime = 0;
    m_llLastElapsedTime = qwTime.QuadPart;
    m_bTimerStopped = FALSE;
}

void cg_timer::stop()
{
    if (!m_bUsingQPF)
        return;

    if (!m_bTimerStopped)
    {
        LARGE_INTEGER qwTime;
        if (m_llStopTime != 0)
            qwTime.QuadPart = m_llStopTime;
        else
            QueryPerformanceCounter(&qwTime);

        m_llStopTime = qwTime.QuadPart;
        m_llLastElapsedTime = qwTime.QuadPart;
        m_bTimerStopped = TRUE;
    }
}

bool cg_timer::is_stopped()
{
    return m_bTimerStopped;
}

double cg_timer::get_elapsed_time()
{
    if (!m_bUsingQPF)
        return -1.0;

    LARGE_INTEGER qwTime;
    if (m_llStopTime != 0) {
        qwTime.QuadPart = m_llStopTime;
    }
    else {
        // get current time
        QueryPerformanceCounter(&qwTime);
    }

    // calculate elapsed time since last call
    double fElapsedTime = (double)(qwTime.QuadPart - m_llLastElapsedTime) / (double)m_llQPFTicksPerSec;
    // remember this call, the variable is a member of the class
    m_llLastElapsedTime = qwTime.QuadPart;
    // return elapsed time
    return fElapsedTime;
}