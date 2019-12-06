#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

#define SIZE 2000
#define OMP_NUM_THREADS 4

int main(int argc, char **argv)
{
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

	int *tab;
	tab=calloc(OMP_NUM_THREADS, sizeof(int));
	start = omp_get_wtime();
#pragma omp parallel
	{
		int index = omp_get_thread_num();
		#pragma omp for
			for (int i = 0; i < SIZE; i++){
				for (int j = 0; j < SIZE; j++){
					tab[index] += matrice_A[i * SIZE + j];
			}
		}

	}

	int res = 0;
	for (int j = 0; j < 4; j++)
	{
		res += tab[j];
	}
	stop = omp_get_wtime();
	t = stop - start;
	printf("---Somme des res partielles---\n");
	printf("Le resultat: %d, temps: %f\n", res, t);
	// Liberations
	free(matrice_A);
	free(tab);
	return EXIT_SUCCESS;
}
