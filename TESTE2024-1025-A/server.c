/**
 * @file: server.c
 * @date: 2016-11-17
 * @author: autor
 */
#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
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
		


    // UDP IPv4: variáveis auxiliares para sendto() / recvfrom()
    socklen_t udp_client_endpoint_length = sizeof(struct sockaddr_in);
    struct sockaddr_in udp_client_endpoint;
    ssize_t udp_read_bytes, udp_sent_bytes;
    char buffer[256];
    //...

    while (1) {
    // UDP IPv4: "recvfrom" do cliente (bloqueante)
    printf("à espera de dados do cliente... "); fflush(stdout);
    if ((udp_read_bytes = recvfrom(udp_server_socket,buffer, sizeof(buffer)-1, 0, (struct sockaddr *) &udp_client_endpoint, &udp_client_endpoint_length)) == -1)
        ERROR(34, "Can't recvfrom client");
    printf("ok.  (%d bytes received)\n", (int)udp_read_bytes);

    buffer[udp_read_bytes]='\0';
    int valid=0;
    
    char hex[10];
    int r, g, b;

    // Extrai 3 números separados por vírgula
    if (sscanf(buffer, "%d,%d,%d", &r, &g, &b) != 3) {
        valid=1;
    }

    if(r<0||r>255||g<0||g>255||b<0||b>255){
        valid=1;
    }

    // Converte para hexadecimal com dois dígitos cada
    if(valid==0){
        sprintf(hex, "#%02X%02X%02X", r, g, b);
        printf("a enviar dados para o cliente... "); fflush(stdout);
        if ((udp_sent_bytes = sendto(udp_server_socket,hex,strlen(hex), 0, (struct sockaddr *) &udp_client_endpoint, udp_client_endpoint_length)) == -1)
            ERROR(35, "Can't sendto client");
        printf("ok.  (%d bytes sent)\n", (int)udp_sent_bytes);
        printf("%s \n",hex);
    }else{

        char ErrorMessage[256];
        strcpy(ErrorMessage,"ERROR IN STRING");
        if ((udp_sent_bytes = sendto(udp_server_socket,ErrorMessage,strlen(ErrorMessage), 0, (struct sockaddr *) &udp_client_endpoint, udp_client_endpoint_length)) == -1)
            ERROR(35, "Can't sendto client");
        printf("ok.  (%d bytes sent)\n", (int)udp_sent_bytes);

    }





	
    }

	// liberta recurso: socket UDP IPv4
	if (close(udp_server_socket) == -1)
		ERROR(33, "Can't close udp_server_socket (IPv4)");
	
	


    cmdline_parser_free(&args);

    return 0;
}
