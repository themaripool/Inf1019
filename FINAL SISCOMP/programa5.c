#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>
#include <signal.h>



int main()
{
	
	int i = 0;

	while(i<10)
	{
		//printf("Programa 5 executando\n");
		sleep(2);
		
		i++;
	}

	return 0;

}



