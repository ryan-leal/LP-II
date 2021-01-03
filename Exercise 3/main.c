#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>

//Porta do servidor, em caso de problema na porta, mude o valor.
#define PORTA_SERVER 9999

//variável do Socket do servidor em forma de var global.
int servidorfd;

//Função que verificará se é palíndromo.
int ehPalindromo(char* palavra, int tamanho){
    //Arrray que receberá a string "ao contrário".
    char contrario[tamanho+1];
    //Coloca o caractere Nulo na string contrario.
    contrario[tamanho] = '\0';
    //Adiciona o \n para ficar no mesmo padrão que a string que chega do cliente.
    contrario[tamanho-1] = '\n';

    //For que irá inserir as letras na string contrario.
    for (int i = 0; i < (tamanho-1); i++)
    {
        //As letras seram invertidas de posição.
        contrario[i] = palavra[(tamanho-2)-i];
    }

    //Se as strings forem iguais...
    if (strcmp(palavra, contrario) == 0)
    {
        return 1;
    } else{
        //Se não forem iguais...
        return 0;
    }
}

//Função que será executada pelas Threads, tratando os clientes.
void* handleCliente(void* p){
    //Converte a variável do socket passada como parâmetro, para tipo.
    int clientefd = (int) p;

    //Variável usada para manter a Thread tratando o cliente até o quanto desejado.
    int finalizado = 0;

    //Criação de uma string para as mensagens que serão enviadas para o cliente.
    char *msg = "\nDIGITE UMA PALAVRA(10): \n";

    //Buffer que vai receber a palavra enviada pelo cliente.
    char buf[11];

    //Enquanto ainda não for sinalizado, a thread continua tratando...
     while (!finalizado)
    {
        //Zera os campos do buffer.
        bzero(buf, 11);

        //A cada iteração, altera a mensagem para enviar uma sinalização para digitar.
        msg = "\nDIGITE UMA PALAVRA(10): \n";
        write(clientefd, msg, strlen(msg));

        //Recebe a palavra digitada pelo cliente.
        read(clientefd, buf, 11);

        //Se houver a mensagem de encerramento...
        if (strstr(buf, "BYEBYE") != NULL)
        {
            //Sinaliza o encerramento do tratamento...
            msg = "ESTOU DESCONECTANDO...";
            write(clientefd, msg, strlen(msg));
            printf("CLIENTE %d DESCONECTADO\n", clientefd);
            finalizado = 1;
            close(clientefd);
            pthread_exit(0);
            
        }
        //Se houver a mensagem de encerramento do server...
        if (strstr(buf, "SHUTDOWN") != NULL)
        {
            msg = "SOLICITANDO DESLIGAMENTO DO SERVIDOR...";
            write(clientefd, msg, strlen(msg));
            printf("CLIENTE %d SOLICITOU O DESLIGAMENTO\n", clientefd);
            //Fecha a conexão do cliente.
            close(clientefd);
            //Encerra o servidor.
            exit(0);        
        }

        //Se for enviada uma mensagem válida e o programa não estiver sinalizado para encerrar...
        if ((strlen(buf) > 0) && (finalizado!=1)){
            printf("Mensagem do cliente %d: %s", clientefd, buf);

            //Variável recebe o resultado da verificação se é palíndromo
            //A função recebe como parâmetro a palavra do cliente e o tamanho dela.
            int palind = ehPalindromo(buf, strlen(buf));

            //Se for palíndromo...
            if (palind == 1)
            {
                //Envia uma mensagem ao cliente indicando que é palíndromo.
                msg = "\nPALINDROMO=TRUE\n";
                write(clientefd, msg, strlen(msg));
            }else{
                //Se não for palíndromo, envia uma mensagem para o cliente indicando que não é.
                msg = "\nPALINDROMO=FALSE\n";
                write(clientefd, msg, strlen(msg));
            }
        }   
    }

    //Fecha conexão do cliente agora que o cliente finalizou.
    close(clientefd);
    //Thread encerra.
    pthread_exit(0);

}

int main(void)
{
    //Threads que irão tratar os clientes.
    pthread_t threadsClientes[10];

    //Variáveis do socket do servidor e do cliente.
    int clientefd = 0;

    //Tamanho do endereço do cliente.
    int sizeEndCLiente;

    //Structs que armazenam as informações de endereço do cliente e do servidor.
    struct sockaddr_in endServidor, endCliente;

    //Criação do socket do servidor.
    servidorfd = socket(AF_INET, SOCK_STREAM, 0);

    //Se der erro na criação do socket, encerra o programa.
    if (servidorfd == -1)
    {
        printf("Erro na criação do Socket do Servidor!\n");
        exit(0);
    }

    //Atribui zero aos campos da struct do endereço do servidor.
    bzero(&endServidor, sizeof(endServidor));

    //Atribui a família que o servidor terá.
    endServidor.sin_family = AF_INET;
    //Atribui o endereço a qual vai se conectar, nesse caso, algum endereço (já que o servidor não sabe)
    endServidor.sin_addr.s_addr = htonl(INADDR_ANY);
    //Atribui o valor da porta utilizada em Big Endian.
    endServidor.sin_port = htons(PORTA_SERVER);

    //Faz a atribuição das propriedades inicializadas e verifica se deu certo, se não deu: encerra o programa.
    if (bind(servidorfd, ((struct sockaddr *)&endServidor), sizeof(endServidor)) != 0)
    {
        printf("Erro na atribuição do socket servidor!\n");
        exit(0);
    }

    //Tenta "Escutar" na porta solicitada, se não der certo encerra o programa.
    if (listen(servidorfd, 4) != 0)
    {
        printf("Erro: Servidor não conseguiu ouvir nada porta %d\n", PORTA_SERVER);
        exit(0);
    }

    printf("Servidor está ouvindo na porta %d\n", PORTA_SERVER);

    //Será utilizada para indicar a thread para cada cliente de acordo com a chegada.
    int t_idx = 0;

    //Ponteiro de char que vai servir para receber o IP do cliente conectado.
    char* ip;

    //Enquanto não houver erro no "aceite" de clientes...
    while(!(clientefd < 0)){
        //Aceita conexões que chegarem até ele.
        clientefd = accept(servidorfd, ((struct sockaddr *)&endCliente), &sizeEndCLiente);
        printf("Conexão recebida!\n");

        //Mensagem de Boas vindas é criada e enviada para o cliente conectado.
        char *msg = "\nBEMVINDO\n";
        write(clientefd, msg, strlen(msg));

        //Converte o IP para o formato de string e printa no servidor.
        ip = inet_ntoa(endCliente.sin_addr);
        printf("IP do cliente conectado: %s\n", ip);

        //Cria as Threads para executarem o tratamento e passam a variável de conexão do cliente.
        pthread_create(&threadsClientes[t_idx], NULL, handleCliente, (void *)clientefd);

        //controla o index usado na criação das Threads.
        t_idx++;
        t_idx = t_idx % 10;
    }

}