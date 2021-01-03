/*
	Explicação do acesso a seção crítica nas duas funções (funcX e funcY):
	Quando as Threads chegarem na função, elas irão passar pela função sem_wait()
	passando como parâmetro o semáforo referente a funçaõ (semaforoX ou semaforoY),
	Ao chegar nessa função, como o valor dos semáforos foi inicializado com 1, então
	a primeira thread que passar ali, a função vai verificar se o valor do semáforo
	é igual a 0, como não é, a função vai apenas decrementar 1 do semáforo e deixar a
	thread passar, mas isso não irá ocorrer com as próximas threads, já que agora
	o semáforo é igual a 0, então a função bloqueia essas threads, enquanto isso
	a thread que passou executa a seção crítica, que nesse caso, seria printar uma mensagem,
	gerar o valor aleatório, somar ele na variável global (X ou Y), printar outra mensagem
	e dormir o tempo referente a função, ao passar pela seção crítica, a thread irá passar
	pela função sem_post(), recebendo como parâmetro o semáforo da função da thread, então
	essa função vai analisar de o valor do semáforo é zero (sim, nesse caso) e se existe
	thread bloqueada (também sim nesse caso) se sim (oque acontece), ela desbloqueia uma das Threads
	e soma 1 ao semáforo da função, oque fará com que uma das threads seja desbloqueada e assim,
	acontece novamente o processo para as demais threads, até que todas passem pela seção cŕitica. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
//Variáveis globais que seram somadas.
float x = 0, y = 0;
//variável global que encerra a execução do programa.
int finaliza = 0;
//Variáveis globais que servem para fazer o controle das threads por iteração.
int concluidaX = 0, concluidaY = 0;
//Variáveis global que são os semáforos usados para a seção crítica.
sem_t semaforoX, semaforoY;
//Variáveis globais que são semáforos para garantir o controle das threads por iteração.
sem_t mutexX, barrierX;
sem_t mutexY, barrierY;

//Função utilizada pelas Threads pares para somar em X, recebendo o índice da thread.
void* funcX(void* p){
	//Variável recebendo o índice da Thread.
	long int index = (long int) p;
	//Variável que será utilizada para receber o valor aleatório.
	float aleatorio = 0;

	printf("Sou a Thread #%ld e estou na funcX\n", index);

	//Enquanto a soma de X e Y não for maior ou igual a 50.
	while(!finaliza){
		//invoca a função wait do semáforo da função X. (explicação acima).
		sem_wait(&semaforoX);
		printf("Sou a Thread #%ld e estou na seção crítica\n", index);
		//Será gerado um valor decimal entre 0.5 e 10.0 e atribui a variável.
		aleatorio = ((float)(rand()%100 + 5))/10;
		//Soma o valor aleatório gerado na variável globa X.
		x += aleatorio;
		printf("Thread #%ld saindo, somei %.2f e valor de X ficou %.2f\n", index, aleatorio, x);
		//Dorme 200ms
		usleep(200000);
		//invoca a função signal do semáforo da função X. (explicação acima).
		sem_post(&semaforoX);

		/*
			Abaixo temos a garantia de que cada Thread só vai somar uma vez
			para cada iteração. Temos então que: cada uma das 5 threads vai
			passar pelo primeiro semáforo um por um e cada um deles vai somar
			uma vez na variável concluidaX, então elas passam em um verificador
			para saber se é a 5º thread, se não for, ela  vai para o segundo
			semáforo, como o valor de barrierX é zero, então as threads vão
			ficar bloqueadas ali, até que a 5ª e última thread passe pelo verificador
			e lá some mais um na barrierX, desbloqueie assim uma das threads
			bloqueadas e também zere o barrierX, ai então, cada thread que 
			passar por esse semáforo vai liberar mais uma thread ao passar 
			pela barrierX e assim até que todas passem.
		*/
		sem_wait(&mutexX);
		concluidaX++;
		sem_post(&mutexX);

		if (concluidaX == 5)
		{
			printf("Todas as Threads somaram em X, reiniciando ciclo\n");
			sem_post(&barrierX);
			concluidaX = 0;
		}

		sem_wait(&barrierX);
		sem_post(&barrierX);
	}
}

