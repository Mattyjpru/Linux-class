# Makefile
# Matthew Pru
# COSC 3750 Spring 2023
# Assignment 5
# Last Modified 3-5-2023
# A Makefile used to compile and run wyls for assignment 5
#################################
CFLAGS=-ggdb -Wall -lm #allows debugging information, warning messages, and only searches the current working directory
CC=gcc #shortcut for compiling program -lm allows ceil() and floor()
.PHONY: clean
#this will create the executable
wyls: wyls.c
	${CC} ${CFLAGS} wyls.c -o wyls
#clean slate. everything is removed. use for testing after edits have been made.
clean:
	/bin/rm -f a.out core.* wyls
