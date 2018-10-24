#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>
#include <signal.h>
#include <sys/sem.h>

#define count 7 
#define TRUE 1
#define FALSE 0



typedef struct Processos
{
	char path[100];
	int prioridade;
	int pid;
	struct Processos *prox;
	
} Processos;

Processos *executando, *aux;

Processos* buscaProxIgual(Processos *l, int prioridade);

int removeProcLista(Processos *l, int pid);

int addLista(Processos proc, Processos *l);

void initLista(Processos *l);

int deletaLista(Processos *l);

int criaProcesso (Processos *executando);

void signalhandler(int signal);

union semun
{
	int val;
	struct semid_ds *buf;
	ushort *array;
};
// inicializa o valor do semáforo
int setSemValue(int semId);
// remove o semáforo
void delSemValue(int semId);
// operação P
int semaforoP(int semId);
//operação V
int semaforoV(int semId);


int main()
{		
	pid_t pid;
	int status;
	int semId;
	
	Processos *proc; //Array de struct de processos
	
	int *j;
	

	//CRIANDO ARRAY DE PROCESSOS NA MEMORIA COMPARTILHADA
	int shmid, shmid2, shmidexec, shmidaux;
	int i = 0;

 	semId = semget (IPC_PRIVATE, 1, 0666 | IPC_CREAT);
   	setSemValue(semId);
    sleep (2);
	
	shmid2 = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	shmid = shmget(IPC_PRIVATE, count*sizeof(Processos), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	shmidexec = shmget(IPC_PRIVATE, sizeof(Processos), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	shmidaux = shmget(IPC_PRIVATE, sizeof(Processos), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);


	executando = (Processos *)shmat(shmidexec, 0, 0);//Local para pegar pid do escalonador
	aux = (Processos *)shmat(shmidaux, 0, 0);
	proc = (Processos *)shmat(shmid, 0, 0);

	j = (int *)shmat(shmid2, 0, 0);
	*j = 0;
	
	executando[i].pid = -1;
	executando[i].prioridade = 100;
	strcpy(executando[i].path, " ");
	
	for(i=0;i<count;i++)
	{		
		proc[i].pid = -1;
		proc[i].prioridade = 100;
		strcpy(proc[i].path, " ");
	}
	//FIM CRIACAO DO ARRAY

	
	if(pid = (fork() != 0)) //Processo pai
	{
		FILE *arq;
		char programas[]="exec.txt",
	    ch1[100], ch2[100], ch3[100];
		char cwd[100];
		char aux[100];
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
					proc[*j].prioridade = atoi(&ch3[11]); //PRIORIDADE
					//printf("%s %d--\n", proc[*j].path, proc[*j].prioridade);			
					
					sleep(1);
					semaforoV(semId);
					*j = *j + 1;
					}
	
		fclose(arq);
		//Fechando arquivo com processos
		waitpid(-1, &status, 0);
		
	}
	else 
	{	
		signal(SIGUSR1, signalhandler);
		Processos *prio1, *prio2, *prio3, *prio4, *prio5, *prio6, *prio7;
		
		prio1 = (Processos *) malloc(sizeof(Processos));
		prio2 = (Processos *) malloc(sizeof(Processos));
		prio3 = (Processos *) malloc(sizeof(Processos));
		prio4 = (Processos *) malloc(sizeof(Processos));
		prio5 = (Processos *) malloc(sizeof(Processos));
		prio6 = (Processos *) malloc(sizeof(Processos));
		prio7 = (Processos *) malloc(sizeof(Processos));
		initLista(prio1);
		initLista(prio2);
		initLista(prio3);
		initLista(prio4);
		initLista(prio5);
		initLista(prio6);
		initLista(prio7);
		
		printf("PID-%d\n", getpid());
		i=0;
		while(i<count)
		{							
			semaforoP(semId);
			
			if(proc[i].prioridade == 1)//LISTAS DE PRIORIDADE
			{
				addLista(proc[i], prio1);
				prio1->pid = criaProcesso(prio1);
				aux->pid = prio1->pid;
				strcpy(aix->path, prio1->path);
				aux->prioridade = prio1->prioridade;				
				kill(getpid(), SIGUSR1);
				
			}
			
			else if(proc[i].prioridade == 2)
			{
				addLista(proc[i], prio2);
				prio2->pid = criaProcesso(prio2);
				aux->pid = prio2->pid;
				strcpy(aix->path, prio2->path);
				aux->prioridade = prio2->prioridade;
				kill(getpid(), SIGUSR1);
			}
			
			else if(proc[i].prioridade == 3)
			{
				addLista(proc[i], prio3);
				prio3->pid = criaProcesso(prio3);
				aux->pid = prio3->pid;
				strcpy(aix->path, prio3->path);
				aux->prioridade = prio3->prioridade;
				kill(getpid(), SIGUSR1);
			}
			
			else if(proc[i].prioridade == 4)
			{
				addLista(proc[i], prio4);
				prio4->pid = criaProcesso(prio4);
				aux->pid = prio4->pid;
				strcpy(aix->path, prio4->path);
				aux->prioridade = prio4->prioridade;
				kill(getpid(), SIGUSR1);
			}
			
			else if(proc[i].prioridade == 5)
			{
				addLista(proc[i], prio5);
				prio5->pid = criaProcesso(prio5);
				aux->pid = prio5->pid;
				strcpy(aix->path, prio5->path);
				aux->prioridade = prio5->prioridade;				
				kill(getpid(), SIGUSR1);
			}
			
			else if(proc[i].prioridade == 6)//ROUND-ROBIN TALVEZ SIGUSR2
			{
				addLista(proc[i], prio6);
				prio6->pid = criaProcesso(prio6);
				aux->pid = prio6->pid;
				strcpy(aix->path, prio6->path);
				aux->prioridade = prio6->prioridade;				
				kill(getpid(), SIGUSR1);
			}
			
			else if(proc[i].prioridade == 7)
			{
				addLista(proc[i], prio7);
				prio7->pid = criaProcesso(prio7);
				aux->pid = prio7->pid;
				strcpy(aix->path, prio7->path);
				aux->prioridade = prio7->prioridade;				
				kill(getpid(), SIGUSR1);
			}
			
			
			
			i++;
		}
		
		executando->pid = criaProcesso(prio1);
		kill(executando->pid , SIGCONT);
		sleep(2);
		kill(getpid(), SIGUSR1);
		for(i=0;i<count;i++)
		{
			waitpid(-1, &status, 0);			
		}
	
		
	free(prio1);
	free(prio2);
	free(prio3);
	free(prio4);
	free(prio5);
	free(prio6);
	free(prio7);	
	}

	
	
	shmdt((void *) executando);
	shmdt((void *) proc);
	return 0;

}


int setSemValue(int semId)
{
	union semun semUnion;
	semUnion.val = 0;
	return semctl(semId, 0, SETVAL, semUnion);
}
void delSemValue(int semId)
{
	union semun semUnion;
	semctl(semId, 0, IPC_RMID, semUnion);
}
int semaforoP(int semId)
{
	struct sembuf semB;
	semB.sem_num = 0;
	semB.sem_op = -1;
	semB.sem_flg = SEM_UNDO;
	semop(semId, &semB, 1);
	return 0;
}
int semaforoV(int semId)
{
	struct sembuf semB;
	semB.sem_num = 0;
	semB.sem_op = 1;
	semB.sem_flg = SEM_UNDO;
	semop(semId, &semB, 1);
	return 0;
}

int criaProcesso (Processos *executando) 
{
	int pid;
	pid = fork(); /* Executa o fork */
  
	/*if(executando->pid != -1)
	{	
		kill(executando->pid, SIGSTOP);
		proc[count-1].prioridade = executando->prioridade;
		strcpy(proc[count-1].path, executando->path);
		proc[count-1].pid = executando->pid;
  	}
	strcpy(executando->path, proc[0].path); 
	executando->prioridade = proc[0].prioridade;
	*/

	if (pid < 0) {
	return -1;
	}
	else if ( pid != 0 ) { /* PAI */
		kill(pid, SIGSTOP);
	/* Pausa o filho */

	return pid;
	} 

	else { /* FILHO */
		execv(executando->path, NULL);
	}
	return 0;
}	

void initLista(Processos *l)
{
	l->prox = NULL;

}	

int addLista(Processos proc, Processos *l)
{
	
	Processos *aux;
	Processos *novo = (Processos*)malloc(sizeof(Processos));
	if(novo==NULL)                     
    {
        printf("Memoria nao alocada");
        return -1;
    }
    
    novo->pid = proc.pid;
    novo->prioridade = proc.prioridade;
    strcpy(novo->path, proc.path);

	
	if(l->prox == NULL)//Lista esta vazia
	{
		l->pid = proc.pid;
		l->prioridade = proc.prioridade;
		l->prox = l;
		strcpy(l->path, proc.path);
	}
	
	else
	{
		aux->pid = l->pid;
		aux->prioridade = l->prioridade;
		aux->prox = l->prox;
		strcpy(aux->path, l->path);
		
		l->pid = proc.pid;
		l->prioridade = proc.prioridade;
		strcpy(l->path, proc.path);
		l->prox = aux->prox;
		aux->prox = l;
		
		
	}
	
	return 0;
}

int removeProcLista(Processos *l, int pid)
{
	int i;
	Processos *aux;
	for(i=0;i<count;i++)
	{
		if(l->prox->pid == pid)
		{
			aux = l->prox->prox;
			free(l->prox);
			l->prox = aux;
			return 0;
		}
		else
		{
			l = l->prox; 
		}
	}
	printf("%d --\n", l->pid);
	printf("PID Nao encontrado\n");
	return -1;
}

int deletaLista(Processos *l)
{
	Processos *aux;
	while(l->prox != NULL)
	{
		aux = l;
		l = l->prox;
		free(aux);
	}
	return 0;
}

/*Processos* buscaProxIgual(Processos *l, int prioridade)
{
	int i;
	for(i=0; i<count; i++)
	{
		if(l->prioridade == prioridade)
			return l;
		
		else
		{
			printf("%d", l->prioridade;);
			l = l->prox;		
		}
	}
	
	printf("Nao achou processo na buscaProxIgual");
}*/


void signalhandler(int signal)
{
	if(signal == SIGUSR1)
	{
		if(aux->prioridade < executando->prioridade)//FALTA SABER QUANDO O CARA ACABA
		{	
			kill(executando->pid, SIGSTOP);
			executando->pid = aux->pid;
			strcpy(executando->path, aux->path);
			executando->prioridade = aux->prioridade;
			kill(executando->pid, SIGCONT);
		}
		
	}
}




