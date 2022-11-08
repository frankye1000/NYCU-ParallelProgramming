/* time=1.3 s*/
#include<iostream>
#include<cstdlib>
#include<pthread.h>
using namespace std;

unsigned int seed = time(NULL);
long long num_in_circle; 
pthread_mutex_t mutex;

void* Thread_function(void* rank);
double random_num();

int main(int argc, char* argv[]){
    int num_of_thread       = atoi(argv[1]);
    long long num_of_tosses = atoll(argv[2]);
    
    pthread_t* thread_handles;
    thread_handles = (pthread_t*) malloc(num_of_thread*sizeof(pthread_t));
    
    long long thread_num_of_tosses = num_of_tosses/num_of_thread;  /* 每個thread要執行幾次投擲 */
    
    pthread_mutex_init(&mutex, NULL);
    
    for(int i=0; i<num_of_thread; i++){
        if(i == num_of_thread-1){    /* 最後一個thread要多做沒有整除完的toss */
            thread_num_of_tosses += (num_of_tosses % num_of_thread);
            pthread_create(&thread_handles[i], NULL, Thread_function, (void*)thread_num_of_tosses);
        }else{
            pthread_create(&thread_handles[i], NULL, Thread_function, (void*)thread_num_of_tosses);
        }
    }

    for(int i=0; i<num_of_thread; i++){
        pthread_join(thread_handles[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    free(thread_handles);

    double pi = 4 * num_in_circle /(( double ) num_of_tosses);
    
    //cout<<"number_in_circle= "<<number_in_circle<<endl;
    printf("pi= %lf", pi);
    
    return 0;
}


void* Thread_function(void* num){

    long long thread_num_of_tosses = (long long) num;
    double x,y;
    long long temp;
    
    pthread_mutex_lock(&mutex);
    for(long long i = 0; i < thread_num_of_tosses; i++){
        x = random_num();
        y = random_num();
        if((x * x + y * y) <= 1.f) num_in_circle++;
    }
    pthread_mutex_unlock(&mutex);
    
    // pthread_mutex_lock(&mutex);
    // num_in_circle+=temp;
    // pthread_mutex_unlock(&mutex);

    return NULL;
}


double random_num(){
    double HI=1.0;
    return static_cast <double> (rand_r(&seed)) /( static_cast <double> (RAND_MAX/(HI)));
}

