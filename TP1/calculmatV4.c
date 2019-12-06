#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>
#define SIZE 2000

int main(int argc, char **argv)
{
	int res = 0;

	int *matrice_A;
	double t, start, stop;
	// Allocations
	matrice_A = (int *)malloc(SIZE * SIZE * sizeof(int));

	// Initialisations
	for (int i = 0; i < SIZE; i++){
		for (int j = 0; j < SIZE; j++){
			matrice_A[i * SIZE + j] = 1;
		}
	}
	start = omp_get_wtime();
#pragma omp parallel for reduction (+:res)
		for (int i = 0; i < SIZE; i++){
			for (int j = 0; j < SIZE; j++){
				res += matrice_A[i * SIZE + j];
			}
		}

	stop = omp_get_wtime();
	t = stop - start;
	printf("---Reduction---\n");
	printf("Le resultat: %d, temps: %f\n", res, t);
	// Liberations
	free(matrice_A);

	return EXIT_SUCCESS;
}
