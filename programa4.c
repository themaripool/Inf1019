#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>
#include <signal.h>

void rotinaDePreempcao(int sinal);

int main()
{
	

	signal(SIGSTOP, rotinaDePreempcao);
	signal(SIGCONT, rotinaDePreempcao);
	
	
	
	printf("%d\n" ,getpid());
	
	while(1==1)
	{
		sleep(1);
		printf("Processo 3 executando\n");
	}
	
	
	


	return 0;

}


void rotinaDePreempcao(int sinal)
{
    switch(sinal)
    {
   	 case SIGSTOP:
		printf("Processo 3 interrompido");
		pause();
		break;
   	 
   	 case SIGCONT:
		printf("Processo 3 executando");
		break;
   	}

}
