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


typedef struct Processos
{
	char path[100];
	int prioridade;
	int pid;
} Processos;

void sortPrioridades(Processos *proc, int cont);

int criaProcesso (int n, Processos *proc);

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
	int *pid_esc;
	int status;
	int semId;
	Processos *proc; //Array de struct de processos

	

	//CRIANDO ARRAY DE PROCESSOS NA MEMORIA COMPARTILHADA
	int shmid, shmid2, shmidpid;
	int i = 0;
	int *j;

 	semId = semget (IPC_PRIVATE, 1, 0666 | IPC_CREAT);
   	setSemValue(semId);
    sleep (2);


	shmid2 = shmget (IPC_PRIVATE, 2 *sizeof (int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	shmidpid = shmget (IPC_PRIVATE, sizeof (int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	shmid = shmget(IPC_PRIVATE, count*sizeof(Processos), IPC_CREAT);

	j = (int *)shmat(shmid2, 0, 0);
	j[0] = 0;
	j[1] = 0;
	pid_esc = (int *)shmat(shmidpid, 0, 0);//Local para pegar pid do escalonador
	
	proc = (Processos *)shmat(shmid, 0, 0);
	//proc = (Processos*)malloc(count*sizeof(Processos));
	for(i=0;i<count;i++)
	{		
		proc[i].pid = -1;
		proc[i].prioridade = 1000;
		strcpy(proc[i].path, " ");
	}
	//FIM CRIACAO DO ARRAY

	

	//sortPrioridades(proc, count);
	
	
	

	
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
					strcpy(proc[j[0]].path, aux); //PATH DO PROGRAMA	
					proc[j[0]].prioridade = atoi(&ch3[11]); //PRIORIDADE
					//printf("%s %d--\n", proc[j[0]].path, proc[j[0]].prioridade);			
										
					sleep(1);
					j[0]++;
					semaforoV(semId);
					
					}
	
		fclose(arq);
		//Fechando arquivo com processos
		
	}
	else 
	{	
		printf("PID-%d\n", getpid());
		i=0;
		while(1==1)
		{	
			
			//sleep(1);
			if(i < count)
			{	
					
				semaforoP(semId);
				proc[i].pid = criaProcesso(i, proc);
				printf("%d ------\n\n", proc[i].pid);
				
			}				
				
			//kill(proc[2].pid, SIGCONT);
			i++;
			
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



void sortPrioridades(Processos *proc, int cont)
{
	int i, j, aux;
	
	for (j = 1; j < cont; j++) {
   		for (i = 0; i < cont - 1; i++) {
     		if (proc[i].prioridade > proc[i + 1].prioridade) {
			   aux = proc[i].prioridade;
			   proc[i].prioridade = proc[i + 1].prioridade;
			   proc[i + 1].prioridade = aux;
     		}
   		}
	 }

	for (i = 0; i < cont; i++) {
 		printf("%d\n", proc[i].prioridade);
	}
	
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

int criaProcesso (int i, Processos *proc) {
  int pid;
  pid = fork(); /* Executa o fork */


  if (pid < 0) {
    return -1;
	}
	else if ( pid != 0 ) { /* PAI */
    kill(pid, SIGSTOP); /* Pausa o filho */
    // printf("PROCESSO %d PAUSADO!\n", pid);
    return pid;
  	} 

	else { /* FILHO */
    execv(proc[i].path, NULL);
  	}
  return 0;
}




