//////////////////////////////////////////////////////////////////
// archive.h
// Matthew Pru
// COSC 3750 Spring 2023
// Assignment 6
// Last Modified 3-20-2023
// header file for archive.c
//////////////////////////////////////////////////////////////////

#ifndef ARCHIVE_H
#define ARCHIVE_H


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
#include<stdlib.h>
#include<sys/time.h>
#include<utime.h>
void createHeader(char* file, struct stat Buf, FILE* wytarFile);
void getFileInfo(char* file, size_t size, FILE* wytarTarget);
void extractFile(char* wytarFile);
void makeDirectoryPath(char* path, mode_t permissions);

struct tar_header
{ 
  char name[100];
  char mode[8];
  char uid[8];
  char gid[8];
  char size[12];
  char mtime[12];
  char chksum[8];
  char typeflag;
  char linkname[100];
  char magic[6];
  char version[2];
  char uname[32];
  char gname[32];
  char devmajor[8];
  char devminor[8];
  char prefix[155];
  char pad[12];
};
#endif
