##################################
# Makefile
# Matthew Pru
# COSC 3750 Spring 2023
# Assignment 3
# Last Modified 2-12-2023
# A Makefile mused to compile and run wycat for assignment 4
#################################
CFLAGS=-ggdb -Wall  #allows debugging information, warning messages, and only searches the current working directory
CC=gcc #shortcut for compiling program
.PHONY: tidy clean
#this will create the executable
wycat: wycat.c
	${CC} ${CFLAGS} wycat.c -o wycat
#cleans up but leaves the executable
tidy:
	/bin/rm -f a.out core.*
#clean slate. everything is removed. use for testing after edits have been made.
clean: tidy
	/bin/rm -f wycat


