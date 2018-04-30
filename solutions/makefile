all: longsubstr.c helperfn.c
	gcc -Wall -Wextra -Werror -lpthread longsubstr.c -I .
	
clean:
	rm *.out
	
run: a.out
	srun -n1 -T20 ./a.out wiki_dump.txt
