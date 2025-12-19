/**
 * @file: server.c
 * @date: 2016-11-17
 * @author: autor
 */
#include <stdint.h>
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
#include <math.h>
int main(int argc, char *argv[]) {
    /* Estrutura gerada pelo utilitario gengetopt */
    struct gengetopt_args_info args;

    /* Processa os parametros da linha de comando */
    if (cmdline_parser(argc, argv, &args) != 0) {
        exit(ERR_ARGS);
    }

    int seed=args.seed_arg;

    if(seed==-1){
        seed=1;
    }
    if(seed<0||seed>(int)(pow(2.0,16.0))-1){

        ERROR(1,"SEED Inváilda!");
    }
    /*
     * Inserir codigo do servidor
     */

    srand(seed);
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
    char buffer[256];
    while (1) {
    
	// UDP IPv4: "recvfrom" do cliente (bloqueante)
    //...
	if ((udp_read_bytes = recvfrom(udp_server_socket,buffer, sizeof(buffer)-1, 0, (struct sockaddr *) &udp_client_endpoint, &udp_client_endpoint_length)) == -1)
		ERROR(34, "Can't recvfrom client");
	printf("[SVC] Received '%s':",buffer);


    buffer[udp_read_bytes]='\0';
    if(strcmp(buffer,"N")==0){

        int numero=(rand()%50)+1;

        uint16_t net_numero=htons(numero);

        if ((udp_sent_bytes = sendto(udp_server_socket, &net_numero, sizeof(net_numero), 0, (struct sockaddr *) &udp_client_endpoint, udp_client_endpoint_length)) == -1)
            ERROR(35, "Can't sendto client");
        printf("sending %d \n",numero);
    }


    if(strcmp(buffer,"E")==0){

        int numero=(rand()%12)+1;

        uint16_t net_numero=htons(numero);

        if ((udp_sent_bytes = sendto(udp_server_socket, &net_numero, sizeof(net_numero), 0, (struct sockaddr *) &udp_client_endpoint, udp_client_endpoint_length)) == -1)
            ERROR(35, "Can't sendto client");
        printf("sending %d \n",numero);
    }




    // UDP IPv4: "sendto" para o cliente

    }
	// liberta recurso: socket UDP IPv4
	if (close(udp_server_socket) == -1)
		ERROR(33, "Can't close udp_server_socket (IPv4)");
	
	
	
	
    cmdline_parser_free(&args);

    return 0;
}
