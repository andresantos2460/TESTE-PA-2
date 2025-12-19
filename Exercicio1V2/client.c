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
#include <unistd.h>
#include "memory.h"
#include "debug.h"
#include "common.h"
#include "client_opt.h"

typedef struct domus_struct {
	char device_name[255];
} domus_struct_t;
int main(int argc, char *argv[]) {
    /* Estrutura gerada pelo utilitario gengetopt */
    struct gengetopt_args_info args;

    /* Processa os parametros da linha de comando */
    if (cmdline_parser(argc, argv, &args) != 0) {
        exit(ERR_ARGS);
    }

	char *request=args.request_arg;
	uint8_t device=args.device_arg;
	if(strcmp(request,"status")!=0&&strcmp(request,"on")!=0&&strcmp(request,"off")!=0){
		ERROR(1,"request invalido");
	}

	domus_struct_t domus[8];

	
	strcpy(domus[0].device_name,"Portão da garagem");
	strcpy(domus[1].device_name,"Iluminação do hall de entrada");
	strcpy(domus[2].device_name,"Iluminação sala");
	strcpy(domus[3].device_name,"Iluminação jardim");
	strcpy(domus[4].device_name,"Persiana 1");
	strcpy(domus[5].device_name,"Persiana 2");
	strcpy(domus[6].device_name,"Persiana 3");
	strcpy(domus[7].device_name,"Piso radiante ");


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

	
	




	// UDP IPv4: variáveis auxiliares para sendto() / recvfrom()
	ssize_t udp_read_bytes, udp_sent_bytes;

   if(strcmp(request,"status")==0){
	uint8_t buffer_send[1];
	buffer_send[0]=0;

	uint16_t status;
	// UDP IPv4: "sendto" para o servidor
	printf("a enviar dados para o servidor... "); fflush(stdout);
	if ((udp_sent_bytes = sendto(udp_client_socket,&buffer_send, sizeof(buffer_send), 0, (struct sockaddr *) &udp_server_endpoint, udp_server_endpoint_length)) == -1)
		ERROR(24, "Can't sendto server");
	printf("ok.  (%d bytes sent)\n", (int)udp_sent_bytes);





	// UDP IPv4: "recvfrom" do servidor (bloqueante)
	printf("à espera de dados do servidor... "); fflush(stdout);
	if ((udp_read_bytes = recvfrom(udp_client_socket,&status, sizeof(status), 0, (struct sockaddr *) &udp_server_endpoint, &udp_server_endpoint_length)) == -1)
		ERROR(25, "Can't recvfrom server");
	printf("ok.  (%d bytes received)\n", (int)udp_read_bytes);	

	status=ntohs(status);

	for(int i=0;i<8;i++){
		if(status&(1<<i)){
			printf("%s : Ligado/aberto \n",domus[i].device_name);
		}else{
			printf("%s : Desligado/Fechado \n",domus[i].device_name);
		}
	}
	
   } 

  


   if(strcmp(request,"on")==0){

	
	uint8_t buffer_send[2];
	buffer_send[0]=1;
	buffer_send[1]=(device-1);

	uint8_t status;
	// UDP IPv4: "sendto" para o servidor
	printf("a enviar dados para o servidor... "); fflush(stdout);
	if ((udp_sent_bytes = sendto(udp_client_socket,&buffer_send, sizeof(buffer_send), 0, (struct sockaddr *) &udp_server_endpoint, udp_server_endpoint_length)) == -1)
		ERROR(24, "Can't sendto server");
	printf("ok.  (%d bytes sent)\n", (int)udp_sent_bytes);





	// UDP IPv4: "recvfrom" do servidor (bloqueante)
	printf("à espera de dados do servidor... "); fflush(stdout);
	if ((udp_read_bytes = recvfrom(udp_client_socket,&status, sizeof(status), 0, (struct sockaddr *) &udp_server_endpoint, &udp_server_endpoint_length)) == -1)
		ERROR(25, "Can't recvfrom server");
	printf("ok.  (%d bytes received)\n", (int)udp_read_bytes);	

	if(status==0){
		printf("Comando Ligar/Abrir - %s ignorado \n",domus[device].device_name);
	}
	if(status==1){
		printf("Comando Ligar/Abrir - %s executado com sucesso \n",domus[device].device_name);
	}
	if(status==2){
		printf("Dispositivo %d Desconhecido \n",(device+1));
	}
   } 




   if(strcmp(request,"off")==0){

	
	uint8_t buffer_send[2];
	buffer_send[0]=2;
	buffer_send[1]=(device-1);

	uint8_t status;
	// UDP IPv4: "sendto" para o servidor
	printf("a enviar dados para o servidor... "); fflush(stdout);
	if ((udp_sent_bytes = sendto(udp_client_socket,&buffer_send, sizeof(buffer_send), 0, (struct sockaddr *) &udp_server_endpoint, udp_server_endpoint_length)) == -1)
		ERROR(24, "Can't sendto server");
	printf("ok.  (%d bytes sent)\n", (int)udp_sent_bytes);





	// UDP IPv4: "recvfrom" do servidor (bloqueante)
	printf("à espera de dados do servidor... "); fflush(stdout);
	if ((udp_read_bytes = recvfrom(udp_client_socket,&status, sizeof(status), 0, (struct sockaddr *) &udp_server_endpoint, &udp_server_endpoint_length)) == -1)
		ERROR(25, "Can't recvfrom server");
	printf("ok.  (%d bytes received)\n", (int)udp_read_bytes);	

	if(status==0){
		printf("Comando Desligar/Fechar - %s ignorado \n",domus[device].device_name);
	}
	if(status==1){
		printf("Comando Desligar/Fechar - %s executado com sucesso \n",domus[device].device_name);
	}
	if(status==2){
		printf("Dispositivo %d Desconhecido \n",(device+1));
	}
   } 

	

	// UDP IPv4: fecha socket (client)
	if (close(udp_client_socket) == -1)
		ERROR(23, "Can't close udp_client_socket (IPv4)");

    cmdline_parser_free(&args);

    return 0;
}
