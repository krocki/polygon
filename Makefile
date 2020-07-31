.SUFFIXES:
TARGETS=poly

CC=gcc
CFLAGS=-g -Wfatal-errors -O0 -Wall
DEPS:=$(wildcard *.h) Makefile

OS:=$(shell uname)
ifeq ($(OS),Darwin) # Mac OS
  GL_FLAGS=-lglfw -framework Cocoa -framework OpenGL -lpthread
  CFLAGS:=$(CFLAGS) -DAPPLE -DGL_SILENCE_DEPRECATION
else # Linux or other
  GL_FLAGS=-lglfw -lGL -lpthread
endif

LFLAGS:=$(LFLAGS) $(GL_FLAGS)

#########

all: $(TARGETS) $(DEPS)

#%.o: %.c $(DEPS)
#	$(CC) $(CFLAGS) -c $< -o $@

%: %.c $(DEPS)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(TARGETS)
