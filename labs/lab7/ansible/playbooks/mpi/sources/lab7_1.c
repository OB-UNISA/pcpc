#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int N = argc == 2 ? atoi(argv[1]) : 10;
    double array[N];

    int rank, P;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &P);

    if (rank == 0)
    {
        for (int i = 0; i < N; i++)
        {
            array[i] = 0.5 + i * 0.8;
        }
    }

    MPI_Bcast(array, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double sum = 0;
    for (int i = 0; i < N; i++)
    {
        sum += array[i];
    }

    printf("Rank: %d, sum_values: %f\n", rank, sum);

    MPI_Finalize();
    return 0;
}
