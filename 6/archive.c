//////////////////////////////////////////////////////////////////
// archive.c
// Matthew Pru
// COSC 3750 Spring 2023
// Assignment 6
// Last Modified 3-20-2023
// contains functions for wytar
//////////////////////////////////////////////////////////////////
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<dirent.h>
#include<errno.h>
#include<tar.h> 
#include<grp.h>
#include<pwd.h>
#include<sys/time.h>
#include<utime.h>
#include "archive.h"
void createHeader(char* file, struct stat Buf, FILE* wytarTarget){
  struct tar_header *header;
  header=malloc(512);
  memset(header, 0, 512);

  //filetype
  if(S_ISDIR(Buf.st_mode)){
    header->typeflag=DIRTYPE;
  }
  else if(S_ISLNK(Buf.st_mode)){
    header->typeflag=SYMTYPE;
  }
  else if(S_ISREG(Buf.st_mode)){
    header->typeflag=REGTYPE;
  }
  else{
    perror(file);
  }
  //version
  memcpy(header->version,TVERSION,TVERSLEN);
  //name and linkname
  if (header->typeflag==DIRTYPE){
    if (strlen(file)<100){
      strcpy(header->name, file);
      if(file[strlen(file)-1]!='/'){
        strcat(header->name, "/");
      }
      strcat(header->name, "\0");
    }
    else if(strlen(file)==100){
      strcpy(header->name, file);
      if(file[strlen(file)-1]!='/'){
        strcat(header->name, "/");
      }
    }
    else if (strlen(file)>255){
      return;
    }
    else{ //implies 100<name length<255
      int j=0;
      int finalFslash;
      for(int i=0; i<strlen(file)&&j<155; i++){
        if(file[i]=='/'){
          finalFslash=i;
        }
        j++;
      }
      if(finalFslash==0){
        return;
      }
      else{
        strncpy(header->prefix,file, finalFslash);
        strncpy(header->name, &file[finalFslash],100);
      }
    }
  }
  else if(header->typeflag==SYMTYPE){
    //get the file that a link points to
    size_t buffSize = 128;
    char linkBuff [buffSize];
    size_t linkChars = readlink(file, linkBuff, buffSize);
    if(linkChars==-1)
    {
      perror(file);
      return;
    }
    else 
    {
      if (linkChars<100){
      strcpy(header->name, file);
      strcat(header->name, "\0");
      strcpy(header->linkname, linkBuff);
      }
      else if(linkChars==100){
      strcpy(header->name, file);
      strcpy(header->linkname, linkBuff);
      }
      else{
        return;
      }
    }
  }
  else{   //regular file
    
    if (strlen(file)<100){
      strcpy(header->name, file);
      strcat(header->name, "\0");
    }
    else if(strlen(file)==100){
      strcpy(header->name, file);
    }
    else if (strlen(file)>255){
      return;
    }
    else{ //implies 100<name length<255
      int j=0;
      int finalFslash;
      for(int i=0; i<strlen(file)&&j<155; i++){
        if(file[i]=='/'){
          finalFslash=i;
        }
        j++;
      }
      if(finalFslash==0){
        return;
      }
      else{
        strncpy(header->prefix,file, finalFslash);
        strncpy(header->name, &file[finalFslash],100);
        if(file[strlen(file)-1]!='/'){
          strcat(header->name, "/");
        }
        strcat(header->name, "\0");
      }
    }
  }
  //uid and uname
  struct passwd* Uid = getpwuid(Buf.st_uid);
  if(Uid!=NULL){
    sprintf(header->uid, "%07o", Buf.st_uid);
    sprintf(header->uname, Uid->pw_name);
    strcat(header->uname, "\0");
  }
  else{
    perror(file);
    return;
  }
  //gid and gname
  struct group* Gid = getgrgid(Buf.st_gid);
  if(Gid!=NULL){
    sprintf(header->gid, "%07o", Buf.st_gid);
    sprintf(header->gname, Gid->gr_name);
    strcat(header->gname, "\0");
  }
  else{
    perror(file);
    return;
  }
  //size
  if(header->typeflag==SYMTYPE||header->typeflag==DIRTYPE){
    sprintf(header->size,"%011o", 0);
  }
  else{
    sprintf(header->size, "%011lo", Buf.st_size);
  }
  //mode
  snprintf(header->mode, 8, "%07o", Buf.st_mode & 07777);
  header->mode[7]='\0';
  //mtime
  sprintf(header->mtime, "%011lo", (unsigned long) Buf.st_mtime);
  //devmajor and devminor
  sprintf(header->devmajor, "%07o", 0);
	sprintf(header->devminor, "%07o", 0);
  //magic
  memcpy(header->magic,TMAGIC,TMAGLEN);
  //prefix gets handled by name section
  //pad gets handled by setting everything to 0's originally
  //checksum
  memset(header->chksum,' ',8);
  unsigned int checksum;
  char* tempHdr=(char*)header;
  checksum =0;
  for(int i=0; i<512; i++){
    checksum+=tempHdr[i];
  }
  sprintf(header -> chksum, "%06o", checksum);
  header->chksum[6]='\0';
  //copy header onto tar file
  fwrite(header, 1, 512, wytarTarget);
  if(header->typeflag==DIRTYPE){
    DIR* Directory=opendir(file);
    if (Directory==NULL)
    {
      perror(file);
      return;
    }
    else
    {   
      struct dirent* Direc=readdir(Directory);
      while(Direc!=NULL)
      {
        if(strcmp(Direc->d_name, ".")!=0&&strcmp(Direc->d_name, "..")!=0){
          char path[512];
          snprintf(path, 512, "%s/%s", file, Direc->d_name);
          lstat(path, &Buf);
          createHeader(path, Buf, wytarTarget);
        }
        Direc=readdir(Directory);
      }
    }
    closedir(Directory);
  }
  if(header->typeflag==REGTYPE){
    getFileInfo(file, Buf.st_size, wytarTarget);
  }
}
void getFileInfo(char* file, size_t size, FILE* wytarTarget){
  FILE* input = fopen(file, "rb");
  size_t inputSize;
  size_t writeSize;
  char buffer[512];
  if(size>0){
    do{
      //write file data to tar file in blocks of 512
      //pad any remainder with 0's
      inputSize=fread(buffer, 1, 512, input);
      if(inputSize<512){
        if(ferror(input)){
          perror(file);
          return;
        }
        else{
          memset(buffer+inputSize, 0, 512-inputSize);
        }
      }
      writeSize = fwrite(buffer, 1, 512, wytarTarget);
    }
    while(inputSize==512&&writeSize==512);
      if(writeSize!=512){
        fprintf(stdout, "Error writing from %s\n", file);
      }
    fclose(input);
  }
}
void extractFile(char* wytarFile){
  FILE* wytarInput = fopen(wytarFile, "rb");
  if (wytarFile==NULL){
    perror(wytarFile);
    return;
  }
  struct stat TarInfo;
  int info = lstat(wytarFile, &TarInfo);
  if(info==-1)
      {
        perror(wytarFile);
        return;
      }
  int tarSize=TarInfo.st_size;
  if(tarSize<512){
    perror(wytarFile);
    return;
  }
  do{
    struct tar_header* currentFile;
    currentFile=malloc(512);
    size_t headerSize = fread(currentFile, 1, 512, wytarInput);
    if(headerSize!=512){
      fprintf(stderr, "Error reading from %s", wytarFile);
    }
    int currentSize= strtol(currentFile->size, NULL, 8);
    //add prefix to name if necessary
    char* stitchedPath;
    if(currentFile->prefix[0]!='\0'){
      char* prefixBuffer= strcat(currentFile->prefix,"/");
      stitchedPath=strcat(prefixBuffer,currentFile->name);
      }
    else{
      strcpy(stitchedPath, currentFile->name);
    }
    //verify the checksum
    unsigned int tempSum;
    tempSum=strtol(currentFile->chksum, NULL, 8);
    memset(currentFile->chksum,' ',8);
    unsigned int checksum=0;
    char* tempHdr=(char*)currentFile;
    for(int i=0; i<512; i++){
      checksum+=tempHdr[i];
      //fprintf(stdout,"%d\t %d\n",checksum, i);
    }
    if(checksum!=tempSum){
      fprintf(stderr, "Error reading from file.\n");
      //return;
    }
    //handle files
    if(currentFile->typeflag==REGTYPE){
      FILE* newFile;
      char* subPath;
      subPath=malloc(256);
      int lastSlashFound=0;
      errno=0;
      tarSize-=512;
      newFile=fopen(stitchedPath, "wb");
      //if we cannot open a file because its 
      //path does not exist, make the path
      if(errno==ENOENT){
        for(int i=strlen(stitchedPath)-1; i>0&&lastSlashFound==0;i--){
          if(stitchedPath[i]=='/'){
            strncpy(subPath, stitchedPath, i);
            lastSlashFound=1;
          }
        }
        makeDirectoryPath(subPath, 0755);
        newFile=fopen(stitchedPath, "wb");
      }
      int blockCount=(currentSize+511)/512;
      int extra=currentSize % 512;
      while(blockCount!=0){
        char* infoBuffer;
        infoBuffer=malloc(512);
        //read in blocks of 512 but ignore padding of zeros
        size_t readSize =fread(infoBuffer, 1, 512, wytarInput);
        if(readSize!=512){
            fprintf(stderr, "Error reading from %s\n", wytarFile);
          }
        if (blockCount==1&&extra!=0){
          size_t writeSize=fwrite(infoBuffer, 1, extra, newFile);
          if(writeSize!=extra){
            fprintf(stderr, "Error writing from %s\n", wytarFile);
          }
        }
        else{

          size_t writeSize=fwrite(infoBuffer, 1, 512, newFile);
          if(writeSize!=512){
            fprintf(stderr, "Error writing from %s\n", wytarFile);
          }
        }
        blockCount--;
      }
      fclose(newFile);
      //change permissions/
      mode_t mode = strtol(currentFile->mode, NULL, 8);
      umask(0);
      int permissionChange=chmod(stitchedPath, mode);
      if(permissionChange==-1){
            fprintf(stderr, "Error changing permissions of %s\n", 
            stitchedPath);
          }
    }
    //handle directories
    else if(currentFile->typeflag==DIRTYPE){
      //check if the directory already exists
      struct stat dirChecker;
      int dirRet=lstat(stitchedPath, &dirChecker);
      if(dirRet!=0){
        makeDirectoryPath(currentFile->name, 0755);
      }
      //change permissions/
      mode_t mode = strtol(currentFile->mode, NULL, 8);
      umask(0);
      int permissionChange=chmod(stitchedPath, mode);
      if(permissionChange==-1){
            fprintf(stderr, "Error changing permissions of %s\n", 
            stitchedPath);
          }
    }
    //handle symbolic links
    else if (currentFile->typeflag==SYMTYPE){
      FILE* newFile;
      char* subPath;
      subPath=malloc(256);
      int lastSlashFound=0;
      errno=0;
      newFile=fopen(stitchedPath, "wb");
      //if we cannot open a file because its 
      //path does not exist, make the path
      if(errno==ENOENT){
        for(int i=strlen(stitchedPath)-1; i>0&&lastSlashFound==0;i--){
          if(stitchedPath[i]=='/'){
            strncpy(subPath, stitchedPath, i);
            lastSlashFound=1;
          }
        }
        makeDirectoryPath(subPath, 0755);
        newFile=fopen(stitchedPath, "wb");
        int linkSet = symlink(currentFile->linkname, stitchedPath);
        if(linkSet==-1){
          perror(stitchedPath);
          return;
        }
        fclose(newFile);
      }
      else{
        newFile=fopen(stitchedPath, "wb");
        int linkSet = symlink(currentFile->linkname, stitchedPath);
        if(linkSet==-1){
          perror(stitchedPath);
          return;
        }
        fclose(newFile);
      }
      
    }
    time_t mTime=strtol(currentFile->mtime,NULL,8);
    struct utimbuf timeChanger;
    timeChanger.modtime=mTime;
    int timeSet=utime(stitchedPath, &timeChanger);
    if (timeSet!=0){
      fprintf(stderr,"Could not set the modification time.\n");
      return;
    }
    tarSize-=512;
  }while(tarSize>1024);
  fclose(wytarInput);
}
void makeDirectoryPath(char* path, mode_t permissions){
  //remove tailing slash
  if (path[strlen(path) - 1] == '/'){
    path[strlen(path) - 1] = '\0';
  }
  int attempt = mkdir(path, permissions);
  //if path is compounded, make all parent directories
  if (attempt!=0){
    for (int i = 0; i<strlen(path); i++){
      if(path[i]== '/'){
        path[i]='\0';
        makeDirectoryPath(path, permissions);
      }
    }
    attempt = mkdir(path, permissions);
  }
  if (attempt != 0){
    fprintf(stderr, "Could not create directory %s\n", path);
  }
}
