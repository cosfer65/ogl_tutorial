/* high resolution timer */
// check if a custom definition exists
#ifndef __cg_timer__
// usualy we define something close to the file name
#define __cg_timer__

class cg_timer
{
public:
    cg_timer();

    void reset(); // resets the timer
    void start(); // starts the timer
    void stop();  // stop (or pause) the timer
    double get_elapsed_time(); // get the time that elapsed between get_elapsed_time() calls
    bool is_stopped(); // returns true if timer stopped

protected:
    bool m_bTimerStopped;
#ifdef _WIN32
    bool m_bUsingQPF;
    LONGLONG m_llQPFTicksPerSec;

    LONGLONG m_llStopTime;
    LONGLONG m_llLastElapsedTime;
    LONGLONG m_llBaseTime;
#else
    struct timeval last;
#endif
};

cg_timer* get_global_timer();

#endif // __cg_timer__
