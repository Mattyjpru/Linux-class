CFLAGS=-ggdb -Wall  #allows debugging information, warning messages, and only s$
CC=gcc #shortcut for compiling program
.PHONY: clean
#this will create the executable
test: test.c
	${CC} ${CFLAGS} test.c -o test
#clean slate. everything is removed. use for testing after edits have been made.
clean:
	/bin/rm -f a.out core.* test


