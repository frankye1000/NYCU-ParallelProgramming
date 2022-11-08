/* time=1.3s */
#include<iostream>
#include<cstdlib>
#include<pthread.h>
using namespace std;

long thread_count;
long long number_of_tosses;
long long number_in_circle; 
pthread_mutex_t mutex;

unsigned int seed = time(NULL);

void* Thread_number_in_circle(void* rank);
double random_num();
inline double fastrand();

int main(int argc, char* argv[]){
    thread_count     = strtol(argv[1],NULL,10);
    number_of_tosses = strtoll(argv[2],NULL,10);
    pthread_t* thread_handles;
    thread_handles = (pthread_t*) malloc(thread_count*sizeof(pthread_t));
    pthread_mutex_init(&mutex, NULL);

    number_in_circle = 0;
    for(long thread=0;thread<thread_count;thread++){
        pthread_create(&thread_handles[thread], NULL, Thread_number_in_circle, (void*)thread);
    }

    for(long thread=0;thread<thread_count;thread++){
        pthread_join(thread_handles[thread],NULL);
    }

    pthread_mutex_destroy(&mutex);
    free(thread_handles);
    
    double pi = 4 * number_in_circle /(( double ) number_of_tosses);
    printf("pi= %lf",pi);
    
    return 0;
}


void* Thread_number_in_circle(void* rank){
    long long i;
    long my_rank   = (long) rank;
    long long my_n = number_of_tosses/thread_count;
    long long my_first_i = my_n * my_rank;
    long long my_last_i  = my_first_i + my_n;
    double x,y;

    pthread_mutex_lock(&mutex);
    for(i = my_first_i; i < my_last_i; i++){
        x = random_num();
        y = random_num();
        if(x * x + y * y <= 1.f) number_in_circle++;
    }
    pthread_mutex_unlock(&mutex);

    return NULL;
}


double random_num(){
    double HI=1.0;
    return static_cast <double> (rand_r(&seed)) /( static_cast <double> (RAND_MAX/(HI)));
}

