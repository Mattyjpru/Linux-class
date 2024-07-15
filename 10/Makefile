# Makefile
# Matthew Pru
# COSC 3750 Spring 2023
# Assignment 10
# Last Modified 4-29-2023
# A Makefile used to compile and run wyshell for assignment 10
#################################
#allows debugging information, warning messages, and only searches the current $
CFLAGS=-ggdb -Wall -Wno-unused-function
#shortcut for compiling program
CC=gcc
.PHONY: clean
#this will create the executable
wyshell: wyshell.h wyshell.c wyscanner.h wyscanner.c
	${CC} ${CFLAGS} wyscanner.c wyshell.c -o wyshell
#clean slate. everything is removed. use for testing after edits have been made.
clean:
	/bin/rm -f a.out core.* wyshell
