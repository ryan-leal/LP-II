#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int vencedora = -1;//Variável Globa que será usada para o vencedor indicar sua vitória.

//Struct com as propriedades de cada lebre.
typedef struct
{
	long index;
	double salto;
}tLebre;

//Função responsável pela corrida que recebe o ponteiro para a propriedade daquela lebre.
void *func(void *props){

	//Variável que aponta para as propriedades da lebre recebendo o ponteiro para as propriedades
	tLebre *myProps = props;

	//Variáveis responsáveis pelo salto e o total pulado até o momento.
	double total = 0, saltoAtual = 0;
	
	printf("Sou a lebre %li e salto %.2lfcm no máximo.\n", myProps->index, myProps->salto);

	//While onde a corrida vai se desenrolar para cada lebre.
	while(total < 1000){
		//Salto de cada rodada dado pelo uso de aleatoriedade e tamanho máximo de salto.
		saltoAtual = (rand()%(int)myProps->salto);

		//O total percorrido pela lebre é atualizado.
		total += saltoAtual;
		printf("[Lebre %li] Saltei %.2lf e percorri %.2lf até agora\n", myProps->index, saltoAtual, total);

		//Caso o salto total dado pela lebre seja igual ou maior que os 10m, ele declara vitória.
		if (total >= 1000)
		{
			printf("[Lebre %li] Sou a Vencedora!\n", myProps->index);

			//Altera a variável global com seu índice para assim se declarar o vencedor.
			vencedora = myProps->index;
		}
		//Dorme um segundo antes do próximo salto (Que preguiçosa...).
		sleep(1);
	}

	pthread_exit(NULL);
}

int main(){
	//Variável de cada lebre/thread.
	pthread_t lebres[10];

	//Propriedades de cada lebre
	tLebre lebreProps[10];

	time_t t;//Variável usada para gerar números aleatórios.

	srand((unsigned) time(&t));//Inicializando o gerador de números aleatórios.

	//Criação das 10 threads/lebres e inicialização e passagem de suas propriedades.
	for (long i = 0; i < 10; ++i)
	{
		lebreProps[i].index = i;
		lebreProps[i].salto = (rand() % 20 + 20);
		pthread_create(&lebres[i], NULL, func, &lebreProps[i]);
	}

	//A main aguarda a vencedora em um looping.
	while(vencedora == -1);

	//Com o valor da vencedora, usando um switch indicamos quem ganhou e encerramos a main
	//encerrando assim também as demais threads/lebres.
	switch (vencedora)
	{
	   case 0:
	     printf("A vencedora foi a Lebre 0! Parabéns!\n");
	     return 0;
	   break;

	   case 1:
	     printf("A vencedora foi a Lebre 1! Parabéns!\n");
	     return 0;
	   break;

	   case 2:
	   	printf("A vencedora foi a Lebre 2! Parabéns!\n");
	   	return 0;
	   break;

	   case 3:
	   	printf("A vencedora foi a Lebre 3! Parabéns!\n");
	   	return 0;
	   break;

	   case 4:
	   	printf("A vencedora foi a Lebre 4! Parabéns!\n");
	   	return 0;
	   break;

	   case 5:
	   	printf("A vencedora foi a Lebre 5! Parabéns!\n");
	   	return 0;
	   break;

	   case 6:
	   	printf("A vencedora foi a Lebre 6! Parabéns!\n");
	   	return 0;
	   break;

	   case 7:
	   	printf("A vencedora foi a Lebre 7! Parabéns!\n");
	   	return 0;
	   break;

	   case 8:
	   	printf("A vencedora foi a Lebre 8! Parabéns!\n");
	   	return 0;
	   break;

	   case 9:
	   	printf("A vencedora foi a Lebre 9! Parabéns!\n");
	   	return 0;
	   break;

	   default:
	   	printf("Deu algum erro ai!\n");
	};
	
	return 0;
}