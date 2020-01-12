.PHONY: all generate_o generate_t

CC=g++
CFLAGS=-c -Wall -x c++
LDFLAGS=
SRCPATH:=src/

all: generate_o generate_t
	
generate_o:
	${CC} ${CFLAGS} test.cpp -o test.o -Isrc
	
%.o: %.cpp
	${CC} ${CFLAGS} $< -o $@

generate_t:
	${CC} -o test.exe test.o
