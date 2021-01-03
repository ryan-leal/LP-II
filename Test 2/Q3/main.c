/*
    -Diferença entre TCP e UDP:
    No TCP temos um canal de comunicção confiável, onde o protocolo
    confirma a entrega da mensagem, como consequência temos um volume
    maior de dados, mas estabelece uma conexão mais confiável, enquanto
    no UDP, não temos a confirmação e ordem da entrega, as mensagens são
    independentes, além de não estabelecer conexão e não ter um canal de
    comunicação confiável.

    -Chamadas feitas no código são necessárias no contexto específico do TCP:
    Nessa implementação, são necessárias para o TCP:
    *clientefd = socket(AF_INET, SOCK_STREAM, 0);, aqui nós dizemos que é um
    socket stream, que é o tipo de socket TCP.
    *write(clientefd, myMsg, strlen(myMsg) + 1); - aqui temos o envio utilizando
    TCP, se estivessemos usando UDP enviariamos com sendto();.
    *read(clientefd, buf, 20); - aqui temos a função de recebimento de mensagens
    para socket TCP.
*/
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

//Define a porta utilizada
#define PORTA_SERV 9999

//Variável que vai receber o socket do cliente e variável que indica quando o programa finaliza.
int clientefd, finalizado = 0;

//Função que a Thread que espera mensagem do servidor vai executar
void* vigia(void* p){
    printf("Sou a Thread Vigia e vou esperar o servidor enviar algo!\n");
    //Buffer que vai receber oque o servidor mandar
    char buf[21];
    //Enquanto não terminar de enviar as 6 strings...
    while(!finalizado){
        //Zera o buffer
        bzero(buf, 20);
        //Espera mensagem do servidor.
        read(clientefd, buf, 20);

        //Caso o tamanho do buffer seja maior que zero, retorna oque foi recebido pelo servidor.
        if (strlen(buf) > 0)
            printf("Recebido do Servidor: %s\n", buf);
    }
}

int main()
{
    //Struct contendo as informações de endereço e porta do servidor.
    struct sockaddr_in endServer;

    //String que vai armazenar os caracteres gerados.
    char myMsg[9];

    //Variável utilizada para gerar números aleatórios.
    time_t t;
   
    //Inicialização do gerador de números aleatórios.
    srand((unsigned) time(&t));

    //Thread que vai vigiar o recebimento de mensagens
    pthread_t threadVigia;

    //Criação do socket TCP do cliente
    clientefd = socket(AF_INET, SOCK_STREAM, 0);

    if (clientefd == -1){
        //Caso haja algum erro durante a criação...
        printf("ERRO NA CRIAÇÃO DO SOCKET CLIENTE!");
        exit(0);
    }

    //Campos com informação do servidor são zerados.
    bzero(&endServer, sizeof(endServer));

    //Campos do servidor são inicializados, com a família do socket...
    endServer.sin_family = AF_INET;
    //endereço IP do server é inicializado como local
    endServer.sin_addr.s_addr = inet_addr("127.0.0.1");
    //Porta do servidor é convertida para Big Endian.
    endServer.sin_port = htons(PORTA_SERV);

    //Associa os atributos ao socket.
    bind(clientefd, ((struct sockaddr_in *)&endServer), sizeof(endServer));

    //Requisição de conexão do cliente ao servidor, é aramazenado então na var con_req.
    int con_req = connect(clientefd, (struct sockaddr_in *)&endServer, sizeof(endServer));

    if (con_req != 0){
        //Caso a conexão com o servidor falhe...
        printf("ERRO NA CONEXÃO");
        exit(0);
    }

    printf("CLIENTE CONECTADO NA PORTA %d\n", PORTA_SERV);



    //Criação da Thread que vai vigiar o recebimento de mensagens.
    pthread_create(&threadVigia, NULL, vigia, NULL);

    //Enquanto não terminar o envio das 6 strings aleatórias.
    while (!finalizado)
    {

        //Caractere nulo é atribuido a mensagem para dar o final da string
        myMsg[8] = '\0';

        //roda 10 vezes a criação da string aleatória...
        for (int i = 0; i < 10; i++)
        {
            //Cria uma string aleatória.
            for (int j = 0; j < 6; j++)
            {
                //Caracteres sendo gerados aleatoriamente.
                myMsg[j] = (rand() % 25 + 65);
            }

            //\n para pular linha.
            myMsg[7] = '\n';

            //Envia a string para o servidor.
            write(clientefd, myMsg, strlen(myMsg) + 1);
            //Dorme 1 segundo
            sleep(1);
        }

        //Ao enviar todas as strings aleatórias, sinaliza para encerrar.
        finalizado = 1;
    }
}