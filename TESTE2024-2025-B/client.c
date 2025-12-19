#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

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
    
    /* verificacao de argumentos */
    if(args.port_arg < 1024 || args.port_arg > 65535){
        fprintf(stderr, "ERROR: Port number invalid must be in [1024, 65535]\n");
        exit(1);
    }
    if(strlen(args.to_arg) > 1){
        fprintf(stderr, "ERROR: invalida argument --to\n");
        exit(1);
    }
    if(strlen(args.from_arg) > 1){
        fprintf(stderr, "ERROR: invalida argument --from\n");
        exit(1);
    }
    args.from_arg[0] = toupper(args.from_arg[0]);
    args.to_arg[0] = toupper(args.to_arg[0]);

    if (strcmp(args.from_arg, "F") != 0 && 
        strcmp(args.from_arg, "K") != 0 && 
        strcmp(args.from_arg, "C") != 0) {
        fprintf(stderr, "ERROR: invalid unit '%s' for --from. Use C, F or K.\n", args.from_arg);
        exit(1);
    }

    if (strcmp(args.to_arg, "F") != 0 && 
        strcmp(args.to_arg, "K") != 0 && 
        strcmp(args.to_arg, "C") != 0) {
        fprintf(stderr, "ERROR: invalid unit '%s' for --to. Use C, F or K.\n", args.to_arg);
        exit(1);
    }

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

	
	// UDP IPv4: variáveis auxiliares para sendto() / recvfrom()
	ssize_t udp_read_bytes, udp_sent_bytes;
	char buffer[64];

    sprintf(buffer, "%.2f:%s:%s", args.value_arg, args.from_arg, args.to_arg);

	// UDP IPv4: "sendto" para o servidor
	printf("a enviar dados para o servidor... "); fflush(stdout);
	if ((udp_sent_bytes = sendto(udp_client_socket, buffer, sizeof(buffer), 0, (struct sockaddr *) &udp_server_endpoint, udp_server_endpoint_length)) == -1)
		ERROR(24, "Can't sendto server");
	printf("ok.  (%d bytes sent)\n", (int)udp_sent_bytes);

    char bufferRecv[64];

	// UDP IPv4: "recvfrom" do servidor (bloqueante)
	printf("à espera de dados do servidor... "); fflush(stdout);
	if ((udp_read_bytes = recvfrom(udp_client_socket, bufferRecv, sizeof(bufferRecv), 0, (struct sockaddr *) &udp_server_endpoint, &udp_server_endpoint_length)) == -1)
		ERROR(25, "Can't recvfrom server");
	printf("ok.  (%d bytes received)\n", (int)udp_read_bytes);	

    printf("[CLIENT] Received %s from Server\n", bufferRecv);

	// UDP IPv4: fecha socket (client)
	if (close(udp_client_socket) == -1)
		ERROR(23, "Can't close udp_client_socket (IPv4)");

    cmdline_parser_free(&args);
    return 0;
}
