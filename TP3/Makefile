test_para: pnm.c
	gcc -Wall pnm.c -o pnm -fopenmp -DPARALLEL=2
	./pnm
test_seq: pnm.c
	gcc -Wall pnm.c -o pnm -fopenmp
	./pnm
