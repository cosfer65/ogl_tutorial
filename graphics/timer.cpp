#include <windows.h>
#include "timer.h"

namespace atlas {
    gl_timer* get_global_timer()
    {
        static gl_timer timer;
        return &timer;
    }

    gl_timer::gl_timer()
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

    void gl_timer::reset()
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

    void gl_timer::start()
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

    void gl_timer::stop()
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

    void gl_timer::advance()
    {
        if (!m_bUsingQPF)
            return;
        m_llStopTime += m_llQPFTicksPerSec / 10;
    }

    double gl_timer::get_absolute_time()
    {
        if (!m_bUsingQPF)
            return -1.0;

        LARGE_INTEGER qwTime;
        if (m_llStopTime != 0)
            qwTime.QuadPart = m_llStopTime;
        else
            QueryPerformanceCounter(&qwTime);

        double fTime = qwTime.QuadPart / (double)m_llQPFTicksPerSec;

        return fTime;
    }

    double gl_timer::get_time()
    {
        if (!m_bUsingQPF)
            return -1.0;

        LARGE_INTEGER qwTime;
        if (m_llStopTime != 0)
            qwTime.QuadPart = m_llStopTime;
        else
            QueryPerformanceCounter(&qwTime);

        double fAppTime = (double)(qwTime.QuadPart - m_llBaseTime) / (double)m_llQPFTicksPerSec;

        return fAppTime;
    }

    double gl_timer::get_elapsed_time()
    {
        if (!m_bUsingQPF)
            return -1.0;

        LARGE_INTEGER qwTime;
        if (m_llStopTime != 0)
            qwTime.QuadPart = m_llStopTime;
        else
            QueryPerformanceCounter(&qwTime);

        double fElapsedTime = (double)(qwTime.QuadPart - m_llLastElapsedTime) / (double)m_llQPFTicksPerSec;
        m_llLastElapsedTime = qwTime.QuadPart;

        return fElapsedTime;
    }

    bool gl_timer::is_stopped()
    {
        return m_bTimerStopped;
    }
}