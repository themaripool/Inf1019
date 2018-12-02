#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int contador = 0;

typedef struct Page
{
	unsigned int addr;
	char tipo;
	int ultimoacesso;
	int acessos;
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
	FILE *arq;
	char programa[20];
	char tipo;
	unsigned int addr;
	int qtdpaginas = (atoi(argv[4])*1000)/atoi(argv[3]);//Verificando quantas paginas cabem na memoria e criando vetor para elas.
	int i, aux;
	Page auxPage;
	Page memoria[qtdpaginas]; //Estrutura da memoria fisica que vai segurar as paginas
	Relatorio relatorio;
	
	strcpy(programa, argv[2]);
	
	
	//Inicializando vetor da memoria com -1
	for(i=0; i<qtdpaginas; i++)
	{
		memoria[i].ultimoacesso = -1;
		memoria[i].acessos = 0;
	}
	
	//Inicializando a estrutura do relatorio
	strcpy(relatorio.nome, argv[1]);
	strcpy(relatorio.algoritmo, argv[0]);
	relatorio.paginas = atoi(argv[2]);
	relatorio.memfisica = atoi(argv[3]);
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
				if(aux == -1)//Pagina nao esta no vetor
				{
					//printf("Entrou1");
					//printf("\n");
					if(memoria[qtdpaginas-1].ultimoacesso == -1)//Sem page fault
					{
						aux = verificaUltimo(memoria, qtdpaginas);
						memoria[aux].addr = auxPage.addr;
						memoria[aux].tipo = auxPage.tipo;
						memoria[aux].ultimoacesso = contador;
						//printf("Sem page fault\n");
					}
					
					else //Page fault
					{
						aux = verificaUltimo(memoria, qtdpaginas);
						memoria[aux].addr = auxPage.addr;
						memoria[aux].tipo = auxPage.tipo;
						memoria[aux].ultimoacesso = contador;
						//printf("page fault\n");
						relatorio.pagefault++;
					}
					
					if(memoria[aux].tipo == 'W') //Houve escrita
					{
						
						relatorio.escritas++;
						//printf("MEMORIA : %c\n", memoria[aux].tipo);
					}
				}
				
				else //Pagina esta no vetor
				{
					//printf("entrou\n");
					memoria[aux].ultimoacesso = contador;
					if(memoria[aux].tipo == 'W'); //Houve escrita
					{
						relatorio.escritas++;
						//printf("Sobre escrita\n");
						
					}	
					//printf("atualizou pagina\n");			
				}
			} 		
			
			else if((strcmp(argv[1], "NRU")) == 0)//CODIGO DO ALGORITMO NRU ----------------------
			{
				aux = verificaPagina(auxPage.addr, memoria, qtdpaginas);
				if(aux == -1)//Pagina nao esta no vetor
				{
					//printf("Entrou1  ");
					if(memoria[qtdpaginas-1].ultimoacesso == -1)//Sem page fault (vetor tem espaco)
					{
						aux = verificaUltimo(memoria, qtdpaginas);
						memoria[aux].addr = auxPage.addr;
						memoria[aux].tipo = auxPage.tipo;
						memoria[aux].ultimoacesso = contador;
						memoria[aux].acessos++;
						//printf("Sem page fault\n");
					}
					
					else //Page fault
					{
						aux = verificaAcessos(memoria, qtdpaginas);
						memoria[aux].addr = auxPage.addr;
						memoria[aux].tipo = auxPage.tipo;
						memoria[aux].ultimoacesso = contador;
						memoria[aux].acessos++;
						//printf("page fault\n");
						relatorio.pagefault++;
					}
					
					if(memoria[aux].tipo == 'W') //Houve escrita
					{
						relatorio.escritas++;
						memoria[aux].acessos++;
						//printf("ESCREVEU\n");
					}
				}
				
				else //Pagina esta no vetor
				{
					memoria[aux].ultimoacesso = contador;
					memoria[aux].acessos++;		
					if(memoria[aux].tipo == 'W') //Houve escrita
					{
						relatorio.escritas++;
						//printf("Sobre escrita\n");
						
					}	
					//printf("acessou pagina do vetor\n");	
					
				}
			}
		
			contador++;		
			//printf("contador: %d\n", contador); 
		}
		
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

int verificaAcessos(Page * memoria, int n)
{
	int i, menor, aux;
	menor = memoria[0].acessos;
	for(i=0;i<n;i++)
	{
		if(menor > memoria[i].acessos)
		{
			menor = memoria[i].acessos;
			aux = i;
		}
	}
	
	return aux;
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

