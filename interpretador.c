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
	
	Processos *proc[count]; //Array de struct de processos
	char programas[]="exec.txt",
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
		proc[i]->prioridade = 1000;
	}
	//FIM CRIACAO DO ARRAY


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
				strcpy(proc[j]->path, aux); //PATH DO PROGRAMA	
				//strcpy(proc[j]->path, ch2); 
				proc[j]->prioridade = atoi(&ch3[11]); //PRIORIDADE
				printf("%s %d--\n", proc[j]->path, proc[j]->prioridade);
				//execv(proc[j]->path, NULL);				
				
				j++; //Passando para o proximo endereco do array de Processos
				sleep(1);
						
				
			}
	sortPrioridades(proc, count);
	fclose(arq);
	//Fechando arquivo com processos

	
	if(pid = (fork() != 0)) //Processo pai
	{
		waitpid(pid, &status, 0);
	}
	//ESCALONADOR
	else 
	{	
		//FOR
			// COMPARA PID DO EXECUTANDO[0] COM PRONTO[0]
			// SE PRIORIDADE DO EXECUTANDO[0] FOR MAIOR QUE O DO PRONTO[0]
			// DAR SIGSTOP EM EXECUTANDO[0], JOGA EXECUTANDO[0] PARA PRONTO[COUNT-1]
			// E JOGAR PRONTO[0] NO EXECUTANDO[0] E DA SORTPRIORIDADE
			// SIGCONT NO EXECUTANDO[0]->PID
		//END-FOR	

	}

	
	for(i=0;i<count;i++)
	{
		free(proc[i]);//Liberando malloc dos processos
	}
	shmdt((void *) proc[0]);
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




