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
	int pid;
} Processos;

void sortPrioridades(Processos *proc[], int cont);


int main()
{	
	
	pid_t pid;
	int status;
	Processos *proc; //Array de struct de processos
	
	
	
	

	//CRIANDO ARRAY DE PROCESSOS NA MEMORIA COMPARTILHADA
	int shmid, shmid2;
	int i = 0;
	int *j;
	key_t key1 = 12345;
	key_t key2 = 33452;//ITERADOR

	shmid2 = shmget (IPC_PRIVATE, sizeof (int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	shmid = shmget(key1, count*sizeof(Processos), IPC_CREAT);

	j = (int *)shmat(shmid2, 0, 0);
	*j = 0;
	
	proc = (Processos *)shmat(shmid, 0, 0);
	//proc = (Processos*)malloc(count*sizeof(Processos));
	for(i=0;i<count;i++)
	{		
		
		proc[i].prioridade = 1000;
	}
	//FIM CRIACAO DO ARRAY

	

	
	
	

	
	if(pid = (fork() != 0)) //Processo pai
	{
		FILE *arq;
		char programas[]="exec.txt",
	    ch1[200], ch2[200], ch3[200];
		char cwd[200];
		char aux[200];
		if (getcwd(cwd, sizeof(cwd)) == NULL) {
		   perror("getcwd() error");
		   return 1;
	   	} 
		//Lendo arquivo com processos
		arq = fopen(programas, "r");
		if(arq == NULL)
				printf("Erro, nao foi possivel abrir o arquivo\n");
		else
			while( (fscanf(arq,"%s %s %s\n", &ch1, &ch2, &ch3))!=EOF )
				{
					
					strcpy(aux,cwd);
					strcat(aux, "/");
					strcpy(aux, strcat(aux, ch2));
					strcpy(proc[*j].path, aux); //PATH DO PROGRAMA	
					//strcpy(proc[j]->path, ch2); 
					proc[*j].prioridade = atoi(&ch3[11]); //PRIORIDADE
					printf("%s %d--\n", proc[*j].path, proc[*j].prioridade);
					//execv(proc[j]->path, NULL);				
					sleep(1);
					*j = *j + 1; //Passando para o proximo endereco do array de Processos
						
				}
	
		fclose(arq);
		//Fechando arquivo com processos
		
		waitpid(pid, &status, 0);
	}
	//ESCALONADOR
	else 
	{	
		printf("PID-%d\n", getpid());
		while(*j<count)
		{
			printf("%s --\n", proc[*j].path);
			execv(proc[*j].path, NULL);
			sleep(1);
		}




		//FOR
			// COMPARA PID DO EXECUTANDO[0] COM PRONTO[0]
			// SE PRIORIDADE DO EXECUTANDO[0] FOR MAIOR QUE O DO PRONTO[0]
			// DAR SIGSTOP EM EXECUTANDO[0], JOGA EXECUTANDO[0] PARA PRONTO[COUNT-1]
			// E JOGAR PRONTO[0] NO EXECUTANDO[0] E DA SORTPRIORIDADE
			// SIGCONT NO EXECUTANDO[0]->PID
		//END-FOR	
	}

	
	
	shmdt((void *) proc);
	return 0;

}



void sortPrioridades(Processos *proc[], int cont)
{
	int i, j, aux;
	
	for (j = 1; j < cont; j++) {
   		for (i = 0; i < cont - 1; i++) {
     		if (proc[i]->prioridade > proc[i + 1]->prioridade) {
			   aux = proc[i]->prioridade;
			   proc[i]->prioridade = proc[i + 1]->prioridade;
			   proc[i + 1]->prioridade = aux;
     		}
   		}
	 }

	/*for (i = 0; i < cont; i++) {
 		printf("%d\n", proc[i]->prioridade);
	}*/
	
}






