#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>

#define count 7

typedef struct Processos
{
	char path[200];
	int prioridade;
} Processos;


int main()
{	
	
	pid_t pid;
	int status;
	
	Processos *proc[count]; //Array de struct de processos
	char url[]="exec.txt",
	     ch1[200], ch2[200], ch3[200];
	FILE *arq;
	char cwd[200];
	char aux[200];
	

	//CRIANDO ARRAY DE PROCESSOS NA MEMORIA COMPARTILHADA
	int shmid;
	int i = 0, j=0;
	key_t key = 12345;


	shmid = shmget(key, count*sizeof(Processos), IPC_CREAT);

	
	proc[0] = (Processos *)shmat(shmid, 0, 0);
	for(i=0;i<count;i++)
	{		
		proc[i] = (Processos*)malloc(count*sizeof(Processos));
	}

	
	//FIM CRIACAO DO ARRAY



   if (getcwd(cwd, sizeof(cwd)) == NULL) {
       perror("getcwd() error");
       return 1;
   } 
	
	//Lendo arquivo com processos
	arq = fopen(url, "r");
	if(arq == NULL)
			printf("Erro, nao foi possivel abrir o arquivo\n");
	else
		while( (fscanf(arq,"%s %s %s\n", &ch1, &ch2, &ch3))!=EOF )
			{
		
				strcpy(aux,cwd);
				strcat(aux, "/");
				strcpy(aux, strcat(aux, ch2));
				strcpy(proc[j]->path, aux);		
				proc[j]->prioridade = atoi(&ch3[11]);; 
				printf("%s %d--\n", proc[j]->path, proc[j]->prioridade);
				
				j++; //Passando para o proximo endereco do array de Processos
				sleep(1);
				//system(cwd);		
				
			}
	
	fclose(arq);
	//Fechando arquivo com processos

	
	if(pid = (fork() != 0)) //Processo pai
	{
		waitpid(pid, &status, 0);
	}
	//ESCALONADOR
	else 
	{
		char aux[200];
		char buffer[20];
		printf("PID = %d \n", getpid());
		
		//CRIANDO PATH COM ARGUMENTO PARA OS PROCESSOS
		strcpy(aux, proc[0]->path);
		strcat(aux, " ");
		sprintf(buffer,"%d",getpid());
		strcat(aux, buffer);
		//printf("%s --\n", aux);
		//FIM DA CRIACAO DO PATH
				
		system(aux);//ABRINDO O PROGRAMA
		exit(1);
	
	}

	
	for(i=0;i<count;i++)
	{
		free(proc[i]);//Liberando malloc dos processos
	}
	shmdt((void *) proc[0]);
	return 0;

}
