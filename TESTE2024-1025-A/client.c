/**
 * @file: client.c
 * @date: 2016-11-17
 * @author: autor
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

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

    /*
     * Inserir codigo do cliente
     */

    char *rbg=args.rbg_arg;
    char *filename=args.jpg_arg;

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

	
	// aqui... a comunicação com o servidor
	// UDP IPv4: variáveis auxiliares para sendto() / recvfrom()
	ssize_t udp_read_bytes, udp_sent_bytes;
        char hexa[256];

	// UDP IPv4: "sendto" para o servidor
	printf("a enviar dados para o servidor... "); fflush(stdout);
	if ((udp_sent_bytes = sendto(udp_client_socket,rbg, strlen(rbg), 0, (struct sockaddr *) &udp_server_endpoint, udp_server_endpoint_length)) == -1)
		ERROR(24, "Can't sendto server");
	printf("ok.  (%d bytes sent)\n", (int)udp_sent_bytes);

	// UDP IPv4: "recvfrom" do servidor (bloqueante)
	printf("à espera de dados do servidor... "); fflush(stdout);
	if ((udp_read_bytes = recvfrom(udp_client_socket,hexa, sizeof(hexa)-1, 0, (struct sockaddr *) &udp_server_endpoint, &udp_server_endpoint_length)) == -1)
		ERROR(25, "Can't recvfrom server");
	printf("ok.  (%d bytes received)\n", (int)udp_read_bytes);	

    hexa[udp_read_bytes]='\0';

    if(hexa[0]=='#'){

        char command[100];
        printf("Recived:%s \n",hexa);
        snprintf(command,sizeof(command),"magick -size 100x100 xc:%s %s",hexa,filename);
	
        int ret = system(command);

        if (ret == -1) {
            perror("system failed");
        }


    }else {
    printf("ELSE ATIVO!");
    }
	// UDP IPv4: fecha socket (client)
	if (close(udp_client_socket) == -1)
		ERROR(23, "Can't close udp_client_socket (IPv4)");



    cmdline_parser_free(&args);

    return 0;
}
