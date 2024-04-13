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

    int count;
    count = scatter(P, rank, array, N, MPI_INT, MPI_COMM_WORLD);
    int min_max[] = {array[0], array[0]};

    if (rank == 0)
    {
        int l_min_max[2];
        for (int i = 1; i < P; i++)
        {
            MPI_Recv(l_min_max, 2, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (l_min_max[0] < min_max[0])
            {
                min_max[0] = l_min_max[0];
            }
            if (l_min_max[1] > min_max[1])
            {
                min_max[1] = l_min_max[1];
            }
        }

        printf("Rank: %d, Min: %d, Max: %d\n", rank, min_max[0], min_max[1]);
    }
    else
    {
        for (int i = 1; i < count; i++)
        {
            if (array[i] < min_max[0])
            {
                min_max[0] = array[i];
            }
            if (array[i] > min_max[1])
            {
                min_max[1] = array[i];
            }
        }

        printf("Rank: %d, local_min: %d, local_max: %d\n", rank, min_max[0], min_max[1]);
        MPI_Send(min_max, 2, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
