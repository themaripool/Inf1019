#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>

int main()
{
	int i = 0;
	
	printf("%d\n" ,getpid());
	kill(getpid(), SIGSTOP);
	
	while(i<30)
	{
		sleep(1);
		printf("Processo executando 2\n");
		i++;
	}
	
	


	return 0;

}


