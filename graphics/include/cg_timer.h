/* high resolution timer */
#ifndef __cg_timer__
#define __cg_timer__

namespace atlas {
    class cg_timer
    {
    public:
        cg_timer();

        void reset(); // resets the timer
        void start(); // starts the timer
        void stop();  // stop (or pause) the timer
        void advance(); // advance the timer by 0.1 seconds
        double get_absolute_time(); // get the absolute system time
        double get_time(); // get the current time
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
}

#endif // __cg_timer__
