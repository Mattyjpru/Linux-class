# Makefile
# Matthew Pru
# COSC 3750 Spring 2023
# Assignment 7
# Last Modified 4-10-2023
# A Makefile used to compile and run wytalk for assignment 7
#################################
#allows debugging information, warning messages, and only searches the current $
CFLAGS=-ggdb -Wall
#shortcut for compiling program
CC=gcc
.PHONY: clean
#this will create the executable
all: wytalkD wytalkC
wytalkD: wytalkD.c socketfun.h socketfun.c
	${CC} ${CFLAGS} wytalkD.c socketfun.c -o wytalkD
wytalkC: wytalkC.c socketfun.h socketfun.c
	${CC} ${CFLAGS} wytalkC.c socketfun.c -o wytalkC
#clean slate. everything is removed. use for testing after edits have been made.
clean:
	/bin/rm -f a.out core.* wytalkC wytalkD
