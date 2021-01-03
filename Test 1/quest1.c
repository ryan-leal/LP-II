#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/*Concorrência acontece quando dois ou mais processos
e/ou threads cooperam entre si para solucionar um problema
dividindo os recursos do processador e existem diversas
linguagens com suporte para programação concorrente tais
como: C/C++/Java e entre outras que dispõe de funcionalidades
para criação e gerenciamento de processos e threads.*/

/*Dentre os elementos utilizados, temos: Pilhas, registradores,
Dados, arquivos, código, Memória e Processamento. Onde é necessário 
que não exista dependências entre instruções e que haja disponibilidade
de recursos.*/

/*A Consistência de memória acontece quando os diversos entes como
threads e processos conseguem acessar e utilizar informações da memória
sem acarretar em problemas como inconsistência de memória.*/

double varGlobal = 0.0;//Variável Global

//Função que a Thread executará que recebe o índice no formato void*.
void *func(void* num){

	long myIndice = (long)num;//Usando cast para receber o valor do índice passado pelo pthread_create().
	printf("Fui criada e recebi %li\n", (long)num);

	//Verifica se é par ou ímpar usando o resto da divisão por 2.
	if (myIndice%2 == 0)
	{
		//Se for par:
		printf("[Thread %li]Verifiquei que meu indice é par\n", myIndice);

		//Incremento 10x com sleep aleatório usando a função rand definida entre 1 e 4.
		for (int i = 0; i < 10; i++)
		{
			
			varGlobal++;
			printf("[Thread %li]Novo Valor de varGlobal eh %.2lf\n", myIndice, varGlobal);
			sleep(rand() % 3 + 1);
		}

	} else{
		//Se for ímpar:
		printf("[Thread %li]Verifiquei que meu indice é ímpar\n", myIndice);

		//Decremento 10x com sleep aleatório usando a função rand definida entre 1 e 4.
		for (int i = 0; i < 10; i++)
		{
			
			varGlobal--;
			printf("[Thread %li]Novo Valor de varGlobal eh %.2lf\n", myIndice, varGlobal);
			sleep(rand() % 3 + 1);
		}
	}

	//Sai da Thread.
	pthread_exit(NULL);
}

int main(){

	pthread_t threads[10];//Array de threads.
	time_t t;//Variável usada para gerar números aleatórios.

	srand((unsigned) time(&t));//Inicializando o gerador de números aleatórios.

	//For onde serão criadas as 10 threads e passados seus índices.
	for (long i = 0; i < 10; i++)
	{
		printf("Criando thread de numero %li\n", i);
		pthread_create(&threads[i], NULL, func, (void*)i);
	}

	//Utilização do join para esperar que todas as threads se encerrem.
	printf("Usando join\n");
	for (int i = 0; i < 10; i++)
	{
		pthread_join(threads[i], NULL);
	}

	//Print do valor final da variável Global.
	printf("[MAIN] Valor de varGlobal eh %.2lf\n", varGlobal);
	
	return 0;
}