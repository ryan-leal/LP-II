#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>

/*A partir de várias execuções concluí que a criação
de processos demanda mais tempo do que a de Threads,
como estudado durante a disciplina, foi visto que ao clonar
um processo você copia grande quantidade parte do processo pai
diferenciando apenas pelo contexto do processo, o que é mais custoso
em memória e processamento, enquanto as threads ocorre o compartilhamento
de recursos, oque acaba gerando praticidade em algumas ocasiões e maior
desempenho*/

/*De forma simplificada, o processo seria uma execução do programa
enquanto a thread seria um segmento do programa, tendo assim as threads
um consumo menor, menor tempo de criação e facilidade de criação
já que o processo demanda tempo para copiar para outra área de memória
oque não é contexto do próprio processo.*/

//Função de print das Threads.
void *func(){
	printf("Nova Thread\n");
	sleep(2);
	pthread_exit(NULL);
}

int main(){
	pthread_t threads[30];//Array de Threads.
	long totalThreads = 0, totalProcessos = 0;//Variáveis que vão contar o tempo em microsegundos da criação das threads e processos.
	struct timeval timeThreads, timeProcessos;//Variáveis structs utilizadas pela função gettimeofday().
	pid_t odin = getpid();//Variável que vai armazenar o pid do processo pai de todos.
      
     

    //Somente o processo pai de todos vai entrar aqui.
	if (getpid() == odin)
	{
		//Obtendo o tempo antes de iniciar a criação de threads e armazenando em totalThreads.
		gettimeofday(&timeThreads, NULL);
    	totalThreads -= timeThreads.tv_usec;

    	//For para criação de trinta threads.
		for (int i = 0; i < 30; i++)
		{
			pthread_create(&threads[i], NULL, func, NULL);
		}
		//Obtendo o tempo depois de criar as 30 threads e obtendo o valor final em totalThreads.
		gettimeofday(&timeThreads, NULL);
		totalThreads += timeThreads.tv_usec;

		printf("Total em microsegundos para criação das 30 Threads: %li\n", totalThreads);
	}
    
	//Variável child para executar o fork e fazer a diferenciação do processo pai de todos.
	pid_t child = fork();
	pid_t grandchilds;

	//Somente processos filho entram aqui.
	if (child == 0)
	{
		//Obtendo o tempo antes de criar 30 processos e armazenando em totalProcessos.
		gettimeofday(&timeProcessos, NULL);
		totalProcessos -= timeProcessos.tv_usec;

		//For com criação dos 30 processos.
		for (int i = 0; i < 30; ++i)
		{
			//Armazenando para fazer a diferenciação de processos e executando o fork
			grandchilds = fork();
			//Só entra aqui se for filho do filho.
			if (grandchilds == 0)
			{
				printf("Novo processo\n");
				sleep(2);
				return 0;
			}
		}
		//Obtendo o valor final do tempo, calculando e armazenando em totalProcessos.
		gettimeofday(&timeProcessos, NULL);
		totalProcessos += timeProcessos.tv_usec;

		printf("Total em microsegundos para criação dos 30 processos: %li\n", totalProcessos);
		return 0;
	}

	sleep(65);
	return 0;
}