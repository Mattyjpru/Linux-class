//////////////////////////////////////////////////////////////////
// wytar.c
// Matthew Pru
// COSC 3750 Spring 2023
// Assignment 6
// Last Modified 3-20-2023
// A homemade C program to run the tar utility which will
// perform operations on/create a tape archive file
//////////////////////////////////////////////////////////////////
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<dirent.h>
#include<errno.h>
#include<tar.h> 
#include<grp.h>
#include<pwd.h>
#include "archive.h"
#include<stdlib.h>
#include<sys/time.h>
#include<utime.h>

int main(int argc, char** argv){
  int option=0;
  int fileFlag=0;
  int xcFlag=0;
  int targetFileNum=0;

  for(int i=1; i<argc; i++)
  {
    if (argv[i][0]=='-')
    {
      int j = 1;
      while(argv[i][j]!= '\0')
        {
        if(argv[i][j]=='c')
        {
          if(xcFlag==0){
            option=1; //c option set
            xcFlag=1;
          }
          else{
            perror(argv[i]);
            return 0;
          }
        }
        else if(argv[i][j]=='x')
        {
          if(xcFlag==0){
            option=2; //x option set
            xcFlag=1;
          }
          else{
            perror(argv[i]);
            return 0;
          }
        }
        else if(argv[i][j]=='f')
        {
          fileFlag=1;
          targetFileNum=i+1;
          if (argv[targetFileNum][0]=='-'){
            fprintf(stderr,"-f must be followed by a target file.\n");
            return 0;
          }
        }
        else
        {
          //an illicit option has been input
          fprintf(stderr,"%s: is not a valid option input.\n",argv[i]);
          //quit
          return -1;
        }
        j++;
      }
    }
  }
  if (fileFlag==0||xcFlag==0){
    fprintf(stderr, "wytar cannot process these instructions"); 
    //if main recieves 0, it will quit
    return -1;
  }
  if (option==1)//implies c option
  {
    FILE* wytarTarget;
    wytarTarget=fopen(argv[targetFileNum], "wb");
    char buffer[512];
    if(wytarTarget==NULL){
        fprintf(stderr, "could not open %s", argv[targetFileNum]);
        return 1;
    }
    for (int i = 4; i<argc; i++){
      if (argv[i]!=argv[targetFileNum]){
        struct stat Buf;
        int argInfo=lstat(argv[i], &Buf);
        if(argInfo==-1)
        {
          perror(argv[i]);
        }
        else{
          createHeader(argv[i], Buf, wytarTarget);
        } 
      }
    }
    memset(buffer,0,512);
    //end the wytar file with two 512 byte blocks of 0's
    int writeSize = fwrite(buffer, 1, 512, wytarTarget);
    if (writeSize!=512){
      perror(argv[targetFileNum]);
      return 1;
    }
    writeSize = fwrite(buffer, 1, 512, wytarTarget);
    if (writeSize!=512){
      perror(argv[targetFileNum]);
      return 1;
    }
    fclose(wytarTarget);
  }
  else if(option==2){//implies x option
      extractFile(argv[targetFileNum]);
  }
  else{return -1;}//something went wrong
  return 0;
}


