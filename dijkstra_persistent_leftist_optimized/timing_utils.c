#include "timing_utils.h"
#include <time.h>
#include <stdio.h>
static clock_t start_time;
void start_timer() {
    start_time = clock();
}

double stop_timer() {
    clock_t end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    
    return elapsed_time;
}