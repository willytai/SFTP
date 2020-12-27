#ifndef __USAGE_H__
#define __USAGE_H__

#include <chrono>
/********************
 * Unit: ms         *
 * Precision: us    *
 *******************/
struct Timer
{
    Timer(double* dst) : _report_dst(dst) {
        _startTimePoint = std::chrono::high_resolution_clock::now();
    }
    ~Timer() {
        auto _endTimePoint = std::chrono::high_resolution_clock::now();
        auto _start = std::chrono::time_point_cast<std::chrono::microseconds>(_startTimePoint).time_since_epoch().count();
        auto _end   = std::chrono::time_point_cast<std::chrono::microseconds>(_endTimePoint).time_since_epoch().count();
        *_report_dst = (double)(_end - _start) * 1e-3;
    }

    std::chrono::high_resolution_clock::time_point _startTimePoint;
    double* _report_dst;
};

/*****************************************
 * Memory/Time Usage from system library *
 ****************************************/
#ifdef __cplusplus
extern "C" {
#endif

#include <sys/resource.h>

static struct rusage usg;

// memory/time usage
struct SysUsage
{
    // in MB
    static double getMem() {
        getrusage( RUSAGE_SELF, &usg );
#ifdef __APPLE__
        return (double)usg.ru_maxrss / (double)(1<<20); // bytes
#else
        return (double)usg.ru_maxrss / (double)(1<<10); // Kbytes
#endif
    }
};

#ifdef __cplusplus
}
#endif
#endif /* __USAGE_H__ */
