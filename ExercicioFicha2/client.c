/**
 * @file: client.c
 * @date: 2016-11-17
 * @author: autor
 */
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include "memory.h"
#include "debug.h"
#include "common.h"
#include "client_opt.h"
#define MAX_ATTEMPTS (3)
#define MSG_DONTWAIT 0x40

struct timeval timeout;
int main(int argc, char *argv[]) {
    /* Estrutura gerada pelo utilitario gengetopt */
    struct gengetopt_args_info args;

    /* Processa os parametros da linha de comando */
    if (cmdline_parser(argc, argv, &args) != 0) {
        exit(ERR_ARGS);
    }

    int p_timeout=args.time_arg;
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
	switch (inet_pton(AF_INET,args.ip_arg, &udp_server_endpoint.sin_addr.s_addr)) {
		case 0:
			fprintf(stderr, "[%s@%d] ERROR - Cannot convert IP address (IPv4): Invalid Network Address\n",
				__FILE__, __LINE__);
			exit(22);
		case -1:
			ERROR(22, "Cannot convert IP address (IPv4)");
	}
	udp_server_endpoint.sin_port = htons(args.port_arg);

    timeout.tv_sec = p_timeout;
    timeout.tv_usec = 0;
    int setsock_ret = setsockopt(udp_client_socket,SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    if(setsock_ret == -1 ){
        ERROR(EXIT_FAILURE,"Cannot setsockopt 'SO_RCVTIMEO'");
    }
	
	// aqui... a comunicação com o servidor

    // UDP IPv4: variáveis auxiliares para sendto() / recvfrom()
    ssize_t udp_read_bytes, udp_sent_bytes;
    char buffer[256]="PING";
    char recv_buffer[256];
    //...

    // UDP IPv4: "sendto" para o servidoj
    printf("a enviar dados para o servidor... "); fflush(stdout);
    if ((udp_sent_bytes = sendto(udp_client_socket,buffer,strlen(buffer), 0, (struct sockaddr *) &udp_server_endpoint, udp_server_endpoint_length)) == -1)
        ERROR(24, "Can't sendto server");
    printf("ok.  (%d bytes sent)\n", (int)udp_sent_bytes);


    //RECEIVE 
    int received = 0;
    int num_attempts = 0;
    while (!received) {

        num_attempts++;
        // UDP IPv4: "sendto" para o servidoj
        printf("a enviar dados para o servidor... "); fflush(stdout);
        if ((udp_sent_bytes = sendto(udp_client_socket,buffer,strlen(buffer), 0, (struct sockaddr *) &udp_server_endpoint, udp_server_endpoint_length)) == -1)
            ERROR(24, "Can't sendto server");
        printf("ok.  (%d bytes sent)\n", (int)udp_sent_bytes);



        udp_read_bytes = recvfrom(
                udp_client_socket,
                recv_buffer,
                sizeof(recv_buffer) - 1,
                MSG_DONTWAIT,
                (struct sockaddr *) &udp_server_endpoint,
                &udp_server_endpoint_length
                );

        if (udp_read_bytes == -1) {

            if (errno == EWOULDBLOCK) {

                if (num_attempts >= MAX_ATTEMPTS) {
                    fprintf(stderr,
                            "[CLIENT] Timeout at recvfrom (%d attempts)\n",
                            num_attempts);
                    exit(EXIT_FAILURE);
                } else {
                    printf("[CLIENT] attempt %d\n", num_attempts);
                    sleep(1);
                }

            } else {
                ERROR(EXIT_FAILURE, "Cannot recv server");
            }

        } else {
            // successful read
            received = 1;
            recv_buffer[udp_read_bytes]='\0';

            printf("%s \n",recv_buffer);
            if(strcmp(buffer,"PONG")==0){

                printf("%s \n",recv_buffer);


            }
        }
    }
	// UDP IPv4: fecha socket (client)
	if (close(udp_client_socket) == -1)
		ERROR(23, "Can't close udp_client_socket (IPv4)");


	
	
	


    cmdline_parser_free(&args);

    return 0;
}
