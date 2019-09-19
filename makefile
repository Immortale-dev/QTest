.PHONY: all generate_o generate_t

CC=g++
CFLAGS=-c -Wall -x c++
LDFLAGS=
SRCPATH:=src/
SRCS:=$(wildcard $(SRCPATH)*.hpp)
OBJS:=$(SRCS:%.hpp=%.o)

all: generate_o generate_t
	
generate_o: ${OBJS}
	${CC} ${CFLAGS} test.cpp -o test.o -Isrc
	
%.o: %.hpp
	${CC} ${CFLAGS} $< -o $@

generate_t:
	${CC} -o test.exe test.o
