/*
	-Mecanismo utilizado na segunda Thread:
	Na minha implementação, a Thread vai entrar em um While() que
	irá esperar que a variável contador chegue a ser 5, ou seja,
	até o motor reiniciar 5 vezes a temperatura, então, a thread vai
	ficar em um while(1) até que a temperatura seja maior ou igual a
	1000ºC, assim que essa condição for atingida, ele sai dessa trava,
	trava função incrementadora (para evitar que ela modifique a var
	de temperatura enquanto ela ainda não foi zerada) utilizando a var
	global espere, incrementando-a em 1 usando uma função atômica, e
	então a temperatura é reiniciada para 600ºC, adiciona um na contagem
	de vezes que reiniciou usando função atômica, depois subtrai 1 da
	variável que faz a função incrementadora esperar, usando função atômica
	assim, enquanto a contagem não chegar a 5, ele continua esse processo.

	-qual seria a alternativa se não houvesse o requisito de detecção da
	temperatura de forma mais imediata possível:
	Poderíamos utilizar barreias, da própria API pthreads e também semáforos
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

//Variável global de temperatura
float temperatura = 600;
//Variável global que conta as execuções da função vigia de temperatura.
int contador = 0;
//Variável global que serve para travar a função 1 enquanto a temperatura é alterada.
int espere = 0;

//Função que a Thread 1 irá incrementar/Decrementar a temperatura com valores entre -10 e 40.
void* incrementador (void* p){
	printf("Sou a Thread 1 e irei modificar a temperatura\n");
	//Variável que recebe o aumento da temperatura do motor.
	float aumento = 0;

	//Enquanto a temperatura não tenha sido reduzida 5 vezes.
	while(contador != 5){
		//Aumento recebe um valor aleatório entre -10 e 40.
		aumento = ((float)(rand() % 500 - 100))/10;
		//Altera a temperatura de acordo com o valor obtido aleatoriamente.
		temperatura += aumento;
		printf("Thread 1: alterei em %.2fºC e agora a temperatura é %.2f\n", aumento, temperatura);
		//Dorme 300ms
		usleep(300000);
		//Trava a função incrementadora enquanto a vigia altera a temperatura
		//Garantindo que durante o reinicio da temperatura, ela não tente acessar ou alterar
		while(espere == 1);
	}
}

//Função que vigia a alteração da temperatura, mudando para 600 quando chegar a 1000.
void* vigia (void* p){
	printf("Sou a Thread 2 e vigiarei a temperatura\n");

	//Enquanto a temperatura não tenha reiniciado 5 vezes.
	while (contador != 5){
		
		//Fica em constante vigia até que a temperatura chegue em 1000.
		while(temperatura<1000);
		//Trava a função incrementadora, usando uma função atômica
		__sync_fetch_and_add(&espere, 1);
		//Altera a temperatura para 600
		temperatura = 600;
		//Adiciona 1 na contagem de vezes que a temperatura reiniciou, usando função atômica.
		__sync_fetch_and_add(&contador, 1);
		//Destrava a função incrementadora, usando função atômica.
		__sync_fetch_and_sub(&espere, 1);
		
		printf("Thread 2: Temperatura acima de 1000ºC pela %dº vez e diminuída para 600ºC\n", contador);
	}
}

int main(){
	//Vetor de variáveis de Thread para as duas threads usadas.
	pthread_t threads[2];

	//Variável utilizada para gerar números aleatórios.
  	time_t t;
   
  	//Inicialização do gerador de números aleatórios.
   	srand((unsigned) time(&t));

   	//As duas Threads são criadas, cada uma indo para sua função.
	pthread_create(&threads[0], NULL, incrementador, NULL);
	pthread_create(&threads[1], NULL, vigia, NULL);

	//Espera as duas Threads retornarem para encerrar o programa.
	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);

	printf("MAIN:A temperatura subiu até 1000ºC 5 vezes, encerrando...\n");
	
	return 0;
}