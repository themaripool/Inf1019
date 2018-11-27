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
	
	
	while(i<10)
	{
		
		i++;
		sleep(1);
	}
	
	

	return 0;

}


