#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>
#include <signal.h>

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
	int *pid_esc;
	int status;
	Processos *proc; //Array de struct de processos

	//CRIANDO ARRAY DE PROCESSOS NA MEMORIA COMPARTILHADA
	int shmid, shmid2, shmidpid;
	int i = 0;
	int *j;
	key_t key1 = 12345;

	shmid2 = shmget (IPC_PRIVATE, 2 *sizeof (int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	shmidpid = shmget (IPC_PRIVATE, sizeof (int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	shmid = shmget(key1, count*sizeof(Processos), IPC_CREAT);

	j = (int *)shmat(shmid2, 0, 0);
	j[0] = 0;
	j[1] = 0;
	pid_esc = (int *)shmat(shmidpid, 0, 0);//Local para pegar pid do escalonador
	
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
					strcpy(proc[j[0]].path, aux); //PATH DO PROGRAMA	
					proc[j[0]].prioridade = atoi(&ch3[11]); //PRIORIDADE
					//printf("%s %d--\n", proc[j[0]].path, proc[j[0]].prioridade);			
										
					sleep(1);
					j[0]++;
					kill(*pid_esc, SIGCONT);
					 //Passando para o proximo endereco do array de Processos
					}
	
		fclose(arq);
		//Fechando arquivo com processos
		
	}
	else 
	{	
		printf("PID-%d\n", getpid());
		
		
		
		while(1==1)
		{
			if(j[1] < count)
			{				
				if(pid = (fork() != 0))
				{
					*pid_esc = getpid();
					kill(getpid(), SIGSTOP);	
					 	
				}

				else //INICIANDO PROCESSOS
				{
					proc[j[1]].pid = getpid();
					execv(proc[j[1]].path, NULL);
				}
			}
			// CODIGO DO ESCALONADOR VAI AQUI
			kill(proc[j[1]].pid, SIGCONT);
			j[1]++;	
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
	shmdt((void *) j);
	shmdt((void *) pid_esc);
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






