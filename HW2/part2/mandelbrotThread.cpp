#include <stdio.h>
#include <thread>

#include "CycleTimer.h"

typedef struct
{
    float x0, x1;
    float y0, y1;
    unsigned int width;
    unsigned int height;
    int maxIterations;
    int *output;
    int threadId;
    int numThreads;
} WorkerArgs;

extern void mandelbrotSerial(
    float x0, float y0, float x1, float y1,
    int width, int height,
    int startRow, int numRows,
    int maxIterations,
    int output[]);

//
// workerThreadStart --
//
// Thread entrypoint.
void workerThreadStart(WorkerArgs *const args)
{
    /*******************v1*******************/
    // double startTime = CycleTimer::currentSeconds();
    // int path        = args->height/args->numThreads;
    // int path_remain = args->height%args->numThreads;  // 會有沒辦法整除的部分(剩餘部分圖像)

    // for(unsigned int i=0;i<args->numThreads;i++){
    //     if(i==args->threadId){ 
    //         if(args->threadId==(args->numThreads-1)){ //最後一個thread要多處理剩餘部分圖像
    //             path += path_remain;
    //             //printf("Thread=%d, path=%d\n",args->threadId,path);
    //             mandelbrotSerial(args->x0, args->y0, args->x1, args->y1,
    //                         args->width, args->height,
    //                         (args->height/args->numThreads)*i, path,
    //                         args->maxIterations,
    //                         args->output);
    //             break;
    //         }else{
    //             //printf("Thread=%d, path=%d\n",args->threadId,path);
    //             mandelbrotSerial(args->x0, args->y0, args->x1, args->y1,
    //                         args->width, args->height,
    //                         (args->height/args->numThreads)*i, path,
    //                         args->maxIterations,
    //                         args->output);
    //             break;
    //         }
    //     }
    // }
    // double endTime = CycleTimer::currentSeconds();
    // double minThread = endTime - startTime;
    // printf("[mandelbrot thread %d]:\t\t[%.3f] ms\n", args->threadId, minThread * 1000);
    /*******************************************/

    /*******************v2*******************/
    double startTime = CycleTimer::currentSeconds();

    for (unsigned int i = args->threadId; i < args->height; i += args->numThreads){
	    mandelbrotSerial(args->x0, args->y0, args->x1, args->y1,
			            args->width, args->height,
			            i, 1,
 		                args->maxIterations,
			            args->output);
    }

    double endTime = CycleTimer::currentSeconds();

    double minThread = endTime - startTime;
    printf("[mandelbrot thread %d]:\t\t[%.3f] ms\n", args->threadId, minThread * 1000);
    /*******************************************/

    
}

//
// MandelbrotThread --
//
// Multi-threaded implementation of mandelbrot set image generation.
// Threads of execution are created by spawning std::threads.
void mandelbrotThread(
    int numThreads,
    float x0, float y0, float x1, float y1,
    int width, int height,
    int maxIterations, int output[])
{
    static constexpr int MAX_THREADS = 32;

    if (numThreads > MAX_THREADS)
    {
        fprintf(stderr, "Error: Max allowed threads is %d\n", MAX_THREADS);
        exit(1);
    }

    // Creates thread objects that do not yet represent a thread.
    std::thread workers[MAX_THREADS];
    WorkerArgs args[MAX_THREADS];

    for (int i = 0; i < numThreads; i++)
    {
        // TODO FOR PP STUDENTS: You may or may not wish to modify
        // the per-thread arguments here.  The code below copies the
        // same arguments for each thread
        args[i].x0 = x0;
        args[i].y0 = y0;
        args[i].x1 = x1;
        args[i].y1 = y1;
        args[i].width = width;
        args[i].height = height;
        args[i].maxIterations = maxIterations;
        args[i].numThreads = numThreads;
        args[i].output = output;

        args[i].threadId = i;
    }

    // Spawn the worker threads.  Note that only numThreads-1 std::threads
    // are created and the main application thread is used as a worker
    // as well.
    for (int i = 1; i < numThreads; i++)
    {
        workers[i] = std::thread(workerThreadStart, &args[i]);
    }

    workerThreadStart(&args[0]);

    // join worker threads
    for (int i = 1; i < numThreads; i++)
    {
        workers[i].join();
    }
}
