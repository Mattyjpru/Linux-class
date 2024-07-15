# Makefile
# Matthew Pru
# COSC 3750 Spring 2023
# Assignment 6
# Last Modified 3-20-2023
# A Makefile used to compile and run wytar for assignment 6
#################################
#allows debugging information, warning messages, and only searches the current working directory
CFLAGS=-ggdb -Wall
#shortcut for compiling program
CC=gcc
.PHONY: clean
#this will create the executable
wytar: wytar.c archive.c archive.h
	${CC} ${CFLAGS} wytar.c archive.c -o wytar
#clean slate. everything is removed. use for testing after edits have been made.
clean:
	/bin/rm -f a.out core.* wytar

