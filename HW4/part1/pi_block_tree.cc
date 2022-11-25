#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoi(argv[1]);
    int world_rank, world_size;
    // ---

    // TODO: MPI init
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Status status;
    
    int SEED = 1234;
    int tag  = 0;
    long long int count = 0;
    long long int num_task = tosses / world_size;
    unsigned int seed = world_rank * SEED, dest = 0;
    long long int local_count = 0;
    srand(seed);

    for (long long int toss = 0; toss < num_task; toss++)
    {
        double x = (double) rand_r(&seed) / RAND_MAX;
        double y = (double) rand_r(&seed) / RAND_MAX;
        double distance = x * x + y * y;
        if (distance <= 1)
        {
	        local_count++;
        }
    }

    // TODO: binary tree redunction
    int base = 1;
    while (base < world_size)
    {
        MPI_Barrier(MPI_COMM_WORLD);
        if (base == 1){
            if (world_rank % 2 == 0)  // 偶數process收
            {
                int i = world_rank + 1;
                long long int buf;
                MPI_Recv(&buf, 1, MPI_UNSIGNED_LONG, i, tag, MPI_COMM_WORLD, &status);
                local_count += buf;
            }
            else
            {
                int dest = world_rank - 1;
                MPI_Send(&local_count, 1, MPI_UNSIGNED_LONG, dest, tag, MPI_COMM_WORLD);
            }
        }
        else{
            int mod = world_rank % base;
            int div = world_rank / base;

            if (mod == 0 && div % 2 == 0)
            {
                int i = world_rank + base;
                long long int buf;
                MPI_Recv(&buf, 1, MPI_UNSIGNED_LONG, i, tag, MPI_COMM_WORLD, &status);
                local_count += buf;
            }
            if (mod == 0 && div % 2 != 0){
                int dest = world_rank - base;
                MPI_Send(&local_count, 1, MPI_UNSIGNED_LONG, dest, tag, MPI_COMM_WORLD);
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);
        base *= 2;
    }

    if (world_rank == 0)
    {
        // TODO: PI result
	    count = local_count;
	    pi_result = 4 * count / (double) tosses;

        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }

    MPI_Finalize();
    return 0;
}
