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
#define timeslice 1




typedef struct No
{
    char nome[100];
    int pid;
    int prioridade;
	int temporestante;
    struct No *prox;

}No;

typedef struct Head
{
    int n;
    No* inicio;

}Head;




Head*  Cria_Lista ()
{
    Head* mo =(Head*)malloc(sizeof(Head));
    mo->n=0;
    mo->inicio=NULL;
    return mo;
}


int Insere_Lista (Head* lista, int pri, char nome[100],int temporestante ,int pid)
{
   if(lista==NULL)
   {
       return 0;
   }

   No* node=(No*)malloc(sizeof(No));
   if(node==NULL)
   {

       return 0;
   }
   node->pid = pid;
   node->prioridade=pri;
   strcpy(node->nome,nome);
   node->temporestante = temporestante;
   node->prox=NULL;
   lista->n++;


   if(lista->inicio==NULL)
   {
    lista->inicio=node;
    return 1;
   }
   else
   {
       No* last=lista->inicio;
       while(last->prox!=NULL)
       {
           last=last->prox;
       }
       last->prox=node;
       return 1;

   }

    return -1;
}


No* Procura_pid (Head*lista,int pid)
{
    if(lista==NULL)
    {
        return NULL;
    }
    No* x=lista->inicio;
    if(x->pid==pid)
    {
        return x;
    }
    else
    {

        while(x->prox!=NULL)
        {
            x=x->prox;
            if(x->pid==pid)
            {
                return x;
            }

        }
    }
    return NULL;
}



No* Procura_pri (Head*lista,int pri)
{
    if(lista==NULL)
    {
        return NULL;
    }
    No* x=lista->inicio;
    if(x->prioridade==pri)
    {
        return x;
    }
    else
    {

        while(x->prox!=NULL)
        {
            x=x->prox;
            if(x->prioridade==pri)
            {
                return x;
            }

        }
    }
    return NULL;
}


int size (Head*lista)
{
    return lista->n;
}


int Remove(Head*lista, No* node)
{
    No* aux;
    if (lista == NULL)
    {
        return 0;
    }
    No* x = lista->inicio;
    if (x == node)
    {
        lista->n--;
        lista->inicio = x->prox;
        free(x);
        return 1;
    }
    else
    {

        while (x->prox != NULL)
        {
            aux = x;
            x = x->prox;
            if (x == node)
            {
                lista->n--;
                aux->prox = x->prox;
                free(x);
                return 1;
            }

        }
    }
    return 0;
}

No* Procura_min_pri (Head*lista)
{
    No*x=NULL;
    int i,j=7;
    for(i=0;i<j;i++)
    {
        x=Procura_pri (lista,i+1);
        if(x!=NULL)
        {
            break;
        }
    }
    return x;
}


No *executando, *aux;

int criaProcesso (No *executando);

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
	
	No *proc; //Array de struct de processos
	
	int *j;
	

	//CRIANDO ARRAY DE PROCESSOS NA MEMORIA COMPARTILHADA
	int shmid, shmid2, shmidexec, shmidaux;
	int i = 0;

 	semId = semget (IPC_PRIVATE, 1, 0666 | IPC_CREAT);
   	setSemValue(semId);
    sleep (2);
	
	shmid2 = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	shmid = shmget(IPC_PRIVATE, count*sizeof(No), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	shmidexec = shmget(IPC_PRIVATE, sizeof(No), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	shmidaux = shmget(IPC_PRIVATE, sizeof(No), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);


	executando = (No *)shmat(shmidexec, 0, 0);//Local para pegar pid do escalonador
	aux = (No *)shmat(shmidaux, 0, 0);
	proc = (No *)shmat(shmid, 0, 0);

	j = (int *)shmat(shmid2, 0, 0);
	*j = 0;
	
	executando->pid = 100;
	executando->prioridade = 100;
	strcpy(executando->nome, " ");
	executando->temporestante = 3;
	
	for(i=0;i<count;i++)
	{		
		proc[i].pid = 100;
		proc[i].prioridade = 100;
		strcpy(proc[i].nome, " ");
		proc[i].temporestante = 3;
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
					
					strcpy(proc[*j].nome, ch2); //PATH DO PROGRAMA	
					proc[*j].prioridade = atoi(&ch3[11]); //PRIORIDADE
					proc[*j].temporestante = 3;
					//printf("%s %d--\n", proc[*j].nome, proc[*j].prioridade);			
					
					*j = *j + 1;
					sleep(1);
					semaforoV(semId);
					
					}
	
		fclose(arq);
		//Fechando arquivo com processos
		waitpid(-1, &status, 0);
		
	}
	else 
	{	
		
		Head *prontos = Cria_Lista(); //Insere_Lista (Head* lista, int pri, char nome, int temporestante, int pid)
		Head *esperando = Cria_Lista();
		No* pront;
		No *aux3, *aux4;
		int k = 0;
		//Procura_pid(prontos, executando->pid);
		//Procura_min_pri (Head*lista)
		//Procura_pri(head*lista)
		//Remove(Head*lista, No* node)
		
		for(i=0;i<300;i++)
		{			
			sleep(timeslice);
			if(i<count)
			{	
				semaforoP(semId);
				strcpy(executando->nome, proc[i].nome);
				proc[i].pid = criaProcesso(executando);
				Insere_Lista(prontos, proc[i].prioridade, proc[i].nome, proc[i].temporestante, proc[i].pid);
			}
			if(executando->temporestante == 0)
			{		
				puts("aaa");
				executando->prioridade = 1000;
				//kill(aux->pid, SIGKILL);
			}	
		
			
			if(i==0)//Primeiro processo
			{
				aux = Procura_min_pri(prontos);
				strcpy(executando->nome, aux->nome);
				executando->pid = aux->pid;
				executando->prioridade = aux->prioridade;
				executando->temporestante = aux->temporestante;
				Remove(prontos, aux);//Tirando aux da lista de prontos, agora ele esta em Executando
				kill(executando->pid, SIGCONT);
				executando->temporestante--;				
			}
			
			else if(executando->prioridade > Procura_min_pri(prontos)->prioridade)
			{	
				kill(executando->pid, SIGSTOP); //Achou com prioridade menor
				Insere_Lista(prontos, executando->prioridade, executando->nome, executando->temporestante, executando->pid);	
				puts("entrou");	
				aux = Procura_min_pri(prontos);
				strcpy(executando->nome, aux->nome);
				executando->pid = aux->pid;
				executando->prioridade = aux->prioridade;
				executando->temporestante = aux->temporestante;
				Remove(prontos, aux); //Tirando aux da lista de prontos agora ele esta em Executando
				kill(executando->pid, SIGCONT);
				
				
				
			}
			executando->temporestante--;
			
			
		}
	
	
		waitpid(-1, &status, 0);
		
	}
	
	
	
	shmdt((void *) executando);
	shmdt((void *) proc);
	return 0;

}

//---------------------------------------------------------------------------------------------------------------
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

int criaProcesso (No *executando) 
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
		execv(executando->nome, NULL);
	}
	return 0;
}




void signalhandler(int signal)
{
	if(signal == SIGUSR1)
	{
		
		
	}

}














