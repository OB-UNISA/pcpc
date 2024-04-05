#include <stdio.h>
#include <mpi.h>

#define N 30

void print_array(int *a, int n)
{
    printf("[");
    for (int i = 0; i < n; i++)
    {
        printf("%d ", a[i]);
    }
    printf("]");
}

int main(int argc, char **argv)
{
    int rank, P;
    int array[N];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &P);

    int leftover = N % (P - 1);
    int num_items = N / (P - 1);

    if (N < (P - 1))
    {
        fprintf(stderr, "N has to be grater than P\n");
        return 1;
    }

    if (rank == 0)
    {
        for (int i = 0; i < N; i++)
        {
            array[i] = i;
        }
        int *p = array;

        for (int i = 1; i < P; i++)
        {
            int count = i - 1 < leftover ? num_items + 1 : num_items;
            MPI_Send(p, count, MPI_INT, i, i, MPI_COMM_WORLD);
            printf("Rank: %d, Message Sent to: %d. Sent %d items\n", rank, i, count);
            p += count;
        }
    }

    else
    {
        int count = rank - 1 < leftover ? num_items + 1 : num_items;
        MPI_Recv(array, count, MPI_INT, 0, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Rank: %d, Message Received. Content: ", rank);
        print_array(array, count);
        printf("\n");
    }

    MPI_Finalize();
    return 0;
}
