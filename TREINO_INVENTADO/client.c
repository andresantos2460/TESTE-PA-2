/**
 * @file: client.c
 * @date: 2016-11-17
 * @author: autor
 */
#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "memory.h"
#include "debug.h"
#include "common.h"
#include "client_opt.h"
#define MAX_ATTEMPTS (3)
#define TIMEOUT (3)
#define MSG_DONTWAIT 0x40


struct timeval timeout;


int main(int argc, char *argv[]) {
    /* Estrutura gerada pelo utilitario gengetopt */
    struct gengetopt_args_info args;

    /* Processa os parametros da linha de comando */
    if (cmdline_parser(argc, argv, &args) != 0) {
        exit(ERR_ARGS);
    }
	
	int request=args.request_arg;
	uint16_t vetor_envio[MAX_BUFFER];

	for(int i=0;i<request;i++){
		int random=rand()%65536;
		vetor_envio[i]=htons(random);
		printf("%04X ",random);
	}


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


	timeout.tv_sec = TIMEOUT;
	timeout.tv_usec = 0;
	int setsock_ret = setsockopt(udp_client_socket,SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	if(setsock_ret == -1 ){
		ERROR(EXIT_FAILURE,"Cannot setsockopt 'SO_RCVTIMEO'");
	}



	// UDP IPv4: variáveis auxiliares para sendto() / recvfrom()
	ssize_t udp_read_bytes, udp_sent_bytes;
	//char buffer[];

    
 //RECEIVE 
    int received = 0;
    int num_attempts = 0;
	uint8_t vetor_recebido[MAX_BUFFER];

	printf("\n");
    while (!received) {

        num_attempts++;
        // UDP IPv4: "sendto" para o servidoj
        printf("a enviar dados para o servidor... "); fflush(stdout);
        if ((udp_sent_bytes = sendto(udp_client_socket,vetor_envio,request*sizeof(uint16_t), 0, (struct sockaddr *) &udp_server_endpoint, udp_server_endpoint_length)) == -1)
            ERROR(24, "Can't sendto server");
        printf("ok.  (%d bytes sent)\n", (int)udp_sent_bytes);



        udp_read_bytes = recvfrom(
                udp_client_socket,
                vetor_recebido,request*sizeof(uint8_t),
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
                    printf("[CLIENT] attempt %d\n", num_attempts+1);
                    sleep(1);
                }

            } else {
                ERROR(EXIT_FAILURE, "Cannot recv server");
            }

        } else {
            // successful read
            received = 1;
        	printf("ok.  (%d bytes received)\n", (int)udp_read_bytes);

			for (int i = 0; i < request; i++)
			{
				/* code */
				printf("%d ",vetor_recebido[i]);
			}
			

         
        }
    }
	
    //===============CODIGO DEPOIS DE RECEBER ===============//
	
	// UDP IPv4: fecha socket (client)
	if (close(udp_client_socket) == -1)
		ERROR(23, "Can't close udp_client_socket (IPv4)");

    cmdline_parser_free(&args);

    return 0;
}
