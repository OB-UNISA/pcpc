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

typedef void(*mycollective(int, int, void *, int, MPI_Datatype, MPI_Comm));
typedef struct BenchRes
{
    double blocking;
    double non_blocking;
    double diff;
} BenchRes;

// ignore this warning because the returned value of mycollective is not used
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
BenchRes benchmark(int P, int rank, int array[], int N, mycollective blocking, mycollective non_blocking)
{
    double start, end;
    double t_blocking, t_non_blocking, t_diff;

    // blocking
    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();

    (*blocking)(P, rank, array, N, MPI_INT, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();
    t_blocking = end - start;

    // not blocking
    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();

    (*non_blocking)(P, rank, array, N, MPI_INT, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();
    t_non_blocking = end - start;
    t_diff = t_blocking - t_non_blocking;

    return ((BenchRes){t_blocking, t_non_blocking, t_diff});
}

void wrap_min(int P, int rank, int array[], int N, MPI_Datatype data_type, MPI_Comm comm)
{
    min(P, rank, array, N, comm);
}

void wrap_i_min(int P, int rank, int array[], int N, MPI_Datatype data_type, MPI_Comm comm)
{
    i_min(P, rank, array, N, comm);
}

void wrap_max(int P, int rank, int array[], int N, MPI_Datatype data_type, MPI_Comm comm)
{
    max(P, rank, array, N, comm);
}

void wrap_i_max(int P, int rank, int array[], int N, MPI_Datatype data_type, MPI_Comm comm)
{
    i_max(P, rank, array, N, comm);
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int N = argc == 2 ? atoi(argv[1]) : 1000000;

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

    double start, end;
    double t_blocking, t_non_blocking, t_diff;

    BenchRes broadcast_res = benchmark(P, rank, array, N, broadcast, i_broadcast);
    BenchRes gather_res = benchmark(P, rank, array, N, gather, i_gather);
    BenchRes scatter_res = benchmark(P, rank, array, N, scatter, i_scatter);
    BenchRes min_res = benchmark(P, rank, array, N, wrap_min, wrap_i_min);
    BenchRes max_res = benchmark(P, rank, array, N, wrap_max, wrap_i_max);

    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0)
    {
        printf("N: %d, P: %d\n", N, P);
        printf("[BROADCAST] Blocking: %f ms, Non-Blocking: %f ms, Diff: %f ms\n", broadcast_res.blocking, broadcast_res.non_blocking, broadcast_res.diff);
        printf("[GATHER]    Blocking: %f ms, Non-Blocking: %f ms, Diff: %f ms\n", gather_res.blocking, gather_res.non_blocking, gather_res.diff);
        printf("[SCATTER]   Blocking: %f ms, Non-Blocking: %f ms, Diff: %f ms\n", scatter_res.blocking, scatter_res.non_blocking, scatter_res.diff);
        printf("[MIN]       Blocking: %f ms, Non-Blocking: %f ms, Diff: %f ms\n", min_res.blocking, min_res.non_blocking, min_res.diff);
        printf("[MAX]       Blocking: %f ms, Non-Blocking: %f ms, Diff: %f ms\n", max_res.blocking, max_res.non_blocking, max_res.diff);
    }
    MPI_Finalize();
    return 0;
}
