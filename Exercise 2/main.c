#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

//Array de caracteres que receberá as letras geradas.
char letras[7];
//Contador de quantas execuções foram realizadas.
int contadorExec = 0;

//Inicialização da variável da barreira.
pthread_barrier_t barrier;

//Função que verificará se é palíndromo.
void ehPalindromo(){
	//Arrray que receberá a string "ao contrário".
	char contrario[7];
	//Coloca o caractere Nulo na string contrario.
	contrario[6] = '\0';

	//For que irá inserir as 6 letras na string contrario.
	for (int i = 0; i < 6; i++)
	{
		//As letras seram invertidas de posição.
		contrario[i] = letras[5-i];
	}

	//Se as strings forem iguais...
	if (strcmp(letras, contrario) == 0)
	{
		printf("É Palíndromo!\n");
	} else{
		//Se não forem iguais...
		printf("Não é Palíndromo\n");
	}
}

//Função que as Threads executarão para obter os caracteres aleatórios.
void* worker (void* p){
	//Converte o parâmetro recebido (ID da Thread) em long int.
	long int n = (long int) p;

	//As Threads Ficaram aqui até acabar a execução do programa.
	while(1)
	{
		sleep(2);
		//Gera um caractere aleatório e insere na posição na string
		//De acordo com seu Thread ID formando no final uma palavra
		letras[n] = (rand() % 25 + 65);

		printf("Sou a Thread %li e gerei a letra %c!\n", n, letras[n]);
		//As Threads Ficaram na barreira até que todas as Threads cheguem na Barreira.
		pthread_barrier_wait(&barrier);	
	}
	
}

int main(){
	//Variável das Threads
	pthread_t threads[6];
	
	//Iniciando o último espaço para que a string tenha fim.	
	letras[6] = '\0';

	//Variável utilizada para gerar números aleatórios.
  	time_t t;
   
  	//Inicialização do gerador de números aleatórios.
   	srand((unsigned) time(&t));

   	//Iniciando a barreira usando a função de inicialização da barreira para 7 threads.
   	pthread_barrier_init(&barrier, NULL, 7);

   	//For para criação das 6 Threads.
	for (long int i = 0; i < 6; i++)
	{
		//Criação das Threads, passando seu indice como parâmetro.
		pthread_create(&threads[i], NULL, worker, (void*)i);
	}

	//A Thread Main ficará aqui até completar as 10 execuções.
	while(1)
	{
		//A thread main vai esperar até que as demais Threads cheguem na barreira.
		pthread_barrier_wait(&barrier);

		//Variável que conta as execuções sendo incrementada.
		contadorExec++;

		printf("Palavra gerada: %s\n", letras);

		//Executa a função verificando se a palavra gerada na execução é Palíndromo ou não.
		ehPalindromo();

		//Caso tenha realizado 10 vezes.
		if (contadorExec == 10)
		{
			//Sai do While(1).
			break;
		}

	}
	
	printf("10 palavras geradas, encerrando a execução...\n");
	printf("Verificando se a função do palíndromo funciona (usando \"MUSSUM\" como teste): ");

	//Realizando teste manual para provar que a funçaõ ehPalíndromo funciona:
	letras[0] = 'M';
	letras[1] = 'U';
	letras[2] = 'S';
	letras[3] = 'S';
	letras[4] = 'U';
	letras[5] = 'M';
	ehPalindromo();
	
	//Barreira é destruída no final da execução do programa.
	pthread_barrier_destroy(&barrier);
	
	return 0;
}