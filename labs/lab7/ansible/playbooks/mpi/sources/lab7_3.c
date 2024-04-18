#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// DOES NOT WORK
int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int N = argc == 2 ? atoi(argv[1]) : 10;

    int rank, P;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    int recv_count = N * P;
    double *recv = NULL, send[N];

    if (rank == 0)
    {
        recv = (double *)malloc(recv_count * sizeof(double));
    }

    for (int i = 0; i < N; i++)
    {
        send[i] = rank;
    }

    MPI_Gather(send, N, MPI_DOUBLE, recv, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        double expected = 0;
        for (int i = 1; i < P; i++)
        {
            expected += i * N;
        }

        double sum = 0;
        for (int i = 0; i < recv_count; i++)
        {
            sum += recv[i];
        }

        printf("Expected sum: %f, recv sum: %f\n", expected, sum);
    }

    MPI_Finalize();
    return 0;
}
