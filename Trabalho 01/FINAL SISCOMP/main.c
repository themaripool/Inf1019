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
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>


#define count 3 //SEMPRE TEM QUE SER IGUAL AO NUMERO DE PROGRAMAS

#define timeslice 1




typedef struct No
{
    char nome[30];
    int pid;
    int prioridade;
	int temporestante;
	int tempoio; //IO
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


int Insere_Lista (Head* lista, int pri, char nome[100],int temporestante, int tempoio ,int pid)
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
   node->tempoio = tempoio;
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

int criaProcesso (No proc);


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
	
	

	//CRIANDO ARRAY DE PROCESSOS NA MEMORIA COMPARTILHADA
	int shmid, shmid2, shmidexec, shmidaux;
	int i = 0;

	// COISAS DO SAIDA.TXT

	FILE *saida; //saida.txt
	saida = fopen("saida.txt", "wt");
	if(saida == NULL)
			printf("Erro, nao foi possivel abrir o arquivo\n");

	//saida = fopen("saida.txt", "w");
	//fclose(saida);
	
	
    
	


	
	///

 	semId = semget (IPC_PRIVATE, 1, 0666 | IPC_CREAT);
   	setSemValue(semId);
	
	shmid = shmget(IPC_PRIVATE, count*sizeof(No), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	shmidexec = shmget(IPC_PRIVATE, sizeof(No), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	shmidaux = shmget(IPC_PRIVATE, sizeof(No), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);


	executando = (No *)shmat(shmidexec, 0, 0);//Local para pegar pid do escalonador
	aux = (No *)shmat(shmidaux, 0, 0);
	proc = (No *)shmat(shmid, 0, 0);

	
	executando->pid = 100;
	executando->prioridade = 100;
	strcpy(executando->nome, " ");
	executando->temporestante = 2;
	executando->tempoio = 3;
	
	for(i=0;i<count;i++)
	{		
		proc[i].pid = 100;
		proc[i].prioridade = 100;
		strcpy(proc[i].nome, " ");
		proc[i].temporestante = 2;
		proc[i].tempoio = 3;
	}
	//FIM CRIACAO DO ARRAY

	
	if(pid = (fork() != 0)) //Processo pai
	{
		FILE *arq;
		int j = 0;
		char programas[]="exec.txt",
	    ch1[100], ch2[100], ch3[100];
		//Lendo arquivo com processos
		arq = fopen(programas, "r");
		if(arq == NULL)
				printf("Erro, nao foi possivel abrir o arquivo\n");
		else
			while( (fscanf(arq,"%s %s %s\n", &ch1, &ch2, &ch3))!=EOF )
				{
					
					strcpy(proc[j].nome, ch2); //PATH DO PROGRAMA	
					proc[j].prioridade = atoi(&ch3[11]); //PRIORIDADE
					proc[j].temporestante = 2;
					proc[j].tempoio = 3;
							
					proc[j].pid = criaProcesso(proc[j]);
					//printf("%s %d %d--\n", proc[j].nome, proc[j].prioridade, proc[j].pid);	
					j++;
					sleep(1);
					semaforoV(semId);
					
					}
	
		fclose(arq);
		//Fechando arquivo com processos
		for(i=0; i<count; i++)
			waitpid(-1, &status, 0);
		   


		
	}
	else 
	{	
		
		Head *prontos = Cria_Lista(); //Insere_Lista (Head* lista, int pri, char nome, int temporestante, int pid)
		Head *esperando = Cria_Lista();
		No* wait;
		int i;
		int k = 0;
		int r;
		//Procura_pid(prontos, executando->pid);
		//Procura_min_pri (Head*lista)
		//Procura_pri(head*lista)
		//Remove(Head*lista, No* node)
		//printf("PID: %d\n", getpid());
		for(k=0;k<30;k++)
		{	
			if(wait == NULL)
			{
				wait =(No*)malloc(sizeof(No));
			}
			
			if(size(esperando)>0)
			{			
				kill(aux->pid, SIGSTOP);
				
					printf("%d - Processo %s entrou em IO\n", k, aux->nome);
					fprintf(saida,"%d - Processo %s entrou em IO\n", k, aux->nome);
					fflush( saida );
					
				
				if(aux->prox != NULL)
				{	
					kill(aux->pid, SIGSTOP);
					//printf("%d - Processo %s entrou em IO\n", k, aux->prox->nome);
					
				}	
			
				
				
				
				for(i=0;i<size(esperando);i++)
				{
					
					aux->tempoio--;
					if(aux->tempoio == 0)
					{
						executando = aux;
						executando->tempoio = 3;
						Insere_Lista(prontos, executando->prioridade, executando->nome, executando->temporestante, executando->tempoio, executando->pid);
						Remove(esperando,aux);
						executando->pid = 100;
						
					}	
					if(aux->prox != NULL)
						aux = aux->prox;
				}							
			}
			
			
					
			if(k<count)
			{
				semaforoP(semId);	
				Insere_Lista(prontos, proc[k].prioridade, proc[k].nome, proc[k].temporestante, proc[k].tempoio, proc[k].pid);	
				
			}
			if(executando->temporestante < 1)
			{
				printf("%d - Processo %s terminou.\n", k, executando->nome);
				fprintf(saida,"%d - Processo %s terminou.\n", k, executando->nome);
				fflush( saida );
			
				kill(executando->pid, SIGSTOP);
				executando->prioridade = 1000;
				executando = Procura_min_pri(prontos);
				
				kill(executando->pid, SIGCONT);
				
				printf("%d - Processo %s iniciou execucao.\n", k, executando->nome);
				fprintf(saida,"%d - Processo %s iniciou execucao.\n", k, executando->nome);
				fflush( saida );
				
				
			}
			
			
			if(executando->pid == 100)//Primeiro processo
			{
				executando = Procura_min_pri(prontos);
				kill(executando->pid, SIGCONT);
				
				printf("%d - Processo %s iniciou execucao.\n", k, executando->nome);
				fprintf(saida,"%d - Processo %s iniciou execucao.\n", k, executando->nome);
				fflush( saida );
				
			}
			
			
			if(executando->prioridade > Procura_min_pri(prontos)->prioridade)//Prioridade maior que de executando achada
			{	
				kill(executando->pid, SIGSTOP);
				
				
				
				printf( "%d - Processo %s pausou.\n", k, executando->nome);
				fprintf(saida, "%d - Processo %s pausou.\n", k, executando->nome);
				fflush( saida );
				
				executando = Procura_min_pri(prontos);
				//printf("%d prioridade\n", executando->prioridade);
				if(executando->prioridade == 6)// Tem operacao de IO
				{
					
					
					printf("%d - Processo %s iniciou execucao.\n", k, executando->nome);
					
					fprintf(saida,"%d - Processo %s iniciou execucao.\n", k, executando->nome);
					fflush( saida );
					kill(executando->pid, SIGCONT);
					
					executando->temporestante--;
				
					Insere_Lista(esperando, executando->prioridade, executando->nome, executando->temporestante, executando->tempoio, executando->pid);
					
					
					Remove(prontos, executando);
					executando->prioridade = 100;
		
					aux = esperando->inicio;
					
					if (size(esperando) == 0){
						kill(executando->pid, SIGSTOP);
						executando = prontos->inicio;
					}
					
					
	
				}
					
				else
				{
					kill(executando->pid, SIGCONT);
					
					printf("%d - Processo %s iniciou execucao.\n", k, executando->nome);
					fprintf(saida,"%d - Processo %s iniciou execucao.\n", k, executando->nome);
					fflush( saida );
					
				}
				//printf("temporestante %d\n", executando->temporestante);
				
			}
			//printf("temporestante %d\n", executando->temporestante);
			if(executando->prioridade!=100)
			{
				executando->temporestante--;	
			}	
			sleep(timeslice);
			
			
		}
		fclose(saida);
		
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

int criaProcesso (No proc) 
{
	int pid;
	pid = fork();
  

	if (pid < 0) {
	return -1;
	}
	else if ( pid != 0 ) { /* PAI */
		kill(pid, SIGSTOP);
	/* Pausa o filho */

	return pid;
	} 

	else { /* FILHO */
		execv(proc.nome, NULL);
	}
	return 0;
}















