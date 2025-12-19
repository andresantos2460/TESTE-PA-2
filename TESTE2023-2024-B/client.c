/**
 * @file: client.c
 * @date: 2016-11-17
 * @author: autor
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "memory.h"
#include "debug.h"
#include "common.h"
#include "client_opt.h"

int main(int argc, char *argv[]) {
    /* Estrutura gerada pelo utilitario gengetopt */
    struct gengetopt_args_info args;

    /* Processa os parametros da linha de comando */
    if (cmdline_parser(argc, argv, &args) != 0) {
        exit(ERR_ARGS);
    }

    srand(time(NULL));

    int numbers_to_generate=args.request_size_arg;

    /*
     * Inserir codigo do cliente
     */


//--- UDP Client :: ligação ------  ( ** não copiar este comentário ** )

	// UDP IPv4: cria socket
	int udp_client_socket;	
	if ((udp_client_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		ERROR(21, "Can't create udp_client_socket (IPv4)");
		
	// UDP IPv4: informação do servidor UDP
	socklen_t udp_server_endpoint_length = sizeof(struct sockaddr_in);	
	struct sockaddr_in udp_server_endpoint;
	memset(&udp_server_endpoint, 0, sizeof(struct sockaddr_in));
	udp_server_endpoint.sin_family = AF_INET;	
	switch (inet_pton(AF_INET, args.ip_arg, &udp_server_endpoint.sin_addr.s_addr)) {
		case 0:
			fprintf(stderr, "[%s@%d] ERROR - Cannot convert IP address (IPv4): Invalid Network Address\n",
				__FILE__, __LINE__);
			exit(22);
		case -1:
			ERROR(22, "Cannot convert IP address (IPv4)");
	}
	udp_server_endpoint.sin_port = htons(args.port_arg);

    if(numbers_to_generate<0||numbers_to_generate>10){
        ERROR(1,"NUMBER TO GENERATE INVALID\n");
    }



    uint16_t numbers_array[10];

    for(int i=0;i<numbers_to_generate;i++){
        numbers_array[i]=(rand()%65536);

    }


    printf("Sending:");
    for(int i=0;i<numbers_to_generate;i++){
        printf(" %04X ",numbers_array[i]);

    }
    // UDP IPv4: variáveis auxiliares para sendto() / recvfrom()
    ssize_t udp_read_bytes, udp_sent_bytes;
    uint8_t recv_buffer[10];
    // UDP IPv4: "sendto" para o servidor
    fflush(stdout);
    if ((udp_sent_bytes = sendto(udp_client_socket,numbers_array, numbers_to_generate*sizeof(uint16_t), 0, (struct sockaddr *) &udp_server_endpoint, udp_server_endpoint_length)) == -1)
        ERROR(24, "Can't sendto server");

    // UDP IPv4: "recvfrom" do servidor (bloqueante)
    fflush(stdout);
    if ((udp_read_bytes = recvfrom(udp_client_socket,recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr *) &udp_server_endpoint, &udp_server_endpoint_length)) == -1)
        ERROR(25, "Can't recvfrom server");

    printf("\n");
    printf("received:");
    for(int i=0;i<numbers_to_generate;i++){
        printf(" %d ",recv_buffer[i]);
        
    }

	// UDP IPv4: fecha socket (clierecv_buffer
	if (close(udp_client_socket) == -1)
		ERROR(23, "Can't close udp_client_socket (IPv4)");



    cmdline_parser_free(&args);

    return 0;
}
