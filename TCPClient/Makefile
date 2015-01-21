# Author: Christopher Schneider & Brett Rowberry
#Assignment: Project 1

#compiler flags:
#   -g turns on debugging info
#   -Wall turns on most compiler warnings
CFLAGS = -g -Wall 
CC = gcc
DEBUG = -g
CFLAGS = $(DEBUG) -Wall
LDLIBS =
TARGET = TCPClient
SOURCES = TCPClient.c
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))

$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS)

# phony target is not the name of a file, 
# avoids file name conflicts and improves performance
# must call 'make clean' for cleaning to occur
.PHONY : all $(TARGET) clean
clean: 
	rm $(TARGET) $(OBJECTS)

#how to have multiple targets in same makefile
#all: pattern coding sort tokenize
#
#pattern: pattern.c
#	gcc -o pattern pattern.c
#coding: coding.c
#	gcc -o coding coding.c
#sort: sort.c
#	gcc -o sort sort.c
#tokenize: tokenize.c
#	gcc -o tokenize tokenize.c