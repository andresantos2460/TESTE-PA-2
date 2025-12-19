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
typedef struct pedido{
    uint32_t n_estudantes;
    uint32_t telefone;
    uint8_t operacao;
}pedido_t;

typedef struct students_contacts{
    uint32_t n_estudantes;
    uint32_t telefone;
}students_contacts_t;

int main(int argc, char *argv[]) {
    /* Estrutura gerada pelo utilitario gengetopt */
    struct gengetopt_args_info args;

    /* Processa os parametros da linha de comando */
    if (cmdline_parser(argc, argv, &args) != 0) {
        exit(ERR_ARGS);
    }



    if (args.port_arg < 1024 || args.port_arg > 65535) {
        ERROR(10, "Invalid port number");
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
		
	
	// aqui... a comunicação com o cliente
	


    // UDP IPv4: variáveis auxiliares para sendto() / recvfrom()
    socklen_t udp_client_endpoint_length = sizeof(struct sockaddr_in);
    struct sockaddr_in udp_client_endpoint;
    ssize_t udp_read_bytes, udp_sent_bytes;
    //char buffer[];
    //...
    students_contacts_t estrututra[200];
    pedido_t pedido;
    int total_conatctos=0;

    while(1){


        //=============CODIGO ANTES DE RECEBER=========//



        // UDP IPv4: "recvfrom" do cliente (bloqueante)
        printf("à espera de dados do cliente... "); fflush(stdout);
        if ((udp_read_bytes = recvfrom(udp_server_socket,&pedido, sizeof(pedido), 0, (struct sockaddr *) &udp_client_endpoint, &udp_client_endpoint_length)) == -1)
            ERROR(34, "Can't recvfrom client");
        printf("ok.  (%d bytes received)\n", (int)udp_read_bytes);



        //===========CODIGO DEPOIS DE RECEBER===========//

    

        uint32_t estudante=ntohl(pedido.n_estudantes);
        uint32_t phone=ntohl(pedido.telefone);
        uint8_t operacao=pedido.operacao;

        if(operacao!=1&&operacao!=2){
                // UDP IPv4: "sendto" para o cliente
        uint32_t codigo=0;
        codigo=htonl(codigo);
        printf("a enviar dados para o cliente... "); fflush(stdout);
        if ((udp_sent_bytes = sendto(udp_server_socket,&codigo, sizeof(codigo), 0, (struct sockaddr *) &udp_client_endpoint, udp_client_endpoint_length)) == -1)
            ERROR(35, "Can't sendto client");
        printf("ok.  (%d bytes sent)\n", (int)udp_sent_bytes);

        }
       

       if(operacao==1){

        if(estudante<2000000||estudante>=3000000){
            uint32_t codigo=4;
            codigo=htonl(codigo);
            printf("a enviar dados para o cliente... "); fflush(stdout);
            if ((udp_sent_bytes = sendto(udp_server_socket,&codigo, sizeof(codigo), 0, (struct sockaddr *) &udp_client_endpoint, udp_client_endpoint_length)) == -1)
                ERROR(35, "Can't sendto client");
            printf("ok.  (%d bytes sent)\n", (int)udp_sent_bytes);
        }

            if(phone<900000000||phone>1000000000){
            uint32_t codigo=5;
            codigo=htonl(codigo);
            printf("a enviar dados para o cliente... "); fflush(stdout);
            if ((udp_sent_bytes = sendto(udp_server_socket,&codigo, sizeof(codigo), 0, (struct sockaddr *) &udp_client_endpoint, udp_client_endpoint_length)) == -1)
                ERROR(35, "Can't sendto client");
            printf("ok.  (%d bytes sent)\n", (int)udp_sent_bytes);
        }
        if(total_conatctos>=200){
            uint32_t codigo=3;
            codigo=htonl(codigo);
       
        printf("a enviar dados para o cliente... "); fflush(stdout);
        if ((udp_sent_bytes = sendto(udp_server_socket,&codigo, sizeof(codigo), 0, (struct sockaddr *) &udp_client_endpoint, udp_client_endpoint_length)) == -1)
            ERROR(35, "Can't sendto client");
        printf("ok.  (%d bytes sent)\n", (int)udp_sent_bytes);
        }
        estrututra[total_conatctos].n_estudantes=estudante;
        estrututra[total_conatctos].telefone=phone;
        total_conatctos++;

        uint32_t codigo=1;
        codigo=htonl(codigo);
       
        printf("a enviar dados para o cliente... "); fflush(stdout);
        if ((udp_sent_bytes = sendto(udp_server_socket,&codigo, sizeof(codigo), 0, (struct sockaddr *) &udp_client_endpoint, udp_client_endpoint_length)) == -1)
            ERROR(35, "Can't sendto client");
        printf("ok.  (%d bytes sent)\n", (int)udp_sent_bytes);
       }



        if(operacao==2){
            uint32_t codigo=1;

            for(int i=0;i<total_conatctos;i++){
                if (estrututra[i].n_estudantes==estudante)
                {
                    codigo=estrututra[i].telefone;
                }
                
            }
            codigo=htonl(codigo);

            printf("a enviar dados para o cliente... "); fflush(stdout);
        if ((udp_sent_bytes = sendto(udp_server_socket,&codigo, sizeof(codigo), 0, (struct sockaddr *) &udp_client_endpoint, udp_client_endpoint_length)) == -1)
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
