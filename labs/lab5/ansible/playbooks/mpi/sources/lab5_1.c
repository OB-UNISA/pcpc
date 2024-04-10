#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define UPPER 100
#define LOWER 0
#define ITERATIONS 10

int rand_num(int upper, int _rand)
{
    srand(time(0) * (_rand + 1));
    return rand() % upper;
}

int mod(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    MPI_Request request = MPI_REQUEST_NULL;
    MPI_Status status;

    int rank, P;
    int v = 0;
    int iteractions = 0;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &P);

    int S = argc == 2 ? atoi(argv[1]) : rand() % (rand_num(P * ITERATIONS * UPPER * 0.7, 1));
    int prev_rank = mod(rank - 1, P);
    int next_rank = mod(rank + 1, P);
    int num = rand_num(UPPER, rank);

    if (rank == 0)
    {
        printf("S: %d\n", S);
        for (int i = 0; i < ITERATIONS; i++)
        {
            v = v <= S ? v + num : v;
            MPI_Isend(&v, 1, MPI_INT, next_rank, next_rank, MPI_COMM_WORLD, &request);

            if (v > S && i > 0)
            {
                iteractions = i + 1;
                break;
            }
            printf("[r:%d s:%d]Rank: %d, num: %d, v: %d\n", prev_rank, next_rank, rank, num, v);
            MPI_Irecv(&v, 1, MPI_INT, prev_rank, rank, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &status);
        }
    }
    else
    {
        for (int i = 0; i < ITERATIONS && v <= S; i++)
        {
            MPI_Irecv(&v, 1, MPI_INT, prev_rank, rank, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &status);

            v = v <= S ? v + num : v;
            printf("[r:%d s:%d]Rank: %d, num: %d, v: %d\n", prev_rank, next_rank, rank, num, v);
            MPI_Isend(&v, 1, MPI_INT, next_rank, next_rank, MPI_COMM_WORLD, &request);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0)
    {
        printf("S: %d, v: %d, i: %d\n", S, v, iteractions);
    }

    MPI_Finalize();
    return 0;
}
