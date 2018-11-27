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


int main(){
  
	//CRIACAO DO ARQUIVO DE SAIDA
	FILE *saida; //saida.txt
	saida = fopen("saida.txt", "wt");
	if(saida == NULL)
			printf("Erro, nao foi possivel abrir o arquivo\n");

  //CRIACAO DO ARQUIVO DE ENTRADA/ LEITURA ARQUIVO DE ENTRADA
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
}
