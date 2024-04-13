#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#include "mycollective.h"

#define UPPER 10000

int rand_num(int upper, int _rand)
{
    srand(time(0) * (_rand + 1));
    return rand() % upper;
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int N = argc == 2 ? atoi(argv[1]) : 100;

    int rank, P;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &P);

    int array[N];

    if (rank == 0)
    {
        for (int i = 0; i < N; i++)
        {
            array[i] = rand_num(UPPER, i);
        }
    }

    int min_res = min(P, rank, array, N, MPI_COMM_WORLD);
    int max_res = max(P, rank, array, N, MPI_COMM_WORLD);
    if (rank == 0)
    {
        printf("Rank %d, Min: %d, Max: %d\n", rank, min_res, max_res);
    }
    else
    {
        printf("Rank %d, local_min: %d, local_max: %d\n", rank, min_res, max_res);
    }

    MPI_Finalize();
    return 0;
}
