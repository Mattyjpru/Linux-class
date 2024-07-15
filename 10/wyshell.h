//////////////////////////////////////////////////////////////////
// wyshell.h
// Matthew Pru
// COSC 3750 Spring 2023
// Assignment 10
// Last Modified 4-29-2023
// for part 3 of a homemade shell
//////////////////////////////////////////////////////////////////
#ifndef WYSHELL
#define WYSHELL
#include<stdio.h>
#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
typedef struct node Node;
struct node{
    Node *prev;
    Node *next;
    char *command;
    char** arg_list;
    int argcount;
    int input, output, error;
    char *in_file;
    char *out_file;
    char *err_file;
    int bgRun;
};

#endif


