/* high resolution timer */
// check if a custom definition exists
#ifndef __c_timer__
// usualy we define something close to the file name
#define __c_timer__

class gl_timer
{
public:
    gl_timer();

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

gl_timer* get_global_timer();

#endif // __c_timer__
