#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

long int contador = 0;

typedef struct Page
{
	unsigned int addr;
	char tipo;
	int R;
	int M;
	int ultimoacesso;
} Page;

typedef struct Relatorio
{	
	//valores fixos
	char nome[20];
	char algoritmo[3];
	int paginas;
	int memfisica;
	//valores variaveis
	int pagefault;
	int escritas;
	
} Relatorio;

int verificaUltimo(Page *memoria, int n);// Procura a pagina que ficou mais tempo sem ser acessada

unsigned int verificaAddr(unsigned int addr, int tamanho); // Pega o addr da pagina

int verificaPagina(unsigned int addr, Page *memoria, int n);// Verifica se a pagina ja esta na memoria

int verificaAcessos(Page * memoria, int n);




/*********************************************MAIN******************************************************/


int main(int argc, char *argv[])
{
	FILE *arq, *saida;
	
	char programa[20];
	char tipo;
	unsigned int addr;
	int qtdpaginas = (atoi(argv[4])*1024)/atoi(argv[3]);//Verificando quantas paginas cabem na memoria e criando vetor para elas.
	int i, aux;
	Page auxPage;
	Page memoria[qtdpaginas]; //Estrutura da memoria fisica que vai segurar as paginas
	Relatorio relatorio;
	
	strcpy(programa, argv[2]);
	
	saida = fopen("saida.txt", "wt");
	if(saida == NULL)
	{
		printf("nao foi possivel abrir o arquivo de saida");
	}

	//Inicializando vetor da memoria com -1
	for(i=0; i<qtdpaginas; i++)
	{
		memoria[i].ultimoacesso = -1;
		memoria[i].M = 0;
		memoria[i].R = 0;
	}
	
	//Inicializando a estrutura do relatorio
	strcpy(relatorio.nome, argv[2]);
	strcpy(relatorio.algoritmo, argv[1]);
	relatorio.paginas = atoi(argv[3]);
	relatorio.memfisica = atoi(argv[4]);
	relatorio.pagefault = 0;
	relatorio.escritas = 0;
	
	if((atoi(argv[3]) < 8) || (atoi(argv[3]) > 32)) //Verificando se o tamanho da pagina eh valida
	{
		printf("Tamanho da pagina invalido, escolha entre os valores 8, 16, 32\n");
		exit(1);
	}
	if((atoi(argv[4]) < 1)||(atoi(argv[4]) > 16))//Verificando se o tamanho da memoria fisica eh valida
	{
		printf("Tamanho da Memoria fisica invalida coloque valores entre 1 e 16\n");
		exit(1);
	}
	
	printf("contador: %d\n", contador);
	//Lendo arquivo
	arq = fopen(programa, "r");
	if(arq == NULL)
		printf("Erro, nao foi possivel abrir o arquivo\n");
	else
	while( (fscanf(arq,"%x %c\n", &addr, &tipo))!=EOF )
		{
			auxPage.addr = verificaAddr(addr,atoi(argv[3]));
			auxPage.tipo = tipo;
			if((strcmp(argv[1], "LRU")) == 0)//CODIGO DO ALGORITMO LRU
			{
				aux = verificaPagina(auxPage.addr, memoria, qtdpaginas);
				if(aux == -1)//Pagina nao esta no vetor (page fault)
				{
				
					relatorio.pagefault++;
					if(memoria[qtdpaginas-1].ultimoacesso == -1)//Vetor nao esta cheio
					{
						aux = verificaUltimo(memoria, qtdpaginas);
						memoria[aux].addr = auxPage.addr;
						memoria[aux].tipo = auxPage.tipo;
						memoria[aux].ultimoacesso = contador;
						memoria[aux].M = 0;
						memoria[aux].R = 0;
	
					}
					
					else //Vetor cheio, substitui alguem
					{
						aux = verificaUltimo(memoria, qtdpaginas); //Algoritmo LRU
						memoria[aux].addr = auxPage.addr;
						memoria[aux].tipo = auxPage.tipo;
						memoria[aux].ultimoacesso = contador;
						if(memoria[aux].M == 1) //Saindo pagina suja
						{
							relatorio.escritas++;
						}
						memoria[aux].R = 0;
						memoria[aux].M = 0;
				
					}
					
					if(memoria[aux].tipo == 'W') //Houve escrita
					{
						
						memoria[aux].M = 1;

					}
				}
				
				else //Pagina esta no vetor
				{
					memoria[aux].ultimoacesso = contador;
					if(memoria[aux].tipo == 'W') //Houve escrita
					{
						memoria[aux].M = 1;
					}		
				}
			} 		
			
			else if((strcmp(argv[1], "NRU")) == 0)//CODIGO DO ALGORITMO NRU ----------------------
			{
				aux = verificaPagina(auxPage.addr, memoria, qtdpaginas);
				if(aux == -1)//Pagina nao esta no vetor (page fault)
				{
					relatorio.pagefault++;
					if(memoria[qtdpaginas-1].ultimoacesso == -1)//vetor tem espaco
					{
						aux = verificaUltimo(memoria, qtdpaginas);
						memoria[aux].addr = auxPage.addr;
						memoria[aux].tipo = auxPage.tipo;
						memoria[aux].ultimoacesso = contador;
						memoria[aux].M = 0;
						memoria[aux].R = 0;
						
					}
					
					else //vetor nao tem espaco, substituicao
					{
						aux = verificaAcessos(memoria, qtdpaginas); //Rodando algoritmo NRU
						memoria[aux].addr = auxPage.addr;
						memoria[aux].tipo = auxPage.tipo;
						memoria[aux].ultimoacesso = contador;
						if(memoria[aux].M == 1) //Saindo pagina suja
						{
							relatorio.escritas++;
						}
						memoria[aux].M = 0;
						memoria[aux].R = 0;
					}
					
					if(memoria[aux].tipo == 'W') //Houve escrita
					{
						memoria[aux].M = 1;
					}
					
				}
				
				else //Pagina esta no vetor
				{
					memoria[aux].ultimoacesso = contador;
					memoria[aux].R = 1;		
					if(memoria[aux].tipo == 'W') //Houve escrita
					{
						memoria[aux].M = 1;
					}	
				}
			}
			
			contador++;	
			if(contador%qtdpaginas == 0)//Zerando bit R
			{
				for(i=0;i<qtdpaginas;i++)
				{
					memoria[i].R = 0;
				}
			}	
			//printf("contador: %d\n", contador); 
		}
	
	fprintf(saida,"Nome: %s\n", relatorio.nome);
	fflush(saida);
	
	fprintf(saida, "Algoritmo: %s\n", relatorio.algoritmo); 	
	fflush(saida);
	
	fprintf(saida, "Tamanho da pagina: %d\n", relatorio.paginas); 
	fflush(saida);	
	
	fprintf(saida, "Tamanho da memoria fisica: %d\n", relatorio.memfisica); 
	fflush(saida);	
	
	fprintf(saida, "Page Faults: %d\n", relatorio.pagefault); 	
	fflush(saida);
	
	fprintf(saida, "Escritas: %d\n", relatorio.escritas); 	
	fflush(saida);
	
	fclose(arq);
	return 0;
}


