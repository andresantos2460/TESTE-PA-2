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

#include "memory.h"
#include "debug.h"
#include "common.h"
#include "client_opt.h"
typedef struct domus_status{
    char device_name[256];
    uint16_t device_status;
}domus_status_t;

int main(int argc, char *argv[]) {
    /* Estrutura gerada pelo utilitario gengetopt */
    struct gengetopt_args_info args;

    /* Processa os parametros da linha de comando */
    if (cmdline_parser(argc, argv, &args) != 0) {
        exit(ERR_ARGS);
    }

    char *ip=args.ip_arg;
    int port=args.port_arg;
    char *request=args.request_arg;
    int device=args.device_arg;
    domus_status_t domus_status_svc[8];


    strcpy(domus_status_svc[0].device_name,"Portao da garagem!");
    strcpy(domus_status_svc[1].device_name,"Iluminação do hall de entrada");
    strcpy(domus_status_svc[2].device_name,"Iluminação sala");
    strcpy(domus_status_svc[3].device_name,"Iluminação jardim");
    strcpy(domus_status_svc[4].device_name,"Persiana 1");
    strcpy(domus_status_svc[5].device_name,"Persiana 2");
    strcpy(domus_status_svc[6].device_name,"Persiana 3");
    strcpy(domus_status_svc[7].device_name,"Piso radiante (ligado / desligado)");



    /*
     * Inserir codigo do cliente
     */
	// UDP IPv4: cria socket
	int udp_client_socket;	
	if ((udp_client_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		ERROR(21, "Can't create udp_client_socket (IPv4)");
		
	// UDP IPv4: informação do servidor UDP
	socklen_t udp_server_endpoint_length = sizeof(struct sockaddr_in);	
	struct sockaddr_in udp_server_endpoint;
	memset(&udp_server_endpoint, 0, sizeof(struct sockaddr_in));
	udp_server_endpoint.sin_family = AF_INET;	
	switch (inet_pton(AF_INET,ip, &udp_server_endpoint.sin_addr.s_addr)) {
		case 0:
			fprintf(stderr, "[%s@%d] ERROR - Cannot convert IP address (IPv4): Invalid Network Address\n",
				__FILE__, __LINE__);
			exit(22);
		case -1:
			ERROR(22, "Cannot convert IP address (IPv4)");
	}
	udp_server_endpoint.sin_port = htons(port);

	
	// aqui... a comunicação com o servidor

    ssize_t udp_read_bytes, udp_sent_bytes;

    if (strcmp(request, "status") == 0){
        uint8_t send_buffer[1];
        send_buffer[0] = 0;
        uint16_t net_status;



        printf("a enviar dados para o servidor... "); fflush(stdout);
        if ((udp_sent_bytes = sendto(udp_client_socket, send_buffer, sizeof(send_buffer), 0, (struct sockaddr *) &udp_server_endpoint, udp_server_endpoint_length)) == -1)
            ERROR(24, "Can't sendto server");
        printf("ok.  (%d bytes sent)\n", (int)udp_sent_bytes);

        // UDP IPv4: "recvfrom" do servidor (bloqueante)
        printf("à espera de dados do servidor... "); fflush(stdout);
        if ((udp_read_bytes = recvfrom(udp_client_socket,&net_status, sizeof(net_status), 0, (struct sockaddr *) &udp_server_endpoint, &udp_server_endpoint_length)) == -1)
            ERROR(25, "Can't recvfrom server");
        printf("ok.  (%d bytes received)\n", (int)udp_read_bytes);	

        uint16_t status = ntohs(net_status);

        for (int i = 0; i < 8; i++) {
            if (status & (1 << i)) {
                printf("%s: Ligado/Aberto\n",domus_status_svc[i].device_name);
            } else {
                printf("%s: Desligado/Fechado\n",domus_status_svc[i].device_name);
            }
        }


    }

    if (strcmp(request, "on") == 0){
        uint8_t send_buffer[2];
        send_buffer[0] = 1;
        send_buffer[1]=device;
        uint16_t net_status;



        printf("a enviar dados para o servidor... "); fflush(stdout);
        if ((udp_sent_bytes = sendto(udp_client_socket, send_buffer, sizeof(send_buffer), 0, (struct sockaddr *) &udp_server_endpoint, udp_server_endpoint_length)) == -1)
            ERROR(24, "Can't sendto server");
        printf("ok.  (%d bytes sent)\n", (int)udp_sent_bytes);

        // UDP IPv4: "recvfrom" do servidor (bloqueante)
        printf("à espera de dados do servidor... "); fflush(stdout);
        if ((udp_read_bytes = recvfrom(udp_client_socket,&net_status, sizeof(net_status), 0, (struct sockaddr *) &udp_server_endpoint, &udp_server_endpoint_length)) == -1)
            ERROR(25, "Can't recvfrom server");
        printf("ok.  (%d bytes received)\n", (int)udp_read_bytes);	

        uint16_t status = ntohs(net_status);

        switch (status) {

            case 0:

                printf("Comando 'Ligar/Abrir'- %s ignorado \n",domus_status_svc[device-1].device_name);
                break;
            case 1:

                printf("Comando 'Ligar/Abrir'- %s Executado com Sucesso \n",domus_status_svc[device-1].device_name);
                break;
            case 2:

                printf("Dispositivo %d Desconhecido\n",device);
                break;

        }

    }

    if (strcmp(request, "off") == 0){
        uint8_t send_buffer[2];
        send_buffer[0] = 2;
        send_buffer[1]=device;
        uint16_t net_status;



        printf("a enviar dados para o servidor... "); fflush(stdout);
        if ((udp_sent_bytes = sendto(udp_client_socket, send_buffer, sizeof(send_buffer), 0, (struct sockaddr *) &udp_server_endpoint, udp_server_endpoint_length)) == -1)
            ERROR(24, "Can't sendto server");
        printf("ok.  (%d bytes sent)\n", (int)udp_sent_bytes);

        // UDP IPv4: "recvfrom" do servidor (bloqueante)
        printf("à espera de dados do servidor... "); fflush(stdout);
        if ((udp_read_bytes = recvfrom(udp_client_socket,&net_status, sizeof(net_status), 0, (struct sockaddr *) &udp_server_endpoint, &udp_server_endpoint_length)) == -1)
            ERROR(25, "Can't recvfrom server");
        printf("ok.  (%d bytes received)\n", (int)udp_read_bytes);	

        uint16_t status = ntohs(net_status);

        switch (status) {

            case 0:

                printf("Comando 'desligar/fechado'- %s ignorado \n",domus_status_svc[device-1].device_name);
                break;
            case 1:

                printf("Comando 'desligar/fechado'- %s Executado com Sucesso \n",domus_status_svc[device-1].device_name);
                break;
            case 2:

                printf("Dispositivo %d Desconhecido\n",device);
                break;

        }

    }
	// UDP IPv4: fecha socket (client)
	if (close(udp_client_socket) == -1)
		ERROR(23, "Can't close udp_client_socket (IPv4)");

	

    cmdline_parser_free(&args);

    return 0;
}
