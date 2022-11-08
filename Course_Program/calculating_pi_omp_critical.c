#include<omp.h>
#include<stdio.h>
static long num_steps = 100000000; double step; 
#define NUM_THREADS 4 

void main () { 
    int i, nthreads;
    double x, pi;
    step = 1.0/(double) num_steps;
    omp_set_num_threads(NUM_THREADS); 
    #pragma omp parallel 
    {
        int i, id, nthrds; 
        double x, sum; 
        id     = omp_get_thread_num();    // get thread id 
        nthrds = omp_get_num_threads();   // get total thread number
        printf("id= %d, nthrds= %d\n", id, nthrds);
        if (id == 0) nthreads = nthrds;   // ????????
        for (i=id, sum=0.0; i < num_steps; i = i+nthrds) { 
            x = (i+0.5)*step; 
            sum += 4.0/(1.0+x*x);
        } 
    
    #pragma omp critacal
        pi += sum * step; 
    }
    printf("pi= %lf\n", pi);
}
