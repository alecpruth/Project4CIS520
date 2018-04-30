all: 3way-pthread.c
	gcc -lpthread 3way-pthread.c -o pthreads
run: Pthread
	./pthreads
schedule: 
	srun -N4 ./pthreads
