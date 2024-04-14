#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

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
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &P);

    int prev_rank = mod(rank - 1, P);
    int next_rank = mod(rank + 1, P);
    int left_rank, forwarded, sum = 0;

    double start, end;
    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();

    MPI_Isend(&rank, 1, MPI_INT, next_rank, next_rank, MPI_COMM_WORLD, &request);

    MPI_Irecv(&left_rank, 1, MPI_INT, prev_rank, rank, MPI_COMM_WORLD, &request);
    MPI_Wait(&request, &status);
    sum += rank + left_rank;

    // forward
    MPI_Isend(&left_rank, 1, MPI_INT, next_rank, next_rank, MPI_COMM_WORLD, &request);

    MPI_Irecv(&forwarded, 1, MPI_INT, prev_rank, rank, MPI_COMM_WORLD, &request);
    MPI_Wait(&request, &status);
    sum += forwarded;

    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();

    printf("Rank: %d, sum: %d\n", rank, sum);
    if (rank == 0)
    {
        printf("TIME: %fms\n", end - start);
    }

    MPI_Finalize();
    return 0;
}
