##################################
# Makefile
# Matthew Pru
# COSC 3750 Spring 2023
# Assignment 3
# Last Modified 2-12-2023
#
# My first attempt at a usable make file
# This will output a program that estimates the value of mathematical 'e'
# to a specified degree
#################################
OBJS=prompt.o compute.o display.o #all object files created by make
CFLAGS=-g -Wall -I. #allows debugging information, warning messages, and only searches the current working directory
CC=@gcc ${CFLAGS} #shortcut for compiling program
.PHONY: tidy clean
#this will create the executable
approxe: approxe.c ${OBJS}
	${CC} approxe.c ${OBJS} -o approxe
prompt.o: prompt.h prompt.c
	${CC} -c prompt.c
compute.o: compute.h compute.c
	${CC} -c compute.c
display.o: display.h display.c
	${CC} -c display.c
#approxe: approxe.c ${OBJS}
#	${CC} approxe.c -o approxe
#cleans up but leaves the executable
tidy:
	/bin/rm -f a.out core.* ${OBJS}
#clean slate. everything is removed. use for testing after edits have been made.
clean: tidy
	/bin/rm -f approxe
