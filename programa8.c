#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>



void rotinaDePreempcao(int sinal);

int main()
{
	
	int i = 0;
	
	printf("%d\n" ,getpid());
	while(i<3)
	{
		sleep(1);
		printf("Processo executando 8\n");
		i++;
	}
	
	

	return 0;

}

