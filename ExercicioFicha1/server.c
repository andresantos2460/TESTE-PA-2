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
#define MAX_DEVICE (8)

int turn_off(int device,uint16_t mask,uint16_t *status);
int turn_on(int device,uint16_t mask,uint16_t *status);
int main(int argc, char *argv[]) {
    /* Estrutura gerada pelo utilitario gengetopt */
    struct gengetopt_args_info args;
    /* Processa os parametros da linha de comando */

    if (cmdline_parser(argc, argv, &args) != 0) {
        exit(ERR_ARGS);
    }
    //port +status
    
    
    int port=args.port_arg;
    uint16_t status=args.status_arg;

    // UDP IPv4: cria socket
    int udp_server_socket;
    if ((udp_server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        ERROR(31, "Can't create udp_server_socket (IPv4)");

    // UDP IPv4: bind a IPv4/porto 
    struct sockaddr_in udp_server_endpoint;
    memset(&udp_server_endpoint, 0, sizeof(struct sockaddr_in));
    udp_server_endpoint.sin_family = AF_INET;
    udp_server_endpoint.sin_addr.s_addr = htonl(INADDR_ANY);  	// Todas as interfaces de rede
    udp_server_endpoint.sin_port = htons(port);	// Server port
    if (bind(udp_server_socket, (struct sockaddr *) &udp_server_endpoint, sizeof(struct sockaddr_in)) == -1){

        ERROR(32, "Can't bind @udp_server_endpoint info");
    }

    printf("[SRRVER] Rwady to receive requests \n");

    // aqui... a comunicação com o cliente
    ssize_t recvfrom_ret,sendto_ret;
    int flags = 0;
    struct sockaddr_in recv_addr;
    socklen_t recv_addr_len;
    uint8_t buffer[2];

    while (1) {
      recv_addr_len = sizeof(struct sockaddr_in);
        recvfrom_ret = recvfrom(udp_server_socket, buffer, sizeof(buffer), flags, (struct sockaddr *)&recv_addr, &recv_addr_len);
        if( recvfrom_ret == -1 ){
            ERROR(1,"[SERVER] Cannot recvfrom");
        }

        uint8_t type = buffer[0];

        if (type == 0 && recvfrom_ret == 1) {

            uint16_t net_status = htons(status);

            sendto_ret = sendto(
                    udp_server_socket,
                    &net_status,              // envia o estado
                    sizeof(net_status),       // 2 bytes
                    flags,
                    (struct sockaddr*)&recv_addr,
                    recv_addr_len
                    );

            if (sendto_ret == -1) {
                ERROR(1, "[SERVER] Cannot sendto");
            }
        }

        if (type == 1 && recvfrom_ret == 2) {

            int device = buffer[1]-1;
            uint16_t mask=1<<device;
            int resultado=0;

            resultado= turn_on(device,mask,&status);

            //colocar codifgo aqii da funcao
            uint16_t net_status = htons(resultado);

            sendto_ret = sendto(
                    udp_server_socket,
                    &net_status,              // envia o estado
                    sizeof(net_status),       // 2 bytes
                    flags,
                    (struct sockaddr*)&recv_addr,
                    recv_addr_len
                    );

            if (sendto_ret == -1) {
                ERROR(1, "[SERVER] Cannot sendto");
            }

        }

        if (type == 2 && recvfrom_ret == 2) {


            int device = buffer[1]-1;
            uint16_t mask=1<<device;
            int resultado=0;

            resultado= turn_off(device,mask,&status);

            //colocar codifgo aqii da funcao
            uint16_t net_status = htons(resultado);

            sendto_ret = sendto(
                    udp_server_socket,
                    &net_status,              // envia o estado
                    sizeof(net_status),       // 2 bytes
                    flags,
                    (struct sockaddr*)&recv_addr,
                    recv_addr_len
                    );

            if (sendto_ret == -1) {
                ERROR(1, "[SERVER] Cannot sendto");
            }


        }
        // liberta recurso: socket UDP IPv4
      

}
  if (close(udp_server_socket) == -1)
    ERROR(33, "Can't close udp_server_socket (IPv4)"); 

    
    cmdline_parser_free(&args);

    return 0;

}
int turn_on(int device,uint16_t mask,uint16_t *status){


            if(device<0||device>7){
                return 2;
            }

            if((*status)&mask){

                return 0;

            }else{

                (*status)|=1<<device;
                return 1;

            }
}
int turn_off(int device,uint16_t mask,uint16_t *status){


            if(device<0||device>7){
                return 2;
            }

            if(((*status)&mask)==0){

                return 0;

            }else{

                (*status) &=~(1<<device);
                return 1;

            }
}
