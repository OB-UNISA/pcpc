#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int N = argc == 2 ? atoi(argv[1]) : 10;

    int rank, P;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    int recv_count = N * P;
    double recv[N], *send;

    if (rank == 0)
    {
        send = (double *)malloc(recv_count * sizeof(double));
        for (int i = 0; i < P; i++)
        {
            for (int j = 0; j < N; j++)
            {
                send[j + i * N] = i;
            }
        }
    }

    MPI_Scatter(send, N, MPI_DOUBLE, recv, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double expected = rank * N, sum = 0;
    for (int i = 0; i < N; i++)
    {
        sum += recv[i];
    }

    printf("Rank: %d, Expected sum: %f, recv sum: %f\n", rank, expected, sum);

    MPI_Finalize();
    return 0;
}
