#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<dirent.h>
#include<pwd.h>
#include<time.h>
#include<unistd.h>
#include<stdlib.h>
#include<grp.h>
#include<sys/stat.h>
#include<sys/types.h>

void listContent(const char* dirname)
{
    DIR* directory = opendir(dirname);
    if (directory==NULL)
    {
      fprintf(stderr, "No such directory");
    }
    struct dirent* direc;
    direc=readdir(directory);
    while(direc!=NULL)
    {
      //if(strcmp(direc->d_name, ".")!=0 && strcmp(direc->d_name,"..")!=0)
     // {
        fprintf(stdout,"%s\n", direc->d_name);
        direc=readdir(directory);
      //}

    }
    closedir(directory);
}

int main (int argc, char** argv)
{
  if (argc<1)
  {
    return 1;
  }
  listContent(".");
  return 0;
}
