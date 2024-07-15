///////////////////////////////////////////////////////////////////////////////
// wyls.c
// Matthew Pru
// COSC 3750 Spring 2023
// Assignment 5
// Last Modified 3-5-2023
// A homemade C program to run the ls -l operation which will
// list the current Directory
///////////////////////////////////////////////////////////////////////////////
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
#include<string.h>
#include<math.h>
void writeLs(char* dirname, struct stat Buf, int options)
{
  struct passwd* Uid = getpwuid(Buf.st_uid);
  struct group* Gid = getgrgid(Buf.st_gid);
  //print all the permissions of the file with no whitespace
  fprintf(stdout, (S_ISDIR(Buf.st_mode)) ? "d" : 
  (S_ISLNK(Buf.st_mode)) ? "l" : "-");
  fprintf(stdout, (Buf.st_mode & S_IRUSR) ? "r" : "-");
  fprintf(stdout, (Buf.st_mode & S_IWUSR) ? "w" : "-");
  fprintf(stdout, (Buf.st_mode & S_IXUSR) ? "x" : "-");
  fprintf(stdout, (Buf.st_mode & S_IRGRP) ? "r" : "-");
  fprintf(stdout, (Buf.st_mode & S_IWGRP) ? "w" : "-");
  fprintf(stdout, (Buf.st_mode & S_IXGRP) ? "x" : "-");
  fprintf(stdout, (Buf.st_mode & S_IROTH) ? "r" : "-");
  fprintf(stdout, (Buf.st_mode & S_IWOTH) ? "w" : "-");
  fprintf(stdout, (Buf.st_mode & S_IXOTH) ? "x" : "-");
  //user id
  if(Uid!=NULL)
  {
    if(options==1||options==3) //n or n and h option set
    {
      fprintf(stdout, "%*d", 8, Buf.st_uid);
    }
    else
    {
      fprintf(stdout, "%*s", 8, Uid->pw_name);
    }
  }
  //group id
  if(Gid!=NULL)
  {
    if(options==1||options==3) //n or n and h option set
    {
      fprintf(stdout, "%*d", 10, Buf.st_gid);
    }
    else
    {
      fprintf(stdout, "%*s", 10, Gid->gr_name);
    }
  }
  //size
  if(options>1)//h or n and h option set
  {
    int b=0;
    double amount = Buf.st_size;
  //this loop in combination with uniType does unit conversion for amount
    while (amount>1024) 
    {
      amount=amount/1024;
      b++;
    }
    char* unitType[]= {"", "K", "M", "G",};
    char output[64];
    double check=amount;
    if(ceil(check)==floor(check)) //check that amount is a whole number
    {
      //omits ".0"
      sprintf(output,"%d%s ", (int)amount, unitType[b]);
    }
    else
    {
      //prints size with appropriate suffix rounded to single decimal
      sprintf(output,"%.01f%s ", amount, unitType[b]);
    }
    fprintf(stdout, "%*s  ", 8, output);
  }
  else //print size in bytes
  {
    fprintf(stdout, "%*ld  ", 10, Buf.st_size);
  }
  //date-time
  char dateTime[144];
  time_t timeObj = Buf.st_mtime;
  struct tm* FileTime = localtime(&timeObj);
  time_t thisTime = time(NULL);
  //check if last modify date was more than 180 days ago
  if(difftime(thisTime, timeObj)>=15552000)
  {
    //fill a buffer with month day year
    strftime(dateTime, sizeof dateTime, "%b %-d %Y", FileTime);
  }
  else
  {
    //fill a buffer with month day hours:minutes
    strftime(dateTime, sizeof dateTime, "%b %-d %H:%M", FileTime);
  }
  fprintf(stdout, "%-12s", dateTime);
  //name
  fprintf(stdout," %s", dirname);
  if(S_ISLNK(Buf.st_mode))
  {
    //get the file that a link points to
    size_t buffSize = 128;
    char linkBuff [buffSize];
    size_t linkChars = readlink(dirname, linkBuff, buffSize);
    if(linkChars==-1)
    {
      perror(dirname);
    }
    else
    {
      fprintf(stdout, " -> %s", linkBuff);
    }
  }
  fprintf(stdout,"\n");
}
/// ///////////////////////////////////////////////////////////////////
void listContent(char* dirname, int options)
{
    struct stat Buf;
    if(lstat(dirname, &Buf)==-1)
    {
      perror(dirname);
    }
    else if(S_ISDIR(Buf.st_mode))
    {
      //send everything in directory to writeLs
      DIR* Directory;
      if ((Directory=opendir(dirname))==NULL)
      {
        fprintf(stderr, "Cannot open: %s\n", dirname);
      }
      else
      {
        struct dirent* Direc;
        while((Direc=readdir(Directory))!=NULL)
        {
          if(strncmp(Direc->d_name, ".", 1)!=0 && 
          strncmp(Direc->d_name,"..", 2)!=0)
          {
            char path[516];
            snprintf(path, 516, "%s/%s", dirname, Direc->d_name);
            lstat(path, &Buf);
            writeLs(Direc->d_name, Buf, options);
          }
        }
        closedir(Directory);
      }
    }
    else
    {
      //send a single file to writeLs
      writeLs(dirname, Buf, options);
    }
}
/// //////////////////////////////////////////////////////////////////
int main (int argc, char** argv)
{
  if (argc<1)
  {
    return 1;
  }
 int options;
 int nOpt=0;
 int hOpt=0;
 int gettingOptions=1; //to switch off option reading
 int argShift=1;
 if(argc==1)
 {
  listContent(".", 0);
 }
 else
 {
  for(int i=1; gettingOptions!=0 && i<argc; i++)
  {
   if (argv[i][0]=='-')
   {
    int j = 1;
    while(argv[i][j]!= '\0')
    {
     if(argv[i][j]=='n')
     {
      nOpt=1; //n option set
     }
     else if(argv[i][j]=='h')
     {
      hOpt=2; //h option set
     }
     else
     {
      //an illicit option has been input
      perror(argv[i]); 
      //quit
      return 1;
     }
     j++;
    }
    //to change where we start reading arguments
    // to pass to listContent
    argShift++;
   }
   else
   {
    // an argument is not an option, stop reading options
    gettingOptions=0;
   }
  }
  //1 = n set, 2= h set, 3 = n and h set
  options = nOpt+hOpt;
  if(argc==argShift)
  {
    //list current directory with n and/or h options
    listContent(".", options);
  }
  else
  {
    for (int i = argShift; i<argc; i++)
    {
      //everything that comes after the executable and options
      listContent(argv[i], options);
    }
  }
 }
 return 0;
}

