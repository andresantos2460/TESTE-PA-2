/**
 * @file main.c
 * @brief Description
 * @date 2018-1-1
 * @author name of author
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "debug.h"
#include "memory.h"

int is_bit_n_set(int input, size_t bit_n);
int bin_to_decimal(const char *bin_s_ptr);
int bit_n_to_zero(int input, size_t bit_to_zero);
int main(int argc, char *argv[]) {
    /* Disable warnings */

    int result=0;
    char buffer[]="1101";
    result=bin_to_decimal(buffer);
    printf("%d \n",result);
    printf("%d \n",is_bit_n_set(1,3));
    printf("%d \n",bit_n_to_zero(8,3));

    return 0;
}
int bin_to_decimal(const char *bin_s_ptr){
    int total=0;
    int power=1;


    for(int i=strlen(bin_s_ptr)-1;i>=0;i--){

        if(bin_s_ptr[i]=='1'){
            total+=power;
        }
        power=power*2;

    }

    return total;

}
int is_bit_n_set(int input,size_t bit_n){

    int mask = 1 << bit_n;

    if((input & mask) != 0){
        return 1;
    }

    return 0;
}
int bit_n_to_zero(int input, size_t bit_to_zero){

    int mask=~(1<<bit_to_zero);
    int result = input&mask;

    return result;
}

