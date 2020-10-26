#include <time.h>

#include "util.h" //implementing

double stopwatch(void){
    struct timespec ts;
    //timespec_get(&ts, TIME_UTC); //-std=c11
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double) ts.tv_sec + ts.tv_nsec/1000000000.0;
}
