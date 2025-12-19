/**
 * @file: client.c
 * @date: 2016-11-17
 * @author: autor
 */
#include <stdint.h>
#include <stdio.h>
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

    int numeros=0;
    int numeros_array[5]={0};

    while (numeros!=5) {

        char buffer[8]="N";
        uint16_t received_value;
        //...

        printf("Sending 'N \n");
        // UDP IPv4: "sendto" para o servidor
        if ((udp_sent_bytes = sendto(udp_client_socket,buffer,strlen(buffer), 0, (struct sockaddr *) &udp_server_endpoint, udp_server_endpoint_length)) == -1)
            ERROR(24, "Can't sendto server");

        // UDP IPv4: "recvfrom" do servidor (bloqueante)
        if ((udp_read_bytes = recvfrom(udp_client_socket,&received_value, sizeof(received_value), 0, (struct sockaddr *) &udp_server_endpoint, &udp_server_endpoint_length)) == -1)
            ERROR(25, "Can't recvfrom server");

        uint16_t resultado = ntohs(received_value);

        int exist=0;
        for (int i=0; i<numeros; i++) {
        
            if(resultado==numeros_array[i]){
                exist=1;
            }
        }

        if(exist==0){
            printf("received %d – NUMBERS:",resultado);
            numeros_array[numeros]=resultado;

            for (int i=0; i<=numeros; i++) {

                printf(" %d ",numeros_array[i]);
            }
            printf("\n");
            numeros++;
        }else{

        printf("received %d –duplicated – discarding it \n",resultado);

        }

    }
    int estrelas=0;
    int estrelas_array[2]={0};

    while (estrelas!=2) {

        char buffer[8]="E";
        uint16_t received_value;
        //...

        printf("Sending 'E' \n");
        // UDP IPv4: "sendto" para o servidor
        if ((udp_sent_bytes = sendto(udp_client_socket,buffer,strlen(buffer), 0, (struct sockaddr *) &udp_server_endpoint, udp_server_endpoint_length)) == -1)
            ERROR(24, "Can't sendto server");

        // UDP IPv4: "recvfrom" do servidor (bloqueante)
        if ((udp_read_bytes = recvfrom(udp_client_socket,&received_value, sizeof(received_value), 0, (struct sockaddr *) &udp_server_endpoint, &udp_server_endpoint_length)) == -1)
            ERROR(25, "Can't recvfrom server");

        uint16_t resultado = ntohs(received_value);

        int exist=0;
        for (int i=0; i<estrelas; i++) {
        
            if(resultado==estrelas_array[i]){
                exist=1;
            }
        }

        if(exist==0){
            printf("received %d – STARS:",resultado);
            estrelas_array[estrelas]=resultado;

            for (int i=0; i<=estrelas; i++) {

                printf(" %d ",estrelas_array[i]);
            }
            printf("\n");
            estrelas++;
        }else{

        printf("received %d –duplicated – discarding it \n",resultado);

        }

    }


    printf("[CLNT] FINSIHED\n");
    printf("[CLNT] NUMBERS\n");
    for (int i=0; i<5; i++) {

        printf(" %d ",numeros_array[i]);
    }
    printf("\n");
    printf("[CLNT] STARS\n");
    for (int i=0; i<2; i++) {

        printf(" %d ",estrelas_array[i]);
    }
    // UDP IPv4: fecha socket (client)
    if (close(udp_client_socket) == -1)
        ERROR(23, "Can't close udp_client_socket (IPv4)");
    



    cmdline_parser_free(&args);

    return 0;
}
