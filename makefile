.PHONY: all generate_o generate_t

CC=g++
CFLAGS=-c -Wall -x c++
LDFLAGS=
SRCPATH:=src/

all: generate_o generate_t
	
generate_o: 
	${CC} ${CFLAGS} ${SRCPATH}qtest.h -o qtest.o

generate_t:
	${CC} -Isrc -o test test.cpp 
