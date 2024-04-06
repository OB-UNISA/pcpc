#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "lab4_4_mycollective.h"

int main(int argc, char **argv)
{
    int rank, P;
    int N = argc == 2 ? atoi(argv[1]) : 30;
    int array[N];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &P);

    for (int i = 0; i < N; i++)
    {
        array[i] = i;
    }

    double start, end;
    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();

    scatter(P, rank, array, N, MPI_INT, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();
    if (rank == 0)
    {
        printf("%d items.\nTime in ms = %f\n", N, end - start);
    }

    MPI_Finalize();
    return 0;
}