//Função utilizada pelas Threads pares para somar em Y, recebendo o índice da thread.
void* funcY(void* p){
	//Armazenando o índice da Thread.
	long int index = (long int) p;
	//Variável que vai armazenar o valor aleatório gerado.
	float aleatorio = 0;

	printf("Sou a Thread #%ld e estou na funcY\n", index);	

	//Enquanto a soma de X e Y não for maior ou igual a 50.
	while(!finaliza){
		//invoca a função wait do semáforo da função Y. (explicação acima).
		sem_wait(&semaforoY);

		printf("Sou a Thread #%ld e estou na seção crítica\n", index);
		//Gera o valor aleatório para somar na variável Y. 
		aleatorio = ((float)(rand()%75+25))/10;
		//Valor aleatório é somado na variável Y.
		y += aleatorio;

		printf("Thread #%ld saindo, somei %.2f e valor de Y ficou %.2f\n", index, aleatorio, y);
		//Dorme de 100ms a 300ms.
		usleep((rand()%300000+100000));

		//invoca a função signal do semáforo da função Y. (explicação acima).
		sem_post(&semaforoY);

		//Garatia de que cada thread só irá somar a cada ciclo de iterações
		//(Explicação detalhada na função X)
		sem_wait(&mutexY);
		concluidaY++;
		sem_post(&mutexY);

		if (concluidaY == 5)
		{
			printf("Todas as Threads somaram em Y, reiniciando ciclo\n");
			sem_post(&barrierY);
			concluidaY = 0;
		}

		sem_wait(&barrierY);
		sem_post(&barrierY);

		

	}
}

//Função que vai esperar que a soma seja maior ou igual a 50 para encerrar o programa.
void* verifica(void* p){
	//Fica esperando que a soma de X e Y seja maior ou igual a 50.
	while(x+y < 50);
	printf("Sou A Thread verificadora e vi que a soma de X e Y é igual ou maior que 50\n");
	//Soma na variável filnaliza de forma atômica, indicando o final do programa.
	__sync_fetch_and_add(&finaliza, 1);


}

int main(){
	//Threads que executarão as funções X e Y.
	pthread_t threads[10];
	//Thread que verifica se a soma de X e Y é maior ou igual a 50.
	pthread_t verificadora;

	//Variável utilizada para gerar números aleatórios.
  	time_t t;
   
  	//Inicialização do gerador de números aleatórios.
   	srand((unsigned) time(&t));

   	//Inicialização dos semáforos das funções e das garantias.
	sem_init(&semaforoX, 0, 1);
	sem_init(&semaforoY, 0, 1);
	sem_init(&mutexX, 0, 1);
	sem_init(&mutexY, 0, 1);
	sem_init(&barrierX, 0, 0);
	sem_init(&barrierY, 0, 0);

	//Criação da Thread que vai verificar se a soma de X e Y é maior ou igual a 50.
	pthread_create(&verificadora, NULL, verifica, NULL);

	//For que vai criar as Threads paras as funções de X e Y.
	for (long int i = 0; i < 10; i++)
	{

		if (i % 2 == 0)
		{
			//Se o índice for par, executa a função X
			pthread_create(&threads[i], NULL, funcX, (void*)i);
		}else{
			//Se não, executa a função Y.
			pthread_create(&threads[i], NULL, funcY, (void*)i);
		}
	}

	//Espera a soma de X e Y ser maior ou igual a 50
	while(!finaliza);
	printf("Main: A soma de X e Y passou de 50, assim, encerramos...\n");

	//Destroi os semáforos utilizados no programa.
	sem_destroy(&semaforoX);
	sem_destroy(&semaforoY);
	sem_destroy(&mutexX);
	sem_destroy(&mutexY);
	sem_destroy(&barrierX);
	sem_destroy(&barrierY);
	return 0;
}