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
#define timeslice 2




typedef struct Processos
{
	char path[100];
	int prioridade;
	int pid;
	int contador;
	struct Processos *prox;
	
} Processos;

Processos *executando, *aux;

Processos *achaUltimo(Processos *l);

int removeProcLista(Processos *l);

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
	
	executando->pid = -1;
	executando->prioridade = 100;
	strcpy(executando->path, " ");
	executando->contador = 0;
	
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
		//Lendo arquivo com processos
		arq = fopen(programas, "r");
		if(arq == NULL)
				printf("Erro, nao foi possivel abrir o arquivo\n");
		else
			while( (fscanf(arq,"%s %s %s\n", &ch1, &ch2, &ch3))!=EOF )
				{
					
					strcpy(proc[*j].path, ch2); //PATH DO PROGRAMA	
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
		signal(SIGUSR2, signalhandler);
		Processos *prio1, *prio2, *prio3, *prio4, *prio5, *prio6, *prio7, *nav;
		
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
		while(i<count)//Processos ainda vindo do interpretador
		{		
			sleep(timeslice);					
			semaforoP(semId);
			if(proc[i].prioridade == 1)//LISTAS DE PRIORIDADE
			{
				addLista(proc[i], prio1);
				prio1->contador = executando->contador;
				nav = achaUltimo(prio1);// Acha o ultimo processo que foi adicionado	
				nav->pid = criaProcesso(nav);
				aux->pid = nav->pid;
				strcpy(aux->path, nav->path);
				aux->prioridade = nav->prioridade;
				aux->contador = nav->contador;			
				kill(getpid(), SIGUSR1);
				
			}
			
			else if(proc[i].prioridade == 2)
			{
				addLista(proc[i], prio2);
				prio2->contador = executando->contador;
				nav = achaUltimo(prio2);// Acha o ultimo processo que foi adicionado	
				nav->pid = criaProcesso(nav);
				aux->pid = nav->pid;
				strcpy(aux->path, nav->path);
				aux->prioridade = nav->prioridade;
				aux->contador = nav->contador;			
				kill(getpid(), SIGUSR1);
			}
			
			else if(proc[i].prioridade == 3)
			{
				addLista(proc[i], prio3);
				prio3->contador = executando->contador;
				nav = achaUltimo(prio3);// Acha o ultimo processo que foi adicionado	
				nav->pid = criaProcesso(nav);
				aux->pid = nav->pid;
				strcpy(aux->path, nav->path);
				aux->prioridade = nav->prioridade;
				aux->contador = nav->contador;			
				kill(getpid(), SIGUSR1);
			}
			
			else if(proc[i].prioridade == 4)
			{
				addLista(proc[i], prio4);
				prio4->contador = executando->contador;
				nav = achaUltimo(prio4);// Acha o ultimo processo que foi adicionado	
				nav->pid = criaProcesso(nav);
				aux->pid = nav->pid;
				strcpy(aux->path, nav->path);
				aux->prioridade = nav->prioridade;
				aux->contador = nav->contador;			
				kill(getpid(), SIGUSR1);
			}
			
			else if(proc[i].prioridade == 5)
			{
				addLista(proc[i], prio5);
				prio5->contador = executando->contador;
				nav = achaUltimo(prio5);// Acha o ultimo processo que foi adicionado	
				nav->pid = criaProcesso(nav);
				aux->pid = nav->pid;
				strcpy(aux->path, nav->path);
				aux->prioridade = nav->prioridade;
				aux->contador = nav->contador;			
				kill(getpid(), SIGUSR1);
			}
			
			else if(proc[i].prioridade == 6)//ROUND-ROBIN TALVEZ SIGUSR2
			{	
				addLista(proc[i], prio6);
				prio6->contador = executando->contador;
				nav = achaUltimo(prio6);// Acha o ultimo processo que foi adicionado	
				nav->pid = criaProcesso(nav);
				aux->pid = nav->pid;
				strcpy(aux->path, nav->path);
				aux->prioridade = nav->prioridade;
				aux->contador = nav->contador;			
				kill(getpid(), SIGUSR1);
			}
			
			else if(proc[i].prioridade == 7)
			{
				addLista(proc[i], prio7);
				prio7->contador = executando->contador;
				nav = achaUltimo(prio7);// Acha o ultimo processo que foi adicionado	
				nav->pid = criaProcesso(nav);
				aux->pid = nav->pid;
				strcpy(aux->path, nav->path);
				aux->prioridade = nav->prioridade;
				aux->contador = nav->contador;			
				kill(getpid(), SIGUSR1);
			}
			i++;
		}
		
		waitpid(-1, &status, 0);			
		
		
		while(1)
		{
			sleep(timeslice);
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

//-----------------------------------------------------------------------------------------------
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
	
	Processos *aux = l;
	Processos *novo = (Processos*)malloc(sizeof(Processos));
	if(novo==NULL)                     
    {
        printf("Memoria nao alocada");
        return -1;
    }
    
    novo->pid = proc.pid;
    novo->prioridade = proc.prioridade;
    strcpy(novo->path, proc.path);
    novo->contador = 0;

	
	if(l->prox == NULL)//Lista esta vazia
	{
		l->pid = proc.pid;
		l->prioridade = proc.prioridade;
		l->prox = l;
		l->contador = 0;
		strcpy(l->path, proc.path);
		free(novo);
	}
	
	else
	{
		
		while(aux->prox != l) //Quero o final da lista circular
		{
			aux = aux->prox;
		}
		novo->prox = aux->prox;
		aux->prox = novo;
				
		
	}
	
	return 0;
}

int removeProcLista(Processos *l)//Nao funciona
{
	Processos *aux, *nav = l;
	if((l->prox == l) && (l->contador == 3))//So tem 1 iten na lista
	{	
		l->prox = NULL;
	}
	while(nav->prox != l)
	{
		if(nav->prox->contador == 3)
		{
			aux = nav->prox;
			nav->prox = aux->prox;
			free(aux);
			
		}
		nav = nav->prox;
	}
	printf("Nenhum processo terminou\n");
	return -1;
}

Processos *achaUltimo(Processos *l)
{
	Processos *nav = l;
	
	if(l->prox == l)
		return l;
		
	else
	{
		while(nav->prox != l)
		{
			nav = nav->prox;
		}
		return nav;
	}
	puts("Nao achou");	
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


void signalhandler(int signal)
{
	if(signal == SIGUSR1)
	{
		if(executando->prioridade >= aux->prioridade) //Executando tem menos prioridade entao troca
		{	
				
			if(aux->prioridade < 6 )//PRIORIDADE
			{	
				if(executando->prioridade != 100)
					kill(executando->pid, SIGSTOP);
				executando->pid = aux->pid;
				strcpy(executando->path, aux->path);
				executando->prioridade = aux->prioridade;
				executando->contador = aux->contador;
				kill(executando->pid, SIGCONT);
				executando->contador++;
				
			}
			else//ROUND-ROBIN
			{
				
			}
			
				
		}
	}

}




