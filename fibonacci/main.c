#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

/**** Parameters ****/

// #define N               70
#define N               70
#define REPEAT          1000            // for parallel case
// #define REPEAT          10000000        // for serial case
#define MULTITHREAD     1
#define GRANULARITY     4
#define USE_FUNCTION    0


typedef int bool;

/**** Implementations ****/

void fibonacci_serial(int n, double *output)
{
    double a = 1.0, a_1 = 0.0;
    output[0] = a;
    for (int i = 1; i != n; ++i) {
        a += a_1;
        a_1 = a - a_1;
        output[i] = a;
    }
}

void *_fibonacci_parallel_job(void *arg)
{
    double *inout = (double *)arg;
    *inout = round(
        ( pow(1.6180339887498949025257388711906969547271728515625, *inout)
            - pow(-0.6180339887498949025257388711906969547271728515625, *inout)
        ) / 2.236067977499789805051477742381393909454345703125
    );
    return NULL;
}

void fibonacci_serial_func(int n, double *output)
{
    for (int i = 0; i != n; ++i) {
        output[i] = i + 1;
        _fibonacci_parallel_job((void *)&output[i]);
    }
}

void fibonacci_parallel_granctrl(int n, double *output)
{
    pthread_t *pool = (pthread_t *)malloc(n * sizeof(pthread_t));
    if (pool == NULL) {
        return;
    }
    for (int i = 0; i < n; i += GRANULARITY) {
        for (int j = i; j != i + GRANULARITY && j != n; ++j) {
            output[j] = j + 1;
            if (pthread_create(&pool[j], NULL, _fibonacci_parallel_job, (void *)&output[j])) {
                free(pool);
                return;
            }
        }
        for (int j = i; j != i + GRANULARITY && j != n; ++j) {
            pthread_join(pool[j], NULL);
        }
    }
    free(pool);
}

void fibonacci_parallel_naiive(int n, double *output)
{
    pthread_t *pool = (pthread_t *)malloc(n * sizeof(pthread_t));
    if (pool == NULL) {
        return;
    }
    for (int i = 0; i != n; ++i) {
        output[i] = i + 1;
        if (pthread_create(&pool[i], NULL, _fibonacci_parallel_job, (void *)&output[i])) {
            free(pool);
            return;
        }
    }
    for (int i = 0; i != n; ++i) {
        pthread_join(pool[i], NULL);
    }
    free(pool);
}

/**** Test Runtime ****/

int main(int argc, const char **argv)
{
    int n = N;
    double *out = (double *)malloc(n * sizeof(double));
    if (out == NULL) {
        return -1;
    }

    // Choosing method
    void (*func)(int, double *) = NULL;
    if (MULTITHREAD) {
        if (GRANULARITY <= 0) { func = fibonacci_parallel_naiive; }
        else { func = fibonacci_parallel_granctrl; }
    } else {
        if (!USE_FUNCTION) { func = fibonacci_serial; }
        else { func = fibonacci_serial_func; }
    }

    // Run
    for (int i = 0; i != REPEAT; ++i) {
        func(n, out);
    }
    // Print to console
    for (int i = 0; i != n; ++i) {
        printf("fib(%d) = %.0lf\n", i, out[i]);
    }
    free(out);
    return 0;
}
