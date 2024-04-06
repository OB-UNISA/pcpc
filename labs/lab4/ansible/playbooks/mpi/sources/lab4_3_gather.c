#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    int rank, P, msg_integer = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &P);

    if (rank == 0)
    {
        for (int i = 1; i < P; i++)
        {
            MPI_Recv(&msg_integer, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Rank: %d, Message Received from %d.\n", rank, i);
        }
    }

    else
    {
        MPI_Send(&msg_integer, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        printf("Rank: %d, Message Sent.\n", rank);
    }

    MPI_Finalize();
    return 0;
}
