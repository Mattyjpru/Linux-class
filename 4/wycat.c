//////////////////////////////////////////////////////////////////////////////////
// Wycat.c
// Matthew Pru
// COSC 3750 Spring 2023
// Assignment 3
// Last Modified 2-12-2023
// A homemade C program to run the cat operation which will
// repeat the contents of a file to standard output
/////////////////////////////////////////////////////////////////////////////////


#include<stdio.h>
#include<errno.h>
#include<string.h>

//function prototypes
void accessFiles(FILE* fp, int bufferSize);

//main function
int main (int argc, char** argv)
{
int buffSize= 4096;
 if ( argc == 1 ) //only ./wycat is read but no other inputs
 {
  accessFiles(stdin, buffSize); //read and write staandard input
 }

 else
 {
  for(int i =1; i<argc; i++) //for all arguments given to ./wycat
  {
   if(strncmp(argv[i], "-", 1)==0) //will indicat reading from standard input
   {
    accessFiles(stdin, buffSize);
   }
   else
   {
    FILE* fp = fopen(argv[i], "r");
    if (fp==NULL) //check if the file exists in the directory
    {
     perror(argv[i]);
    }
    else
    {
     accessFiles(fp, buffSize); //use the file to read and write
     fclose(fp);
    }
   }
  }
 }
return 0;
}

//function declarations
//a function for reading and writing functionality. this includes stdin
void accessFiles(FILE* fp, int bufferSize)
{
 char buffer[bufferSize];
 size_t in, out;
 do
 {
  in = fread(buffer, 1, bufferSize, fp);
  if(in<bufferSize) //ideally this means end of file
  {
   if(ferror(fp))
    {
     fprintf(stderr, "Error reading file");
    }
  }
  out = fwrite(buffer, 1, in, stdout); //write everything in the buffer to stdout
   if(in!=out)
   {
    fprintf(stderr,"Output error");
   }
 }
 while(in==bufferSize);
}