int verificaUltimo(Page* memoria, int n)
{
	int i, menor, aux;
	menor = memoria[0].ultimoacesso;
	for(i=0;i<n;i++)
	{
		if(menor > memoria[i].ultimoacesso)
		{
			menor = memoria[i].ultimoacesso;
			aux = i;
		}
	}
	
	return aux;
}

int verificaAcessos(Page * memoria, int n) //ALGORITMO  DE NRU
{
	int i;
	for(i=0; i<n; i++) //Verificando se R = 0 e M = 0
	{
		if((memoria[i].R == 0)&&(memoria[i].M == 0))
		{
			return i;
		}
	}
	
	for(i=0; i<n; i++) //Verificando se R = 0 e M = 1
	{
		if((memoria[i].R == 0)&&(memoria[i].M == 1))
		{
			return i;
		}
	}
	
	for(i=0; i<n; i++) //Verificando se R = 1 e M = 0
	{
		if((memoria[i].R == 1)&&(memoria[i].M == 0))
		{
			return i;
		}
	}
	
	for(i=0; i<n; i++) //Verificando se R = 1 e M = 1
	{
		if((memoria[i].R == 1)&&(memoria[i].M == 1))
		{
			return i;
		}
	}
	return -1;
}

unsigned int verificaAddr(unsigned int addr, int tamanho)
{
	if(tamanho == 8)//PAGINAS DE 8 KB
	{
		addr = addr>>13;
	}

	else if(tamanho == 16)//PAGINAS DE 16KB
	{
		addr = addr>>14;
	}
				
	else if(tamanho == 32)//PAGINAS DE 32KB
	{
		addr = addr>>15;
	}
	
	return addr;
}

int verificaPagina(unsigned int addr, Page *memoria, int n) //Retorna o indice
{
	int i;
	for(i=0; i<n; i++)
	{
		if(addr == memoria[i].addr)
		{
			return i; 
		}	
	}
	return -1; //NAO ACHOU
}

