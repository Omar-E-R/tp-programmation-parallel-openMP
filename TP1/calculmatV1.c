#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

#define SIZE 2000

int main(int argc, char **argv)
{
	int res = 0, i, j;
	double t, start, stop;
	int *matrice_A;



	// Allocations
	matrice_A = (int *)malloc(SIZE * SIZE * sizeof(int));

	// Initialisations
	for (i = 0; i < SIZE; i++)
	{
		for (j = 0; j < SIZE; j++)
		{
			matrice_A[i * SIZE + j] = 1;
		}
	}
	start = omp_get_wtime();
	for (i = 0; i < SIZE; i++)
	{
		for (j = 0; j < SIZE; j++)
		{
			res += matrice_A[i * SIZE + j];
		}
	}
	stop = omp_get_wtime();
	t = stop - start;
	printf("---Sequentielle---\n");
	printf("Le resultat: %d, temps: %f\n", res,t);
	// Liberations
	free(matrice_A);

	return EXIT_SUCCESS;
}
