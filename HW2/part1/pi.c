/*Ver 3 : 0.5s*/
# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <immintrin.h>

# include "simdxorshift128plus.h"

volatile long long int num_cycle = 0;
pthread_mutex_t mutex;

void* my_function(void* param);


int main(int argc, char** argv){
    if (argc != 3){
	    printf("usage: ./pi.out {CPU core} {Number of tosses}\n");
	    return 1;
    }

    int thread_count       = atoi(argv[1]);
    long long int num_toss = atoll(argv[2]);

    pthread_t* thread_handles;
    thread_handles = (pthread_t*)malloc(thread_count * sizeof(pthread_t));
    long long int num_thread_exe = num_toss / thread_count;
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < thread_count; i++)
    {
        if (i == thread_count - 1)
        {
            num_thread_exe += num_toss % thread_count;
            pthread_create(&thread_handles[i], NULL, my_function, (void*)&num_thread_exe);
        }
        else
        {
            pthread_create(&thread_handles[i], NULL, my_function, (void*)&num_thread_exe);
        }
    }

    for (int i = 0; i < thread_count; i++) pthread_join(thread_handles[i], NULL);

    free(thread_handles);
    pthread_mutex_destroy(&mutex);
    float pi = 4 * num_cycle / ((float) num_toss);
    printf("%lf\n", pi);

    return 0;
}

/* reference: https://github.com/lemire/SIMDxorshift */
void* my_function(void* num_thread_exe){
    long long int my_num_thread_exe = *(int*)num_thread_exe;
    long long int my_cycles = 0;
    avx_xorshift128plus_key_t mykey;
    avx_xorshift128plus_init(324, 4444, &mykey);
    __m256 full = _mm256_set_ps(INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX);

    for (int i = 0; i < my_num_thread_exe; i+=8){
        __m256i x_i = avx_xorshift128plus(&mykey);
        __m256 x_f  = _mm256_cvtepi32_ps(x_i);
        __m256 x    = _mm256_div_ps(x_f, full);

        __m256i y_i = avx_xorshift128plus(&mykey);
        __m256 y_f  = _mm256_cvtepi32_ps(y_i);
        __m256 y    = _mm256_div_ps(y_f, full);

        __m256 x_2 = _mm256_mul_ps(x, x);
        __m256 y_2 = _mm256_mul_ps(y, y);
        __m256 sum = _mm256_add_ps(x_2, y_2);

        float val[8];
        _mm256_store_ps(val, sum);

        for (int j = 0; j < 8; j++){
            if (val[j] <= 1.f) ++my_cycles;
        }
    }

    pthread_mutex_lock(&mutex);
    num_cycle += my_cycles;
    pthread_mutex_unlock(&mutex);
    return NULL;
}
