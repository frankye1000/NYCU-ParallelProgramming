#include<stdio.h>
#include "omp.h"
int thread_count=3;
void main(){
    #pragma omp parallel num_threads(thread_count)
    {
        int ID = omp_get_thread_num();
        printf("Hello from thrad %d of %d\n", ID, thread_count);
    }
}