/*
 * Omar EL RIFAI
 * TP2 - Sac a dos
 * Pour tester avec la version parallelise avec le nb de threads par defaut,
 * il faut definir la constante lors de la compilation grace a l'option -D :
 * gcc ELRIFAI_Omar_tp2.c -otest -DPARALLEL -fopenmp -Wall
 * Rappel : Vous pouvez toujours tapez la commande ci-dessous au terminal:
 * Pour executer le prog(parallelise et sequentiel) sur tous les fichiers: make normal_test
 * Pour executer le prog(parallelise en 2 threads) sur le fichier pb6: make test_2_threads
 * Pour executer le prog(parallelise en 4 threads) sur le fichier pb6: make test_4_threads
 * Pour executer le prog(parallelise en 8 threads) sur le fichier pb6: make test_8_threads
 * Pour executer le prog(parallelise en 16 threads) sur le fichier pb6: make test_16_threads
 * Pour faire tous les tests: make
 */

#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

#define MAX_NUM_OBJ 100

int num_obj = 0;
int capacity;
int weight[MAX_NUM_OBJ];
int utility[MAX_NUM_OBJ];

//Variables globales partagee
int *sac, *E;


void read_problem(char *filename){
	char line[256];

	FILE *problem = fopen(filename, "r");
	if (problem == NULL){
		fprintf(stderr, "File %s not found.\n", filename);
		exit(-1);
	}

	while (fgets(line, 256, problem) != NULL){
		switch (line[0]){
		case 'c': // capacity
			if (sscanf(&(line[2]), "%d\n", &capacity) != 1){
				fprintf(stderr, "Error in file format in line:\n");
				fprintf(stderr, "%s", line);
				exit(-1);
			}
			break;

		case 'o': // graph size
			if (num_obj >= MAX_NUM_OBJ){
				fprintf(stderr, "Too many objects (%d): limit is %d\n", num_obj, MAX_NUM_OBJ);
				exit(-1);
			}
			if (sscanf(&(line[2]), "%d %d\n", &(weight[num_obj]), &(utility[num_obj])) != 2){
				fprintf(stderr, "Error in file format in line:\n");
				fprintf(stderr, "%s", line);
				exit(-1);
			}
			else
				num_obj++;
			break;

		default:
			break;
		}
	}
	if (num_obj == 0){
		fprintf(stderr, "Could not find any object in the problem file. Exiting.");
		exit(-1);
	}
}

/*Elle trouvre si il faut inclure L'objet i, et elle renvoit l'utilit� ajout� en tout cas*/
int find_max(int index, int j, int masse){
	int var1, var2;

	var1 = sac[(index - 1) * capacity + j];
	var2 = utility[index] + sac[(index - 1) * capacity + j-masse];

	return var1 < var2 ? var2 : var1;

}

int main(int argc, char* argv[]){
	//Variables pour calculer les durees des differents executions
	double t1, t2, t3, start, stop, start_tab[MAX_NUM_OBJ], stop_tab[MAX_NUM_OBJ];
	if (argc != 2){
		printf("Usage : %s <file>\n", argv[0]);
		exit(1);
	}
#ifdef PARALLEL
	printf("---PARALLELE---\n");
#elif PARALLEL_NUM_THREADS
	printf("---PARALLELE %d THREADS---\n", PARALLEL_NUM_THREADS);
#else
	printf("---SEQUENTIELLE---\n");
#endif
	//Lire le fichier donner par argv[1] et compter son temps d'execution
	start = omp_get_wtime();
	read_problem(argv[1]);
	stop = omp_get_wtime();
	t1 = stop - start;
	//Afficher le num d'objet et la capacite max du sac
	printf("num_obj=%d\n", num_obj);
	printf("capacity=%d\n", capacity);
	//Incrementer la variable capacity une fois pour toute, au lieu de l'incrementer � chaque utilisation
	capacity++;

	//Allocation de memoire vierge (contenant que des Zeros)
	sac=(int*)calloc(num_obj*capacity, sizeof(int));
	E=(int*)calloc(num_obj, sizeof(int));

	//Calcul les lignes de Sac une par une commen�ant par la premiere
	for (int i = 0; i < num_obj; i++){
	start_tab[i]=omp_get_wtime();//on enregistre le temps de debut de calcul de chaque ligne
#ifdef PARALLEL//Si on ne precise pas le Nb de Threads (Nb par defaut=> OMP_NUM_THREADS) qui depend de la machine
#pragma omp parallel for
#elif PARALLEL_NUM_THREADS//Si on precise le Nb de Threads
#pragma omp parallel for num_threads(PARALLEL_NUM_THREADS)
#endif
		for (int j = 0; j < capacity; j++){//la partie effectuee par chacune des threads
			if (i == 0){
				if (weight[i] <= j){
					sac[0 + j] = utility[0];
				}
				else{
					sac[0 + j] = 0;
				}
			}
			else{
				if (weight[i] <= j){
					sac[i * capacity + j] = find_max(i, j, weight[i]);
				}
				else{
					sac[i * capacity + j] = sac[(i - 1) * capacity + j];
				}
			}
		}
		stop_tab[i] = omp_get_wtime(); //on enregistre le temps de fin de calcul de chaque ligne
	}
	/* CALCUL DE LA MOYENNE DES TEMPS POUR CALCULER UNE LIGNE*/
	t2=0;
	for (int i = 0; i < num_obj; i++){
		t2+=stop_tab[i]-start_tab[i];
	}
	t2/=num_obj;

	/* ON RECHERCHE LE TABLEAU E DES OBJETS INCLUS -non parallelisable-*/
	int i = num_obj - 1;
	int j = capacity - 1;

	start = omp_get_wtime();
	while(i>0){
		if (sac[i * capacity + j] > sac[(i - 1) * capacity + j]){//Si Objet i est inclus
			E[i] = 1;
			j -= weight[i];

		}
		else{//Si Objet i n'est pas inclus
			E[i] = 0;
		}
		i--;//on monte dans le sac
	}
	if (sac[j]!=0){//Si le premier objet est inclus
		E[0]=1;
	}else{
		E[0]=0;
	}
	stop = omp_get_wtime();
	t3 = stop - start;//Fin de l'algo de recherche d'objet ajoutes

	/*AFFICHAGE DE L'UTILITE MAX, LE TABLEAU E ET DES TEMPS D'EXECUTIONS*/
	printf("Utilité maximale: %d\n", sac[(num_obj-1)*capacity + capacity-1]);//Utilit� max est sac[Num_obj-1][Capacity]
	printf("E: ");
	for (int i = 0; i < num_obj; i++){// Affichage de tableau des objet ajoutes
		if(E[i]==1)
			printf("%d ",i);
	}
	printf("\n");
	printf("time(read_problem)= %f\n", t1);//temps necessaire pour lire les fichiers txt
	printf("time(calcul Une Ligne)= %f\n", t2); //moyenne de temps necessaire pour calculer une ligne de S
	printf("time(recherche Obj)= %f\n", t3);//temps necessaire pour rechercher les objets ajoutes
	printf("\n");
	printf("\n");
	//liberer le memoire alloue
	free(sac);
	free(E);
	return 0;
}