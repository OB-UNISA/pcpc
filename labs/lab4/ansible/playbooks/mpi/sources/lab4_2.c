#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

int main(int argc, char **argv)
{
    int rank, len = 0, st;
    char *str;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        char c;
        str = malloc(1 * sizeof(char));
        for (; (c = getchar()) != '\n' && c != EOF; len++)
        {
            str = realloc(str, sizeof(char) * (len + 1));
            str[len] = c;
        }
        str = realloc(str, sizeof(char) * len);
        str[len] = '\0';

        MPI_Send(str, len, MPI_CHAR, 1, 1, MPI_COMM_WORLD);
        printf("Rank: %d, Message Sent: %s\n", rank, str);
    }

    else if (rank == 1)
    {
        MPI_Status status;
        MPI_Probe(0, 1, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_CHAR, &len);
        printf("Rank: %d, Message Len: %d\n", rank, len);
        str = malloc(len * sizeof(char));

        MPI_Recv(str, len, MPI_CHAR, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Rank: %d, Message Received: %s\n", rank, str);
    }

    MPI_Finalize();
    return 0;
}
