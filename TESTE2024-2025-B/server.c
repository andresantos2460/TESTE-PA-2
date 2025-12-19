#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#include "debug.h"
#include "common.h"
#include "server_opt.h"

int main(int argc, char *argv[]) {
    /* Estrutura gerada pelo utilitario gengetopt */
    struct gengetopt_args_info args;

    /* Processa os parametros da linha de comando */
    if (cmdline_parser(argc, argv, &args) != 0) {
        exit(ERR_ARGS);
    }

    if(args.port_arg < 1024 || args.port_arg > 65535){
        fprintf(stderr, "ERROR: port number invalid must be in [1024, 65535]\n");
        exit(1);
    }

    // UDP IPv4: cria socket
	int udp_server_socket;
	if ((udp_server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		ERROR(31, "Can't create udp_server_socket (IPv4)");

	// UDP IPv4: bind a IPv4/porto 
	struct sockaddr_in udp_server_endpoint;
	memset(&udp_server_endpoint, 0, sizeof(struct sockaddr_in));
	udp_server_endpoint.sin_family = AF_INET;
	udp_server_endpoint.sin_addr.s_addr = htonl(INADDR_ANY);  	// Todas as interfaces de rede
	udp_server_endpoint.sin_port = htons(args.port_arg);	// Server port
	if (bind(udp_server_socket, (struct sockaddr *) &udp_server_endpoint, sizeof(struct sockaddr_in)) == -1)
		ERROR(32, "Can't bind @udp_server_endpoint info");
		
    socklen_t udp_client_endpoint_length = sizeof(struct sockaddr_in);
    struct sockaddr_in udp_client_endpoint;
    ssize_t udp_read_bytes, udp_sent_bytes;

    /* string onde sera colocada a string antes da conversao */
    char tempBuffer[64];
    char strSendToClient[64];
    char unidadeOrigem, unidadeConversao;
    float valorTemperatura;
    char unidadePosConversao;
    float valorDivisao = 9.0/5.0;

    // aqui... a comunicação com o cliente
    while(1){
        // UDP IPv4: variáveis auxiliares para sendto() / recvfrom()
        // UDP IPv4: "recvfrom" do cliente (bloqueante)
        printf("à espera de dados do cliente... "); fflush(stdout);
        if ((udp_read_bytes = recvfrom(udp_server_socket, tempBuffer, sizeof(tempBuffer), 0, (struct sockaddr *) &udp_client_endpoint, &udp_client_endpoint_length)) == -1)
            ERROR(34, "Can't recvfrom client");
        printf("ok.  (%d bytes received)\n", (int)udp_read_bytes);

        printf("[SERVER] String received: '%s'\n", tempBuffer);

        sscanf(tempBuffer, "%f:%c:%c", &valorTemperatura, &unidadeOrigem, &unidadeConversao);
        unidadeOrigem    = toupper(unidadeOrigem);
        unidadeConversao = toupper(unidadeConversao);

        switch (unidadeOrigem) {
            case 'C':
                if(unidadeConversao == 'F'){
                    printf("C --> F\n");
                    valorTemperatura = valorTemperatura * valorDivisao + 32;
                    printf("[SERVER] Temperature converted to %.2fFº\n", valorTemperatura);
                    unidadePosConversao = 'F'; 
                }
                if(unidadeConversao == 'K'){
                    valorTemperatura = valorTemperatura+273.15;
                    printf("[SERVER] Temperature converted to %.2fKº\n", valorTemperatura);
                    unidadePosConversao = 'K'; 
                }
            break;
            case 'K':
                if(unidadeConversao == 'F'){
                    valorTemperatura = (valorTemperatura-273.15) * valorDivisao + 32;
                    printf("[SERVER] Temperature converted to %.2fFº\n", valorTemperatura);
                    unidadePosConversao = 'F'; 
                }
                if(unidadeConversao == 'C'){
                    valorTemperatura = valorTemperatura-273.15;
                    printf("[SERVER] Temperature converted to %.2fCº\n", valorTemperatura); 
                    unidadePosConversao = 'C'; 
                }
            break;
            case 'F':
                if(unidadeConversao == 'C'){
                    valorTemperatura = (valorTemperatura-32) * (5.0/9.0);
                    printf("[SERVER] Temperature converted to %.2fCº\n", valorTemperatura);
                    unidadePosConversao = 'C'; 
                }
                if(unidadeConversao == 'K'){
                    valorTemperatura = (valorTemperatura - 32) * (5.0/9.0) + 273.15;
                    printf("[SERVER] Temperature converted to %.2fKº\n", valorTemperatura);
                    unidadePosConversao = 'K'; 
                }
            break;
            default:
                printf("[SERVER] Im in default switch\n");
            break;
        }

        printf("[SERVER] Temperature converted\n");

        sprintf(strSendToClient,"%.2f º%c", valorTemperatura, unidadePosConversao);

        printf("[SERVER] String to send: '%s'\n", strSendToClient);

        // UDP IPv4: "sendto" para o cliente
        printf("a enviar dados para o cliente... "); fflush(stdout);
        if ((udp_sent_bytes = sendto(udp_server_socket, strSendToClient , strlen(strSendToClient), 0, (struct sockaddr *) &udp_client_endpoint, udp_client_endpoint_length)) == -1)
            ERROR(35, "Can't sendto client");
        printf("ok.  (%d bytes sent)\n", (int)udp_sent_bytes);
    }

	// liberta recurso: socket UDP IPv4
	if (close(udp_server_socket) == -1)
		ERROR(33, "Can't close udp_server_socket (IPv4)");

    cmdline_parser_free(&args);
    return 0;
}