#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

//Variável Global que indica se o parque está fechado ou não.
int fechado = 0;
//Vetor dos turnos, utilizado para o algoritmo do ticket.
int turno[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,};
//Variáveis utilizadas no algoritmo do ticket
int num = 1;
int prox = 1;
//Variável que indica quantas threads passaram pela barreira
int cheio = 0;
//Variável Global que trava a passagem pela barreira
int trava = 0;
//Variável global que é utilizada para indicar se a volta atual acabou.
int acabou = 0;
//Variável Global das Voltas do carro.
int voltas = 0;
//Variável que indica se o carro está pronto para dar a volta.
int carroPronto = 0;

//Função utilizada para fazer a Thread esperar a volta acabar, recebendo o número da thread.
void entraCarro(long int n){
	printf("THREAD %li: estou no carro..\n", n);
	while(!acabou);//Barreira para esperar até acabar a volta
	cheio--;//Decrementa, indicando que a "pessoa está saindo do carro".
}

//Função que fará a Thread esperar um tempo aleatório, recebendo o número da thread.
void passeio(long int n){
	int aleatorio = (rand()%20);//Variável que recebe o número aleatório.
	printf("THREAD %li: estou indo passear %d segundos\n", n, aleatorio);
	sleep(aleatorio);//Thread dorme o tempo aleatório.
}

//Função que faz o carro esperar um tempo fixo.
void* esperaCarro(void* a){
	sleep(5);

	if (trava == 0)
	{
		//Caso o carro não esteja cheio mas vai sair assim mesmo...
		printf("CARRO: Esperei 5s e não entraram o suficiente, iniciando mesmo assim...\n");
		carroPronto = 1;
	}else{
		//Caso o carro esteja cheio e pronto pra sair...
		printf("CARRO: Esperei 5s e o carro está cheio...\n");
		carroPronto = 1;
	}
}

//Função que representa a fila da montanha russa, recebendo o número da thread.
void* fila(void* a){
	long int n = (long int) a;//Variável recebe o valor do parametro convertido para long int.

	//Enquanto o parque não estiver fechado, as pessoas continuaram entrando na fila.
	while(!fechado){
		printf("THREAD %li: entrei na fila\n", n);

		//O turno de cada Thread é definido por ordem de chegada
		turno[n] = __sync_fetch_and_add(&num, 1);

		//Barreira que será usada para deixar passar apenas a Thread da vez
		//e somente se a trava não esteja ativada (carro cheio).
		while((turno[n] != prox) || (trava));

		//Caso tenham 5 pessoas no carro (0-4), a trava é ativada, pois o carro está cheio.
		if (cheio == 4)
		{
			trava = 1;
		}

		printf("THREAD %li: Passei pela Barreira...\n", n);
		sleep(1);

		//Incrementa as variáveis indicando se o carro está cheio
		//e passando a vez pro próximo ticket.
		cheio++;
		prox++;

		//A Thread entra no carro, passando o seu número.
		entraCarro(n);

		//A thread espera um tempo aleatório.
		passeio(n);
	}

}

//Função que representa o funcionamento do Carro da Montanha Russa.
void* carroFunc(void* b){
	//Variável da Thread que irá fazer a função esperar o tempo.
	pthread_t tempoCarro;
	//Variável que receberá o tempo aleatório da volta.
	int tempoVolta;

	//Enquanto não completar as 10 voltas...
	while(voltas < 10){
		//Muda a variável indicando que o carro está pronto para partir.
		carroPronto = 0;

		//Função criando a Thread que executará a função de espera de 5s. 
		pthread_create(&tempoCarro, NULL, esperaCarro, NULL);

		//Muda o valor da variável indicando que a volta não acabou.
		acabou = 0;

		//Enquanto o carro não tiver esperado os 5s, a barreira segura a thread.
		while(!carroPronto);
		sleep(1);

		//Inicializa a variável do tempo aleatório.
		tempoVolta = (rand()%20+1);
		printf("CARRO: Começando a volta %d de %d segundos\n", voltas+1, tempoVolta);
		sleep(tempoVolta);//Espera o tempo aleatório.

		printf("CARRO: Terminei a volta...\n");
		//incrementa a variável das voltas completadas.
		voltas++;

		//Indica que a volta acabou.
		acabou = 1;

		printf("CARRO: Irei esperar esvaziar...\n");
		//Espera esvaziar o carro.
		while(cheio != 0);
		//Zera a trava, permitindo mais pessoas entrarem no carro.
		trava = 0;
	}

	printf("CARRO: Fechando o Parque!\n");
	//Fechando o Parque depois das 10 voltas.
	fechado = 1;
}
int main(){
	//Array de Threads que representam as pessoas do parque.
	pthread_t threads[20];
	//Variável da Thread do Carro.
	pthread_t carro;
	//Variável utilizada para gerar números aleatórios.
  	time_t t;
   
  	//Inicialização do gerador de números aleatórios.
   	srand((unsigned) time(&t));

   	//Criação da Thread do carro.
	pthread_create(&carro, NULL, carroFunc, NULL);
	
	//Criação das 20 Threads de pessoas
	for (long i = 0; i < 20; i++)
	{
		pthread_create(&threads[i], NULL, fila, (void*)i);
	}

	//Barreira que manterá a main esperando que o parque feche.
	while(!fechado);
	printf("MAIN: Parque fechado com sucesso, encerrando atividades...\n");
	sleep(2);
	
	return 0;
}