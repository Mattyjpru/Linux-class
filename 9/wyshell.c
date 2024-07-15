//////////////////////////////////////////////////////////////////
// wyshell.c
// Matthew Pru
// COSC 3750 Spring 2023
// Assignment 9
// Last Modified 4-22-2023
// for part 2 of a homemade shell
//////////////////////////////////////////////////////////////////
#include"wyshell.h"
#include"wyscanner.h"
#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include <sys/types.h>
#include<sys/wait.h>
int main(){
  char *tokens[]={ "QUOTE_ERROR", "ERROR_CHAR", "SYSTEM_ERROR",
        "EOL", "REDIR_OUT", "REDIR_IN", "APPEND_OUT",
        "REDIR_ERR", "APPEND_ERR", "REDIR_ERR_OUT", "SEMICOLON",
        "PIPE", "AMP", "WORD" };

  Node *Head=NULL;
  Node *current=NULL;
  int ret;
  char *retptr;
  char buf[1024];
  int pidBin[256];
  int pbCounter=0;    
  int status;
  while(1) { //read from user input
    Head=calloc(1,sizeof(Node));
    if(Head == NULL) {
      perror("calloc()");
      return 1;
    }
    current=Head;
    current->arg_list=calloc(48,sizeof(char*));
    if(current->arg_list == NULL) {
      perror("calloc()");
      return 1;
    }
    current->argcount=0;
    current->input=STDIN_FILENO;
    current->output=STDOUT_FILENO;
    current->error=STDERR_FILENO;
    fprintf(stdout,"$> ");
    retptr=fgets(buf,256,stdin);
    if(retptr == NULL) {
      if(feof(stdin)) {
        return 0;
      }
      else {
        fprintf(stdout,"fgets from stdin");
        return 1;
      }
    }
    //flag for handling user inputting newline 
    //and skipping rest of line
    int skipLine=1;
    ret=parse_line(buf);

    while(ret !=  EOL){ //parse while
      skipLine=0;
      switch(ret) {
        case WORD:
          if(current->command==NULL){
            current->command = strdup(lexeme);
            current->arg_list[current->argcount++]=strdup(lexeme);
          }
          else{
            current->arg_list[current->argcount]=strdup(lexeme);
            current->argcount++;
          }
          break;

        /////redirection
        case REDIR_OUT:
          if (current->output==STDOUT_FILENO){
            current->output=REDIR_OUT;
            fprintf(stdout," >\n");
            ret=parse_line(NULL);
            if (ret==WORD){
              current->out_file=strdup(lexeme);
            }
            else{
              fprintf(stdout,"redirect must be followed by a file name\n");
              skipLine=1;
              ret = EOL;
            }
          }
          else{
            fprintf(stdout,"Ambiguous output redirection\n");
            skipLine=0;
            ret=EOL;
          }
          break;

        case REDIR_IN:
          if (current->input==STDIN_FILENO){
            current->input=REDIR_IN;
            ret=parse_line(NULL);
            if (ret==WORD){
              current->in_file=strdup(lexeme);
            }
            else{
              fprintf(stdout,"redirect must be followed by a file name\n");
              skipLine=1;
              ret = EOL;
            }
          }
          else{
            fprintf(stdout,"Ambiguous input redirection\n");
            skipLine=0;
            ret=EOL;
          }
          break;

        /////piping
        case PIPE:
          if (current->output!=STDOUT_FILENO){
            fprintf(stdout,"Pipe error\n");
            skipLine=1;
            ret=EOL;
          }
          else if(current->command!=NULL){
            ret=parse_line(NULL);
            if (ret!=WORD){
              fprintf(stdout,"Pipe error\n");
              skipLine=1;
              ret=EOL;             
            }
            else{
              current->next=calloc(1, sizeof(Node));
              current->next->prev=current;
              current->output=PIPE;
              current=current->next;
              current->input=PIPE;
              current->output=STDOUT_FILENO;
              current->error=STDERR_FILENO;
              current->arg_list=calloc(48,sizeof(char*));
              if(current->arg_list == NULL) {
                perror("calloc()");
                return 1;
              }
              continue;
            }
          }
          else{
            fprintf(stdout,"Pipe error\n");
            skipLine=1;
            ret=EOL;
          }
          
          break;

        case SEMICOLON:
          if(current->command!=NULL){ 
            ret=parse_line(NULL);
            if (ret==WORD){
              current->next=calloc(1, sizeof(Node));
              current->next->prev=current;
              current=current->next;
              current->input=STDIN_FILENO;
              current->output=STDOUT_FILENO;
              current->error=STDERR_FILENO;
              current->arg_list=calloc(48,sizeof(char*));
              current->bgRun=0;
              if(current->arg_list == NULL) {
                perror("calloc()");
                return 1;
              }
            }
            else if(ret!=EOL){
              fprintf(stdout,"semicolon error\n");
              skipLine=1;
              ret=EOL;
            }
            continue;
          }
          else{
            fprintf(stdout,"semicolon error\n");
            skipLine=1;
            ret=EOL;
          }
          break;

        /////misc
        case APPEND_OUT:
          if (current->output==STDOUT_FILENO){
            current->output=APPEND_OUT;
            ret=parse_line(NULL);
            if (ret==WORD){
              current->out_file=strdup(lexeme);
            }
            else{
              fprintf(stdout,"append must be followed by a file name\n");
              skipLine=0;
              ret = EOL;
            }
          }
          else{
            fprintf(stdout,"Ambiguous output redirection\n");
            skipLine=1;
            ret=EOL;
          }
          break;

        case AMP:
          ret=parse_line(NULL);
          if (ret!=EOL&&ret!=SEMICOLON){
            fprintf(stdout,"invalid '&' directive\n");
            skipLine=1;
            ret = EOL;
          }
          else{
            Node* holder=NULL;
            holder=current;
            current->bgRun=1;
            while(current->input==PIPE){
              current=current->prev;
              current->bgRun=1;//for running in the background
            }
            current=holder;
          }
          break;

        /////error redirection
        case REDIR_ERR:
          if(current->error==STDERR_FILENO){
            current->error=REDIR_ERR;
            ret=parse_line(NULL);
            if (ret==WORD){
              current->err_file=strdup(lexeme);
            }
            else{
              fprintf(stdout,"redirect must be followed by a file name\n");
              skipLine=1;
              ret = EOL;
            }
          }
          else{
            fprintf(stdout,"Too many error redirection calls\n");
            skipLine=1;
            ret=EOL;
          } 
          break;

        case APPEND_ERR:
          if(current->error==STDERR_FILENO){
            current->error=APPEND_ERR;
            ret=parse_line(NULL);
            if (ret==WORD){
              current->err_file=strdup(lexeme);
            }
            else{
              fprintf(stdout,"append must be followed by a file name\n");
              skipLine=1;
              ret = EOL;
            }
          }
          else{
            fprintf(stdout,"Too many error redirection calls\n");
            skipLine=1;
            ret = EOL;
          }
          break;

        case REDIR_ERR_OUT:
          if(current->error==STDERR_FILENO){
            current->error=current->output;
          }
          else{
            fprintf(stdout,"Too many error redirection calls\n");
            skipLine=1;
            ret = EOL;
          }
          break;

        /////errors
        case ERROR_CHAR:
          current->error=ERROR_CHAR;
          fprintf(stdout, "error character: %d\n",ERROR_CHAR);
          ret=EOL;
          skipLine=1;
          break;

        case QUOTE_ERROR:
          current->error=QUOTE_ERROR;
          fprintf(stdout, "quote error\n");
          ret=EOL;
          skipLine=1;
          break;

        case SYSTEM_ERROR:
          current->error=SYSTEM_ERROR;
          perror("system error\n");
          exit(1);

        default:
        fprintf(stdout, "%d: %s\n",ret,tokens[ret%96]);
      }
      if (ret!=EOL){
        ret=parse_line(NULL);
      }
    }//end parse while

    if(skipLine==0){
      current=Head;
      do{//fork loop
        //forking stuff to actually run the commands
        pid_t pid = fork();
        if(pid>0){
        //parent
          if (current->bgRun==0){
            pidBin[pbCounter]=pid;
            pbCounter++;
          }
        }
        else if(pid==0){
          //child
          int execution=execvp(current->command, current->arg_list);
          if (execution== -1){
            perror("Execution error"); 
            exit(1);
          }
        }
        else{
          //error message and skip line
          perror(current->command);
          exit(1);
        }
        current=current->next;
      }while(current!=NULL);//end fork loop
      
    }
    for(int i=0;i<pbCounter; i++){
          waitpid(pidBin[i], &status, 0);
      }

    int bgWaiter=waitpid(-1, &status, WNOHANG);
    while(bgWaiter>0){
        bgWaiter=waitpid(-1, &status, WNOHANG);
      }

    while(Head!=NULL){
      current=Head;
      Head=Head->next;
      for(int i=0; i<current->argcount; i++){
        free(current->arg_list[i]);
      }
      free(current->arg_list);
      free(current);
    }

  } //end read from user input
  return 0;
}
