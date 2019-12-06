###########################################################################
# Tapez make test_@_threads en remplacant @ par 2, 4, 8 ou 16 pour        #
# tester le porgramme parallelisé en different nb de threads              #
# Tapez make normal_test  pour tester le prog version parallelisé et      #
#  version seq sur les 3 fichiers de txt pb1, pb2, et pb6                 #
###########################################################################
CC := gcc
CFLAGS :=  -Wall -fopenmp
SOURCE := ELRIFAI_Omar_tp2.c
PROG_PARA := progPara
PROG_SEQ := progSeq

all: normal_test test_2_threads test_4_threads test_8_threads test_16_threads


normal_test:$(PROG_PARA) $(PROG_SEQ)
	@echo "***pb1***"
	@./$(PROG_PARA) pb/pb1.txt
	@echo "***pb1***"
	@./$(PROG_SEQ) pb/pb1.txt
	@echo "***pb2***"
	@./$(PROG_PARA) pb/pb2.txt
	@echo "***pb2***"
	@./$(PROG_SEQ) pb/pb2.txt
	@echo "***pb6***"
	@./$(PROG_PARA) pb/pb6.txt
	@echo "***pb6***"
	@./$(PROG_SEQ) pb/pb6.txt


$(PROG_PARA):  $(SOURCE)
	$(CC)  $(SOURCE) -o $@ $(CFLAGS) -DPARALLEL
$(PROG_SEQ):  $(SOURCE)
	$(CC)  $(SOURCE) -o $@ $(CFLAGS)

#ON FAIT DES DIFFERENTS TESTS SUR pb6 EN CHANGEANT LE NOMBRE DE THREADS(2, 4, 8, 16)
test_2_threads: $(SOURCE)
	@$(CC)  $(SOURCE) -o $@s $(CFLAGS) -DPARALLEL_NUM_THREADS=2
	@echo "***pb6***"
	@./$@s pb/pb6.txt

test_4_threads: $(SOURCE)
	@$(CC)  $(SOURCE) -o $@s $(CFLAGS) -DPARALLEL_NUM_THREADS=4
	@echo "***pb6***"
	@./$@s pb/pb6.txt

test_8_threads: $(SOURCE)
	@$(CC)  $(SOURCE) -o $@s $(CFLAGS) -DPARALLEL_NUM_THREADS=8
	@echo "***pb6***"
	@./$@s pb/pb6.txt

test_16_threads: $(SOURCE)
	@$(CC)  $(SOURCE) -o $@s $(CFLAGS) -DPARALLEL_NUM_THREADS=16
	@echo "***pb6***"
	@./$@s pb/pb6.txt

.PHONY: clean

clean:
	rm -f test_[1-9]*_threadss prog*