//////////////////////////////////////////////////////////////////
// wytalkC.c
// Matthew Pru
// COSC 3750 Spring 2023
// Assignment 7
// Last Modified 4-10-2023
// client functionality for a basic chat program
//////////////////////////////////////////////////////////////////
#include<unistd.h>
#include "socketfun.h"
#include<sys/socket.h>
#include<sys/types.h>
#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>


int main(int argc, char** argv){
    int connection=request_connection(argv[1], 51100);
    if (connection==-1){
        perror(argv[1]);
        exit(1);
    }
    char sockBuff[2048];
    char userBuff[2048];
    int homemadeBool=1;
    int r, w;
    int here=0;
    while(homemadeBool==1){
        //writing to the buffer
        if(fgets(userBuff, 2048, stdin)==NULL){
            perror("fgets()");
            break;
        }
        w=write(connection, userBuff, strlen(userBuff));
        if (w==-1){
            fprintf(stderr,"Error writing to socket.");
            break;
        }
        //reading from the buffer
        r=read(connection, sockBuff, 1);
        while(r>0){
            if(sockBuff[here]=='\n'){
                break;
            }
            here++;
            r=read(connection, sockBuff+here, 1);
        }
        if(r==0){
            fprintf(stdout, "%s\n", sockBuff);
            break;
        }
        if (r==-1){
            fprintf(stdout, "%s\n", sockBuff);
            fprintf(stderr,"Error reading from socket.");
            break;
        }
        sockBuff[here]='\0';
        here=0;
        fprintf(stdout, "%s\n", sockBuff);
    }
    close(connection);
return 0;  
}
